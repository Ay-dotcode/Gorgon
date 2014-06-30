#pragma once

#include "../Geometry/Size.h"
#include "../Geometry/Bounds.h"
#include "../GL.h"
#include "../Graphics.h"
#include "../Containers/Image.h"
#include "Drawables.h"

namespace Gorgon { namespace Graphics {

	/// This class represents an image depends on a GL texture. Fulfills the requirements of Graphics::TextureSource.
	/// Unless GL::Texture created by this object, it is not destroyed by constructor. This is because a GL::Texture could be 
	/// shared between multiple Graphics::Textures. 
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
			owner=true;
		}

		Texture(Texture &other) : id(other.id), size(other.size), owner(false) {
			memcpy(coordinates, other.coordinates, sizeof(coordinates));
		}

		Texture(Texture &&other) : id(other.id), size(other.size), owner(other.owner) {
			memcpy(coordinates, other.coordinates, sizeof(coordinates));
			other.owner=false;
		}

		~Texture() {
			Destroy();
		}

		/// Sets the texture to the given id with the given size. Resets the coordinates to cover entire
		/// GL texture
		void Set(const Containers::Image &image) {
			Destroy();

			this->id=GL::GenerateTexture(image);
			this->size=image.GetSize();
			owner=true;

			memcpy(coordinates, fullcoordinates, sizeof(fullcoordinates));
		}

		/// Sets the texture to the given id with the given size. Resets the coordinates to cover entire
		/// GL texture
		void Set(GL::Texture id, const Geometry::Size &size) {
			Destroy();

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
			Destroy();

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

		/// Remove the texture from this object. If this object is the owner of the texture, then it is destroyed.
		void Destroy() {
			if(owner) {
				//GL::DestroyTexture(id);
			}
			id=0;
			owner=false;
		}

		/// Releases the texture id that might be owned by this object without destroying it
		GL::Texture Release() {
			auto id=this->id;
			this->id=0;
			owner=false;
			size={0, 0};

			return id;
		}

	protected:

		/// GL texture id
		GL::Texture id = 0;

		/// Size of the texture
		Geometry::Size size ={0, 0};

		/// Whether this object owns this texture
		bool owner = false;

		/// Readily calculated texture coordinates of the image. Normally spans entire
		/// GL texture, however, could be changed to create texture atlas. These coordinates
		/// are kept in floating point u,v representation for quick consumption by the GL
		Geometry::Pointf coordinates[4];
	};


	/// This is a solid texture based image class. 
	class TextureImage : public Texture, public virtual Image {
	public:
		/// Default constructor, creates an empty texture
		TextureImage() { }

		/// Copy constructor
		TextureImage(TextureImage &other) : Texture(other) {
		}

		/// Move constructor
		TextureImage(TextureImage &&other) {
			id=other.id;
			size=other.size;
			owner=other.owner;
			memcpy(coordinates, other.coordinates, sizeof(coordinates));
			other.owner=false;
		}

		/// Regular, full texture constructor
		TextureImage(GL::Texture id, const Geometry::Size &size) : Texture(id, size) {
			Set(id, size);
		}

		/// Atlas constructor, specifies a region of the texture
		TextureImage(GL::Texture id, const Geometry::Size &size, const Geometry::Bounds &location) : Texture(id, size, location) {
			Set(id, size, location);
		}

		/// This constructor creates a new texture from the given Image
		TextureImage(const Containers::Image &image) : Texture(image) {
		}

		/// Regular, full texture constructor
		TextureImage(GL::Texture id, const Geometry::Size &size) : Texture(id, size) { }

		/// Atlas constructor, specifies a region of the texture
		TextureImage(GL::Texture id, const Geometry::Size &size, const Geometry::Bounds &location) : Texture(id, size, location) { }

		/// This constructor creates a new texture from the given Image
		TextureImage(const Containers::Image &image) : Texture(image) { }
	};

} }