#pragma once

#include "../Geometry/Size.h"
#include "../Geometry/Bounds.h"
#include "../GL.h"
#include "../Graphics.h"
#include "../Containers/Image.h"
#include "Drawables.h"

namespace Gorgon { namespace Graphics {

	/// This class represents an image depends on a GL texture. Fulfills the requirements of Graphics::TextureSource.
	/// Texture objects never destroys GL::Texture its bound to. This is because a GL::Texture could be shared between
	/// multiple Graphics::Textures. 
	class Texture : public virtual TextureSource {
	public:

		/// Default constructor, creates an empty texture
		Texture() {
			Set(0, {0, 0});
		}

		/// Regular, full texture constructor
		Texture(GL::Texture id, const Geometry::Size &size) {
			Set(id, size);
		}

		/// Atlas constructor, specifies a region of the texture
		Texture(GL::Texture id, const Geometry::Size &size, const Geometry::Bounds &location) {
			Set(id, size, location);
		}

		/// This constructor creates a new texture from the given Image
		Texture(const Containers::Image &image) : Texture(GL::GenerateTexture(image), image.GetSize()) {
		}

		/// Sets the texture to the given id with the given size. Resets the coordinates to cover entire
		/// GL texture
		void Set(GL::Texture id, const Geometry::Size &size) {
			this->id=id;
			this->size=size;

			memcpy(coordinates, fullcoordinates, sizeof(fullcoordinates));
		}

		/// Sets the texture to the given id with the given size. Calculates the texture coordinates
		/// for the specified location in pixels
		/// @param   id id of the texture, reported by the underlying GL framework
		/// @param   size of the GL texture in pixels
		/// @param   location is the location of this texture over GL texture in pixels.
		void Set(GL::Texture id, const Geometry::Size &size, const Geometry::Bounds &location) {
			this->id=id;
			this->size=size;

			coordinates[0] ={float(location.Left)/size.Width, float(location.Top)/size.Height};
			coordinates[1] ={float(location.Right)/size.Width, float(location.Top)/size.Height};
			coordinates[2] ={float(location.Right)/size.Width, float(location.Bottom)/size.Height};
			coordinates[3] ={float(location.Left)/size.Width, float(location.Bottom)/size.Height};
		}

		/// Returns GL::Texture to be drawn.
		virtual GL::Texture GetID() const {
			return id;
		}

		/// Returns the size of the texture in pixels
		virtual Geometry::Size GetSize() const {
			return size;
		}

		/// Returns the coordinates of the texture to be used
		virtual const Geometry::Pointf *GetCoordinates() const {
			return coordinates;
		}

		/// Destroys the GL::Texture that this object points to
		void Destroy() {
			//GL::DestroyTexture(id);
			id=0;
		}

	protected:

		/// GL texture id
		GL::Texture id;

		/// Size of the texture
		Geometry::Size size;

		/// Readily calculated texture coordinates of the image. Normally spans entire
		/// GL texture, however, could be changed to create texture atlas. These coordinates
		/// are kept in floating point u,v representation for quick consumption by the GL
		Geometry::Pointf coordinates[4];
	};

} }