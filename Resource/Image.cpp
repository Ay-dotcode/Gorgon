#include "../External/PNG/png.h"
#include "../External/PNG/pngstruct.h"
#include "Image.h"
#include "File.h"
#include "NullImage.h"
#include "../Encoding/LZMA.h"
#include "../External/JPEG/jpeglib.h"
#include "../Engine/GGEMain.h"
#include <cmath>
#include "../Utils/BasicMath.h"

using namespace gge::resource;
using namespace gge::graphics;
using namespace gge::utils;
using namespace std;

namespace gge { namespace resource {

	void ReadDataFromInputStream(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
		if(!png_ptr->io_ptr)
			return;  

		istream& inputStream = *(istream*)(png_ptr->io_ptr);
		inputStream.read((char*)outBytes,byteCountToRead);
	} 

	Image *LoadImageResource(File& File, istream &Data, int Size) {
		int i;
		Image *img=new Image;
		img->file=&File;

		bool lateloading=false;
		//BYTE *compressionprops;
		graphics::ColorMode::Type m;
		 
		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);
			auto currenttarget=Data.tellg()+size;

			if(gid==GID::Image_Props) {
				int w, h;

				ReadFrom(Data,w);
				ReadFrom(Data,h);
				ReadFrom(Data,m);
				ReadFrom<int>(Data);

				img->Resize(w,h,m);

				if(size>16) {
					lateloading=ReadFrom<char>(Data)!=0;
				}

				if(size!=17)
					Data.seekg(size-17,ios::cur);
			} 
			else if(gid==GID::Guid) {
				img->guid.LoadLong(Data);
			}
			else if(gid==GID::SGuid) {
				img->guid.Load(Data);
			}
			else if(gid==GID::Image_Cmp_Props) {
				ReadFrom(Data, img->Compression);

				if(img->Compression==GID::LZMA) {
					img->CompressionProps=new Byte[Image::Lzma.PropertySize()];
					Data.read((char*)img->CompressionProps, Image::Lzma.PropertySize());
				}
			} else if(gid==GID::Image_Data) {
				if(lateloading) {
					img->DataLocation=(int)Data.tellg();
					img->DataSize=size;

					Data.seekg(size,ios::cur);
				} else {
					if(img->Data.GetSize()!=size)
						throw std::runtime_error("Image data size mismatch!");

					Data.read((char*)img->Data.GetBuffer(),size);

					img->isLoaded=true;
				}
			} else if(gid==GID::Image_Cmp_Data) {
				if(lateloading) {
					img->DataLocation=(int)Data.tellg();
					img->DataSize=size;

					Data.seekg(size,ios::cur);
				} 
				else {
					if(img->Compression==GID::LZMA) {
						Byte *buffer=img->Data.GetBuffer();
						Image::Lzma.Decode(Data, buffer, img->CompressionProps, img->Data.GetSize());

						utils::CheckAndDeleteArray(img->CompressionProps);
					} 
					else if(img->Compression==GID::JPEG) {
						int cpos=(int)Data.tellg();
						jpeg_decompress_struct cinf;
						jpeg_decompress_struct* cinfo=&cinf;


						//Create an error handler
						jpeg_error_mgr jerr;

						//point the compression object to the error handler
						cinfo->err=jpeg_std_error(&jerr);

						//Initialize the decompression object
						jpeg_create_decompress(cinfo);

						FILE *data2;
#ifdef MSVC
						fopen_s(&data2, File.GetFilename().c_str(), "rb");
#else
						data2=fopen(File.GetFilename().c_str(), "rb");
#endif
						fseek(data2,cpos,SEEK_SET);
						//Specify the data source
						jpeg_stdio_src(cinfo, data2);
						cinfo->src->fill_input_buffer(cinfo);

						//Decode the jpeg data into the image
						//Read in the header
						jpeg_read_header(cinfo, true);

						//start to decompress the data
						jpeg_start_decompress(cinfo);

						int channels=cinfo->num_components;

						if(channels==3)
							m=graphics::ColorMode::BGR;
						else if(channels==4)
							m=graphics::ColorMode::ABGR;
						else if(channels==1)
							m=graphics::ColorMode::Alpha;

						if(img->GetMode()!=ColorMode::ARGB)
							throw std::runtime_error("Image data size mismatch!");

						int stride=img->GetWidth()*channels;
						int rowsRead=0;

						Byte ** rowPtr = new Byte * [img->getheight()];
						for(i=0;i<img->GetHeight();i++)
							rowPtr[i]=img->Data+stride*i;

						while(cinfo->output_scanline < cinfo->output_height) {
							//read in this row
							rowsRead+=jpeg_read_scanlines(cinfo, rowPtr+rowsRead, cinfo->output_height - rowsRead);
						}

						delete rowPtr;
						fclose(data2);
					} 
					else if(img->Compression==GID::PNG) {
						png_structp png_ptr;
						png_infop info_ptr;

						unsigned long width,height;
						int bit_depth,color_type;

						unsigned char sig[8];

						ReadFrom(Data,sig);
						if (!png_check_sig(sig, 8))
							goto errorout;

						png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,NULL);
						if (!png_ptr)
							goto errorout;

						info_ptr = png_create_info_struct(png_ptr);
						if (!info_ptr) {
							png_destroy_read_struct(&png_ptr, NULL, NULL);
							goto errorout;
						}

						png_set_read_fn(png_ptr, dynamic_cast<istream*>(&Data), &ReadDataFromInputStream);

						png_set_sig_bytes(png_ptr, 8);
						png_read_info(png_ptr, info_ptr);

						png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)&width, (png_uint_32*)&height, &bit_depth,
							&color_type, NULL, NULL, NULL);


						if (color_type == PNG_COLOR_TYPE_PALETTE)
							png_set_expand(png_ptr);
						if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
							png_set_expand(png_ptr);
						if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
							png_set_expand(png_ptr);
						if (color_type == PNG_COLOR_TYPE_PALETTE)
							png_set_palette_to_rgb(png_ptr);
						if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
							png_set_expand_gray_1_2_4_to_8(png_ptr);
						if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
							png_set_tRNS_to_alpha(png_ptr);
						if (bit_depth == 16)
							png_set_strip_16(png_ptr);
						if (color_type == PNG_COLOR_TYPE_GRAY ||
							color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
							png_set_gray_to_rgb(png_ptr);

						unsigned int  i, rowbytes;
						unsigned char **  row_pointers=new unsigned char*[height];

						png_read_update_info(png_ptr, info_ptr);

						rowbytes = png_get_rowbytes(png_ptr, info_ptr);
						int pChannels = (int)png_get_channels(png_ptr, info_ptr);

						unsigned char *image_data;

						image_data = new unsigned char[rowbytes*height];

						if (image_data == NULL) {
							png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
							goto errorout;
						}

						for (i = 0;  i < height;  ++i)
							row_pointers[i] = image_data + i*rowbytes;

						png_read_image(png_ptr, row_pointers);

						png_read_end(png_ptr, NULL);

						png_destroy_read_struct(&png_ptr, &info_ptr, NULL);


						//currently only RGB is supported
						if(pChannels!=4)
							goto errorout;

						for(unsigned y=0;y<height;y++) {
							for(unsigned x=0;x<width;x++) {
								img->Data[(x+y*width)*4+2]=row_pointers[y][x*4];
								img->Data[(x+y*width)*4+1]=row_pointers[y][x*4+1];
								img->Data[(x+y*width)*4+0]=row_pointers[y][x*4+2];
								img->Data[(x+y*width)*4+3]=row_pointers[y][x*4+3];
							}
						}

						delete[] row_pointers;
						delete[] image_data;

						img->isLoaded=true;
					}
					else {
errorout:
						Data.seekg(currenttarget, ios::beg);
					}

					img->isLoaded=true;
				}
			} 
			else if(gid==GID::Image_Palette) {
				img->Palette=new Byte[size];
				Data.read((char*)img->Palette, size);
			}
		}

		return img;
	}

	bool Image::Load(bool force) {
		if(isLoaded && !force)
			return false;

		int i;
		FILE *gfile;
#ifdef MSVC
		errno_t err;
		//To be compatible with JPEG read
		err=fopen_s(&gfile, file->GetFilename().c_str(), "rb");
		if(err != 0) return false;
#else
		gfile=fopen(file->GetFilename().c_str(), "rb");
		if(!gfile) return false;
#endif

		fseek(gfile,DataLocation,SEEK_SET);
		
		if(this->Compression==GID::LZMA) {
			this->Data.Resize(this->GetWidth()*this->GetHeight()*this->GetBPP());

			Byte *buffer=Data.GetBuffer();
			Image::Lzma.Decode(Data, buffer, CompressionProps, Data.GetSize());

			utils::CheckAndDeleteArray(CompressionProps);
		} else if(Compression==GID::JPEG) {
			jpeg_decompress_struct cinf;
			jpeg_decompress_struct* cinfo=&cinf;


			//Create an error handler
			jpeg_error_mgr jerr;

			//point the compression object to the error handler
			cinfo->err=jpeg_std_error(&jerr);

			//Initialize the decompression object
			jpeg_create_decompress(cinfo);

			//Specify the data source
			jpeg_stdio_src(cinfo, gfile);
			cinfo->src->fill_input_buffer(cinfo);

			//Decode the jpeg data into the image
			//Read in the header
			jpeg_read_header(cinfo, true);

			//start to decompress the data
			jpeg_start_decompress(cinfo);

			int channels=cinfo->num_components;

			if(channels==3)
				Mode=graphics::ColorMode::BGR;
			else if(channels==4)
				Mode=graphics::ColorMode::ABGR;
			else if(channels==1)
				Mode=graphics::ColorMode::Alpha;

			Data.Resize(GetWidth()*GetHeight()*channels);

			int stride=GetWidth()*channels;
			int rowsRead=0;

			Byte ** rowPtr = new Byte * [GetHeight()];
			for(i=0;i<GetHeight();i++)
				rowPtr[i]=Data+stride*i;

			while(cinfo->output_scanline < cinfo->output_height) {
				//read in this row
				rowsRead+=jpeg_read_scanlines(cinfo, rowPtr+rowsRead, cinfo->output_height - rowsRead);
			}

			delete rowPtr;
		} else {
			this->Data.Resize(DataSize);
			fread(this->Data,1,DataSize,gfile);
		}

		fclose(gfile);

		this->isLoaded=true;

		return true;
	}

	void Image::Prepare(GGEMain &main, resource::File &file) {
#ifdef _DEBUG
			if(Data==NULL) {
				os::DisplayMessage("Image Resource","Data is not loaded yet.");
				assert(0);
			}
#endif
		
		if(Texture.ID>0)
			system::DestroyTexture(Texture);

		Texture = graphics::system::GenerateTexture(Data, GetWidth(), GetHeight(), GetMode());
	}

	bool Image::PNGExport(string filename) {
		int i;
		FILE*file;
#ifdef MSVC
		errno_t err;
		err=fopen_s(&file,filename.data(),"wb");
		if(err != 0) return false;
#else
		file=fopen(filename.data(),"wb");
		if(!file) return false;
#endif
		

		png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr)
		   return false;

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
		   png_destroy_write_struct(&png_ptr,
			 (png_infopp)NULL);
		   return false;
		}
		setjmp(png_jmpbuf(png_ptr));
		png_init_io(png_ptr, file);
		png_set_IHDR(png_ptr, info_ptr, GetWidth(), GetHeight(),
	       8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		Byte *newdata=new Byte[GetWidth()*GetHeight()*4];
		for(i=0;i<GetWidth()*GetHeight();i++) {
			newdata[i*4+2]=Data[i*4+0];
			newdata[i*4+1]=Data[i*4+1];
			newdata[i*4+0]=Data[i*4+2];
			newdata[i*4+3]=Data[i*4+3];
		}

		png_write_info(png_ptr, info_ptr);
		Byte **rows=new Byte*[GetHeight()];
		for(i=0;i<GetHeight();i++) {
			rows[i]=newdata+i*GetWidth()*4;
		}
		png_write_image(png_ptr, rows);
		png_write_end(png_ptr, NULL);


		fclose(file);

		return true;
	}
	Image::PNGReadError Image::ImportPNG(string filename) {

		png_structp png_ptr;
		png_infop info_ptr;

		unsigned long width,height;
		int bit_depth,color_type;

		FILE *infile;
#ifdef MSVC
		errno_t err;
		err=fopen_s(&infile, filename.c_str(), "rb");
		if(err != 0) return Image::FileNotFound;
#else
		infile=fopen(filename.c_str(), "rb");
		if(!infile) return Image::FileNotFound;
#endif
	    unsigned char sig[8];

		fread(sig, 1, 8, infile);
		if (!png_check_sig(sig, 8))
			return Image::Signature;

		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,NULL);
		if (!png_ptr)
			return Image::OutofMemory;   /* out of memory */

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			return Image::OutofMemory;   /* out of memory */
		}

		if (setjmp(png_ptr->longjmp_buffer)) {
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return Image::ErrorHandlerProblem;
		}

		png_init_io(png_ptr, infile);
		png_set_sig_bytes(png_ptr, 8);
		png_read_info(png_ptr, info_ptr);

		png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)&width, (png_uint_32*)&height, &bit_depth,
			&color_type, NULL, NULL, NULL);


		if (color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_expand(png_ptr);
		if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
			png_set_expand(png_ptr);
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_expand(png_ptr);
		if (color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_palette_to_rgb(png_ptr);
		if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
			png_set_expand_gray_1_2_4_to_8(png_ptr);
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_tRNS_to_alpha(png_ptr);
		if (bit_depth == 16)
			png_set_strip_16(png_ptr);
		if (color_type == PNG_COLOR_TYPE_GRAY ||
			color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png_ptr);

		unsigned int  i, rowbytes;
		unsigned char **  row_pointers=new unsigned char*[height];

		png_read_update_info(png_ptr, info_ptr);

		rowbytes = png_get_rowbytes(png_ptr, info_ptr);
		int pChannels = (int)png_get_channels(png_ptr, info_ptr);

		unsigned char *image_data;

		image_data = new unsigned char[rowbytes*height];

		if (image_data == NULL) {
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return Image::OutofMemory;
		}

		for (i = 0;  i < height;  ++i)
			row_pointers[i] = image_data + i*rowbytes;

		png_read_image(png_ptr, row_pointers);

		png_read_end(png_ptr, NULL);

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);


		Resize(width,height,graphics::ColorMode::ARGB);

		//currently only RGB is supported
		if(pChannels==4) {
			for(unsigned y=0;y<height;y++) {
				for(unsigned x=0;x<width;x++) {
					Data[(x+y*width)*4+2]=row_pointers[y][x*4];
					Data[(x+y*width)*4+1]=row_pointers[y][x*4+1];
					Data[(x+y*width)*4+0]=row_pointers[y][x*4+2];
					Data[(x+y*width)*4+3]=row_pointers[y][x*4+3];
				}
			}
		}
		else {
			for(unsigned y=0;y<height;y++) {
				for(unsigned x=0;x<width;x++) {
					Data[(x+y*width)*4+2]=row_pointers[y][x*3];
					Data[(x+y*width)*4+1]=row_pointers[y][x*3+1];
					Data[(x+y*width)*4+0]=row_pointers[y][x*3+2];
					Data[(x+y*width)*4+3]=0xff;
				}
			}
		}
		
		delete[] row_pointers;
		delete[] image_data;
		
		isLoaded=true;
		LeaveData=true;

		fclose(infile);

		return NoError;
	}

	inline float gaussian(float amount, int dist) {
		return exp(-(float)dist*(float)dist/(2*amount*amount))/sqrt(2*Pi*amount*amount);
	}

	Image &Image::Blur(float amount, int windowsize/*=-1*/) {
		if(windowsize==-1)
			windowsize=max(1,int(amount*1.5));

		Image *img=new Image(Width+windowsize*2,Height+windowsize*2,Mode);

		if(Mode==ColorMode::ARGB || Mode==ColorMode::ABGR) {
			blurargb(amount, windowsize, img);
		}
		else {
			bluralpha(amount, windowsize, img);
		}

		return *img;
	}

	void Image::blurargb(float amount, int windowsize, Image *img) {
		float *kernel=new float[windowsize+1];
		for(int i=0;i<=windowsize;i++)
			kernel[i]=gaussian(amount, i);

		int newimw=img->Width, newimh=img->Height;
		int bpp=GetBPP();

		for(int y=0;y<newimh;y++) {
			for(int x=0;x<newimw;x++) {
				for(int c=0;c<4;c++) {
					float sum=0, sum_weights=0;
					for(int yy=-windowsize;yy<=windowsize;yy++) {
						for(int xx=-windowsize;xx<=windowsize;xx++) {
							int oldcoordx=x+xx-windowsize, oldcoordy=y+yy-windowsize;

							if(oldcoordx>=0 && oldcoordy>=0 && oldcoordx<Width && oldcoordy<Height) {
								sum+=kernel[abs(xx)]*kernel[abs(yy)]*Data[ (oldcoordx+oldcoordy*Width)*bpp+c ];
							}
							sum_weights+=kernel[abs(xx)]*kernel[abs(yy)];
						}
					}

					img->Data[ (x+y*img->Width)*bpp+c ]=(int)Round(sum/sum_weights);
				}
			}
		}

		delete[] kernel;
	}

	void Image::bluralpha(float amount, int windowsize, Image *img) {
		float *kernel=new float[windowsize+1];
		for(int i=0;i<=windowsize;i++)
			kernel[i]=gaussian(amount, i);

		int newimw=img->Width, newimh=img->Height;
		//int bpp=GetBPP();

		for(int y=0;y<newimh;y++) {
			for(int x=0;x<newimw;x++) {
				float sum=0, sum_weights=0;
				for(int yy=-windowsize;yy<=windowsize;yy++) {
					for(int xx=-windowsize;xx<=windowsize;xx++) {
						int oldcoordx=x+xx-windowsize, oldcoordy=y+yy-windowsize;

						if(oldcoordx>=0 && oldcoordy>=0 && oldcoordx<Width && oldcoordy<Height) {
							sum+=kernel[abs(xx)]*kernel[abs(yy)]*Data[ (oldcoordx+oldcoordy*Width) ];
						}
						sum_weights+=kernel[abs(xx)]*kernel[abs(yy)];
					}
				}

				img->Data[ (x+y*img->Width) ]=(int)Round(sum/sum_weights);
			}
		}

		delete[] kernel;
	}

	Image &Image::Shadow(float amount, int windowsize/*=-1*/) {
		if(windowsize==-1)
			windowsize=max(1,int(amount*1.5));

		Image *img=new Image(Width+windowsize*2,Height+windowsize*2,ColorMode::Alpha);

		if(Mode==ColorMode::ARGB || Mode==ColorMode::ABGR) {
			shadowargb(amount, windowsize, img);
		}
		else {
			shadowalpha(amount, windowsize, img);
		}

		return *img;
	}

	void Image::shadowargb(float amount, int windowsize, Image *img) {
		float *kernel=new float[windowsize+1];
		for(int i=0;i<=windowsize;i++)
			kernel[i]=gaussian(amount, i);

		int newimw=img->Width, newimh=img->Height;
		int bpp=GetBPP();

		for(int y=0;y<newimh;y++) {
			for(int x=0;x<newimw;x++) {
				float sum=0, sum_weights=0;
				for(int yy=-windowsize;yy<=windowsize;yy++) {
					for(int xx=-windowsize;xx<=windowsize;xx++) {
						int oldcoordx=x+xx-windowsize, oldcoordy=y+yy-windowsize;

						if(oldcoordx>=0 && oldcoordy>=0 && oldcoordx<Width && oldcoordy<Height) {
							sum+=kernel[abs(xx)]*kernel[abs(yy)]*Data[ (oldcoordx+oldcoordy*Width)*bpp+3 ];
						}
						sum_weights+=kernel[abs(xx)]*kernel[abs(yy)];
					}
				}

				img->Data[ (x+y*img->Width) ]=(int)Round(sum/sum_weights);
			}
		}

		delete[] kernel;
	}

	void Image::shadowalpha(float amount, int windowsize, Image *img) {
		float *kernel=new float[windowsize+1];
		for(int i=0;i<=windowsize;i++)
			kernel[i]=gaussian(amount, i);

		int newimw=img->Width, newimh=img->Height;
		//int bpp=GetBPP();

		for(int y=0;y<newimh;y++) {
			for(int x=0;x<newimw;x++) {
				float sum=0, sum_weights=0;
				for(int yy=-windowsize;yy<=windowsize;yy++) {
					for(int xx=-windowsize;xx<=windowsize;xx++) {
						int oldcoordx=x+xx-windowsize, oldcoordy=y+yy-windowsize;

						if(oldcoordx>=0 && oldcoordy>=0 && oldcoordx<Width && oldcoordy<Height) {
							sum+=kernel[abs(xx)]*kernel[abs(yy)]*Data[ oldcoordx+oldcoordy*Width ];
						}
						sum_weights+=kernel[abs(xx)]*kernel[abs(yy)];
					}
				}

				img->Data[ (x+y*img->Width) ]=(int)Round(sum/sum_weights);
			}
		}

		delete[] kernel;
	}

	encoding::LZMA Image::Lzma(false);


	NullImage *NullImage::ni;
} }
