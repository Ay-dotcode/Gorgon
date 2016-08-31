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

	void Layer::propagate_mouseevent(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button, float amount, MouseHandler &handlers) {
		auto prev_t = Transform;
        auto prev_c = Clip;

		Transform.Translate(-(float)bounds.Left, -(float)bounds.Top);
        Clip -= Geometry::Size(bounds.Left, bounds.Top);
        
        if(bounds.Width() && bounds.Width() < Clip.Width)
            Clip.Width = bounds.Width();
        
        if(bounds.Height() && bounds.Height() < Clip.Height)   
            Clip.Height = bounds.Height();

        if(event == Input::Mouse::EventType::Out) {
            throw std::logic_error("Regular layers cannot handle mouse events.");
        }
        else if(event == Input::Mouse::EventType::Up) {
            throw std::logic_error("Regular layers cannot handle mouse events.");
        }
        else if(event == Input::Mouse::EventType::Over) {
            for(auto &l : children) {
                l.propagate_mouseevent(event, location, button, amount, handlers);
                
                if(handlers)
                    break;
            }
        }
        else { //click/scroll/move/down
            for(auto &l : children) {
                l.propagate_mouseevent(event, location, button, amount, handlers);

                if(handlers)
                    break;
            }
        }

		Transform = prev_t;
        Clip = prev_c;
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
