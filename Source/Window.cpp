#include "Window.h"
#include "OS.h"

#include "Graphics/Layer.h"


namespace Gorgon {
	

	void Window::Render() {
		WindowManager::internal::switchcontext(*data);
		Graphics::internal::ResetTransform(GetSize());
		GL::Clear();


		Layer::Render();

		WindowManager::internal::finalizerender(*data);
	}

}
