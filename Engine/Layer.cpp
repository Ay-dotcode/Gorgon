#include "Layer.h"
#include "GGEMain.h"

using namespace gge::input;
using namespace gge::utils;

namespace gge {
	namespace graphics {
		extern Point translate;
	}
	using namespace graphics;

	bool InputLayer::PropagateMouseEvent(input::mouse::Event::Type event, utils::Point location, int amount) {
		if(LayerBase::PropagateMouseEvent(event, location, amount))
			return true;

		return EventProvider::PropagateMouseEvent(event, location-BoundingBox.TopLeft(), amount);
	}

	bool LayerBase::PropagateMouseEvent(input::mouse::Event::Type event, utils::Point location, int amount) {
		if( 
			(isVisible && BoundingBox.isInside(location)) || 
			input::mouse::Event::isUp(event) || 
			(event&input::mouse::Event::Out) || 
			(input::mouse::PressedObject && (event&input::mouse::Event::Move)) ) 
		{
			for(utils::SortedCollection<LayerBase>::Iterator i=SubLayers.First(); i.isValid(); i.Next()) {
				if(i->PropagateMouseEvent(event, location-BoundingBox.TopLeft(), amount))
					return true;
			}
		}

		return false;
	}

	void LayerBase::Render() {
		translate+=BoundingBox.TopLeft();
		if(isVisible) {
			for(utils::SortedCollection<LayerBase>::Iterator i=SubLayers.Last(); i.isValid(); i.Previous()) {
				i->Render();
			}
		}
		translate-=BoundingBox.TopLeft();
	}

	LayerBase::LayerBase(int X,int Y ) : 
		parent(NULL), 
		wrapper(NULL), 
		BoundingBox(X, Y, X+Main.getWidth(), Y+Main.getHeight()),
		isVisible(true) {

	}

	LayerBase::LayerBase( const utils::Point &p ) : 
		parent(NULL), 
		wrapper(NULL), 
		BoundingBox(p.x, p.y, p.x+Main.getWidth(), p.y+Main.getHeight()),
		isVisible(true)
	{

	}

	LayerBase::LayerBase() : 
		parent(NULL), 
		wrapper(NULL),
		isVisible(true),
		BoundingBox(Main.BoundingBox)
	{

	}

}