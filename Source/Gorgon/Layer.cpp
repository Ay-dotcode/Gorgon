#include "Layer.h"

namespace Gorgon {
	
	void Layer::Add(Layer &layer) {
		if(layer.parent)
			layer.parent->Remove(layer);

		layer.parent=this;
		children.Add(layer);
		layer.located(this);

		if(layer.bounds.Width() == 0)
			layer.bounds.SetWidth(bounds.Width());

		if(layer.bounds.Height() == 0)
			layer.bounds.SetHeight(bounds.Height());

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

	const Geometry::Bounds Layer::EntireRegion;

	Gorgon::MouseHandler Layer::propagate_mouseevent(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button) {
		auto prev = Transform;

		Transform.Translate(-(float)bounds.Left, -(float)bounds.Top);

		for(auto &l : children) {
			auto ret = l.propagate_mouseevent(event, location, button);

			if(ret.IsValid())
				return ret;
		}

		Transform = prev;

		return {};
	}

	Gorgon::MouseHandler Layer::propagate_scrollevent(Input::Mouse::ScrollType direction, Geometry::Point location, int amount) {
		auto prev = Transform;

		Transform.Translate(-(float)bounds.Left, -(float)bounds.Top);

		for(auto &l : children) {
			auto ret = l.propagate_scrollevent(direction, location, amount);

			if(ret.IsValid())
				return ret;
		}

		Transform = prev;

		return {};
	}

	Geometry::Transform3D Transform;
}
