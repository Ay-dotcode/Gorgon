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


namespace Gorgon {
	
}
