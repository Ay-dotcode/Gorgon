#include "../GL.h"


#ifdef WIN32
#	include <windows.h>
#elif defined(LINUX)
#	include <GL/glx.h>
#	include <unistd.h>
#endif

#include <GL/gl.h>


#	define GL_BGR	0x80E0
#	define GL_BGRA	0x80E1

#include "../Graphics/Color.h"
#include "../Containers/Image.h"
#include "../OS.h"

namespace Gorgon { namespace GL {

	GLenum getGLColorMode(Graphics::ColorMode mode) {
		switch(mode) {
		case Graphics::ColorMode::Alpha:
			return GL_ALPHA;
		case Graphics::ColorMode::Grayscale_Alpha:
			return GL_LUMINANCE_ALPHA;
		case Graphics::ColorMode::BGR:
			return GL_BGR;
		case Graphics::ColorMode::RGB:
			return GL_RGB;
		case Graphics::ColorMode::BGRA:
			return GL_BGRA;
		case Graphics::ColorMode::RGBA:
			return GL_RGBA;
		default:
			return GL_RGBA;
		}
	}

	void settexturedata(Texture tex, const Containers::Image &data) {
		glBindTexture(GL_TEXTURE_2D, tex);

		GLenum colormode=getGLColorMode(data.GetMode());

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, 0x0);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		glTexImage2D(GL_TEXTURE_2D, 0, Graphics::GetBytesPerPixel(data.GetMode()), data.GetSize().Width, data.GetSize().Height, 0,
			colormode, GL_UNSIGNED_BYTE, data.RawData());
	}

	Texture GenerateTexture(const Containers::Image &data) {
		Texture tex;
		glGenTextures(1, &tex);
		settexturedata(tex, data);

		return tex;
	}

	void UpdateTexture(Texture tex, const Containers::Image &data) {
		settexturedata(tex, data);
	}

	void DestroyTexture(Texture tex) {
		glDeleteTextures(1, &tex);
	}

	void SetupContext(const Geometry::Size &size) {
		std::string gl_version(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
		if(String::To<float>(gl_version)<3.0) {
			OS::DisplayMessage("OpenGL version 3.0 and above is required. Your OpenGL version is "+gl_version);
			exit(2);
		}

		glShadeModel(GL_SMOOTH);
		glClearColor(0.4f, 0.2f, 0.0f, 1.0f);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glViewport(0, 0, size.Width, size.Height);

		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glFlush();
		glFinish();
	}


	void Resize(const Geometry::Size &size) {
		glViewport(0, 0, size.Width, size.Height);					// Reset The Current Viewport
	}


} }
