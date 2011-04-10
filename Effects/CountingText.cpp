#include "CountingText.h"

using namespace gge;
using namespace gre;

namespace geffects {
	void CountingText::Setup(float From, float To, int Time) {
		if(Time)
			speed=(To-From)/Time;
		else
			speed=0;

		from=From;
		current=from;
		to=To;

		this->progressed=0;
		this->Play();
	}

	bool CountingText::isFinished() {
		return current==to;
	}

	void CountingText::Print(I2DColorizableGraphicsTarget *target, int X, int Y) {
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

	void CountingText::Process(int Time) {
		if(from>to) {
			current=from+Time*speed;
			if(current<to)
				current=to;
		} else {
			current=from+Time*speed;
			if(current>to)
				current=to;
		}
	}
}