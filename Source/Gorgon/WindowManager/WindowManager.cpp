#include "../WindowManager.h"
#include "../Window.h"
#include "../Input/DnD.h"

namespace Gorgon { 

	Containers::Collection<Window> Window::windows;
	const Containers::Collection<Window> &Window::Windows=Window::windows;

	const Geometry::Point Window::automaticplacement={std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};

	const Window::FullscreenTag Window::Fullscreen={};

	Geometry::Size Window::GetMinimumRequiredSize() {
		Geometry::Size sz = {0,0};
		for(auto &w : Windows) {
			auto size = w.GetSize();

			if(size.Width > sz.Width)
				sz.Width = size.Width;

			if(size.Height > sz.Height)
				sz.Height = size.Height;
		}

		return sz;
	}
	
	namespace WindowManager {
		/// @cond INTERNAL
		intptr_t context = 0;
		/// @endcond

		intptr_t CurrentContext() {
			return context;
		}
		
		void init();
        
        void Initialize() {
            init();
        }
	}
}

