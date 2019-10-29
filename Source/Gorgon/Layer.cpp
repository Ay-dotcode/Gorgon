#include "Layer.h"
#include "Window.h"
#include "Graphics/Color.h"

namespace Gorgon {


	std::vector<Geometry::Transform3D> prev_Transform;
	std::vector<Geometry::Bounds>      prev_Clip;

	extern Geometry::Transform3D Transform;
	extern Graphics::RGBAf		  LayerColor;
	extern Geometry::Bounds      Clip;

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
		dotransformandclip();

		for(auto &l : children) {
            if(l.IsVisible()) {
                l.Render();
            }
		}

		reverttransformandclip();
	}

	bool Layer::propagate_mouseevent(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button, float amount, MouseHandler &handlers) {

		bool ret = false;

		dotransformandclip(true);

        if(event == Input::Mouse::EventType::Out) {
            throw std::logic_error("Regular layers cannot handle mouse events.");
        }
        else if(event == Input::Mouse::EventType::Up) {
            throw std::logic_error("Regular layers cannot handle mouse events.");
        }
        else {
            for(auto &l : children) {
                if(l.IsVisible()) {
                    if(l.propagate_mouseevent(event, location, button, amount, handlers)) {
                        ret=true;
                        break;
                    }
                }
            }
        }

		reverttransformandclip();

		return ret;
	}

	void Layer::dotransformandclip(bool inverse) {
		prev_Transform.push_back(Transform);
		prev_Clip.push_back(Clip);

		if(inverse)
			Transform.Translate(-(Gorgon::Float)bounds.Left, -(Gorgon::Float)bounds.Top, 0);
		else
			Transform.Translate((Gorgon::Float)bounds.Left, (Gorgon::Float)bounds.Top, 0);

		if(bounds.Left > 0)
			Clip.Left += bounds.Left;

		if(bounds.Top > 0)
			Clip.Top  += bounds.Top;

		int ebw = bounds.Width();
		if(bounds.Width() && bounds.Left < 0)
			ebw += bounds.Left;

		int ebh = bounds.Height();
		if(bounds.Height() && bounds.Top < 0)
			ebh += bounds.Top;

		if(bounds.Width() && ebw < Clip.Width())
			Clip.SetWidth(ebw);

		if(bounds.Height() && ebh < Clip.Height())
			Clip.SetHeight(ebh);
	}

	void Layer::reverttransformandclip() {
		Transform = prev_Transform.back();
		Clip = prev_Clip.back();

		prev_Transform.pop_back();
		prev_Clip.pop_back();
	}

    Layer::~Layer() {
        for(auto &l : children)
            Remove(l);

        for(auto &win : Window::Windows)
            win.deleting(this);

        if(parent) {
            parent->Remove(this);
        }
    }

	Geometry::Transform3D Transform;
	Graphics::RGBAf		  LayerColor;
	Geometry::Bounds      Clip;

	const Geometry::Bounds Layer::EntireRegion = {0,0,0,0};

}
