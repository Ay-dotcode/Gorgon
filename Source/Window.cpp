#include "Window.h"
#include "OS.h"

#ifdef WIN32
#	define WINGDIAPI	__declspec(dllimport)
#	define APIENTRY		__stdcall
#elif defined(LINUX)
#	include <GL/glx.h>
#	include <unistd.h>
#endif

#include <gl/GL.h>

namespace Gorgon {

	void Window::setupglcontext() {
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

		glViewport(0, 0, bounds.Width(), bounds.Height());

		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

	}

}
