#pragma once


#include "../Geometry/Point.h"
#include "../Geometry/Size.h"
#include "../Geometry/Rectangle.h"

#include "../Graphics.h"
#include "TextureTargets.h"


namespace Gorgon { namespace Graphics {

	/// Represents a drawable object, that can be drawn to the given point. Size of the object
	/// is assumed to be fixed. Drawing a single drawable can cause many texture to be added
	/// to the given texture target
	class Drawable {
	public:

		/// Draw to the given coordinates
		void Draw(TextureTarget &target, int x, int y) const {
			draw(target, {x, y});
		}

		/// Draw to the given coordinates
		void Draw(TextureTarget &target, const Geometry::Point &p) const {
			draw(target, p);
		}

	protected:
		/// This is the only function that needs to implemented for a drawable
		virtual void draw(TextureTarget &target, const Geometry::Point &p) const = 0;
	};


	/// A drawable object that does not have a size and requires a region to draw.
	class SizelessDrawable {
	public:

		/// Draw to the given area
		void DrawIn(TextureTarget &target, int x, int y, int w, int h) const {
			drawin(target, {x, y, w, h});
		}

		/// Draw to the given area
		void DrawIn(TextureTarget &target, const Geometry::Point &p, int w, int h) const {
			drawin(target, {p, w, h});
		}

		/// Draw to the given area
		void DrawIn(TextureTarget &target, int x, int y, const Geometry::Size &size) const {
			drawin(target, {x, y, size});
		}

		/// Draw to the given area
		void DrawIn(TextureTarget &target, const Geometry::Point &p, const Geometry::Size &size) const {
			drawin(target, {p, size});
		}

		/// Draw in the given area
		void DrawIn(TextureTarget &target, const Geometry::Rectangle &r) const {
			drawin(target, r);
		}

		/// Draw to fill the given target
		void DrawIn(TextureTarget &target) const {
			drawin(target, {0, 0, target.GetTargetSize()});
		}

		/// Draw to the given coordinates with the given size according to the given controller
		void DrawIn(TextureTarget &target, const SizeController &controller, int x, int y, int w, int h) const {
			drawin(target, controller, {x, y, w, h});
		}

		/// Draw to the given area according to the given controller
		void DrawIn(TextureTarget &target, const SizeController &controller, const Geometry::Point &p, int w, int h) const {
			drawin(target, controller, {p, w, h});
		}

		/// Draw to the given area the given size according to the given controller
		void DrawIn(TextureTarget &target, const SizeController &controller, int x, int y, const Geometry::Size &size) const {
			drawin(target, controller, {x, y, size});
		}

		/// Draw to the given area with the given size according to the given controller
		void DrawIn(TextureTarget &target, const SizeController &controller, const Geometry::Point &p, const Geometry::Size &size) const {
			drawin(target, controller, {p, size});
		}


		/// Draw in the given area according to the given controller
		void DrawIn(TextureTarget &target, const SizeController &controller, const Geometry::Rectangle &r) const {
			drawin(target, controller, r);
		}

		/// Draw to fill the given target according to the given controller
		void DrawIn(TextureTarget &target, const SizeController &controller) const {
			drawin(target, controller, {0, 0, target.GetTargetSize()});
		}


		/// Calculates the adjusted size of this drawable depending on the given area. This function calculates
		/// actual the size of the drawable when it is drawn in an area that has the given size. When a size parameter
		/// is set to -1, the object returns its native size. Native size might not be available for some objects
		/// however, they will try to return a logical value.
		const Geometry::Size CalculateSize(const Geometry::Size &area) const {
			return calculatesize(area);
		}

		/// Calculates the adjusted size of this drawable depending on the given area. This function calculates
		/// actual the size of the drawable when it is drawn in an area that has the given size. When a size parameter
		/// is set to -1, the object returns its native size. Native size might not be available for some objects
		/// however, they will try to return a logical value.
		const Geometry::Size CalculateSize(int w=-1, int h=-1) const {
			return calculatesize({w, h});
		}

		/// Calculates the adjusted size of this drawable depending on the given area and controller. This function calculates
		/// actual the size of the drawable when it is drawn in an area that has the given size. When a size parameter
		/// is set to -1, the object returns its native size. Native size might not be available for some objects
		/// however, they will try to return a logical value.
		const Geometry::Size CalculateSize(const SizeController &controller, const Geometry::Size &area) const {
			return calculatesize(controller, area);
		}

		/// Calculates the adjusted size of this drawable depending on the given area and controller. This function calculates
		/// actual the size of the drawable when it is drawn in an area that has the given size. When a size parameter
		/// is set to -1, the object returns its native size. Native size might not be available for some objects
		/// however, they will try to return a logical value.
		const Geometry::Size CalculateSize(const SizeController &controller, int w=-1, int h=-1) const {
			return calculatesize(controller, {w, h});
		}

	protected:
		/// This function should draw this drawable inside the given rectangle
		virtual void drawin(TextureTarget &target, const Geometry::Rectangle &r) const = 0;

		/// This function should draw this drawable inside the given rectangle according to the given controller.
		/// If this object already have a size controller, this given controller should be given priority.
		virtual void drawin(TextureTarget &target, const SizeController &controller, const Geometry::Rectangle &r) const = 0;

		/// This function should return the size of the object when it is requested to be drawn in the given area. If size contains
		/// is a negative value, this function should try to return native size of the object. If no such size exists, a logical size
		/// should be returned.
		virtual Geometry::Size calculatesize(const Geometry::Size &area) const = 0;

		/// This function should return the size of the object when it is requested to be drawn in the given area. This variant should
		/// use the given size controller. If the object already has a controller, given controller should be given priority. If h parameter
		/// is a negative value, this function should try to return native size of the object. If no such size exists, a logical size
		/// should be returned.
		virtual Geometry::Size calculatesize(const SizeController &controller, const Geometry::Size &s) const = 0;
	};


	////This is a basic drawing object
	class RectangularDrawable : public virtual Drawable, public virtual SizelessDrawable {
	public:

		using Drawable::Draw;

		using SizelessDrawable::DrawIn;

		/// Draw to the given area by stretching object to fit
		void DrawStretched(TextureTarget &target, int x, int y, int w, int h) const {
			drawstretched(target, {x, y, w, h});
		}

		/// Draw to the given area by stretching object to fit
		void DrawStretched(TextureTarget &target, const Geometry::Point &p, int w, int h) const {
			drawstretched(target, {p, w, h});
		}

		/// Draw to the given area by stretching object to fit
		void DrawStretched(TextureTarget &target, int x, int y, const Geometry::Size &size) const {
			drawstretched(target, {x, y, size});
		}

		/// Draw to the given area by stretching object to fit
		void DrawStretched(TextureTarget &target, const Geometry::Point &p, const Geometry::Size &size) const {
			drawstretched(target, {p, size});
		}

		/// Draw to the given area by stretching object to fit
		void DrawStretched(TextureTarget &target, const Geometry::Rectangle &r) const {
			drawstretched(target, r);
		}



		/// Draw the object to the target by specifying coordinates for four corners
		void Draw(TextureTarget &target, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) const {
			draw(target, {x1, y1}, {x2, y2}, {x3, y3}, {x4, y4});
		}

		/// Draw the object to the target by specifying coordinates for four corners
		void Draw(TextureTarget &target, const Geometry::Pointf &p1, const Geometry::Pointf &p2, const Geometry::Pointf &p3, const Geometry::Pointf &p4) const {
			draw(target, p1, p2, p3, p4);
		}


		/// Draw the object rotated to the given angle in radians, full C++11 support will enable the use of 90deg like qualifiers
		void DrawRotated(TextureTarget &target, const Geometry::Point &p, float angle, const Geometry::Pointf & origin=Geometry::Point(0, 0)) const;

		/// Draw the object rotated to the given angle in radians, full C++11 support will enable the use of 90deg like qualifiers
		void DrawRotated(TextureTarget &target, int x, int y, float angle, float oX=0, float oY=0) const {
			DrawRotated(target, {x, y}, angle, {oX, oY});
		}



		/// Draws the object to the target using the given tiling information
		void DrawIn(TextureTarget &target, Tiling tiling, int x, int y, int w, int h) const {
			drawin(target, tiling, {x, y, w, h});
		}

		/// Draws the object to the target using the given tiling information
		void DrawIn(TextureTarget &target, Tiling tiling, const Geometry::Point &p, int w, int h) const {
			drawin(target, tiling, {p, w, h});
		}

		/// Draws the object to the target using the given tiling information
		void DrawIn(TextureTarget &target, Tiling tiling, int x, int y, const Geometry::Size &size) const {
			drawin(target, tiling, {x, y, size});
		}

		/// Draws the object to the target using the given tiling information
		void DrawIn(TextureTarget &target, Tiling tiling, const Geometry::Point &p, const Geometry::Size &size) const {
			drawin(target, tiling, {p, size});
		}


		/// Draws the object to the target using the given tiling information
		void DrawIn(TextureTarget &target, Tiling tiling, const Geometry::Rectangle &r) const {
			drawin(target, tiling, r);
		}





		/// Draws the object with the given screen and texture coordinates. Texture coordinates are given between 0 and 1. If the 
		/// coordinates are out of bounds the texture is repeated
		void Draw(TextureTarget &target, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4) const {
			draw(target, {x1, y1}, {x2, y2}, {x3, y3}, {x4, y4}, {u1, v1}, {u2, v2}, {u3, v3}, {u4, v4});
		}

		/// Draws the object with the given screen and texture coordinates. Texture coordinates are given between 0 and 1. If the 
		/// coordinates are out of bounds the texture is repeated
		void Draw(TextureTarget &target, const Geometry::Pointf &p1, const Geometry::Pointf &p2, const Geometry::Pointf &p3, const Geometry::Pointf &p4, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4) const {
			draw(target, p1, p2, p3, p4, {u1, v1}, {u2, v2}, {u3, v3}, {u4, v4});
		}

		/// Draws the object with the given screen and texture coordinates. Texture coordinates are given between 0 and 1. If the 
		/// coordinates are out of bounds the texture is repeated
		void Draw(TextureTarget &target, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, const Geometry::Pointf &t1, const Geometry::Pointf &t2, const Geometry::Pointf &t3, const Geometry::Pointf &t4) const {
			draw(target, {x1, y1}, {x2, y2}, {x3, y3}, {x4, y4}, t1, t2, t3, t4);
		}

		/// Draws the object with the given screen and texture coordinates. Texture coordinates are given between 0 and 1. If the 
		/// coordinates are out of bounds the texture is repeated
		void Draw(TextureTarget &target, const Geometry::Pointf &p1, const Geometry::Pointf &p2, const Geometry::Pointf &p3, const Geometry::Pointf &p4, const Geometry::Pointf &t1, const Geometry::Pointf &t2, const Geometry::Pointf &t3, const Geometry::Pointf &t4) const {
			draw(target, p1, p2, p3, p4, t1, t2, t3, t4);
		}

		/// Draws the object with the given screen and texture coordinates. Texture coordinates are given between 0 and 1. If the 
		/// coordinates are out of bounds the texture is repeated
		void Draw(TextureTarget &target, float x, float y, float w, float h, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4) const {
			draw(target, {x, y}, {x+w, y}, {x+w, y+h}, {x, y+h}, {u1, v1}, {u2, v2}, {u3, v3}, {u4, v4});
		}

		/// Draws the object with the given screen and texture coordinates. Texture coordinates are given between 0 and 1. If the 
		/// coordinates are out of bounds the texture is repeated
		void Draw(TextureTarget &target, float x, float y, float w, float h, const Geometry::Pointf &t1, const Geometry::Pointf &t2, const Geometry::Pointf &t3, const Geometry::Pointf &t4) const {
			draw(target, {x, y}, {x+w, y}, {x+w, y+h}, {x, y+h}, t1, t2, t3, t4);
		}

		/// Draws the object with the given screen and texture coordinates. Texture coordinates are given between 0 and 1. If the 
		/// coordinates are out of bounds the texture is repeated
		void Draw(TextureTarget &target, const Geometry::Pointf &p, float w, float h, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4) const {
			Draw(target, p.X, p.Y, w, h, {u1, v1}, {u2, v2}, {u3, v3}, {u4, v4});
		}

		/// Draws the object with the given screen and texture coordinates. Texture coordinates are given between 0 and 1. If the 
		/// coordinates are out of bounds the texture is repeated
		void Draw(TextureTarget &target, const Geometry::Pointf &p, float w, float h, const Geometry::Pointf &t1, const Geometry::Pointf &t2, const Geometry::Pointf &t3, const Geometry::Pointf &t4) const {
			Draw(target, p.X, p.Y, w, h, t1, t2, t3, t4);
		}

		/// Draws the object with the given screen and texture coordinates. Texture coordinates are given between 0 and 1. If the 
		/// coordinates are out of bounds the texture is repeated
		void Draw(TextureTarget &target, float x, float y, const Geometry::Sizef &size, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4) const {
			Draw(target, x, y, size.Width, size.Height, {u1, v1}, {u2, v2}, {u3, v3}, {u4, v4});
		}

		/// Draws the object with the given screen and texture coordinates. Texture coordinates are given between 0 and 1. If the 
		/// coordinates are out of bounds the texture is repeated
		void Draw(TextureTarget &target, float x, float y, const Geometry::Sizef &size, const Geometry::Pointf &t1, const Geometry::Pointf &t2, const Geometry::Pointf &t3, const Geometry::Pointf &t4) const {
			Draw(target, x, y, size.Width, size.Height, t1, t2, t3, t4);
		}

		/// Draws the object with the given screen and texture coordinates. Texture coordinates are given between 0 and 1. If the 
		/// coordinates are out of bounds the texture is repeated
		void Draw(TextureTarget &target, const Geometry::Pointf &p, const Geometry::Sizef &size, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4) const {
			Draw(target, p.X, p.Y, size.Width, size.Height, {u1, v1}, {u2, v2}, {u3, v3}, {u4, v4});
		}

		/// Draws the object with the given screen and texture coordinates. Texture coordinates are given between 0 and 1. If the 
		/// coordinates are out of bounds the texture is repeated
		void Draw(TextureTarget &target, const Geometry::Pointf &p, const Geometry::Sizef &size, const Geometry::Pointf &t1, const Geometry::Pointf &t2, const Geometry::Pointf &t3, const Geometry::Pointf &t4) const {
			Draw(target, p.X, p.Y, size.Width, size.Height, t1, t2, t3, t4);
		}

		/// Returns the size of this object
		const Geometry::Size GetSize() const {
			return getsize();
		}



	protected:

		using SizelessDrawable::drawin;

		/// This function should draw the object to the given point.
		virtual void draw(TextureTarget &target, const Geometry::Point &p) const {
			auto size=getsize();
			draw(target, p, {float(p.X+size.Width), float(p.Y)}, {float(p.X+size.Width), float(p.Y+size.Height)}, {float(p.X), float(p.Y+size.Height)});
		}

		/// This method should draw to object inside the given quad with the given texture coordinates.
		/// The texture should be considered repeating and any values outside 0-1 range should be treated
		/// as such
		virtual void draw(TextureTarget &target, const Geometry::Pointf &p1, const Geometry::Pointf &p2,
			const Geometry::Pointf &p3, const Geometry::Pointf &p4,
			const Geometry::Pointf &tex1, const Geometry::Pointf &tex2,
			const Geometry::Pointf &tex3, const Geometry::Pointf &tex4) const = 0;

		/// This function should draw the object inside the given quad. The object should be stretched as
		/// necessary
		virtual void draw(TextureTarget &target, const Geometry::Pointf &p1, const Geometry::Pointf &p2,
			const Geometry::Pointf &p3, const Geometry::Pointf &p4) const = 0;

		/// This function should draw the object to the target area. The object should be stretched along
		/// both dimensions to fit into the given area.
		/// It might be logical to override this as it is possible to avoid additional function calls and
		/// if statements
		virtual void drawstretched(TextureTarget &target, const Geometry::Rectangle &r) const {
			drawin(target, Tiling::None, r);
		}

		/// This function should draw the object to the target area. The object should be tiled or cut
		/// to fit the given area
		/// It might be logical to override this as it is possible to avoid additional function calls and
		/// if statements
		virtual void drawin(TextureTarget &target, const Geometry::Rectangle &r) const override { 
			drawin(target, Tiling::Both, r); 
		}

		/// Should return the exact size of this object
		virtual Geometry::Size getsize() const = 0;
	};

	/// This is a solid texture based image. It handles the drawing automatically. Does not supply implementation
	/// for Texture.
	class Image : public virtual RectangularDrawable, public virtual TextureSource {
	public:

	protected:
		virtual Geometry::Size getsize() const override {
			return GetImageSize();
		}

		virtual Geometry::Size calculatesize(const Geometry::Size &s) const override {
			return GetImageSize();
		}

		virtual Geometry::Size calculatesize(const SizeController &controller, const Geometry::Size &s) const override {
			return controller.CalculateSize(getsize(), s);
		}

		virtual void draw(TextureTarget &target, const Geometry::Pointf &p1, const Geometry::Pointf &p2,
			const Geometry::Pointf &p3, const Geometry::Pointf &p4) const override {
			target.Draw(*this, p1, p2, p3, p4);
		}

		virtual void draw(TextureTarget &target, const Geometry::Pointf &p1, const Geometry::Pointf &p2,
			const Geometry::Pointf &p3, const Geometry::Pointf &p4,
			const Geometry::Pointf &tex1, const Geometry::Pointf &tex2,
			const Geometry::Pointf &tex3, const Geometry::Pointf &tex4) const override {
			target.Draw(*this, p1, p2, p3, p4, tex1, tex2, tex3, tex4);
		}

		virtual void drawstretched(TextureTarget &target, const Geometry::Rectangle &r) const override {
			target.Draw(*this, Tiling::None, r);
		}

		virtual void drawin(TextureTarget &target, const Geometry::Rectangle &r) const override {
			target.Draw(*this, Tiling::Both, r);
		}

		virtual void drawin(TextureTarget &target, const SizeController &controller, const Geometry::Rectangle &r) const override {
			target.Draw(*this, controller.GetTiling(), controller.CalculateArea(getsize(), r.GetSize())+r.TopLeft());
		}
	};

} }
