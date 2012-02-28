#include "CountingText.h"

using namespace gge;
using namespace gge::graphics;
using namespace gge::resource;
using namespace gge::animation;

namespace gge { namespace effects {

	CountingText::CountingText( animation::Timer &controller, bool owner/*=false*/ ) : 
		Base(controller, owner),
		Color(0xff000000),
		Font(NULL),
		Shadow(),
		Width(0),
		Align(TextAlignment::Center),
		Decimals(0),
		from(0), to(0), speed(0), current(0)
	{

	}

	CountingText::CountingText( bool create/*=false*/ ) : 
		Base(create),
		Color(0xff000000),
		Font(NULL),
		Shadow(),
		Width(0),
		Align(TextAlignment::Center),
		Decimals(0),
		from(0), to(0), speed(0), current(0)
	{

	}


	void CountingText::Setup(float From, float To, int Time) {
		if(Time)
			speed=(To-From)/Time;
		else
			speed=0;

		from=From;
		current=from;
		to=To;

		if(Controller) Controller->ResetProgress();
	}

	void CountingText::Print(ColorizableImageTarget2D *target, int X, int Y) {
		char text[50];

		if(Format.length()) {
			sprintf_s<50>(text, Format.data(), current);
		} else {
			char tmp[10];
			sprintf_s<10>(tmp, "%%.%if", Decimals);
			sprintf_s<50>(text, tmp, current);
		}

		Font->Print(target, X, Y, Width, text, Color, Align, Shadow);
	}

	animation::ProgressResult::Type CountingText::Progress() {
		if(from==to)
			return animation::ProgressResult::Finished;

		if(!Controller)
			return animation::ProgressResult::None;

		if(Controller->GetProgress()<0) {
			throw std::runtime_error("CountingText cannot handle negative animation time.");
		}

		int Time=Controller->GetProgress();

		if(from>to) {
			current=from+Time*speed;
			if(current<to)
				current=to;
		} else {
			current=from+Time*speed;
			if(current>to)
				current=to;
		}

		if(from==to)
			return animation::ProgressResult::Finished;
		else
			return animation::ProgressResult::None;
	}

} }
