#include <fstream>
#include "Bitmap.h"

#include "../Encoding/PNG.h"
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

	bool Bitmap::ImportPNG(const std::string &filename) {
		std::ifstream file(filename, std::ios::binary);

		if(!file.is_open() || !file.good()) return false;

		Destroy();

		data=new Containers::Image();
		Encoding::Png.Decode(file, *data);

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
