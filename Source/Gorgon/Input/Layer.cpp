#include "Layer.h"

namespace Gorgon { namespace Input {

    bool needsclip(Input::Mouse::EventType event) {
        switch(event) {
            case Input::Mouse::EventType::Over:
            case Input::Mouse::EventType::Out:
            case Input::Mouse::EventType::Up:
            case Input::Mouse::EventType::MovePressed:
                return false;
            default: 
                return true;
        }
    }
    
	bool Layer::propagate_mouseevent(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button, float amount, MouseHandler &handlers) {   
        if(event == Input::Mouse::EventType::OverCheck) {
            auto prev_t = Transform;
            
            Transform.Translate(-(float)bounds.Left, -(float)bounds.Top);
            
            mytransform = Transform;
            Transform = prev_t;
        }

		auto curlocation = mytransform * location;
        
        if(needsclip(event)) {
            auto prev_c = Clip;
            
            Clip -= Geometry::Size(bounds.Left, bounds.Top);
            
            if(bounds.Width() && bounds.Width() < Clip.Width)
                Clip.Width = bounds.Width();
            
            if(bounds.Height() && bounds.Height() < Clip.Height)   
                Clip.Height = bounds.Height();
            
            bool out = false;
            if(
                curlocation.X < 0 || 
                curlocation.Y < 0 || 
                curlocation.X >= Clip.Width || 
                curlocation.Y >= Clip.Height
            )
                out = true;
                
            
            Clip = prev_c;
            
            if(out) return false;
        }

        if(event == Input::Mouse::EventType::Over) {
            if(over)
                over(*this);
            
            return true;
        }
        else if(event == Input::Mouse::EventType::Out) {
            if(out)
                out(*this);
            
            return true;
        }
        else if(event == Input::Mouse::EventType::Up) {
            if(up)
                up(*this, curlocation, button);
            
            return true;
        }
        else if(event == Input::Mouse::EventType::MovePressed) {
            if(move)
                move(*this, curlocation);
            
            return true;
        }
        else if(event == Input::Mouse::EventType::OverCheck) {
            Gorgon::Layer::propagate_mouseevent(event, curlocation, button, amount, handlers);
        }
        else { //click/scroll/move/down
            if(Gorgon::Layer::propagate_mouseevent(event, curlocation, button, amount, handlers))
                return true;
        }

		if(hitcheck && !hitcheck(*this, curlocation)) 
			return false;
        
		if(event == Input::Mouse::EventType::Click && click) {
			click(*this, curlocation, button);
            
            handlers.Add(this);

			return true;
		}
		if(event == Input::Mouse::EventType::Down) {
            if(down) {
                down(*this, curlocation, button);
            }
            
            if(down || click) {
                return true;
            }
		}
		else if(event == Input::Mouse::EventType::Scroll_Vert && vscroll) {
			vscroll(*this, curlocation, amount);

            handlers.Add(this);

			return true;
		}
		else if(event == Input::Mouse::EventType::Scroll_Hor && hscroll) {
			hscroll(*this, curlocation, amount);

            handlers.Add(this);

			return true;
		}
		else if(event == Input::Mouse::EventType::Zoom && zoom) {
			zoom(*this, curlocation, amount);

            handlers.Add(this);

			return true;
		}
		else if(event == Input::Mouse::EventType::Rotate && rotate) {
			rotate(*this, curlocation, amount);

            handlers.Add(this);

			return true;
		}
		else if(event == Input::Mouse::EventType::OverCheck && over) {
            //dont call, window will decide which layers to call
            handlers.Add(this);

			return true;
		}

		return false;
	}

} }
