#include "Layer.h"

namespace gge {
	extern int trX,trY;
	extern MouseEventObject *pressedObject;

	bool InputLayer::PropagateMouseEvent(MouseEventType event, int x, int y, void *data) {
		if( isVisible && ((x>X && y>Y && x<X+W && y<Y+H) || (event&MOUSE_EVENT_UP) || (pressedObject && event&MOUSE_EVENT_MOVE)) ) {
			if(LayerBase::PropagateMouseEvent(event, x-X, y-Y, data))
				return true;

			if( isVisible && ((x>X && y>Y && x<X+W && y<Y+H) || (event&MOUSE_EVENT_UP) || (pressedObject->parent==this && event&MOUSE_EVENT_MOVE)) )
				return BasicPointerTarget::PropagateMouseEvent(event, x-X, y-Y, data);
			else
				return false;
		}

		return false;
	}

	bool LayerBase::PropagateMouseEvent(MouseEventType event, int x, int y, void *data) {
		if( isVisible && ((x>X && y>Y && x<X+W && y<Y+H) || (event&MOUSE_EVENT_UP) || (pressedObject && event&MOUSE_EVENT_MOVE)) ) {
			LinkedListOrderedIterator<LayerBase> it=SubLayers;
			LayerBase *layer;
			
			while(layer=it)
				if(layer->PropagateMouseEvent(event, x-X, y-Y, data))
					return true;
		}

		return false;
	}

	bool InputLayer::PropagateMouseScrollEvent(int amount, MouseEventType event, int x, int y, void *data) {
		if( isVisible && ((x>X && y>Y && x<X+W && y<Y+H)) ) {
			if(LayerBase::PropagateMouseScrollEvent(amount, event, x-X, y-Y, data))
				return true;

			if( isVisible && ((x>X && y>Y && x<X+W && y<Y+H) || (event&MOUSE_EVENT_UP) || (pressedObject->parent==this && event&MOUSE_EVENT_MOVE)) )
				return BasicPointerTarget::PropagateMouseScrollEvent(amount, event, x-X, y-Y, data);
			else
				return false;
		}

		return false;
	}

	bool LayerBase::PropagateMouseScrollEvent(int amount, MouseEventType event, int x, int y, void *data) {
		if( isVisible && ((x>X && y>Y && x<X+W && y<Y+H)) ) {
			LinkedListOrderedIterator<LayerBase> it=SubLayers;
			LayerBase *layer;
			
			while(layer=it)
				if(layer->PropagateMouseScrollEvent(amount, event, x-X, y-Y, data))
					return true;
		}

		return false;
	}

	void LayerBase::Render() {
		trX+=X;
		trY+=Y;
		if(isVisible) {
			LinkedListOrderedIterator<LayerBase> it=SubLayers.GetReverseOrderedIterator();
			LayerBase *layer;
			
			while(layer=it)
				layer->Render();
		}
		trX-=X;
		trY-=Y;
	}
}