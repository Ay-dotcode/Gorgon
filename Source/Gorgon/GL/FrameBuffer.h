#pragma once

#include "../Geometry/Size.h"
#include "../GL.h"
#include "../Containers/Collection.h"

namespace Gorgon { namespace GL {

	/**
	* This is a frame buffer object that can be used for render to texture tasks. This object
	* cannot be copied.
	*/
	class FrameBuffer {
		friend void Resize(const Geometry::Size &size);
	public:
		FrameBuffer(const FrameBuffer &) = delete;

		FrameBuffer &operator = (const FrameBuffer &) = delete;

		/// Does not perform any action, allows late generation of buffers.
		FrameBuffer() {
			buffers.Add(this);
		}

		/// Generates a frame buffer. If software frame buffer does not work, this will
		/// create only a texture. Use UpdateTexture to update the texture.
		explicit FrameBuffer(bool depth) : FrameBuffer() {
			Generate(depth);
		}

		/// Generates a frame buffer. If software frame buffer does not work, this will
		/// create only a texture. Use UpdateTexture to update the texture.
		void Generate(bool depth);

		/// Destroys the frame buffer
		void Destroy();

		~FrameBuffer() {
			Destroy();
			buffers.Remove(this);
		}

		/// Returns if the frame buffer ready
		bool IsReady() const {
			return texture != 0 && buffer != 0;
		}

		/// Returns if the frame buffer is fully functional.
		bool IsFunctional() const;

		/// Returns the texture of this buffer
		Texture GetTexture() const {
			return texture;
		}

		/// Begin using this frame buffer.
		void Use();

		/// Stop using this buffer and render to screen instead
		void RenderToScreen();

		/// Whether hardware supports frame buffers. If it does not, using frame buffer
		/// has no effect. However, frame buffer will generate a screen sized texture
		/// keeping it updated with the screen resolution. Thus, even if operation is
		/// performed on software, it is still a good idea to use FrameBuffer class
		static bool HardwareSupport;

	private:
#ifdef OPENGL
		Texture  texture = 0;
		uint32_t buffer  = 0;
		uint32_t depth   = 0;
#endif

		//this is used to resize buffer on a geometry change.
		Containers::Collection<FrameBuffer> buffers;
	};
} }
