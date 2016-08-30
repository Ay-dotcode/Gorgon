#include "Layer.h"
#include "Window.h"

namespace Gorgon {
	
	void Layer::Add(Layer &layer) {
		if(layer.parent)
			layer.parent->Remove(layer);

		layer.parent=this;
		children.Add(layer);
		layer.located(this);

		added(layer);
	}

	void Layer::Insert(Layer &layer, long under) {
		if(layer.parent)
			layer.parent->Remove(layer);

		layer.parent=this;
		children.Insert(layer, under);
		layer.located(this);

		if(layer.bounds.Width() == 0) 
			layer.bounds.SetWidth(bounds.Width());

		if(layer.bounds.Height() == 0) 
			layer.bounds.SetHeight(bounds.Height());

		added(layer);
	}

	void Layer::Render() {
		auto prev = Transform;

		Transform.Translate((float)bounds.Left, (float)bounds.Top);

		for(auto &l : children) {
			l.Render();
		}

		Transform = prev;
	}

	Gorgon::MouseHandler Layer::propagate_mouseevent(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button, int amount) {
		auto prev_t = Transform;
        auto prev_c = Clip;

		Transform.Translate(-(float)bounds.Left, -(float)bounds.Top);
        Clip -= Geometry::Size(bounds.Left, bounds.Top);
        
        if(bounds.Width() && bounds.Width() < Clip.Width)
            Clip.Width = bounds.Width();
        
        if(bounds.Height() && bounds.Height() < Clip.Height)   
            Clip.Height = bounds.Height();

		for(auto &l : children) {
			auto ret = l.propagate_mouseevent(event, location, button, amount);

			if(ret)
				return ret;
		}

		Transform = prev_t;
        Clip = prev_c;

		return nullptr;
	}

    Layer::~Layer() {
        for(auto &win : Window::Windows)
            win.deleting(this);

        if(parent) {
            parent->Remove(this);
        }
    }

	Geometry::Transform3D Transform;
    Geometry::Size        Clip;

	const Geometry::Bounds Layer::EntireRegion = {0,0,0,0};

}
