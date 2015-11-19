#pragma once

#include <vector>

#include "../Types.h"
#include "../Geometry/Point.h"
#include "../Geometry/Size.h"
#include "../Graphics/Color.h"

namespace Gorgon {
	namespace Containers {

		/// This class is a container for image data. It supports different color modes and access to the
		/// underlying data through () operator. This object implements move semantics. Since copy constructor is
		/// expensive, it is deleted against accidental use. If a copy of the object is required, use Duplicate function.
		class Image {
		public:

			/// Constructs an empty image data
			Image() {
				
			}

			/// Constructs a new image data with the given width, height and color mode. This constructor 
			/// does not initialize data inside the image
			Image(const Geometry::Size &size, Graphics::ColorMode mode) : size(size), mode(mode) {
				bpp=Graphics::GetBytesPerPixel(mode);
				data=(Byte*)malloc(size.Area()*bpp*sizeof(Byte));
			}

			/// Copy constructor is disabled
			Image(const Image &) = delete;

			/// Move constructor
			Image(Image &&data) : Image() {
				Swap(data);
			}

			/// Copy assignment is disabled
			Image &operator=(const Image &) = delete;

			/// Move assignment
			Image &operator=(Image &&other) { 
				if(this == &other) return *this;
				
				Destroy();
				Swap(other);
				
				return *this;
			}

			/// Destructor
			~Image() {
				Destroy();
			}

			/// Duplicates this image, essentially performing the work of copy constructor
			Image Duplicate() const {
				Image data;
				data.Assign(this->data, size, mode);

				return data;
			}

			/// Resizes the image to the given size and color mode. This function discards the contents
			/// of the image and does not perform any initialization.
			void Resize(const Geometry::Size &size, Graphics::ColorMode mode) {
#ifndef NDEBUG
				if(!size.IsValid())
					throw std::runtime_error("Image size cannot be negative");
#endif

				// Check if resize is really necessary
				if(this->size==size && this->bpp==Graphics::GetBytesPerPixel(mode))
					return;

				this->size   = size;
				this->mode   = mode;
				this->bpp    = Graphics::GetBytesPerPixel(mode);

				if(data) {
					free(data);
				}
				
				data=(Byte*)malloc(size.Area()*bpp*sizeof(Byte));
			}

			/// Assigns the image to the copy of the given data. Ownership of the given data
			/// is not transferred. If the given data is not required elsewhere, consider using
			/// Assume function. This variant performs resize and copy at the same time. The given 
			/// data should have the size of width*height*Graphics::GetBytesPerPixel(mode)*sizeof(Byte). 
			/// This function does not perform any checks for the data size while copying it.
			/// If width or height is 0, the newdata is not accessed and this method effectively
			/// Destroys the current image. In this case, both width and height should be specified as 0.
			void Assign(Byte *newdata, const Geometry::Size &size, Graphics::ColorMode mode) {
#ifndef NDEBUG
				if(!size.IsValid())
					throw std::runtime_error("Image size cannot be negative");
#endif
				this->size   = size;
				this->mode   = mode;
				this->bpp    = Graphics::GetBytesPerPixel(mode);

				if(data && data!=newdata) {
					free(data);
				}
				
				if(size.Area()*bpp>0) {
					data=(Byte*)malloc(size.Area()*bpp*sizeof(Byte));
					memcpy(data, newdata, size.Area()*bpp*sizeof(Byte));
				}
				else {
					data=nullptr;
				}
			}

			/// Assigns the image to the copy of the given data. Ownership of the given data
			/// is not transferred. If the given data is not required elsewhere, consider using
			/// Assume function. The size and color mode of the image stays the same. The given 
			/// data should have the size of width*height*Graphics::GetBytesPerPixel(mode)*sizeof(Byte). 
			/// This function does not perform any checks for the data size while copying it.
			void Assign(Byte *newdata) {
				memcpy(data, newdata, size.Area()*bpp*sizeof(Byte));
			}

			/// Assumes the ownership of the given data. This variant changes the size and
			/// color mode of the image. The given data should have the size of 
			/// width*height*Graphics::GetBytesPerPixel(mode)*sizeof(Byte). This function
			/// does not perform any checks for the data size while assuming it.
			/// newdata could be nullptr however, in this case
			/// width, height should be 0. mode is not assumed to be ColorMode::Invalid while
			/// the image is empty, therefore it could be specified as any value.
			void Assume(Byte *newdata, const Geometry::Size &size, Graphics::ColorMode mode) {
#ifndef NDEBUG
				if(!size.IsValid())
					throw std::runtime_error("Image size cannot be negative");
#endif
				this->size   = size;
				this->mode   = mode;
				this->bpp    = Graphics::GetBytesPerPixel(mode);

				if(data && data!=newdata) {
					free(data);
				}
				
				data=newdata;
			}

			/// Assumes the ownership of the given data. The size and color mode of the image stays the same.
			/// The given data should have the size of width*height*Graphics::GetBytesPerPixel(mode)*sizeof(Byte).
			/// This function does not perform any checks for the data size while assuming it.
			void Assume(Byte *newdata) {
				if(data && data!=newdata) {
					free(data);
				}
				
				data=newdata;
			}

			/// Returns and disowns the current data buffer. If image is empty, this method will return a nullptr.
			Byte *Release() {
				auto temp=data;
				data=nullptr;
				Destroy();

				return temp;
			}

			/// Cleans the contents of the buffer by setting every byte it contains to 0.
			void Clean() {
#ifndef NDEBUG
				if(!data) {
					throw std::runtime_error("Image data is empty");
				}
#endif

				memset(data, 0, size.Area()*bpp*sizeof(Byte));
			}

			/// Destroys this image by setting width and height to 0 and freeing the memory
			/// used by its data. Also color mode is set to ColorMode::Invalid
			void Destroy() {
				if(data) {
					free(data);
					data=nullptr;
				}
				size   = {0, 0};
				bpp    = 0;
				mode   = Graphics::ColorMode::Invalid;
			}

			/// Swaps this image with another. This function is used to implement move semantics.
			void Swap(Image &other) {
				using std::swap;

				swap(size,   other.size);
				swap(bpp,    other.bpp);
				swap(data,   other.data);
			}

			/// Returns the raw data pointer
			Byte *RawData() {
				return data;
			}

			/// Returns the raw data pointer
			const Byte *RawData() const {
				return data;
			}

			/// Provides access to the given component in x and y coordinates. This
			/// function performs bounds checking only on debug mode.
			Byte &operator()(const Geometry::Point &p, unsigned component=0) {
#ifndef NDEBUG
				if(p.X<0 || p.Y<0 || p.X>=size.Width || p.Y>=size.Height || component>=bpp) {
					throw std::runtime_error("Index out of bounds");
				}
#endif
				return data[bpp*(size.Width*p.Y+p.X)+component];
			}

			/// Provides access to the given component in x and y coordinates. This
			/// function performs bounds checking only on debug mode.
			Byte operator()(const Geometry::Point &p, unsigned component=0) const {
#ifndef NDEBUG
				if(p.X<0 || p.Y<0 || p.X>=size.Width || p.Y>=size.Height || component>=bpp) {
					throw std::runtime_error("Index out of bounds");
				}
#endif
				return data[bpp*(size.Width*p.Y+p.X)+component];
			}

			/// Provides access to the given component in x and y coordinates. This
			/// function returns 0 if the given coordinates are out of bounds. This
			/// function works slower than the () operator.
			Byte Get(const Geometry::Point &p, unsigned component=0) const {
				if(p.X<0 || p.Y<0 || p.X>=size.Width || p.Y>=size.Height || component>=bpp) {
					return 0;
				}

				return data[bpp*(size.Width*p.Y+p.X)+component];
			}

			/// Returns the size of the image
			Geometry::Size GetSize() const {
				return size;
			}

			/// Total size of this image in bytes
			unsigned long GetTotalSize() const {
				return size.Area()*bpp;
			}

			/// Returns the color mode of the image
			Graphics::ColorMode GetMode() const {
				return mode;
			}

			/// Returns the bytes occupied by a single pixel of this image
			unsigned GetBytesPerPixel() const {
				return bpp;
			}

		protected:
			/// Data that stores pixels of the image
			Byte *data = nullptr;

			/// Width of the image
			Geometry::Size size = {0, 0};

			/// Color mode of the image
			Graphics::ColorMode mode = Graphics::ColorMode::Invalid;

			/// Bytes per pixel information
			unsigned bpp = 0;
		};

		/// Swaps two images. Should be used unqualified for ADL.
		inline void swap(Image &l, Image &r) {
			l.Swap(r);
		}


	}
}
