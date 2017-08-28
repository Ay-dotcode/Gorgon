/// @file

#pragma once
#include "../Enum.h"

namespace Gorgon { namespace Input {

	namespace Mouse {

		/// The type of a mouse event. Out/Up occurs if Over/Down is handled. 
		/// Click occurs only if Down is not handled. 
		ENUMCLASS EventType {
            HitCheck, ///< Checks if the coordinate hits the layer, always called first
			Over,
			Move,
			MovePressed, ///< Move event while a button is pressed
			Out,
			Down,
			DownPressed, ///< Down event while a button is already pressed
			Click,
			Up,
            Scroll_Vert,
            Scroll_Hor,
            Zoom,
            Rotate
		};

        /// Lists the mouse button constants
		ENUMCLASS Button {
            None   = 0 ,
			Left   = 1 ,
			Right  = 2 ,
			Middle = 4 ,
			X1	   = 8 ,
			X2	   = 16,
			All    = 31,
		};
        
        inline Button operator |(Button l, Button r) {
            return Button((int)l | (int)r);
        }
        
        inline Button operator &(Button l, Button r) {
            return Button((int)l & (int)r);
        }
        
        inline bool operator &&(Button l, Button r) {
            return (int)(l&r) > 0;
        }
        
        inline Button operator ~(Button l) {
            return Button(~(unsigned)l);
        }

		DefineEnumStrings(
			Button, 
			{Button::Left, "Left"},
			{Button::Right, "Right"},
			{Button::Middle, "Middle"},
			{Button::X1, "X1"},
			{Button::X2, "X2"},
		);

		ENUMCLASS ScrollType {
			Vertical,
			Horizontal,
			Zoom
		};

	}

}}
