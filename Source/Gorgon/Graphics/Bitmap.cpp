#include <fstream>
#include "Bitmap.h"

#include "../Encoding/PNG.h"
#include "../Encoding/JPEG.h"
#include "../IO/Stream.h"
#include "../Utils/Assert.h"

namespace Gorgon { namespace Graphics {
	
	void Bitmap::Prepare() {
		if(data) {
			Graphics::Texture::Set(*data);
		}
	}

	void Bitmap::Discard() {
		if(data) {
			delete data;
			data=nullptr;
		}
	}

	bool Bitmap::Import(const std::string &filename) {
		auto dotpos = filename.find_last_of('.');
		if(dotpos!=-1) {
			auto ext = filename.substr(dotpos+1);

			if(String::ToLower(ext) == "png") {
				return ImportPNG(filename);
			}
			else if(String::ToLower(ext) == "jpg" || String::ToLower(ext) =="jpeg") {
				return ImportJPEG(filename);
			}
			else if(String::ToLower(ext) == "bmp") {
				return ImportBMP(filename);
			}
		}

		std::ifstream file(filename, std::ios::binary);

		static const uint32_t pngsig  = 0x474E5089;
		static const uint32_t jpgsig1 = 0xE0FFD8FF;
		static const uint32_t jpgsig2 = 0xE1FFD8FF;
		static const uint32_t bmpsig  = 0x4D42;

		uint32_t sig = IO::ReadUInt32(file);

		file.close();

		if(sig == pngsig) {
			return ImportPNG(filename);
		}
		else if(sig == jpgsig1 || sig == jpgsig2) {
			return ImportJPEG(filename);
		}
		else if((sig&0xffff) == bmpsig) {
			return ImportBMP(filename);
		}

		throw std::runtime_error("Unsuppoted file format");
	}

	bool Bitmap::Import(std::istream &file) {
		static const uint32_t pngsig  = 0x474E5089;
		static const uint32_t jpgsig1 = 0xE0FFD8FF;
		static const uint32_t jpgsig2 = 0xE1FFD8FF;
		static const uint32_t bmpsig  = 0x4D42;

		uint32_t sig = IO::ReadUInt32(file);

		file.seekg(-4, std::ios::cur);

		if(sig == pngsig) {
			return ImportPNG(file);
		}
		else if(sig == jpgsig1 || sig == jpgsig2) {
			return ImportJPEG(file);
		}
		else if((sig&0xffff) == bmpsig) {
			return ImportBMP(file);
		}

		throw std::runtime_error("Unsuppoted file format");
	}

	bool Bitmap::ImportPNG(const std::string &filename) {
		std::ifstream file(filename, std::ios::binary);

		if(!file.is_open() || !file.good()) return false;

		Destroy();

		data=new Containers::Image();
		Encoding::Png.Decode(file, *data);

		return true;
	}

	bool Bitmap::ImportJPEG(const std::string &filename) {
		std::ifstream file(filename, std::ios::binary);

		if(!file.is_open() || !file.good()) return false;

		Destroy();

		data=new Containers::Image();
		Encoding::Jpg.Decode(file, *data);

		return true;
	}

	bool Bitmap::ImportBMP(const std::string &filename) {
		std::ifstream file(filename, std::ios::binary);

		if(!file.is_open() || !file.good()) return false;

		Destroy();

		data=new Containers::Image();
		return data->ImportBMP(filename);
	}

	bool Bitmap::ImportPNG(std::istream &file) {
		Destroy();

		data=new Containers::Image();
		Encoding::Png.Decode(file, *data);

		return true;
	}

	bool Bitmap::ImportJPEG(std::istream &file) {
		Destroy();

		data=new Containers::Image();
		Encoding::Jpg.Decode(file, *data);

		return true;
	}

	bool Bitmap::ImportBMP(std::istream &file) {
		Destroy();

		data=new Containers::Image();
		return data->ImportBMP(file);
	}

	bool Bitmap::ExportPNG(const std::string &filename) {
		ASSERT(data, "Image data does not exists");

		ASSERT(GetMode()==Graphics::ColorMode::RGB				|| 
			   GetMode()==Graphics::ColorMode::RGB				|| 
			   GetMode()==Graphics::ColorMode::Grayscale		||
			   GetMode()==Graphics::ColorMode::Grayscale_Alpha, 
			   "Unsupported color mode");

		std::ofstream file(filename, std::ios::binary);
		if(!file.is_open())
			return false;

		Encoding::Png.Encode(*data, file);

		return true;
	}
	
	void Bitmap::StripRGB() {
		ASSERT(HasAlpha(GetMode()), "Unsupported color mode");

		int alpha = AlphaIndex(GetMode());

		auto &data = *this->data;

		Containers::Image img(data.GetSize(), ColorMode::Alpha);

		for(int y=0; y<data.GetSize().Height; y++) {
			for(int x=0; x<data.GetSize().Width; x++) {
				img({x, y}, 0) = data({x, y}, alpha);
			}
		}

		Assume(img);
	}

	void Bitmap::StripAlpha() {
		if(!HasAlpha(GetMode())) return;

		int alpha = AlphaIndex(GetMode());

		auto &data = *this->data;

		Containers::Image img(data.GetSize(), (ColorMode)((int)GetMode()&~(int)ColorMode::Alpha));

		for(int y=0; y<data.GetSize().Height; y++) {
			for(int x=0; x<data.GetSize().Width; x++) {
				int cc = 0;
				for(int c=0; c<(int)data.GetBytesPerPixel(); c++) {
					if(c!=alpha) {
						img({x, y}, cc++) = data({x, y}, c);
					}
				}
			}
		}

		Assume(img);
	}


	Geometry::Margins Bitmap::Trim(bool left, bool top, bool right, bool bottom) {
		ASSERT(data, "Image data does not exists");

		ASSERT(HasAlpha(GetMode()), "Unsupported color mode");

		Geometry::Margins ret(0, 0, 0, 0);

		int alpha = AlphaIndex(GetMode());

		auto &data = *this->data;

		if(left) {
			for(int x=0; x<data.GetSize().Width; x++) {
				bool empty = true;

				for(int y=0; y<data.GetSize().Height; y++) {
					if(data({x, y}, alpha) != 0) {
						empty = false;
						break;
					}
				}

				if(empty) {
					ret.Left++;
				}
				else {
					break;
				}
			}
		}

		if(top) {
			for(int y=0; y<data.GetSize().Height; y++) {
				bool empty = true;

				for(int x=0; x<data.GetSize().Width; x++) {
					if(data({x, y}, alpha) != 0) {
						empty = false;
						break;
					}
				}

				if(empty) {
					ret.Top++;
				}
				else {
					break;
				}
			}
		}

		if(right) {
			for(int x=data.GetSize().Width-1; x>=0; x--) {
				bool empty = true;

				for(int y=0; y<data.GetSize().Height; y++) {
					if(data({x, y}, alpha) != 0) {
						empty = false;
						break;
					}
				}

				if(empty) {
					ret.Right++;
				}
				else {
					break;
				}
			}
		}

		if(bottom) {
			for(int y=data.GetSize().Height-1; y>=0; y--) {
				bool empty = true;

				for(int x=0; x<data.GetSize().Width; x++) {
					if(data({x, y}, alpha) != 0) {
						empty = false;
						break;
					}
				}

				if(empty) {
					ret.Bottom++;
				}
				else {
					break;
				}
			}
		}

		if(ret.Total().Area() == 0) return ret;

		if(ret.TotalX() >= data.GetSize().Width || ret.TotalY() >= data.GetSize().Height) return ret;

		Containers::Image img(data.GetSize() - ret.Total(), GetMode());

		int yy=0;
		for(int y=ret.Top; y<data.GetSize().Height - ret.Bottom; y++) {
			int xx=0;
			for(int x=ret.Left; x<data.GetSize().Width - ret.Right; x++) {
				for(int c=0; c<(int)data.GetBytesPerPixel(); c++) {
					img({xx, yy}, c) = data({x, y}, c);
				}
				xx++;
			}
			yy++;
		}

		Assume(img);

		return ret;
	}

	Containers::Image Bitmap::ReleaseData() {
		if(data==nullptr) {
	#ifndef NDEBUG
			throw std::runtime_error("No data to release");
	#endif

			return { };
		}
		else {
			Containers::Image temp=std::move(*data);

			delete data;
			data=nullptr;

			return temp;
		}
	}
	
	
} }
