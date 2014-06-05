#pragma once

#include "../Geometry/Size.h"
#include "../Geometry/Bounds.h"
#include "../GL.h"

namespace Gorgon { namespace Graphics {

	/// This class represents an image depends on a GL texture.
	class TextureImage {
	public:

		/// Default constructor, creates an empty texture
		TextureImage() {
			Set(0, {0, 0});
		}

		/// Regular, full texture constructor
		TextureImage(GL::Texture id, const Geometry::Size &size) {
			Set(id, size);
		}

		/// Atlas constructor, specifies a region of the texture
		TextureImage(GL::Texture id, const Geometry::Size &size, const Geometry::Bounds &location) {
			Set(id, size, location);
		}

		/// Sets the texture to the given ID with the given size. Resets the coordinates to cover entire
		/// GL texture
		void Set(GL::Texture id, const Geometry::Size &size) {
			ID  =id;
			Size=size;

			Coordinates[0] ={0.f, 0.f};
			Coordinates[1] ={1.f, 0.f};
			Coordinates[2] ={1.f, 1.f};
			Coordinates[3] ={0.f, 1.f};
		}

		/// Sets the texture to the given ID with the given size. Calculates the texture coordinates
		/// for the specified location in pixels
		/// @param   id ID of the texture, reported by the underlying GL framework
		/// @param   size of the GL texture in pixels
		/// @param   location is the location of this texture over GL texture in pixels.
		void Set(GL::Texture id, const Geometry::Size &size, const Geometry::Bounds &location) {
			ID  =id;
			Size=size;

			Coordinates[0] ={float(location.Left)/size.Width, float(location.Top)/size.Height};
			Coordinates[1] ={float(location.Right)/size.Width, float(location.Top)/size.Height};
			Coordinates[2] ={float(location.Right)/size.Width, float(location.Bottom)/size.Height};
			Coordinates[3] ={float(location.Left)/size.Width, float(location.Bottom)/size.Height};
		}

		/// GL texture id
		GL::Texture ID;

		/// Size of the texture
		Geometry::Size Size;

		/// Readily calculated texture coordinates of the image. Normally spans entire
		/// GL texture, however, could be changed to create texture atlas. These coordinates
		/// are kept in floating point u,v representation for quick consumption by the GL
		Geometry::Pointf Coordinates[4];
	};

} }