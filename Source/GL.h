#pragma once

#include <stdint.h>

#include "Geometry/Size.h"

namespace Gorgon {
	
	namespace Containers {
		class Image;
	}
	
	/// This namespace contains underlying graphics library functions. These functions are presented
	/// in a OS/Window manager generic way. However, direct access to GL functions might be necessary
	/// in many cases. In those cases you may include GL/OpenGL.h library header. This header might
	/// expose operating system dependent headers. In the future there might be more supported GL libraries.
	/// In that case, you should choose to include the library you wish to use.
	namespace GL {

#ifdef OPENGL
		typedef uint32_t Texture;
#endif

		/// This function generates a texture from the given image data.
		Texture GenerateTexture(const Containers::Image &data);

		/// Updates the given texture to contain the given data
		void UpdateTexture(Texture texure, const Containers::Image &data);

		/// Destroys the given texture
		void DestroyTexture(Texture texure);

		/// Performs first time initialization on GL context
		void SetupContext(const Geometry::Size &size);

		/// Resizes the active context
		void Resize(const Geometry::Size &size);
	}
}
