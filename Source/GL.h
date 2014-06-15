#pragma once

#include <stdint.h>

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
		Texture GenerateTexture(Containers::Image &data);
	}
}
