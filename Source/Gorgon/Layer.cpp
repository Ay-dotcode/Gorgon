#include "Layer.h"
#include "Window.h"
#include "Graphics/Color.h"
#include "Input/Mouse.h"

namespace Gorgon {

    namespace Input {
        
        bool needsclip(Input::Mouse::EventType event);
        
    }
    
	std::vector<Geometry::Transform3D> prev_Transform;
    std::vector<Geometry::Bounds>      prev_Clip;
    std::vector<Geometry::Point>       prev_Offset;

	extern Geometry::Transform3D Transform;
	extern Graphics::RGBAf		 LayerColor;
	extern Geometry::Bounds      Clip;
    extern Geometry::Point       Offset;

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
        
        auto curlocation = Transform * location;
        
        auto size = GetCalculatedSize();
        
        if(Input::needsclip(event)) {
            bool out = false;
            if(
                curlocation.X < 0 || 
                curlocation.Y < 0 || 
                curlocation.X >= size.Width || 
                curlocation.Y >= size.Height
            )
                out = true;
                
                if(out) {
                    reverttransformandclip();
                    
                    return false;
                }
        }
        
        
        if(event == Input::Mouse::EventType::Out) {
            throw std::logic_error("Regular layers cannot handle mouse events.");
        }
        else if(event == Input::Mouse::EventType::Up) {
            throw std::logic_error("Regular layers cannot handle mouse events.");
        }
        else {
            for(int i=children.GetSize()-1; i>=0; i--) {
            //for(int i=0; i<children.GetSize(); i++) {
                auto &l = children[i];
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
        prev_Offset.push_back(Offset);

        Offset += bounds.TopLeft();

		if(inverse)
			Transform.Translate(-(Gorgon::Float)bounds.Left, -(Gorgon::Float)bounds.Top, 0);
		else
			Transform.Translate((Gorgon::Float)bounds.Left, (Gorgon::Float)bounds.Top, 0);


        auto curbounds = Geometry::Bounds(Offset, bounds.GetSize());

        if(Clip.Left < curbounds.Left)
            Clip.Left = curbounds.Left;

        if(bounds.Width() > 0 && Clip.Right > curbounds.Right)
            Clip.Right = curbounds.Right;

        if(Clip.Top < curbounds.Top)
            Clip.Top = curbounds.Top;

        if(bounds.Height() > 0 && Clip.Bottom > curbounds.Bottom)
            Clip.Bottom = curbounds.Bottom;

        if(Clip.Left > Clip.Right)
            Clip.Right = Clip.Left;

        if(Clip.Top > Clip.Bottom)
            Clip.Bottom = Clip.Top;
    }

	void Layer::reverttransformandclip() {
		Transform = prev_Transform.back();
		Clip = prev_Clip.back();
        Offset = prev_Offset.back();

		prev_Transform.pop_back();
		prev_Clip.pop_back();
        prev_Offset.pop_back();
	}

    Layer::~Layer() {
        while(children.GetSize())
            Remove(*children.First());

        for(auto &win : Window::Windows)
            win.deleting(this);

        if(parent) {
            parent->Remove(this);
        }
    }

	Geometry::Transform3D Transform;
    Graphics::RGBAf		  LayerColor;
    Geometry::Bounds      Clip;
    Geometry::Point       Offset;

	const Geometry::Bounds Layer::EntireRegion = {0,0,0,0};

}
