#pragma once

#include "../Layer.h"
#include "../Geometry/Bounds.h"

namespace Gorgon { namespace Input {

		class Layer : public Gorgon::Layer {
		public:
			using Gorgon::Layer::Layer;

			Layer() : ClickEvent(this), MouseButtonEvent(this), MouseMoveEvent(this), MouseRegionEvent(this)  { }

			ConsumableEvent<Layer, Geometry::Point, Input::Mouse::Button> ClickEvent; //click
			ConsumableEvent<Layer, Geometry::Point, Input::Mouse::Button, bool> MouseButtonEvent; //down - up
			ConsumableEvent<Layer, Geometry::Point> MouseMoveEvent; //move
			ConsumableEvent<Layer, Geometry::Point, bool> MouseRegionEvent; //in - out

		protected:

			/// Propagates a mouse event. Some events will be called directly.
			virtual MouseHandler propagate_mouseevent(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button) override;

			/// Propagates a scroll event.
			//virtual MouseHandler propagate_scrollevent(Input::Mouse::ScrollType direction, Geometry::Point location, int amount) override;

		};

} }
