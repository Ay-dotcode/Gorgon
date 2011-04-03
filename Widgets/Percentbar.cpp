#include "Percentbar.h"

namespace gge { namespace widgets {
	Percentbar::Percentbar(gge::widgets::SliderBP *BP, gge::widgets::IWidgetContainer &container, PercentbarDirections direction) : Progressbar(BP,container) {
		init(direction);
	}

	Percentbar::Percentbar(gge::widgets::SliderBP *BP, PercentbarDirections direction) : Progressbar(BP) {
		init(direction);
	}

	void Percentbar::init(PercentbarDirections direction) {
		style=(SliderStyles)direction;
		Setup(STS_TicksNumbers,false,true,false,true,SRA_None,false,false);

		minimum=0;
		maximum=100;
		numberdistance=50;
		value=0;
		steps=0.01;
		OverlaySlidingDuration=0;
		OverlaySlidingDurationValueMod=200;
		setNumberDistance(numberdistance);
		
		setNumberofTicks(11);
		setNumberFormat(0,"%%");
	}
} }

