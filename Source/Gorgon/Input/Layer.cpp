#include "Layer.h"

namespace Gorgon { namespace Input {

	MouseHandler Layer::propagate_mouseevent(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button, int amount) {
		auto prev_t = Transform;
        auto prev_c = Clip;
        
		Transform.Translate(-(float)bounds.Left, -(float)bounds.Top);
        Clip -= Geometry::Size(bounds.Left, bounds.Top);
        
        if(bounds.Width() && bounds.Width() < Clip.Width)
            Clip.Width = bounds.Width();
        
        if(bounds.Height() && bounds.Height() < Clip.Height)   
            Clip.Height = bounds.Height();

		auto curlocation = Transform * location;
        
		if(
            curlocation.X < 0 || 
            curlocation.Y < 0 || 
            curlocation.X >= Clip.Width || 
            curlocation.Y >= Clip.Height
        )
            goto out;
        
		if(event == Input::Mouse::EventType::Click && click) {
			click(*this, curlocation, button);
            
            return this;
		}

    out:
		Transform = prev_t;
        Clip = prev_c;
        
		return nullptr;
	}

} }
