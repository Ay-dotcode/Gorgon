#include "../WindowManager.h"
#include "../Window.h"

namespace Gorgon { 

	Containers::Collection<Window> Window::windows;
	const Containers::Collection<Window> &Window::Windows=Window::windows;

	const Geometry::Point Window::automaticplacement={std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};

	namespace WindowManager {
	}
}

