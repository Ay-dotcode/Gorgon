#pragma once

#include "../Utils/GGE.h"
#include "../Resource/GRE.h"
#include "../Engine/Animator.h"
#include "../Engine/Graphics.h"
#include "../Resource/BitmapFontResource.h"

using namespace gge;
using namespace gre;

namespace geffects {

	////This effect displays a counting number going from a given value to another one.
	class CountingText : public AnimatorBase {
	public:
		////This event is fired when the animation
		/// completes
		EventChain<CountingText, empty_event_params> FinishedEvent;

		////Color of the text, default is black
		RGBint Color;
		////Text shadow, default is none
		ShadowParams Shadow;
		////Alignment of the text, default is left
		TextAlignment Align;
		////Width of the text
		int Width;
		////Number of decimal places, default is 0
		int Decimals;
		////Customized printing format, printf style that can feature a %f as the current value
		string Format;
		////The font to be used
		BitmapFontResource *Font;


		////Initializes the effect
		CountingText(BitmapFontResource *Font=NULL, RGBint color=RGBint(0xff000000), int Width=0, TextAlignment Align=TEXTALIGN_LEFT, ShadowParams Shadow=ShadowParams(), int Decimals=0) : 
			Color(color),
			Font(Font),
			Shadow(Shadow),
			Width(Width),
			Align(Align),
			Decimals(Decimals),
			from(0), to(0), speed(0), current(0), FinishedEvent("Finished", this)
		{ 
			AnimatorBase::FinishedEvent=FinishedEvent;
		}

		float Current() { return current; }
		////Sets source and destination to the given values and allows time duration to reach the
		/// destination
		void Setup(float From, float To, int Time);
		////Sets current destination to the given value and allows time duration to reach it
		void Setup(float To, int Time) { Setup(current, To, Time); }
		
		////Prints the current text to a layer
		void Print(I2DColorizableGraphicsTarget *target, int X, int Y);

	protected:
		float from;
		float to;
		float current;
		float speed;


		virtual bool isFinished();
		virtual void Process(int Time);
	};
}