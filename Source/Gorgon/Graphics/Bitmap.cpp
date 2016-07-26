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
        }
        
        std::ifstream file(filename, std::ios::binary);
        
        static const uint32_t pngsig  = 0x474E5089;
        static const uint32_t jpgsig1 = 0xE0FFD8FF;
        static const uint32_t jpgsig2 = 0xE1FFD8FF;
        
        uint32_t sig = IO::ReadUInt32(file);
        
        file.close();
        
        if(sig == pngsig) {
            return ImportPNG(filename);
        }
        else if(sig == jpgsig1 || sig == jpgsig2) {
            return ImportJPEG(filename);
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
	
	bool Bitmap::ExportPNG(const std::string &filename) {
		ASSERT(data, "Image data does not exists");

		ASSERT(GetMode()==Graphics::ColorMode::RGBA, "Unsupported color mode");

		std::ofstream file(filename, std::ios::binary);
		if(!file.is_open())
			return false;

		Encoding::Png.Encode(*data, file);

		return true;
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
