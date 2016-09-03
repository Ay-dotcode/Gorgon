#pragma once

#include "../Geometry/Point.h"
#include "../Geometry/Size.h"
#include "TextureTargets.h"
#include "Color.h"
#include <stdint.h>

namespace Gorgon { namespace Graphics {

	using Glyph = int32_t;
	
	/**
	 * Should be implemented by the systems aimed to render fonts on the screen.
	 */
	class FontRenderer {
	public:
		/// This function should render the given character to the target at the specified location
		/// and color. If chr does not exists, this function should perform no action. location and
		/// color can be modified as per the needs of renderer. If the kerning returns integers
		/// location will always be an integer.
		virtual void Render(Glyph chr, TextureTarget &target, Geometry::Pointf location, RGBAf color) const = 0;

		/// This function should return the size of the requested glyph. If it does not exists,
		/// 0x0 should be returned
		virtual Geometry::Size GetSize(Glyph chr) const = 0;

		/// This function should return true if this font renderer supports only 7-bit ASCII
		virtual bool IsASCII() const = 0;

		/// This function should return true if this font is fixed width. This will suppress calls
		/// to GetSize function.
		virtual bool IsFixedWidth() const = 0;
		
		/// This function should return the additional distance between given glyphs. Returned value
		/// could be (in most cases it is) negative. Non-integer values would break pixel perfect
		/// rendering.
		virtual float KerningDistance(Glyph chr1, Glyph chr2) const = 0;
	};

} }
