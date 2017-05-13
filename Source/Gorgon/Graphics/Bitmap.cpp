#include "Bitmap.h"

#include <fstream>
#include <algorithm>
#include <numeric>

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

		if(GetMode()!=Graphics::ColorMode::RGB				&&
		   GetMode()!=Graphics::ColorMode::RGBA				&&
		   GetMode()!=Graphics::ColorMode::Grayscale		&&
		   GetMode()!=Graphics::ColorMode::Grayscale_Alpha  &&
		   GetMode()!=Graphics::ColorMode::Alpha)
			throw std::runtime_error("Unsupported color mode");

		std::ofstream file(filename, std::ios::binary);
		if(!file.is_open())
			return false;

		Encoding::Png.Encode(*data, file);

		return true;
	}

	bool Bitmap::ExportPNG(std::ostream &out) {
		ASSERT(data, "Image data does not exists");

		if(GetMode()!=Graphics::ColorMode::RGB				&&
		   GetMode()!=Graphics::ColorMode::RGBA				&&
		   GetMode()!=Graphics::ColorMode::Grayscale		&&
		   GetMode()!=Graphics::ColorMode::Grayscale_Alpha  &&
		   GetMode()!=Graphics::ColorMode::Alpha)
			throw std::runtime_error("Unsupported color mode");

		Encoding::Png.Encode(*data, out);

		return true;
	}

	bool Bitmap::ExportBMP(const std::string &filename) {
		ASSERT(data, "Image data does not exists");

		return data->ExportBMP(filename);
	}

	bool Bitmap::ExportBMP(std::ostream &out) {
		ASSERT(data, "Image data does not exists");

		return data->ExportBMP(out);
	}

	bool Bitmap::ExportJPG(const std::string &filename, int quality) {
		ASSERT(data, "Image data does not exists");

		if(GetMode()!=Graphics::ColorMode::RGB &&
		   GetMode()!=Graphics::ColorMode::Grayscale)
			throw std::runtime_error("Unsupported color mode");
		
		if(quality < 0 || quality > 100)
			throw std::runtime_error("Unsupported quality");

		std::ofstream file(filename, std::ios::binary);
		if(!file.is_open())
			return false;

		Encoding::Jpg.Encode(*data, file, quality);

		return true;
	}

	bool Bitmap::ExportJPG(std::ostream &out, int quality) {
		ASSERT(data, "Image data does not exists");

		if(GetMode()!=Graphics::ColorMode::RGB &&
		   GetMode()!=Graphics::ColorMode::Grayscale)
			throw std::runtime_error("Unsupported color mode");

		if(quality < 0 || quality > 100)
			throw std::runtime_error("Unsupported quality");

		Encoding::Jpg.Encode(*data, out, quality);

		return true;
	}

	void Bitmap::StripRGB() {
		ASSERT(HasAlpha(GetMode()), "Unsupported color mode");

		int alpha = AlphaIndex(GetMode());

		auto &data = *this->data;

		Containers::Image img(data.GetSize(), ColorMode::Alpha);

		for(int y=0; y<data.GetHeight(); y++) {
			for(int x=0; x<data.GetWidth(); x++) {
				img(x, y, 0) = data(x, y, alpha);
			}
		}

		Assume(img);
	}

	void Bitmap::StripAlpha() {
		if(!HasAlpha(GetMode())) return;

		int alpha = AlphaIndex(GetMode());

		auto &data = *this->data;

		Containers::Image img(data.GetSize(), (ColorMode)((int)GetMode()&~(int)ColorMode::Alpha));

		for(int y=0; y<data.GetHeight(); y++) {
			for(int x=0; x<data.GetWidth(); x++) {
				int cc = 0;
				for(int c=0; c<(int)data.GetBytesPerPixel(); c++) {
					if(c!=alpha) {
						img(x, y, cc++) = data(x, y, c);
					}
				}
			}
		}

		Assume(img);
	}

	std::vector<Geometry::Bounds> Bitmap::CreateLinearAtlas(Containers::Collection<const Bitmap> list, AtlasMargin margins) {
        std::vector<Geometry::Bounds> ret;
        std::map<const Bitmap *, Geometry::Bounds> mapping;
        
        int N = list.GetSize();
        
        if(N == 0) return ret;
        if(N == 1) return {Geometry::Bounds(0, 0, list[0].GetSize())};

		int marginwidth = 0;

		if(margins == Zero) {
			marginwidth = 1;
		}

        auto mode = list[0].GetMode();
        
        int minw = list[0].GetWidth() + marginwidth;
        
        std::vector<const Bitmap *> ordering;
        for(auto &bmp : list) {
            ordering.push_back(&bmp);
            
            if(minw > bmp.GetWidth() + marginwidth)
                minw = bmp.GetWidth() + marginwidth;
        }
        
        //first height based, so the items with similar heights would be next to each other
        //second sort criteria is width as wider items should be packed at start. Items
        //are reverse sorted for performance
        list.Sort([](const Bitmap &l, const Bitmap &r) {
            if(l.GetHeight() == r.GetHeight())
                return l.GetWidth() < r.GetWidth();
            else
                return l.GetHeight() > r.GetHeight();
        });
        
        int totalw = std::accumulate(list.begin(), list.end(), 0, [margins](int l, const Bitmap &r) {
            return l + r.GetWidth() + (margins == Zero ? 1 : 0);
        });
        
        int avgw = (int)std::ceil(float(totalw) / N);
        
        // average line height would be skewed towards high
        int avgh = list[int(N*2/3)].GetHeight();
		if(margins == Zero)
			avgh += 1;
        
        int lines = (int)std::round(std::sqrt(float(N)/avgh*avgw));
        
        int x = 0, y = 0, maxy = 0;
        int w = (int)std::ceil((float)totalw/lines);
        w += minw;
        int maxx = 0;

		
		if(margins == Zero) {
			x = marginwidth;
			y = marginwidth;
		}
        
        //build positions
        for(int i=0; i<N; i++) {
            auto &bmp = *list.Last();
            if(x + bmp.GetWidth() + marginwidth < w) {
                auto size = bmp.GetSize();
                
                mapping[&bmp] = {x, y, size};
                
                x += size.Width + marginwidth;
                
                if(size.Height > maxy) maxy = size.Height;
                
                list.Remove(list.GetSize()-1);
            }
            else {
                bool found = false;
                
                if(w - x > minw) { //search to find a smaller image
                    
                    int maxw = w - x - 1 - marginwidth;
                    
                    for(auto it = list.Last(); it.IsValid(); it.Previous()) {
                        auto size = it->GetSize();
                        
                        if(size.Width <= maxw) {
                            mapping[it.CurrentPtr()] = {x, y, size};
                            
                            x += size.Width + marginwidth;
                            
                            if(size.Height > maxy) maxy = size.Height;
                            
                            it.Remove();
                            
                            found = true;
                            break;
                        }
                    }
                }
                
                if(!found) { //too small
                    y += maxy + marginwidth;
                    maxy = 0;
                    
                    if(maxx < x) maxx = x;
                    
					if(margins == Zero)
						x = 1;
					else
						x = 0;

                    i--;
                }
            }
        }
        
        if(x) {
            y += maxy;
        }
        
        Resize({maxx, y + marginwidth}, mode);

		if(margins == Zero) {
			data->Clear();
		}
        
        //channel count
        int C = GetBytesPerPixel();
        
        for(auto p : mapping) {
            auto b = p.second;
            
            if(p.first->GetMode() != mode)
                throw std::runtime_error("Color modes are not uniform in atlas generation");
            
            for(int y=0; y<b.Height(); y++) {
                for(int x=0; x<b.Width(); x++) {
                    for(int c=0; c<C; c++) {
                        (*data)(x+b.Left, y+b.Top, c) = (*p.first)(x, y, c);
                    }
                }
            }
        }
        
        ret.reserve(N);
        for(int i=0; i<N; i++) {
            ret.push_back(mapping[ordering[i]]);
        }

		//ExportPNG("atlas.png");
        
        return ret;
    }
    
    std::vector<TextureImage> Bitmap::CreateAtlasImages(std::vector<Geometry::Bounds> boundaries) const {
		if(GetID() == 0) throw std::runtime_error("Cannot map atlas from an unprepared image");
        
        std::vector<TextureImage> ret;
        ret.reserve(boundaries.size());
        
        for(auto b : boundaries) {
            ret.emplace_back(GetID(), GetMode(), GetSize(), b);
        }
        
        return ret;
    }

	Geometry::Margin Bitmap::Trim(bool left, bool top, bool right, bool bottom) {
		ASSERT(data, "Image data does not exists");

		ASSERT(HasAlpha(GetMode()), "Unsupported color mode");

		Geometry::Margin ret(0, 0, 0, 0);

		int alpha = AlphaIndex(GetMode());

		auto &data = *this->data;

		if(left) {
			for(int x=0; x<data.GetWidth(); x++) {
				bool empty = true;

				for(int y=0; y<data.GetHeight(); y++) {
					if(data(x, y, alpha) != 0) {
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
			for(int y=0; y<data.GetHeight(); y++) {
				bool empty = true;

				for(int x=0; x<data.GetWidth(); x++) {
					if(data(x, y, alpha) != 0) {
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
			for(int x=data.GetWidth()-1; x>=0; x--) {
				bool empty = true;

				for(int y=0; y<data.GetHeight(); y++) {
					if(data(x, y, alpha) != 0) {
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
			for(int y=data.GetHeight()-1; y>=0; y--) {
				bool empty = true;

				for(int x=0; x<data.GetWidth(); x++) {
					if(data(x, y, alpha) != 0) {
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

		if(ret.TotalX() == 0 && ret.TotalY() == 0) return ret;

		if(ret.TotalX() >= data.GetWidth() || ret.TotalY() >= data.GetHeight()) 
            return ret;

		Containers::Image img(data.GetSize() - ret.Total(), GetMode());

		int yy=0;
		for(int y=ret.Top; y<data.GetHeight() - ret.Bottom; y++) {
			int xx=0;
			for(int x=ret.Left; x<data.GetWidth() - ret.Right; x++) {
				for(int c=0; c<(int)data.GetBytesPerPixel(); c++) {
					img(xx, yy, c) = data(x, y, c);
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
			ASSERT_DUMP(false, "No data to release");
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

	Graphics::Bitmap Bitmap::Rotate90() const {
		ASSERT(data, "Bitmap data is not set");

		Graphics::Bitmap target(GetHeight(), GetWidth(), GetMode());

		int h = target.GetHeight();
		ForAllPixels([&](int x, int y, int c) {
			target(y, h - x - 1, c) = (*this)(x, y, c);
		});

		return target;
	}

	Graphics::Bitmap Bitmap::Rotate180() const {
		ASSERT(data, "Bitmap data is not set");

		Graphics::Bitmap target(GetSize(), GetMode());

		int h = target.GetHeight();
		int w = target.GetWidth();
		ForAllPixels([&](int x, int y, int c) {
			target(w - x - 1, h - y - 1, c) = (*this)(x, y, c);
		});

		return target;
	}

	Graphics::Bitmap Bitmap::Rotate270() {
		ASSERT(data, "Bitmap data is not set");

		Graphics::Bitmap target(GetHeight(), GetWidth(), GetMode());

		int w = target.GetWidth();
		ForAllPixels([&](int x, int y, int c) {
			target(w - y - 1, x, c) = (*this)(x, y, c);
		});

		return target;
	}


} }
