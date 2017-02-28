#pragma once

#include "../Engine/Layer.h"
#include "../Engine/Input.h"

namespace gge { namespace widgets {

	class ExtenderLayer : public LayerBase {
	public:
		ExtenderLayer() : LayerBase() { }

		ExtenderLayer(const utils::Bounds &b) : LayerBase(b) { }

		ExtenderLayer(int L, int T, int R, int B) : LayerBase(L,T,R,B) { }

		ExtenderLayer(int X,int Y) : LayerBase(X,Y) { }

		ExtenderLayer(const utils::Point &p) : LayerBase(p) { }

		virtual bool PropagateMouseEvent(input::mouse::Event::Type event, utils::Point location, int amount) {
			if(event==input::mouse::Event::Over || event==input::mouse::Event::DragOver) {
				if(!IsVisible)
					return false;

				bool ret=false;

				if(LayerBase::PropagateMouseEvent(event, location, amount))
					ret=true;

				return ret;
			} 
			else if(event==input::mouse::Event::Out || event==input::mouse::Event::DragOut) {
				bool ret=false;

				if(LayerBase::PropagateMouseEvent(event, location, amount))
					ret=true;

				return ret;
			} 
			else {
				if(!IsVisible)
					return false;

				return LayerBase::PropagateMouseEvent(event, location, amount);
			}
		}
	};

}}