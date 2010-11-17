#include "Progressbar.h"

namespace gorgonwidgets {
	Progressbar::Progressbar(gorgonwidgets::SliderBP *BP, gorgonwidgets::IWidgetContainer &container) : SliderBase(BP,container,SS_Horizontal) {
		init();
	}

	Progressbar::Progressbar(gorgonwidgets::SliderBP *BP) : SliderBase(BP,SS_Horizontal) {
		init();
	}

	void Progressbar::init() {
		Setup(STS_None,false,true,false,true,SRA_None,false,false);

		minimum=0;
		maximum=100;
		value=0;
		steps=0.01;
		OverlaySlidingDuration=0;
		OverlaySlidingDurationValueMod=200;

		SetPassive();
	}
}
