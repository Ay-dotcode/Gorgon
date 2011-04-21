#include "Tinting.h"
#include "../Engine/GGEMain.h"

using namespace gge::graphics;

namespace gge { namespace effects {
	void Tinting::Setup(RGBint From, RGBint To, int Time) {

		from=From;
		current=From;
		to=To;

		Target->Ambient=from;

		if(Time) {
			speed.a=(float)(to.a-from.a)/Time;
			speed.r=(float)(to.r-from.r)/Time;
			speed.g=(float)(to.g-from.g)/Time;
			speed.b=(float)(to.b-from.b)/Time;
		} else {
			speed.a=0;
			speed.r=0;
			speed.g=0;
			speed.b=0;
		}

		this->progressed=0;
		this->lasttick=Main.CurrentTime;
		this->Play();
	}

	bool Tinting::isFinished() {
		return current.a==to.a && current.r==to.r && current.g==to.g && current.b==to.b;
	}

	void Tinting::Process(int Time) {
		if(from.a>to.a) {
			current.a=from.a+Time*speed.a;

			if(current.a<to.a)
				current.a=to.a;
		} else {
			current.a=from.a+Time*speed.a;
			if(current.a>to.a)
				current.a=to.a;
		}
		Target->Ambient.a=(Byte)current.a*255;

		if(from.r>to.r) {
			current.r=from.r+Time*speed.r;

			if(current.r<to.r)
				current.r=to.r;
		} else {
			current.r=from.r+Time*speed.r;
			if(current.r>to.r)
				current.r=to.r;
		}
		Target->Ambient.r=(Byte)current.r*255;

		if(from.g>to.g) {
			current.g=from.g+Time*speed.g;

			if(current.g<to.g)
				current.g=to.g;
		} else {
			current.g=from.g+Time*speed.g;
			if(current.g>to.g)
				current.g=to.g;
		}
		Target->Ambient.g=(Byte)current.g*255;

		if(from.b>to.b) {
			current.b=from.b+Time*speed.b;

			if(current.b<to.b)
				current.b=to.b;
		} else {
			current.b=from.b+Time*speed.b;
			if(current.b>to.b)
				current.b=to.b;
		}
		Target->Ambient.b=(Byte)current.b*255;
	}
} }
