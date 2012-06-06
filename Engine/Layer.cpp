#include "Layer.h"
#include "GGEMain.h"
#include "Graphics.h"

using namespace gge::input;
using namespace gge::utils;
using namespace gge::graphics::system;

namespace gge {
	namespace graphics {
		extern Point translate;
	}
	using namespace graphics;

	bool InputLayer::PropagateMouseEvent(input::mouse::Event::Type event, utils::Point location, int amount) {
		if(event==input::mouse::Event::Over || event==input::mouse::Event::DragOver) {
			if(!(IsVisible && BoundingBox.isInside(location)))
				return false;

			bool ret=false;

			if(EventProvider::PropagateMouseEvent(event, location-BoundingBox.TopLeft(), amount))
				ret=true;

			if(LayerBase::PropagateMouseEvent(event, location, amount))
				ret=true;

			return ret;
		} 
		else if(event==input::mouse::Event::Out || event==input::mouse::Event::DragOut) {
			bool ret=false;

			int isin=(IsVisible && BoundingBox.isInside(location)) ? 1 : 0;

			if(LayerBase::PropagateMouseEvent(event, location, amount & isin))
				ret=true;

			if(ret)
				isin=false;

			if(EventProvider::PropagateMouseEvent(event, location-BoundingBox.TopLeft(), amount & isin))
				ret=true;

			return ret;
		} 
		else {
			if(!(IsVisible && BoundingBox.isInside(location)) && 
				!(event==input::mouse::Event::Move && input::mouse::PressedObject) &&
				!(event==input::mouse::Event::DragMove)
				)
				return false;

			if(LayerBase::PropagateMouseEvent(event, location, amount))
				return true;

			return EventProvider::PropagateMouseEvent(event, location-BoundingBox.TopLeft(), amount);
		}
	}

	bool LayerBase::PropagateMouseEvent(input::mouse::Event::Type event, utils::Point location, int amount) {
		if(event==input::mouse::Event::Over || event==input::mouse::Event::DragOver) {
			if(!(IsVisible && BoundingBox.isInside(location)))
				return false;
			
			for(utils::SortedCollection<LayerBase>::Iterator i=SubLayers.First(); i.IsValid(); i.Next()) {
				if(i->PropagateMouseEvent(event, location-BoundingBox.TopLeft(), amount))
					return true;
			}

		}
		else if(event==input::mouse::Event::Out || event==input::mouse::Event::DragOut) {
			bool ret=false;
			int isin=(IsVisible && BoundingBox.isInside(location)) ? 1 : 0;

			for(utils::SortedCollection<LayerBase>::Iterator i=SubLayers.First(); i.IsValid(); i.Next()) {
				if(i->PropagateMouseEvent(event, location-BoundingBox.TopLeft(), isin & amount)) {
					ret=true;
					isin=false;
				}
			}

			return ret;
		}
		else if(event==input::mouse::Event::Click) {
			for(utils::SortedCollection<LayerBase>::Iterator i=SubLayers.First(); i.IsValid(); i.Next()) {
				if(i->PropagateMouseEvent(event, location-BoundingBox.TopLeft(), amount))
					return true;
			}
		}
		else {
			if(
				(IsVisible && BoundingBox.isInside(location)) || 
				(input::mouse::PressedObject && (event&input::mouse::Event::Move)) ||
				(event==input::mouse::Event::DragMove)  
				)
			{
				for(utils::SortedCollection<LayerBase>::Iterator i=SubLayers.First(); i.IsValid(); i.Next()) {
					if(i->PropagateMouseEvent(event, location-BoundingBox.TopLeft(), amount))
						return true;
				}
			}
		}

		return false;
	}

	void LayerBase::Render() {
		translate+=BoundingBox.TopLeft();
		glPushMatrix();
		glTranslatef((float)BoundingBox.Left, (float)BoundingBox.Top, 0);

		if(IsVisible) {
			for(utils::SortedCollection<LayerBase>::Iterator i=SubLayers.Last(); i.IsValid(); i.Previous()) {
				i->Render();
			}
		}

		glPopMatrix();
		translate-=BoundingBox.TopLeft();
	}

	LayerBase::LayerBase(int X,int Y ) : 
		parent(NULL), 
		wrapper(NULL), 
		BoundingBox(X, Y, X+Main.getWidth(), Y+Main.getHeight()),
		IsVisible(true) {

	}

	LayerBase::LayerBase( const utils::Point &p ) : 
		parent(NULL), 
		wrapper(NULL), 
		BoundingBox(p.x, p.y, p.x+Main.getWidth(), p.y+Main.getHeight()),
		IsVisible(true)
	{

	}

	LayerBase::LayerBase() : 
		parent(NULL), 
		wrapper(NULL),
		IsVisible(true),
		BoundingBox(Main.BoundingBox)
	{

	}

	LayerBase::~LayerBase() {
		if(parent)
			parent->Remove(this);

		for(auto it=SubLayers.First();it.IsValid();it.Next())
			it->parent=NULL;
	}

}