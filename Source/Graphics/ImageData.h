#pragma once

#include <vector>

#include "../Types.h"
#include "../Graphics.h"
#include "Color.h"

namespace Gorgon {
	namespace Graphics {

		class ImageData {
		public:
			ImageData() : width(0), height(0), mode(ColorMode::Invalid), bpp(0) {
				
			}

			ImageData(unsigned width, unsigned height, ColorMode mode=ColorMode::RGBA) : width(width), height(height), mode(mode) {
				bpp=Graphics::GetBytesPerPixel(mode);
				data=(Byte*)malloc(width*height*bpp*sizeof(Byte));
			}

			ImageData(const ImageData &) = delete;

			ImageData(ImageData &&data) : ImageData() {
				Swap(data);
			}

			ImageData &operator=(const ImageData &) = delete;

			ImageData &operator=(ImageData &&other) { 
				Destroy();
				Swap(other);
			}

			~ImageData() {
				Destroy();
			}

			virtual void Resize(unsigned width, unsigned height, ColorMode mode) {
				this->width  = width;
				this->height = height;
				this->mode   = mode;
				this->bpp    = Graphics::GetBytesPerPixel(mode);

				if(data) {
					free(data);
				}
				data=(Byte*)malloc(width*height*bpp*sizeof(Byte));
			}

			void Destroy() {
				if(data) {
					free(data);
					data=nullptr;
				}
				width  = 0;
				height = 0;
				bpp    = 0;
				mode   = ColorMode::Invalid;
			}

			void Swap(ImageData &other) {
				using std::swap;

				swap(width,  other.width);
				swap(height, other.height);
				swap(bpp,    other.bpp);
				swap(data,   other.data);
			}


			Byte &operator()(unsigned x, unsigned y, unsigned component=0) {
				return data[bpp*(width*y+x)+component];
			}

			Byte operator()(unsigned x, unsigned y, unsigned component=0) const {
				return data[bpp*(width*y+x)+component];
			}


			unsigned GetWidth() const {
				return width;
			}

			unsigned GetHeight() const {
				return height;
			}

			ColorMode GetMode() const {
				return mode;
			}

			unsigned GetBytesPerPixel() const {
				return bpp;
			}

			Byte *RawData() { return data; }

		protected:
			Byte *data=nullptr;
			unsigned width, height;
			unsigned bpp;
			ColorMode mode;
		};

		void swap(ImageData &l, ImageData &r) {
			l.Swap(r);
		}


	}
}
