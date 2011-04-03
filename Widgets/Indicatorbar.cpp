#include "Indicatorbar.h"

namespace gge { namespace widgets {
	Indicatorbar::Indicatorbar(gge::widgets::SliderBP *BP, gge::widgets::IWidgetContainer &container, IndicatorbarDirections direction) : Progressbar(BP,container) {
		init(direction);
	}

	Indicatorbar::Indicatorbar(gge::widgets::SliderBP *BP, IndicatorbarDirections direction) : Progressbar(BP) {
		init(direction);
	}

	void Indicatorbar::init(IndicatorbarDirections direction) {
		style=(SliderStyles)direction;
		Setup(STS_LocatedTicksNames,false,true,false,true,SRA_None,false,false);

		minimum=0;
		maximum=100;
		numberdistance=20;
		value=0;
		steps=0.01;
		OverlaySlidingDuration=0;
		OverlaySlidingDurationValueMod=200;
		setNumberDistance(numberdistance);
	}
} }
