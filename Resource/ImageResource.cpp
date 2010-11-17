#include "ImageResource.h"
#include "ResourceFile.h"
#include "../External/LZMA/LzmaDecode.h"
#include "../External/JPEG/jpeglib.h"
#include "../External/PNG/png.h"
#include "../Engine/GGEMain.h"

namespace gre {
	ResourceBase *LoadImageResource(ResourceFile* File, FILE* Data, int Size) {
		int i;
		ImageResource *img=new ImageResource;
		img->File=File;

		int tpos=ftell(Data)+Size;
		bool lateloading=0;
		//BYTE *compressionprops;
		gge::ColorMode m;

		while(ftell(Data)<tpos) {
			int gid,size,tmpint;
			fread(&gid,1,4,Data);
			fread(&size,1,4,Data);

			if(gid==GID_IMAGE_PROPS) {
				int w, h;

				fread(&w,1,4,Data);
				fread(&h,1,4,Data);
				fread(&m,1,4,Data);
				fread(&tmpint,1,4,Data);

				img->Resize(w,h,m);

				if(size>16)
					fread(&lateloading,1,1,Data);
				else
					fseek(Data,1,SEEK_CUR);

				if(size!=17)
					fseek(Data,size-17,SEEK_CUR);
			} 
			else if(gid==GID_GUID) {
				img->guid=new Guid(Data);
			}
			else if(gid==GID_IMAGE_CMP_PROPS) {
				fread(&img->Compression,1,4,Data);
				if(img->Compression==GID_LZMA) {
					img->CompressionProps=new BYTE[LZMA_PROPERTIES_SIZE];
					fread(img->CompressionProps,1,LZMA_PROPERTIES_SIZE,Data);
				}
			} else if(gid==GID_IMAGE_DATA) {
				if(lateloading) {
					img->DataLocation=ftell(Data);
					img->DataSize=size;

					fseek(Data,size,SEEK_CUR);
				} else {
					if(img->Data.GetSize()!=size)
						size=size;//throw std::runtime_error("Image data size mismatch!");

					fread(img->Data,1,size,Data);

					img->isLoaded=true;
				}
			} else if(gid==GID_IMAGE_CMP_DATA) {
				if(lateloading) {
					img->DataLocation=ftell(Data);
					img->DataSize=size;

					fseek(Data,size,SEEK_CUR);
				} else {
					if(img->Compression==GID_LZMA) {
						BYTE *tmpdata=new BYTE[size];
						fread(tmpdata,1,size,Data);
						size_t processed,processed2;
						CLzmaDecoderState state;

						LzmaDecodeProperties(&state.Properties,img->CompressionProps,LZMA_PROPERTIES_SIZE);
						state.Probs = (CProb *)malloc(LzmaGetNumProbs(&state.Properties) * sizeof(CProb));

						LzmaDecode(&state,tmpdata,size,&processed,img->Data,img->getWidth()*img->getHeight()*img->getBPP(),&processed2);

						free(state.Probs);
						delete tmpdata;
						delete img->CompressionProps;
					} else if(img->Compression==GID_JPEG) {
						int cpos=ftell(Data);
						jpeg_decompress_struct cinf;
						jpeg_decompress_struct* cinfo=&cinf;


						//Create an error handler
						jpeg_error_mgr jerr;

						//point the compression object to the error handler
						cinfo->err=jpeg_std_error(&jerr);

						//Initialize the decompression object
						jpeg_create_decompress(cinfo);

						//Specify the data source
						jpeg_stdio_src(cinfo, Data);
						cinfo->src->fill_input_buffer(cinfo);

						//Decode the jpeg data into the image
						//Read in the header
						jpeg_read_header(cinfo, true);

						//start to decompress the data
						jpeg_start_decompress(cinfo);

						int channels=cinfo->num_components;

						if(channels==3)
							m=BGR;
						else if(channels==4)
							m=ABGR_32BPP;
						else if(channels=1)
							m=ALPHAONLY_8BPP;

						if(img->getMode()!=m)
							throw std::runtime_error("Image data size mismatch!");

						int stride=img->getWidth()*channels;
						int rowsRead=0;

						BYTE ** rowPtr = new BYTE * [img->getHeight()];
						for(i=0;i<img->getHeight();i++)
							rowPtr[i]=img->Data+stride*i;

						while(cinfo->output_scanline < cinfo->output_height) {
							//read in this row
							rowsRead+=jpeg_read_scanlines(cinfo, rowPtr+rowsRead, cinfo->output_height - rowsRead);
						}

						delete rowPtr;
						fseek(Data,cpos+size,SEEK_SET);
					}

					img->isLoaded=true;
				}
			} else if(gid==GID_IMAGE_PALETTE) {
				img->Palette=new BYTE[size];
				fread(img->Palette,1,size,Data);
			}
		}

		return img;
	}

	bool ImageResource::Load() {
		if(isLoaded)
			return false;

		int i;
		FILE *gfile=fopen(File->getFilename().data(),"rb");
		fseek(gfile,DataLocation,SEEK_SET);
		
		if(this->Compression==GID_LZMA) {
			this->Data.Resize(this->getWidth()*this->getHeight()*this->getBPP());
			BYTE *tmpdata=new BYTE[DataSize];
			fread(tmpdata,1,this->DataSize,gfile);
			size_t processed,processed2;
			CLzmaDecoderState state;

			LzmaDecodeProperties(&state.Properties,this->CompressionProps,LZMA_PROPERTIES_SIZE);
			state.Probs = (CProb *)malloc(LzmaGetNumProbs(&state.Properties) * sizeof(CProb));

			LzmaDecode(&state,tmpdata,DataSize,&processed,this->Data,this->getWidth()*this->getHeight()*this->getBPP(),&processed2);

			free(state.Probs);
			delete tmpdata;
			delete this->CompressionProps;
		} else if(Compression==GID_JPEG) {
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
				Mode=BGR;
			else if(channels==4)
				Mode=ABGR_32BPP;
			else if(channels=1)
				Mode=ALPHAONLY_8BPP;

			Data.Resize(getWidth()*getHeight()*channels);

			int stride=getWidth()*channels;
			int rowsRead=0;

			BYTE ** rowPtr = new BYTE * [getHeight()];
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
	}

	void ImageResource::Prepare(GGEMain *main) {
#ifdef _DEBUG
			if(Data==NULL) {
				DisplayMessage("Image Resource","Data is not loaded yet.");
				assert(0);
			}
#endif
		
		if(Texture.ID>0)
			DestroyTexture(&Texture);

		Texture = gge::GenerateTexture(Data, getWidth(), getHeight(), getMode());
	}

	void ImageResource::PNGExport(string filename) {
		int i;
		FILE*file=fopen(filename.data(),"wb");

		png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr)
		   return;

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
		   png_destroy_write_struct(&png_ptr,
			 (png_infopp)NULL);
		   return;
		}
		setjmp(png_jmpbuf(png_ptr));
		png_init_io(png_ptr, file);
		png_set_IHDR(png_ptr, info_ptr, getWidth(), getHeight(),
	       8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		BYTE *newdata=new BYTE[getWidth()*getHeight()*4];
		for(i=0;i<getWidth()*getHeight();i++) {
			newdata[i*4+2]=Data[i*4+0];
			newdata[i*4+1]=Data[i*4+1];
			newdata[i*4+0]=Data[i*4+2];
			newdata[i*4+3]=Data[i*4+3];
		}

		png_write_info(png_ptr, info_ptr);
		BYTE **rows=new BYTE*[getHeight()];
		for(i=0;i<getHeight();i++) {
			rows[i]=newdata+i*getWidth()*4;
		}
		png_write_image(png_ptr, rows);
		png_write_end(png_ptr, NULL);


		fclose(file);
	}
	PNGReadError ImageResource::ImportPNG(string filename) {

		png_structp png_ptr;
		png_infop info_ptr;

		unsigned long width,height;
		int bit_depth,color_type;

		FILE *infile=fopen(filename.c_str(),"rb");

	    unsigned char sig[8];

		fread(sig, 1, 8, infile);
		if (!png_check_sig(sig, 8))
			return Signature;

		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,NULL);
		if (!png_ptr)
			return OutofMemory;   /* out of memory */

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			return OutofMemory;   /* out of memory */
		}

		if (setjmp(png_ptr->jmpbuf)) {
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return ErrorHandlerProblem;
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
			return OutofMemory;
		}

		for (i = 0;  i < height;  ++i)
			row_pointers[i] = image_data + i*rowbytes;

		png_read_image(png_ptr, row_pointers);

		png_read_end(png_ptr, NULL);

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);


		Resize(width,height,ColorMode::ARGB_32BPP);

		//currently only RGB is supported
		if(pChannels!=3)
			return UnimplementedType;

		for(int y=0;y<height;y++) {
			for(int x=0;x<width;x++) {
				Raw2DGraphic::Data.operator []((x+y*width)*4+2)=row_pointers[y][x*3];
				Raw2DGraphic::Data.operator []((x+y*width)*4+1)=row_pointers[y][x*3+1];
				Raw2DGraphic::Data.operator []((x+y*width)*4+0)=row_pointers[y][x*3+2];
				Raw2DGraphic::Data.operator []((x+y*width)*4+3)=0xff;
			}
		}
		
		delete[] row_pointers;
		delete[] image_data;
		
		isLoaded=true;
		LeaveData=true;

		fclose(infile);

		return NoError;
	}
	void ImageResource::DrawResized(I2DGraphicsTarget *Target, int X, int Y, int W, int H, gge::Alignment Align) {


		int h=this->Height(H);
		int w=this->Width(W);

		if(Align & ALIGN_CENTER)
			X+=(W-w)/2;
		else if(Align & ALIGN_RIGHT)
			X+= W-w;

		if(Align & ALIGN_MIDDLE)
			Y+=(H-h)/2;
		else if(Align & ALIGN_BOTTOM)
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

}
