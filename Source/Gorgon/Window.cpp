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

	void Window::mouse_down(Geometry::Point location, Input::Mouse::Button button) {
        pressed = pressed | button;
        
		Transform = {};
        Clip = bounds.GetSize();
        
        down.Clear();
        Layer::propagate_mouseevent(Input::Mouse::EventType::Down, location, button, 1, down);
    }
    
    void Window::mouse_up(Geometry::Point location, Input::Mouse::Button button) {
        pressed = pressed & ~button;
        
		Transform = {};
        Clip = bounds.GetSize();
        
        if(down) {
            down[0].propagate_mouseevent(Input::Mouse::EventType::Up, location, button, 0, down);
            
            down = nullptr;
        }
        else {
            MouseHandler handler;
            Layer::propagate_mouseevent(Input::Mouse::EventType::Click, location, button, 1, handler);
        }
	}
	
	void Window::mouse_event(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button, float amount) {
		Transform = {};
        Clip = bounds.GetSize();

        MouseHandler handler;
        Layer::propagate_mouseevent(event, location, button, amount, handler);
    }

    void Window::mouse_location() {
		Transform = {};
        Clip = bounds.GetSize();
        
        MouseHandler newover;
        Layer::propagate_mouseevent(Input::Mouse::EventType::OverCheck, mouselocation, Input::Mouse::Button::None, 0, newover);
        
        //first check outs
        for(auto &l : over.layers) {
            if(newover.layers.Find(l) == newover.layers.end()) {
                l.propagate_mouseevent(Input::Mouse::EventType::Out, mouselocation, Input::Mouse::Button::None, 0, newover);
            }
        }
       
        //first check outs
        for(auto &l : newover.layers) {
            if(over.layers.Find(l) == over.layers.end()) {
                l.propagate_mouseevent(Input::Mouse::EventType::Over, mouselocation, Input::Mouse::Button::None, 1, newover);
            }
        }
        
        over.layers.Swap(newover.layers);
        
        if(down) {
            down[0].propagate_mouseevent(Input::Mouse::EventType::Move, mouselocation, Input::Mouse::Button::None, 0, down);
        }
        else {
            newover.Clear();
            Layer::propagate_mouseevent(Input::Mouse::EventType::Move, mouselocation, Input::Mouse::Button::None, 0, newover);
        }
    }

}
