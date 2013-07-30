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
#include "../Encoding/PNG.h"

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

	Image *LoadImageResource(File& file, istream &Data, int Size) {
		Image *img=new Image;
		img->file=&file;

		//BYTE *compressionprops;
		 
		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);
			//auto currenttarget=Data.tellg()+size;

			if(gid==GID::Image_Props) {
				img->LoadProperties(Data, size);
			} 
			else if(gid==GID::Guid) {
				img->guid.LoadLong(Data);
			}
			else if(gid==GID::SGuid) {
				img->guid.Load(Data);
			}
			else if(gid==GID::Image_Cmp_Props) {
				img->Compression=(Image::Compressor)ReadFrom<int>(Data);

				if(img->Compression==GID::LZMA) {
					img->CompressionProps=new Byte[Image::Lzma.PropertySize()];
					Data.read((char*)img->CompressionProps, Image::Lzma.PropertySize());
				}
			} else if(gid==GID::Image_Data) {
				if(img->LateLoading) {
					img->DataLocation=(int)Data.tellg();
					img->DataSize=size;

					Data.seekg(size,ios::cur);
				} else {
					if(img->Data.GetSize()!=size)
						throw std::runtime_error("Image data size mismatch!");

					img->LoadData(Data, size);
				}
			} else if(gid==GID::Image_Cmp_Data) {
				if(img->LateLoading) {
					img->DataLocation=(int)Data.tellg();
					img->DataSize=size;

					Data.seekg(size,ios::cur);
				} 
				else {
					img->LoadData(Data, size);
				}
			} 
			else{
				EatChunk(Data, size);
			}
		}

		return img;
	}

	bool Image::Load(bool force) {
		if(isLoaded && !force)
			return true;

		ifstream gfile(this->file->GetFilename(), ios::binary);
		if(!gfile.is_open())
			return false;

		gfile.seekg(DataLocation,ios::beg);
		LoadData(gfile,DataSize);

		return true;
	}

	void Image::Prepare(GGEMain &main, resource::File &file) {
		Prepare();
	}

	void Image::Prepare() {
#ifdef _DEBUG
		if(Data==NULL) {
			os::DisplayMessage("Image Resource","Data is not loaded yet.");
			assert(0);
		}
#endif

		if(Texture.ID>0) {
			if(GetWidth()==Texture.W && GetHeight()==Texture.H)
				graphics::system::UpdateTexture(Texture, Data, GetMode());
			else {
				graphics::system::DestroyTexture(Texture);
				Texture = graphics::system::GenerateTexture(Data, GetWidth(), GetHeight(), GetMode());
			}
		}
		else
			Texture = graphics::system::GenerateTexture(Data, GetWidth(), GetHeight(), GetMode());
	}

	bool Image::ExportPNG(string filename) {
		Byte *newdata=new Byte[GetWidth()*GetHeight()*4];
		for(int i=0;i<GetWidth()*GetHeight();i++) {
			newdata[i*4+2]=Data[i*4+0];
			newdata[i*4+1]=Data[i*4+1];
			newdata[i*4+0]=Data[i*4+2];
			newdata[i*4+3]=Data[i*4+3];
		}

		ofstream file(filename, std::ios::binary);
		if(!file.is_open())
			return false;

		encoding::Png.Encode(newdata, file, GetWidth(), GetHeight());

		delete newdata;
		
		return true;
	}
	Image::ImageReadError Image::ImportPNG(string filename) {

		std::ifstream file(filename, ios::binary);

		if(!file.is_open())
			return FileNotFound;

		std::vector<Byte> imagedata;

		encoding::Info inf;
		try {
			inf=encoding::Png.Decode(file, imagedata);
		}
		catch(...) {
			return ReadError;
		}

		Resize(inf.Width, inf.Height, graphics::ColorMode::ARGB);

		if(inf.Alpha) {
			for(int y=0;y<inf.Height;y++) {
				for(int x=0;x<inf.Width;x++) {
					Data[(x+y*inf.Width)*4+2]=imagedata[(y*inf.RowBytes)+x*4+0];
					Data[(x+y*inf.Width)*4+1]=imagedata[(y*inf.RowBytes)+x*4+1];
					Data[(x+y*inf.Width)*4+0]=imagedata[(y*inf.RowBytes)+x*4+2];
					Data[(x+y*inf.Width)*4+3]=imagedata[(y*inf.RowBytes)+x*4+3];
				}
			}
		}
		else {
			for(int y=0;y<inf.Height;y++) {
				for(int x=0;x<inf.Width;x++) {
					Data[(x+y*inf.Width)*4+2]=imagedata[(y*inf.RowBytes)+x*3+0];
					Data[(x+y*inf.Width)*4+1]=imagedata[(y*inf.RowBytes)+x*3+1];
					Data[(x+y*inf.Width)*4+0]=imagedata[(y*inf.RowBytes)+x*3+2];
					Data[(x+y*inf.Width)*4+3]=255;
				}
			}
		}

		isLoaded=true;

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

	bool Image::LoadData(std::istream &gfile, int size) {
		if(this->Compression==GID::LZMA) {
			this->Data.Resize(this->GetWidth()*this->GetHeight()*this->GetBPP());

			Byte *buffer=Data.GetBuffer();
			Image::Lzma.Decode(gfile, buffer, CompressionProps, Data.GetSize());

			utils::CheckAndDeleteArray(CompressionProps);
		}
		else if(this->Compression==GID::PNG) {
			Mode=graphics::ColorMode::ARGB;

			this->Data.Resize(this->GetWidth()*this->GetHeight()*4);

			std::vector<Byte> imagedata;
			encoding::Info inf;
			try {
				inf=encoding::Png.Decode(gfile, imagedata);
			}
			catch(...) {
				return false;
			}

			if(inf.Alpha) {
				for(int y=0;y<inf.Height;y++) {
					for(int x=0;x<inf.Width;x++) {
						Data[(x+y*inf.Width)*4+2]=imagedata[(y*inf.RowBytes)+x*4+0];
						Data[(x+y*inf.Width)*4+1]=imagedata[(y*inf.RowBytes)+x*4+1];
						Data[(x+y*inf.Width)*4+0]=imagedata[(y*inf.RowBytes)+x*4+2];
						Data[(x+y*inf.Width)*4+3]=imagedata[(y*inf.RowBytes)+x*4+3];
					}
				}
			}
			else {
				for(int y=0;y<inf.Height;y++) {
					for(int x=0;x<inf.Width;x++) {
						Data[(x+y*inf.Width)*4+2]=imagedata[(y*inf.RowBytes)+x*3+0];
						Data[(x+y*inf.Width)*4+1]=imagedata[(y*inf.RowBytes)+x*3+1];
						Data[(x+y*inf.Width)*4+0]=imagedata[(y*inf.RowBytes)+x*3+2];
						Data[(x+y*inf.Width)*4+3]=255;
					}
				}
			}
		}
		else if(Compression==0) {
			this->Data.Resize(size);
			gfile.read((char*)this->Data.GetBuffer(), size);
		}

		this->isLoaded=true;

		return true;
	}

	void Image::LoadProperties(std::istream &in, int size) {
		int w, h;
		ColorMode::Type m;

		ReadFrom(in,w);
		ReadFrom(in,h);
		ReadFrom(in,m);
		Compression=(Image::Compressor)ReadFrom<int>(in);

		Resize(w,h,m);

		if(size>16) {
			LateLoading=ReadFrom<char>(in)!=0;
		}

		if(size!=17)
			in.seekg(size-17,ios::cur);
	}

	Image::ImageReadError Image::Import(std::string filename) {
		if(filename.substr(filename.length()-4)==".png" || filename.substr(filename.length()-4)==".PNG" ) {
			return ImportPNG(filename);
		}

		return Image::FileNotFound;
	}



	encoding::LZMA Image::Lzma(false);


	NullImage *NullImage::ni;
} }
