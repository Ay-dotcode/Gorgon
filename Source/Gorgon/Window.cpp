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

	Gorgon::MouseHandler Window::propagate_mouseevent(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button, int amount) {
		Transform = {};
        Clip = bounds.GetSize();
        
        if(event == Input::Mouse::EventType::Down) {
            down = Layer::propagate_mouseevent(event, location, button, amount);
            
            return down;
        }
        else if(event == Input::Mouse::EventType::Up) {
            if(down) {
                down->propagate_mouseevent(event, location, button, amount);
                
                auto temp = down;
                
                down = nullptr;
                
                return temp;
            }
            else {
                return Layer::propagate_mouseevent(Input::Mouse::EventType::Click, location, button, amount);
            }
        }
        else {
            return Layer::propagate_mouseevent(event, location, button, amount);
        }
	}


}
