#pragma once

#include "../Graphics.h"
#include "Color.h"
#include "../Geometry/Point.h"
#include "../Geometry/Size.h"
#include "../Geometry/Rectangle.h"

namespace Gorgon { namespace Graphics {

	/// This interface defines a class that can be used as a common target for texture based drawing
	class TextureTarget {
	public:
		enum DrawMode {
			Normal,
		};

		/// Draws a simple texture to the screen. This variant allows every corner on the target to be specified. The texture target should
		/// be cleared before the texture drawn on it is destroyed
		virtual void Draw(const TextureSource &image, const Geometry::Pointf &p1, const Geometry::Pointf &p2, 
			const Geometry::Pointf &p3, const Geometry::Pointf &p4, RGBAf color = RGBAf(1.f)) = 0;

		/// Draws a textureless solid colored rectangle on the screen.
		virtual void Draw(const Geometry::Pointf &p1, const Geometry::Pointf &p2, 
			const Geometry::Pointf &p3, const Geometry::Pointf &p4, RGBAf color = RGBAf(1.f)) = 0;

		/// Draws a textureless solid colored rectangle on the screen.
		virtual void Draw(const Geometry::Pointf &location, const Geometry::Sizef &size, RGBAf color = RGBAf(1.f)) {
			Draw({location, size}, color);
		}

		/// Draws a textureless solid colored rectangle on the screen.
		virtual void Draw(const Geometry::Rectanglef &location, RGBAf color = RGBAf(1.f)) {
			Draw(location.TopLeft(), location.TopRight(), location.BottomRight(), location.BottomLeft(), color);
		}

		/// Draws a textureless solid colored rectangle to cover the texture target.
		virtual void Draw(RGBAf color = RGBAf(1.f)) {
			Draw({0.f, 0.f}, Geometry::Sizef(GetTargetSize()), color);
		}

		/// Draws a simple texture to the screen. This variant allows every corner on the target and on the texture be specified. The texture 
		/// target should be cleared before the texture drawn on it is destroyed
		virtual void Draw(const TextureSource &image,
			const Geometry::Pointf &p1, const Geometry::Pointf &p2, 
			const Geometry::Pointf &p3, const Geometry::Pointf &p4, 
			const Geometry::Pointf &tex1, const Geometry::Pointf &tex2, 
			const Geometry::Pointf &tex3, const Geometry::Pointf &tex4, RGBAf color = RGBAf(1.f)) = 0;

		/// Draws a simple image to the screen to the given position
		virtual void Draw(const TextureSource &image, const Geometry::Pointf &location, RGBAf color) {
			Draw(image, {location, image.GetImageSize()}, color);
		}

		/// Draws a simple image to the screen to the given position with the given size.
		virtual void Draw(const TextureSource &image, const Geometry::Rectanglef &location, RGBAf color = RGBAf(1.f)) {
			Draw(image, location.TopLeft(), location.TopRight(), location.BottomLeft(), location.BottomRight(), color);
		}

		/// Draws a simple image to the screen using the given tiling method, coordinates and size
		virtual void Draw(const TextureSource &image, Tiling tiling, const Geometry::Rectanglef &location, RGBAf color = RGBAf(1.f)) = 0;

		/// Clears drawing buffer, in layer architecture this request only affects
		/// the layer itself, not its children
		virtual void Clear()=0;

		/// Get size of the target
		virtual Geometry::Size GetTargetSize() const = 0;
		
		/// Returns current draw mode
		virtual DrawMode GetDrawMode() const = 0;

		/// Sets current draw mode
		virtual void SetDrawMode(DrawMode mode) = 0 ;
	};

	/// This interface defines a target that can change the color of drawings on it
	class ColorizableTarget {
	public:
		/// Changes current drawing color
		virtual void SetCurrentColor(const RGBAf &color) = 0;

		/// Returns current drawing color
		virtual RGBAf GetCurrentColor() = 0;
	};

	/// This interface defines a texture target that can change the color of drawings on it
	class ColorizableTextureTarget : public TextureTarget, public ColorizableTarget {
	};

} }
