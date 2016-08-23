#include "Layer.h"

namespace Gorgon { namespace Input {

	MouseHandler Layer::propagate_mouseevent(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button) {
		Transform.Translate(-(float)bounds.Left, -(float)bounds.Top);

		auto curlocation = Transform * location;
		if(curlocation.X < 0 || curlocation.X >= bounds.Width() || curlocation.Y < 0 || curlocation.Y >= bounds.Height()) return { };

		if(event == Input::Mouse::EventType::Click) {
			ClickEvent(curlocation, button);
		}

		return { };
	}

} }