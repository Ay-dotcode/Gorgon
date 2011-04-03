#include "Heatbar.h"

namespace gge { namespace widgets {
	Heatbar::Heatbar(gge::widgets::SliderBP *BP, gge::widgets::IWidgetContainer &container, HeatbarDirections direction) : 
		Progressbar(BP,container) 
	{
		init(direction);
	}

	Heatbar::Heatbar(gge::widgets::SliderBP *BP, HeatbarDirections direction) : 
		Progressbar(BP) 
	{
		init(direction);
	}

	void Heatbar::init(HeatbarDirections direction) {
		style=(SliderStyles)direction;
		Setup(STS_TicksNumbers,false,true,false,true,SRA_None,false,false);

		minimum=0;
		maximum=100;
		numberdistance=20;
		value=0;
		steps=0.01;
		OverlaySlidingDuration=0;
		OverlaySlidingDurationValueMod=200;
		setNumberDistance(numberdistance);
		AlwaysShowCap=true;
	}
} }
