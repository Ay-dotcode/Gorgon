#pragma once

#include <stdexcept>
#include <memory>

#include "Base.h"
#include "../Graphics/Animations.h"
#include "../Graphics/Texture.h"
#include "../Containers/Image.h"
#include "../Graphics/Bitmap.h"

namespace Gorgon { namespace Resource {
	class File;
	class Image;
	class Reader;

	/// This resource contains images. It allows draw, load, import, export functionality. An image resource may work
	/// without its data buffer. In order to be drawn, an image object should be prepared. Both data and texture
	/// might be released from the image resource. This allows safe destruction of the file tree without destroying
	/// the necessary information.
	class Image : 
		public Base, public Graphics::Bitmap
	{
	public:
		Image() = default;

		/// Assumes the given bitmap and converts it to a resource.
		Image(Graphics::Bitmap &&source) {
			Swap(source);
		}

		virtual GID::Type GetGID() const { return GID::Image; }
		
		
		/// Changes the compression mode. It only works if this image is saved along with a file. Currently only GID::None
		/// and GID::PNG works.
		void SetCompression(GID::Type compression) {
			this->compression=compression;
		}
		
		/// Returns the compression mode of this image resourc
		GID::Type GetCompression() const {
			return compression;
		}

		virtual void Prepare() override { Bitmap::Prepare(); }

		virtual void Discard() override { Bitmap::Discard(); }

		/// Trims the exterior empty regions of this image, returning the trimming margins. Notice that trimming image does not
		/// prepare the new data to be drawn, a separate call to Prepare function is necessary
		//Geometry::Margins Trim(bool left=true, bool right=true, bool top=true, bool bottom=true);

		/// Loads the image from the disk. This function requires image to be tied to a resource file.
		bool Load();

		/// Returns whether the image data is loaded. Data loading is a valid information in only resource context. If this image
		/// is created manually, it is always considered loaded even if no data is set for it.
		bool IsLoaded() const { return isloaded; }

		/// This function loads a image resource from the given file
		static Image *LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size);

	protected:
		virtual ~Image() { }
		
		/// Loads the image from the data stream
		bool load(std::shared_ptr<Reader> reader, unsigned long size, bool forceload);
		
		void save(Writer &writer) override;
		
		virtual void loaded() override;

		/// Compression mode
		GID::Type compression = GID::PNG;

		/// Entry point of this resource within the physical file. This value is stored for 
		/// late loading purposes
		unsigned long entrypoint = -1;

		/// Used to handle late loading
		std::shared_ptr<Reader> reader;

		/// Whether this image resource is loaded or not
		bool isloaded = true;
	};
} }
