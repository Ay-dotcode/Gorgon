#include "Window.h"
#include "OS.h"

#ifdef WIN32
#	define WINGDIAPI	__declspec(dllimport)
#	define APIENTRY		__stdcall
#	include <gl/GL.h>
#elif defined(LINUX)
#	include <GL/glx.h>
#	include <unistd.h>
#endif

#include "Graphics/Layer.h"


namespace Gorgon {
	

	void Window::Render() {
		WindowManager::internal::switchcontext(*data);
		Graphics::internal::ResetTransform(GetSize());

		Layer::Render();
	}

}
