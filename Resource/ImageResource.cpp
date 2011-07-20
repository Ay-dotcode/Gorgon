#include "ImageResource.h"
#include "ResourceFile.h"
#include "../External/LZMA/LzmaDecode.h"
#include "../External/JPEG/jpeglib.h"
#include "../External/PNG/png.h"
#include "../Engine/GGEMain.h"

using namespace gge::resource;
using namespace gge::graphics;
using namespace std;

namespace gge { namespace resource {
	ResourceBase *LoadImageResource(File& File, istream &Data, int Size) {
		int i;
		ImageResource *img=new ImageResource;
		img->File=&File;

		bool lateloading=false;
		//BYTE *compressionprops;
		graphics::ColorMode m;

		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);

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
					img->CompressionProps=new Byte[LZMA_PROPERTIES_SIZE];
					Data.read((char*)img->CompressionProps, LZMA_PROPERTIES_SIZE);
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
				} else {
					if(img->Compression==GID::LZMA) {
						Byte *tmpdata=new Byte[size];
						Data.read((char*)tmpdata, size);

						size_t processed,processed2;
						CLzmaDecoderState state;

						LzmaDecodeProperties(&state.Properties,img->CompressionProps,LZMA_PROPERTIES_SIZE);
						state.Probs = (CProb *)malloc(LzmaGetNumProbs(&state.Properties) * sizeof(CProb));

						LzmaDecode(&state,tmpdata,size,&processed,img->Data,img->getWidth()*img->getHeight()*img->getBPP(),&processed2);

						free(state.Probs);
						delete tmpdata;
						delete img->CompressionProps;
					} else if(img->Compression==GID::JPEG) {
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
						fopen_s(&data2, File.getFilename().c_str(), "rb");
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
							m=graphics::BGR;
						else if(channels==4)
							m=graphics::ABGR_32BPP;
						else if(channels=1)
							m=graphics::ALPHAONLY_8BPP;

						if(img->getMode()!=m)
							throw std::runtime_error("Image data size mismatch!");

						int stride=img->getWidth()*channels;
						int rowsRead=0;

						Byte ** rowPtr = new Byte * [img->getHeight()];
						for(i=0;i<img->getHeight();i++)
							rowPtr[i]=img->Data+stride*i;

						while(cinfo->output_scanline < cinfo->output_height) {
							//read in this row
							rowsRead+=jpeg_read_scanlines(cinfo, rowPtr+rowsRead, cinfo->output_height - rowsRead);
						}

						delete rowPtr;
						fclose(data2);
						Data.seekg(size, ios::cur);
					}

					img->isLoaded=true;
				}
			} else if(gid==GID::Image_Palette) {
				img->Palette=new Byte[size];
				Data.read((char*)img->Palette, size);
			}
		}

		return img;
	}

	bool ImageResource::Load() {
		if(isLoaded)
			return false;

		int i;
		FILE *gfile;
		errno_t err;
		//To be compatible with JPEG read
		err=fopen_s(&gfile, File->getFilename().data(), "rb");
		if(err != 0) return false;

		fseek(gfile,DataLocation,SEEK_SET);
		
		if(this->Compression==GID::LZMA) {
			this->Data.Resize(this->getWidth()*this->getHeight()*this->getBPP());
			Byte *tmpdata=new Byte[DataSize];
			fread(tmpdata,1,this->DataSize,gfile);
			size_t processed,processed2;
			CLzmaDecoderState state;

			LzmaDecodeProperties(&state.Properties,this->CompressionProps,LZMA_PROPERTIES_SIZE);
			state.Probs = (CProb *)malloc(LzmaGetNumProbs(&state.Properties) * sizeof(CProb));

			LzmaDecode(&state,tmpdata,DataSize,&processed,this->Data,this->getWidth()*this->getHeight()*this->getBPP(),&processed2);

			free(state.Probs);
			delete tmpdata;
			delete this->CompressionProps;
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
				Mode=graphics::BGR;
			else if(channels==4)
				Mode=graphics::ABGR_32BPP;
			else if(channels=1)
				Mode=graphics::ALPHAONLY_8BPP;

			Data.Resize(getWidth()*getHeight()*channels);

			int stride=getWidth()*channels;
			int rowsRead=0;

			Byte ** rowPtr = new Byte * [getHeight()];
			for(i=0;i<getHeight();i++)
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

	void ImageResource::Prepare(GGEMain &main) {
#ifdef _DEBUG
			if(Data==NULL) {
				os::DisplayMessage("Image Resource","Data is not loaded yet.");
				assert(0);
			}
#endif
		
		if(Texture.ID>0)
			DestroyTexture(&Texture);

		Texture = graphics::GenerateTexture(Data, getWidth(), getHeight(), getMode());
	}

	bool ImageResource::PNGExport(string filename) {
		int i;
		errno_t err;
		FILE*file;
		err=fopen_s(&file,filename.data(),"wb");
		if(err != 0) return false;
		

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
		png_set_IHDR(png_ptr, info_ptr, getWidth(), getHeight(),
	       8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		Byte *newdata=new Byte[getWidth()*getHeight()*4];
		for(i=0;i<getWidth()*getHeight();i++) {
			newdata[i*4+2]=Data[i*4+0];
			newdata[i*4+1]=Data[i*4+1];
			newdata[i*4+0]=Data[i*4+2];
			newdata[i*4+3]=Data[i*4+3];
		}

		png_write_info(png_ptr, info_ptr);
		Byte **rows=new Byte*[getHeight()];
		for(i=0;i<getHeight();i++) {
			rows[i]=newdata+i*getWidth()*4;
		}
		png_write_image(png_ptr, rows);
		png_write_end(png_ptr, NULL);


		fclose(file);

		return true;
	}
	ImageResource::PNGReadError ImageResource::ImportPNG(string filename) {

		png_structp png_ptr;
		png_infop info_ptr;

		unsigned long width,height;
		int bit_depth,color_type;

		FILE *infile;
		errno_t err;
		err=fopen_s(&infile, filename.c_str(), "rb");
		if(err != 0) return ImageResource::FileNotFound;

	    unsigned char sig[8];

		fread(sig, 1, 8, infile);
		if (!png_check_sig(sig, 8))
			return ImageResource::Signature;

		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,NULL);
		if (!png_ptr)
			return ImageResource::OutofMemory;   /* out of memory */

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			return ImageResource::OutofMemory;   /* out of memory */
		}

		if (setjmp(png_ptr->jmpbuf)) {
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return ImageResource::ErrorHandlerProblem;
		}

		png_init_io(png_ptr, infile);
		png_set_sig_bytes(png_ptr, 8);
		png_read_info(png_ptr, info_ptr);

		png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
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
			png_set_gray_1_2_4_to_8(png_ptr);
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

		int pRowbytes = rowbytes = png_get_rowbytes(png_ptr, info_ptr);
		int pChannels = (int)png_get_channels(png_ptr, info_ptr);

		unsigned char *image_data;

		image_data = new unsigned char[rowbytes*height];

		if (image_data == NULL) {
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return ImageResource::OutofMemory;
		}

		for (i = 0;  i < height;  ++i)
			row_pointers[i] = image_data + i*rowbytes;

		png_read_image(png_ptr, row_pointers);

		png_read_end(png_ptr, NULL);

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);


		Resize(width,height,graphics::ARGB_32BPP);

		//currently only RGB is supported
		if(pChannels!=3)
			return ImageResource::UnimplementedType;

		for(unsigned y=0;y<height;y++) {
			for(unsigned x=0;x<width;x++) {
				Raw2DGraphic::Data[(x+y*width)*4+2]=row_pointers[y][x*3];
				Raw2DGraphic::Data[(x+y*width)*4+1]=row_pointers[y][x*3+1];
				Raw2DGraphic::Data[(x+y*width)*4+0]=row_pointers[y][x*3+2];
				Raw2DGraphic::Data[(x+y*width)*4+3]=0xff;
			}
		}
		
		delete[] row_pointers;
		delete[] image_data;
		
		isLoaded=true;
		LeaveData=true;

		fclose(infile);

		return NoError;
	}
	void ImageResource::DrawResized(I2DGraphicsTarget *Target, int X, int Y, int W, int H, gge::Alignment::Type Align) {


		int h=this->Height(H);
		int w=this->Width(W);

		if(Align & Alignment::Center)
			X+=(W-w)/2;
		else if(Align & Alignment::Right)
			X+= W-w;

		if(Alignment::isMiddle(Align))
			Y+=(H-h)/2;
		else if(Alignment::isBottom(Align))
			Y+= H-h;

		if(VerticalTiling.Type==ResizableObject::Stretch) {
			if(HorizontalTiling.Type==ResizableObject::Stretch) {
				this->Draw(Target, X,Y , w,h);
			} else {
				this->DrawHTiled(Target, X,Y , w,h);
			}
		} else {
			if(HorizontalTiling.Type==ResizableObject::Stretch) {
				this->DrawVTiled(Target, X,Y , w,h);
			} else {
				this->DrawTiled(Target, X,Y , w,h);
			}
		}
	}

} }
