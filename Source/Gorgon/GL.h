#pragma once

#include <stdint.h>

#include "Geometry/Size.h"
#include "../Graphics/Color.h"

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

		class FrameBuffer;

#ifdef OPENGL
		/// This is a GLTexture descriptor. This may change depending on the GL used. It will always be
		/// copy constructible/assignable and comparable.
		typedef uint32_t Texture;
#endif

		/// This function generates a texture from the given image data.
		Texture GenerateTexture(const Containers::Image &data);

		/// This function generates a texture from the given image data.
		Texture GenerateEmptyTexture(const Geometry::Size &size, Graphics::ColorMode mode);

		/// Updates the given texture to contain the given data
		void UpdateTexture(Texture texure, const Containers::Image &data);

		/// Destroys the given texture
		void DestroyTexture(Texture texure);

		/// Begins using the given frame buffer. 
		void RenderToTexture(FrameBuffer &buffer);

		/// Stops rendering to a texture and start rendering to a buffer
		void RenderToScreen();

		/// Performs first time initialization on GL context
		void SetupContext(const Geometry::Size &size);

		/// Resizes the active context
		void Resize(const Geometry::Size &size);

		/// Clears the window pointed by the active context
		void Clear();
	}
}
