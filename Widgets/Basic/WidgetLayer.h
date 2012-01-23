#pragma once



#include "..\..\Engine\GraphicLayers.h"
#include "..\..\Engine\Input.h"





namespace gge { namespace widgets {

	class WidgetLayer : public graphics::Colorizable2DLayer, public input::mouse::CallbackProvider {
	public:


		WidgetLayer() : Colorizable2DLayer() { }

		WidgetLayer(const utils::Bounds &b) : Colorizable2DLayer(b) { }

		WidgetLayer(int L, int T, int R, int B) : Colorizable2DLayer(L,T,R,B) { }

		WidgetLayer(int X,int Y) : Colorizable2DLayer(X,Y) { }

		WidgetLayer(const utils::Point &p) : Colorizable2DLayer(p) { }



		virtual bool PropagateMouseEvent(input::mouse::Event::Type event, utils::Point location, int amount) {
			if(event==input::mouse::Event::Over) {
				if(!(isVisible && BoundingBox.isInside(location)))
					return false;

				bool ret=false;

				if(CallbackProvider::PropagateMouseEvent(event, location-BoundingBox.TopLeft(), amount))
					ret=true;

				if(LayerBase::PropagateMouseEvent(event, location, amount))
					ret=true;

				return ret;
			} 
			else if(event==input::mouse::Event::Out) {
				bool ret=false;

				int isin=(isVisible && BoundingBox.isInside(location)) ? 1 : 0;

				if(LayerBase::PropagateMouseEvent(event, location, amount & isin))
					ret=true;

				if(ret)
					isin=false;

				if(CallbackProvider::PropagateMouseEvent(event, location-BoundingBox.TopLeft(), amount & isin))
					ret=true;

				return ret;
			} 
			else {
				if(!(isVisible && BoundingBox.isInside(location)) && !(event==input::mouse::Event::Move && input::mouse::PressedObject))
					return false;

				if(LayerBase::PropagateMouseEvent(event, location, amount))
					return true;

				return CallbackProvider::PropagateMouseEvent(event, location-BoundingBox.TopLeft(), amount);
			}
		}
	};

} }