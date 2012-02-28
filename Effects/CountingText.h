#pragma once

#include "../Resource/GRE.h"
#include "../Engine/Animation.h"
#include "../Engine/Graphics.h"
#include "../Resource/BitmapFont.h"


namespace gge { namespace effects {

	////This effect displays a counting number going from a given value to another one.
	class CountingText : public animation::Base {
	public:
		////Color of the text, default is black
		graphics::RGBint Color;
		////Text shadow, default is none
		gge::ShadowParams Shadow;
		////Alignment of the text, default is left
		gge::TextAlignment::Type Align;
		////Width of the text
		int Width;
		////Number of decimal places, default is 0
		int Decimals;
		////Customized printing format, printf style that can feature a %f as the current value
		string Format;
		////The font to be used
		resource::BitmapFont *Font;


		////Initializes the effect
		CountingText(animation::Timer &controller, bool owner=false);

		////Initializes the effect
		explicit CountingText(bool create=false);

		void Adjust(resource::BitmapFont *Font=NULL, 
			graphics::RGBint color=graphics::RGBint(0xff000000), 
			int Width=0, TextAlignment::Type Align=TextAlignment::Left, 
			ShadowParams Shadow=ShadowParams(), int Decimals=0) {

		}

		float Current() { return current; }
		////Sets source and destination to the given values and allows time duration to reach the
		/// destination
		void Setup(float From, float To, int Time);
		////Sets current destination to the given value and allows time duration to reach it
		void Setup(float To, int Time) { Setup(current, To, Time); }
		
		////Prints the current text to a layer
		void Print(graphics::ColorizableImageTarget2D *target, int X, int Y);

	protected:
		float from;
		float to;
		float current;
		float speed;


		virtual animation::ProgressResult::Type Progress();
	};
} }
