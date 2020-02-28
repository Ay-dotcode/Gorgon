#include "Window.h"
#include "OS.h"

#include "Graphics/Layer.h"
#include "Graphics/Color.h"
#include "GL/FrameBuffer.h"
#include "Input/DnD.h"

#include "Config.h"


namespace Gorgon {

	extern Graphics::RGBAf LayerColor;
	Geometry::Size ScreenSize;
    
    void Window::Swap(Window &other) {
        using std::swap;
        swap(data, other.data);

        swap(pressed, other.pressed);
        down.Swap(other.down);
        over.Swap(other.over);
        swap(allowresize, other.allowresize);
        swap(cursorover, other.cursorover);
        swap(mousedownlocation, other.mousedownlocation);
        swap(mouselocation, other.mouselocation);
        
        swap(pointerlayer, other.pointerlayer);
        
		swap(iswmpointer, other.iswmpointer);
		swap(showptr, other.showptr);
        swap(switchbacktolocalptr, other.switchbacktolocalptr);
        swap(glsize, other.glsize);

		Pointers.Swap(other.Pointers);

        if(data) {
            windows.Add(this);
		}
        else
            windows.Remove(this);
        
        if(other.data) {
			windows.Add(other);
		}
        else
            windows.Remove(other);

		updatedataowner();
		other.updatedataowner();
        
        Layer::Swap(other);
	}
	
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
        Offset = {0, 0};
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
        
		if(Input::IsDragging()) {
			//to support dragging without the need of holding mouse down (start the event after click)
		}
		else {
			if(down) {
				down[0].propagate_mouseevent(Input::Mouse::EventType::DownPressed, location, button, 1, down);
			}
			else {
				Layer::propagate_mouseevent(Input::Mouse::EventType::Down, location, button, 1, down);
			}
		}
    }
    
    void Window::mouse_up(Geometry::Point location, Input::Mouse::Button button) {
        pressed = pressed & ~button;
        
		Transform = {};
        Clip = bounds;

		if(Input::IsDragging()) {
			if(Input::GetDragOperation().HasTarget()) {
				MouseHandler handler;
				Input::GetDragOperation().GetTarget().propagate_mouseevent(Input::Mouse::EventType::Up, location, button, 0, handler);
			}
			else {
				Input::CancelDrag();
			}

			if(down) {
				down[0].propagate_mouseevent(Input::Mouse::EventType::Up, location, button, 0, down);

				if(pressed == Input::Mouse::Button::None)
					down.Clear();
			}
		}
		else {
			if(down) {
				down[0].propagate_mouseevent(Input::Mouse::EventType::Up, location, button, 0, down);
            
				if(pressed == Input::Mouse::Button::None)
					down.Clear();
			}
			else {
				if(mousedownlocation.Distance(location) <= WindowManager::ClickThreshold) {
					MouseHandler handler;
					Layer::propagate_mouseevent(Input::Mouse::EventType::Click, location, button, 1, handler);
				}
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
        Layer::propagate_mouseevent(Input::Mouse::EventType::HitCheck, mouselocation, Input::Mouse::Button::None, 0, newover);
        
        //first check outs
        for(auto &l : over.layers) {
            if(newover.layers.Find(l) == newover.layers.end() || (Input::IsDragging() && !dynamic_cast<Input::DropTarget*>(&l))) {
                l.propagate_mouseevent(Input::Mouse::EventType::Out, mouselocation, Input::Mouse::Button::None, 0, newover);
            }
        }
       
        //then signal new overs
        for(auto &l : newover.layers) {
            if(over.layers.Find(l) == over.layers.end() && !(Input::IsDragging() && !dynamic_cast<Input::DropTarget*>(&l))) {
                l.propagate_mouseevent(Input::Mouse::EventType::Over, mouselocation, Input::Mouse::Button::None, 1, newover);
            }
        }
        
        over.layers.Swap(newover.layers);
        
        if(down && !Input::IsDragging()) {
            down[0].propagate_mouseevent(Input::Mouse::EventType::MovePressed, mouselocation, Input::Mouse::Button::None, 0, down);
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

		throw std::runtime_error("Window is not on any monitors.");
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
}
