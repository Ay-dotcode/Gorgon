#include "Window.h"
#include "OS.h"

#include "Graphics/Layer.h"
#include "Graphics/Color.h"
#include "GL/FrameBuffer.h"


namespace Gorgon {

	extern Graphics::RGBAf LayerColor;
	Geometry::Size ScreenSize;
	
	void Window::activatecontext() {
		WindowManager::internal::switchcontext(*data);
	}

	void Window::Render() {
		activatecontext();
		if(glsize != bounds.GetSize()) {
			GL::FrameBuffer::UpdateSizes();
			glsize = bounds.GetSize();
		}
		ResetTransform(GetSize());
		LayerColor = Graphics::RGBAf(1.f);
		GL::Clear();
		ScreenSize = bounds.GetSize();
		Clip = bounds;

		Layer::Render();

		WindowManager::internal::finalizerender(*data);
	}

	void Window::mouse_down(Geometry::Point location, Input::Mouse::Button button) {
        pressed = pressed | button;
        
		Transform = {};
        Clip = bounds;

		mousedownlocation = location;
        
        down.Clear();
        Layer::propagate_mouseevent(Input::Mouse::EventType::Down, location, button, 1, down);
    }
    
    void Window::mouse_up(Geometry::Point location, Input::Mouse::Button button) {
        pressed = pressed & ~button;
        
		Transform = {};
        Clip = bounds;
        
        if(down) {
            down[0].propagate_mouseevent(Input::Mouse::EventType::Up, location, button, 0, down);
            
            down.Clear();
        }
        else {
			if(mousedownlocation.Distance(location) <= ClickThreshold) {
				MouseHandler handler;
				Layer::propagate_mouseevent(Input::Mouse::EventType::Click, location, button, 1, handler);
			}
        }
	}
	
	void Window::mouse_event(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button, float amount) {
		Transform = {};
        Clip = bounds;

        MouseHandler handler;
        Layer::propagate_mouseevent(event, location, button, amount, handler);
    }

    void Window::mouse_location() {
        if(!iswmpointer && pointerlayer->IsVisible()) {
            pointerlayer->Clear();
            ASSERT(Pointers.IsValid(), "No pointer is set");
            
            Pointers.Current().Draw(*pointerlayer, mouselocation);
        }
        
		Transform = {};
        Clip = bounds;
        
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

	const WindowManager::Monitor &Window::GetMonitor() const {
		auto location = GetExteriorBounds().Center();

		for(const auto &mon : WindowManager::Monitor::Monitors()) {
			if(IsInside(mon.GetArea(), location))
				return mon;
		}

		throw std::runtime_error("Window is not any monitors.");
	}
	
	void Window::added(Layer &) {
        children.MoveBefore(*pointerlayer, children.GetSize());
    }
    
    void Window::SwitchToLocalPointers() {
        HidePointer();
        iswmpointer = false;
        ShowPointer();
    }
    
    void Window::SwitchToWMPointers() {
        HidePointer();
        iswmpointer = true;
        ShowPointer();
    }

	int Window::ClickThreshold = 5;
}
