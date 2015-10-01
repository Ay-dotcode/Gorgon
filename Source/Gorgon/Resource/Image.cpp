#include "Image.h"
#include "File.h"
#include "../Encoding/PNG.h"

namespace Gorgon { namespace Resource {

	void Image::Prepare() {
		if(data) {
			Graphics::Texture::Set(*data);
		}
	}

	void Image::Discard() {
		if(data) {
			delete data;
			data=nullptr;
		}
	}

	bool Image::ImportPNG(const std::string &filename) {
		std::ifstream file(filename, std::ios::binary);

		if(!file.is_open() || !file.good()) return false;

		Destroy();

		data=new Containers::Image();
		Encoding::Png.Decode(file, *data);

		if(reader) {
			reader->NoLongerNeeded();
			reader.reset();
		}

		isloaded=true;

		return true;
	}

	Image *Image::LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size) {
		auto image=new Image;

		image->isloaded=false;

		if(!image->load(reader, size, false)) {
			delete image;
			return nullptr;
		}

		return image;
	}

	bool Image::Load() {
		if(isloaded)			return true;
		if(!reader)				return false;
		if(!reader->TryOpen())	return false;


		reader->Seek(entrypoint-4);

		auto size=reader->ReadChunkSize();

		auto ret=load(reader, size, true);

		if(ret && isloaded) {
			reader->NoLongerNeeded();
			reader.reset();
		}

		return ret;
	}

	bool Image::load(std::shared_ptr<Reader> reader, unsigned long totalsize, bool forceload) {

		auto target = reader->Target(totalsize);

		entrypoint = reader->Tell();


		bool load=false;
		auto width=0;
		auto height=0;
		auto mode=Graphics::ColorMode::Invalid;
		GID::Type compression;


		while(!target) {
			auto gid = reader->ReadGID();
			auto size= reader->ReadChunkSize();

			if(gid==GID::Image_Props) {
				width=reader->ReadInt32();
				height=reader->ReadInt32();
				mode=reader->ReadEnum32<Graphics::ColorMode>();
				compression=reader->ReadGID();

				load=!reader->ReadBool() || forceload;

				if(!load) {
					reader->KeepOpen();
					this->reader=reader;
				}

				//for future compatibility
				reader->EatChunk(size-20);
			}
			else if(gid==GID::Image_Data) {
				if(load) {
					Destroy();
					this->data=new Containers::Image({width, height}, mode);

					if(this->data->GetTotalSize() != size) {
						throw std::runtime_error("Image size mismatch");
					}

					reader->ReadArray(this->data->RawData(), size);

					isloaded=true;
				}
				else {
					reader->EatChunk(size);
				}
			}
			else if(gid==GID::Image_Cmp_Data) {
				if(load) {
					Destroy();
					data=new Containers::Image();

					if(compression==GID::PNG) {
						Encoding::Png.Decode(reader->GetStream(), *data);
					}
					else {
						throw LoadError(LoadError::Unknown, "Unknown compression type.");
					}

					if(this->data->GetMode()!=mode || this->data->GetSize()!=Geometry::Size{width, height}) {
						throw std::runtime_error("Image size or mode mismatch");
					}

					isloaded=true;
				}
				else {
					reader->EatChunk(size);
				}
			}
			else if(gid==GID::Image_Cmp_Props) {
				if(size>4) {
					throw LoadError(LoadError::VersionMismatch, "Old version image compression");
				}
				else {
					reader->EatChunk(size);
				}
			}
			else {
				if(!reader->ReadCommonChunk(*this, gid, size)) {
					Utils::ASSERT_FALSE("Unknown chunk: "+String::From(gid));
					reader->EatChunk(size);
				}
			}
		}

		return true;
	}
	
	void Image::save(Writer &writer) {
		ASSERT(data, "Image data does not exists");
		ASSERT(GetMode()==Graphics::ColorMode::RGBA, "Unsupported color mode");
		
		auto start = writer.WriteObjectStart(this);
		
		auto propstart = writer.WriteChunkStart(GID::Image_Props);
		writer.WriteInt32(size.Width);
		writer.WriteInt32(size.Height);
		writer.WriteEnum32(GetMode());
		writer.WriteGID(compression);
		writer.WriteBool(false);
		writer.WriteEnd(propstart);
		
		if(compression==GID::None) {
			writer.WriteChunkHeader(GID::Image_Data, data->GetTotalSize());
			writer.WriteArray(data->RawData(), data->GetTotalSize());
		}
		else if(compression==GID::PNG) {
			auto cmpdat = writer.WriteChunkStart(GID::Image_Cmp_Data);
			Encoding::Png.Encode(*data, writer.GetStream());
			writer.WriteEnd(cmpdat);
		}
		else {
			throw std::runtime_error("Unknown compression mode: "+String::From(compression));
		}
		
		writer.WriteEnd(start);
	}

	bool Image::ExportPNG(const std::string &filename) {
		ASSERT(data, "Image data does not exists");

		ASSERT(GetMode()==Graphics::ColorMode::RGBA, "Unsupported color mode");

		std::ofstream file(filename, std::ios::binary);
		if(!file.is_open())
			return false;

		Encoding::Png.Encode(*data, file);

		return true;
	}

} }



#ifdef asdfasdfasdf
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
#include "../Encoding/JPEG.h"

#undef max

using namespace gge::resource;
using namespace gge::graphics;
using namespace gge::utils;
using namespace std;

namespace gge { namespace resource {



	void Image::Prepare(GGEMain &main, resource::File &file) {
		Prepare();
	}

	void Image::Prepare() {
#ifdef _DEBUG
		if(!Data.GetSize()) {
			os::DisplayMessage("Image Resource","Data is not loaded yet.");
			assert(0);
		}
#endif

		if(Texture.ID>0) {
			if(ImageData::GetWidth()==Texture.W && ImageData::GetHeight()==Texture.H)
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
		if(GetMode()!=graphics::ColorMode::ARGB) {
			throw std::runtime_error("Unsupported color mode");
		}
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

		encoding::PNG::Info inf;
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
			if(this->Data.GetSize()==0) return false;

			std::vector<Byte> imagedata;
			encoding::PNG::Info inf;
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
		else if(filename.substr(filename.length()-5)==".JPEG" || filename.substr(filename.length()-5)==".jpeg" || filename.substr(filename.length()-4)==".jpg" || filename.substr(filename.length()-4)==".JPG" ) {
			return ImportJPEG(filename);
		}

		return Image::UnknownImageType;
	}

	Image::ImageReadError Image::ImportJPEG(std::string filename) {

		std::ifstream file(filename, ios::binary);

		if(!file.is_open())
			return FileNotFound;

		std::vector<Byte> imagedata;

		encoding::JPEG::Info inf;
		try {
			inf=encoding::Jpg.Decode(file, imagedata);
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
			if(inf.Color) {
				for(int y=0;y<inf.Height;y++) {
					for(int x=0;x<inf.Width;x++) {
						Data[(x+y*inf.Width)*4+2]=imagedata[(y*inf.RowBytes)+x*3+0];
						Data[(x+y*inf.Width)*4+1]=imagedata[(y*inf.RowBytes)+x*3+1];
						Data[(x+y*inf.Width)*4+0]=imagedata[(y*inf.RowBytes)+x*3+2];
						Data[(x+y*inf.Width)*4+3]=255;
					}
				}
			}
			else {
				for(int y=0;y<inf.Height;y++) {
					for(int x=0;x<inf.Width;x++) {
						Data[(x+y*inf.Width)*4+2]=imagedata[(y*inf.RowBytes)+x];
						Data[(x+y*inf.Width)*4+1]=imagedata[(y*inf.RowBytes)+x];
						Data[(x+y*inf.Width)*4+0]=imagedata[(y*inf.RowBytes)+x];
						Data[(x+y*inf.Width)*4+3]=255;
					}
				}
			}
		}

		isLoaded=true;

		return NoError;
	}

	gge::utils::Margins Image::Trim(bool left, bool right, bool top, bool bottom) {
		gge::utils::Margins margins(0, 0, 0, 0);
		int xx = 0, yy = 0;
		if(Mode != graphics::ColorMode::ABGR && Mode != graphics::ColorMode::ARGB)
			return margins;

		if(left) {
			auto x = Data.begin() + 3;
			auto y = x;
			for(; y != Data.end(); y += (Width * 4)) {
				if(*y != 0)
					break;

				yy++;
				if(yy == Height) {
					x += 4;
					y = x;
					margins.Left++;
					yy = 0;
				}
				if(margins.Left == Width - 1)
					break;

			}
		}

		if(right) {
			yy = Height - 1;
			auto x = Data.end() - 1;
			auto y = x;
			for(; y != Data.begin(); y -= (Width * 4)) {
				if(*y != 0)
					break;

				yy--;
				if(yy == 0) {
					x -= 4;
					y = x;
					margins.Right++;
					yy = Height - 1;
				}

				if(margins.Right == Width - 1)
					break;
			}
		}

		if(top) {
			xx = 0;
			auto x = Data.begin() + 3;
			for(; x != Data.end(); x += 4) {
				if(*x != 0)
					break;

				xx++;
				if(xx == Width - 1) {
					margins.Top++;
					xx = 0;
				}

				if(margins.Top == Height - 1)
					break;
			}
		}

		if(bottom) {
			xx = Width - 1;
			auto x = Data.end() - 1;
			for(; x != Data.begin(); x -= 4) {
				if(*x != 0) 
					break;

				xx--;
				if(xx == 0) {
					margins.Bottom++;
					xx = Width - 1;
				}

				if(margins.Bottom == Height - 1)
					break;
			}
		}

		gge::utils::CastableManagedBuffer<Byte> buffer;
		int h = Height - (margins.Top + margins.Bottom);
		int w = Width - (margins.Left + margins.Right);

		if(h<=0) {
			h=1;
			margins.Top=0;
			margins.Bottom=Height-1;
		}
		if(w<=0) {
			w=1;
			margins.Left=0;
			margins.Right=Width-1;
		}

		buffer.Resize(w * h * GetBPP());
		auto x = (Data.begin() + (GetBPP() * (margins.Top*Width+margins.Left)));
		auto rightinc = margins.Right * GetBPP();
		auto leftinc =  margins.Left * GetBPP();
		auto end = x + ((Width * h * GetBPP()) - (leftinc + rightinc));
		xx = 0;


		int index = 0;
		int jumppoint=w*GetBPP();
		int leap=leftinc + rightinc;
		for(; x < end; ++x) {
			buffer[index++] = *x;
			xx++;
			if(xx == jumppoint) {
				x += leap;
				xx = 0;
			}
		}

		ImageData::Data=buffer;
		ImageData::Width=w;
		ImageData::Height=h;

		return margins;
	}

	std::vector<utils::Bounds> Image::CreateLinearAtlas(utils::OrderedCollection<Image> list) {
		std::vector<utils::Bounds> ret;

		if(list.GetCount()==0)
			return ret;

		int maxh=0, totalw=0;
		for(auto &im : list) {
			int w=im.GetWidth();
			int h=im.GetHeight();

			if(h>maxh) maxh=h;

			totalw+=w;
		}

		//determine number of lines
		int lines=int(utils::Round(std::sqrt(float(totalw*maxh))/maxh));
		if(lines==0) lines=1;

		std::vector<int> linepos(lines);

		//determine image width
		for(auto &im : list) {
			int min=std::numeric_limits<int>::max();
			int minindex=0;
			for(int i=0;i<lines;i++) {
				if(linepos[i]<min) {
					min=linepos[i];
					minindex=i;
				}
			}

			linepos[minindex]+=im.GetWidth();
		}

		int width=utils::Max(linepos.begin(), linepos.end());
		int height=lines*maxh;

		std::fill(linepos.begin(), linepos.end(), 0);

		Resize(width, height, list[0].GetMode());

		//place images
		for(auto &im : list) {
			int min=std::numeric_limits<int>::max();
			int minindex=0;
			for(int i=0;i<lines;i++) {
				if(linepos[i]<min) {
					min=linepos[i];
					minindex=i;
				}
			}

			//copy
			int w=im.GetWidth();
			int h=im.GetHeight();
			int xx=linepos[minindex];
			int yy=minindex*maxh;
			int tx=xx+w;
			int ty=yy+h;
			int B=GetBPP();
			int sind=0; //source index
			int tind; //target index
			int stride=w*B;
			auto tdata=Data.GetBuffer(); //target data
			auto sdata=im.getdata().GetBuffer(); //source data

			ret.emplace_back(xx, yy, tx, ty);

			for(int y=yy;y<ty;y++) {
				tind=y*width*B+xx*B;
				for(int i=0;i<stride;i++) {
					tdata[tind++]=sdata[sind++];
				}
			}

			linepos[minindex]+=w;
		}
		

		return ret;
	}

	utils::OrderedCollection<Image> Image::CreateAtlasImages(const std::vector<utils::Bounds> &boundaries) const {
		if(Texture.ID<=0) throw std::runtime_error("Cannot map atlas from an unprepared image");

		utils::OrderedCollection<Image> ret;

		float fullwidth =(float)GetWidth();
		float fullheight=(float)GetHeight();
		auto  textureid =Texture.ID;
		for(auto boundary : boundaries) {
			Image *img=new Image;

			img->parent=this;
			img->Texture.ID=textureid;

			img->Texture.ImageCoord[0].s=
			img->Texture.ImageCoord[3].s= boundary.Left/fullwidth;

			img->Texture.ImageCoord[1].s=
			img->Texture.ImageCoord[2].s= boundary.Right/fullwidth;

			img->Texture.ImageCoord[0].t=
			img->Texture.ImageCoord[1].t= boundary.Top/fullheight;

			img->Texture.ImageCoord[2].t=
			img->Texture.ImageCoord[3].t= boundary.Bottom/fullheight;

			img->Texture.W=boundary.Width();
			img->Texture.H=boundary.Height();

			img->isLoaded=true;
			img->file=file;
			img->LeaveData=false;

			ret.Add(img);
		}

		return ret;
	}



	encoding::LZMA Image::Lzma(false);


	NullImage *NullImage::ni;
} }
#endif