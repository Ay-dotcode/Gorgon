#pragma once

#include <stdexcept>
#include <memory>

#include "Base.h"
#include "../Graphics/Animations.h"
#include "../Graphics/Texture.h"
#include "../Containers/Image.h"

namespace Gorgon { namespace Resource {
	class File;
	class Image;

	/// This resource contains images. It allows draw, load, import, export functionality. An image resource may work
	/// without its data buffer. In order to be drawn, an image object should be prepared. Both data and texture
	/// might be released from the image resource. This allows safe destruction of the file tree without destroying
	/// the necessary information.
	class Image : 
		public Base, public virtual Graphics::RectangularAnimationProvider, public virtual Graphics::Image,
		public virtual Graphics::RectangularAnimation, private virtual Graphics::Texture, public virtual Graphics::TextureSource
	{
	public:

		/// Default constructor will create an empty image
		Image() {

		}

		/// Creates an uninitialized image of the given size and color mode. Prepare function should be called
		/// to be able to draw this image.
		explicit Image(const Geometry::Size &size, Graphics::ColorMode mode=Graphics::ColorMode::RGBA) : 
		data(new Containers::Image{size, mode}) {

#ifndef NDEBUG
			if(size.Width<0 || size.Height<0) {
				throw std::runtime_error("Size of an image cannot be negative.");
			}
#endif

			RemoveMe();
		}

		/// Copy constructor is disabled. 
		Image(const Image &) = delete;

		/// Move constructor
		Image(Image &&other) {
			Swap(other);
		}

		/// Swaps two images, mostly used for move constructor
		void Swap(Image &other) {
			using std::swap;

			swap(data, other.data);
			swap(compression, other.compression);
			swap(entrypoint, other.entrypoint);
			swap(file, other.file);
			swap(isloaded, other.isloaded);

			Graphics::Texture::Swap(other);
		}

		/// Copy assignment is disabled
		Image &operator=(Image &) = delete;

		/// Move assignment
		Image &operator =(Image &&other) {
			Discard();
			Graphics::Texture::Destroy();

			Swap(other);
		}

		/// Duplicates this image. Only the data portion is duplicated. No other information is
		/// transferred to the image. Omitted information includes resource related data and
		/// texture related data. Therefore, before drawing the newly duplicated image, it should
		/// be prepared for drawing to work.
		Image Duplicate() const {
			Image img;
			if(data)
				img.Assign(*data);

			return img;
		}

		/// Destroys image data
		virtual ~Image() {
			if(data) delete data;
		}

		/// if used as animation, this object will not be deleted
		virtual void DeleteAnimation() override { }

		virtual GID::Type GetGID() const { return GID::Image; }

		/// Releases the image data. The image data returned by this function is moved out. Data is passed by value, thus
		/// if it is not moved into a Containers::Image, it will be destroyed.
		Containers::Image ReleaseData() {
			if(data==nullptr) {
#ifndef NDEBUG
				throw std::runtime_error("No data to release");
#endif

				return{ };
			}
			else {
				Containers::Image temp=std::move(*data);

				delete data;
				data=nullptr;

				return temp;
			}
		}

		/// Releases the texture held by this image. Texture is passed by value, thus
		/// if it is not moved into a Graphics::TextureImage, it will be destroyed.
		Graphics::TextureImage ReleaseTexture() {
			auto sz = Graphics::Texture::size;
			return{Graphics::Texture::Release(), sz};
		}

		/// Checks if this image resource has a data attached to it
		bool HasData() const {
			return data!=nullptr;
		}

		/// Checks if this image resource has a texture attached to it.
		bool HasTexture() const {
			return Graphics::Texture::id!=0;
		}

		/// Assigns the given image as the data of this image resource.
		/// Notice that changing data does not prepare the data to be drawn, a separate call to Prepare 
		/// function is necessary
		void Assign(const Containers::Image &image) {
			if(!data) {
				data=new Containers::Image;
			}

			*data = image.Duplicate();
		}

		/// Assigns the image to the copy of the given data. Ownership of the given data
		/// is not transferred. If the given data is not required elsewhere, consider using
		/// Assume function. This variant performs resize and copy at the same time. The given 
		/// data should have the size of width*height*Graphics::GetBytesPerPixel(mode)*sizeof(Byte). 
		/// This function does not perform any checks for the data size while copying it.
		/// If width or height is 0, the newdata is not accessed and this method effectively
		/// Destroys the current image. In this case, both width and height should be specified as 0.
		/// Notice that changing data does not prepare the data to be drawn, a separate call to Prepare 
		/// function is necessary.
		void Assign(Byte *newdata, const Geometry::Size &size, Graphics::ColorMode mode) {
			if(!data) {
				data=new Containers::Image;
			}

			data->Assign(newdata, size, mode);
		}

		/// Assigns the image to the copy of the given data. Ownership of the given data
		/// is not transferred. If the given data is not required elsewhere, consider using
		/// Assume function. The size and color mode of the image stays the same. The given 
		/// data should have the size of width*height*Graphics::GetBytesPerPixel(mode)*sizeof(Byte). 
		/// This function does not perform any checks for the data size while copying it. Notice that 
		/// changing data does not prepare the data to be drawn, a separate call to Prepare function 
		/// is necessary.
		void Assign(Byte *newdata) {
			if(!data) {
				throw std::runtime_error("Data is not set");
			}

			data->Assign(newdata);
		}

		/// Assumes the contents of the given image as image data. The given parameter is moved from
		/// and will become an empty image. Notice that assuming data does not prepare the data to be drawn, 
		/// a separate call to Prepare function is necessary.
		void Assume(Containers::Image &image) {
			if(!data) {
				data=new Containers::Image;
			}

			*data = std::move(image);
		}

		/// Assumes the ownership of the given data. This variant changes the size and
		/// color mode of the image. The given data should have the size of 
		/// width*height*Graphics::GetBytesPerPixel(mode)*sizeof(Byte). This function
		/// does not perform any checks for the data size while assuming it.
		/// newdata could be nullptr however, in this case
		/// width, height should be 0. mode is not assumed to be ColorMode::Invalid while
		/// the image is empty, therefore it could be specified as any value. Notice that assuming data
		/// does not prepare the data to be drawn, a separate call to Prepare function is necessary.
		void Assume(Byte *newdata, const Geometry::Size &size, Graphics::ColorMode mode) {
			if(!data) {
				data=new Containers::Image;
			}

			data->Assume(newdata, size, mode);
		}

		/// Assumes the ownership of the given data. The size and color mode of the image stays the same.
		/// The given data should have the size of width*height*Graphics::GetBytesPerPixel(mode)*sizeof(Byte).
		/// This function does not perform any checks for the data size while assuming it. Notice that assuming data
		/// does not prepare the data to be drawn, a separate call to Prepare function is necessary.
		void Assume(Byte *newdata) {
			if(!data) {
				throw std::runtime_error("Data is not set");
			}

			data->Assume(newdata);
		}

		/// Resizes the image to the given size and color mode. This function discards the contents
		/// of the image and does not perform any initialization.
		void Resize(const Geometry::Size &size, Graphics::ColorMode mode=Graphics::ColorMode::RGBA) {
			if(!data) {
				data=new Containers::Image;
			}

			data->Resize(size, mode);
		}

		/// Provides access to the given component in x and y coordinates. This function performs bounds checking 
		/// only on debug mode. Notice that changing a pixel does not prepare the new data to be drawn, a separate 
		/// call to Prepare function is necessary.
		Byte &operator()(const Geometry::Point &p, unsigned component=0) {
#ifndef NDEBUG
			if(!data) {
				throw std::runtime_error("Data is not set");
			}
#endif

			return (*data)(p, component);
		}

		/// Provides access to the given component in x and y coordinates. This
		/// function performs bounds checking only on debug mode.
		Byte operator()(const Geometry::Point &p, unsigned component=0) const {
#ifndef NDEBUG
			if(!data) {
				throw std::runtime_error("Data is not set");
			}
#endif

			return (*data)(p, component);
		}

		/// Provides access to the given component in x and y coordinates. This
		/// function returns 0 if the given coordinates are out of bounds. This
		/// function works slower than the () operator.
		Byte Get(const Geometry::Point &p, unsigned component=0) const {
#ifndef NDEBUG
			if(!data) {
				throw std::runtime_error("Data is not set");
			}
#endif

			return (*data)(p, component);
		}

		/// Returns the bytes occupied by a single pixel of this image
		unsigned GetBytesPerPixel() const {
#ifndef NDEBUG
			if(!data) {
				throw std::runtime_error("Image data is not set");
			}
#endif
			return data->GetBytesPerPixel();
		}

		/// Returns the color mode of the image
		Graphics::ColorMode GetMode() const {
#ifndef NDEBUG
			if(!data) {
				throw std::runtime_error("Image data is not set");
			}
#endif
			return data->GetMode();
		}

		/// Returns the size of this image resource. It is possible for an image to become unsynchronized due to
		/// a modification to the image data. Image texture size takes precedence if this happens.
		Geometry::Size GetSize() const {
			if(Graphics::Texture::id!=0) {
				return Graphics::Texture::GetImageSize();
			}
			else if(data) {
				return data->GetSize();
			}
			else {
#ifndef NDEBUG
				throw std::runtime_error("Image contains no data");
#endif

				return{0, 0};
			}
		}

		/// This function prepares image for drawing
		virtual void Prepare() override;

		/// This function discards image data
		virtual void Discard() override;

		/// Imports a PNG file to become the new data of this image resource. Notice that importing does not
		/// prepare the data to be drawn, a separate call to Prepare function is necessary
		void ImportPNG(const std::string &filename);

		/// Imports a JPEG file to become the new data of this image resource. Notice that importing does not
		/// prepare the data to be drawn, a separate call to Prepare function is necessary
		void ImportJPEG(const std::string &filename);

		/// Imports an image file to become the new data of this image resource. Type of the image is determined
		/// from the extension. If the extension is not available please use either ImportPNG or ImportJPEG functions.
		/// Notice that importing does not prepare the data to be drawn, a separate call to Prepare function is necessary
		void Import(const std::string &filename);

		/// Exports the data of the image resource to a PNG file. This function requires image data to be present.
		/// If image data is already discarded, there is no way to retrieve it.
		bool ExportPNG(const std::string &filename);


		virtual const Image &CreateAnimation(Gorgon::Animation::Timer &controller, bool owner=false) const override { 
#ifndef NDEBUG
			if(owner) {
				throw std::runtime_error("Images cannot own timers or controllers.");
			}
#endif
			return *this;
		}

		virtual const Image &CreateAnimation(bool create=false) const override { return *this; }

		/// Creates the blurred version of this image as a new separate image. This function creates another image since
		/// it is not possible to apply blur in place. You may use move assignment to modify the original `img = img.Blur(1.2);`
		/// @param  amount is variance of the blur. This value is measured in pixels however, image will have blurred
		///         edges more than the given amount.
		/// @param  windowsize is the size of the effect window. If the value is -1, the window size is automatically
		///         determined. Reducing window size will speed up this function.
		Image Blur(float amount, int windowsize=-1) const;

		/// Creates a smooth drop shadow by using alpha channel of this image. Resultant image has Grayscale_Alpha color
		/// mode. This function creates another image since it is not possible to apply blur in place. You may use move 
		/// assignment to modify the original `img = img.Blur(1.2);`
		/// @param  amount is variance of the blur. This value is measured in pixels however, image will have blurred
		///         edges more than the given amount.
		/// @param  windowsize is the size of the effect window. If the value is -1, the window size is automatically
		///         determined. Reducing window size will speed up this function.
		Image Shadow(float amount, int windowsize=-1) const;

		/// Transforms this image to a grayscale image. This function has no effect if the image is already grayscale
		/// @param  ratio of the transformation. If ratio is 0, image is not modified. If the ratio is 1, image will be transformed
		///         into fully grayscale image. Values between 0 and 1 will desaturate the image depending on the given ratio.
		///         If the ratio is 1, color mode of the image will be modified to Grayscale or Grayscale_Alpha.
		void Grayscale(float ratio=1.0f);

		/// This function removes transparency information from the image
		void StripTransparency();

		/// Assumes all image heights are similar and all images have same color mode
		std::vector<Geometry::Bounds> CreateLinearAtlas(const Containers::Collection<Image> &list);

		/// Creates images from the given atlas image and map. Prepares every image as well. This requires image to be prepared
		Containers::Collection<Image> CreateAtlasImages(const std::vector<Geometry::Bounds> &boundaries) const;

		/// Trims the exterior empty regions of this image, returning the trimming margins. Notice that trimming image does not
		/// prepare the new data to be drawn, a separate call to Prepare function is necessary
		//Geometry::Margins Trim(bool left=true, bool right=true, bool top=true, bool bottom=true);

		/// Loads the image from the disk. This function requires image to be tied to a resource file.
		bool Load();

		/// Returns whether the image data is loaded. Data loading is a valid information in only resource context. If this image
		/// is created manually, it is always considered loaded even if no data is set for it.
		bool IsLoaded() const { return isloaded; }

		/// This function loads a image resource from the given file
		static Image *LoadResource(File &file, std::istream &data, unsigned long size);

	protected:
		/// When used as animation, an image is always persistent and it never finishes.
		virtual bool Progress(unsigned &leftover) override { return true; }

		using Texture::GetImageSize;

		/// Loads the image from the data stream
		bool load(std::istream &data, unsigned long size, bool forceload);

		/// Container for the image data, could be null indicating its discarded
		Containers::Image *data = nullptr;

		/// Compression mode
		GID::Type compression;

		/// Entry point of this resource within the physical file. This value is stored for 
		/// late loading purposes
		unsigned long entrypoint = -1;

		/// Used to handle late loading
		std::weak_ptr<File> file;

		/// Whether this image resource is loaded or not
		bool isloaded = true;
	};
} }
