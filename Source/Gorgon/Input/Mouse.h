#pragma once
#include "../Enum.h"

namespace Gorgon { namespace Input {

	namespace Mouse {

		/// The type of a mouse event. Out/Up occurs if Over/Down is handled. 
		/// Click occurs only if Down is not handled. 
		enum class EventType {
			Over,
			Move,
			Out,
			Down,
			Click,
			Up,
		};

		enum class Button {
			Left   = 1 ,
			Right  = 2 ,
			Middle = 4 ,
			X1	   = 8 ,
			X2	   = 16
		};

		DefineEnumStrings(
			Button, 
			{Button::Left, "Left"}
		);

		enum class ScrollType {
			Vertical,
			Horizontal,
			Zoom
		};

	}

}}
