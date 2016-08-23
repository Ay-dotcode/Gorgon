#include "Window.h"
#include "OS.h"

#include "Graphics/Layer.h"


namespace Gorgon {
	

	void Window::Render() {
		WindowManager::internal::switchcontext(*data);
		ResetTransform(GetSize());
		GL::Clear();


		Layer::Render();

		WindowManager::internal::finalizerender(*data);
	}

	Gorgon::MouseHandler Window::propagate_mouseevent(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button) {
		Transform = {};

		return Layer::propagate_mouseevent(event, location, button);
	}

	Gorgon::MouseHandler Window::propagate_scrollevent(Input::Mouse::ScrollType direction, Geometry::Point location, int amount) {
		Transform ={};

		return Layer::propagate_scrollevent(direction, location, amount);
	}

}
