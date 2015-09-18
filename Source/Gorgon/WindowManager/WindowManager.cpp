#include "../WindowManager.h"
#include "../Window.h"

namespace Gorgon { 

	Containers::Collection<Window> Window::windows;
	const Containers::Collection<Window> &Window::Windows=Window::windows;

	const Geometry::Point Window::automaticplacement={std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};

	const Window::FullscreenTag Window::Fullscreen={};
	
	namespace WindowManager {
		/// @cond INTERNAL
		intptr_t context = 0;
		/// @endcond

		intptr_t CurrentContext() {
			return context;
		}
	}
}

