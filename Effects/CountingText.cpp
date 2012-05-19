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
		std::string text;

		if(Format.length()) {
			char t[50];
#ifdef MSVC
			sprintf_s(t, Format.data(), current);
#else
			sprintf(&text[0], Format.data(), current);
#endif
			text=t;
		} else {
			std::stringstream ss;
			ss<<std::setprecision(Decimals)<<current;
			text=ss.str();
		}

		Font->Print(target, X, Y, Width, text, Color, Align, Shadow);
	}

	animation::ProgressResult::Type CountingText::Progress() {
		if(current==to)
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
