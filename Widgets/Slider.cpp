#include "Slider.h"

namespace gorgonwidgets {
	Slider::Slider(gorgonwidgets::SliderBP *BP, gorgonwidgets::IWidgetContainer &container, SliderStyles Style) : 
		SliderBase(BP,container,Style),
		ChangeEvent("Change", this)
	{
		init();
	}

	Slider::Slider(gorgonwidgets::SliderBP *BP,  SliderStyles Style) : 
		SliderBase(BP,Style),
		ChangeEvent("Change", this)
	{
		init();
	}

	void Slider::init() {
		SliderTickSyle tickstyle;
		if(style!=SS_Horizontal && style!=SS_Verticle)
			tickstyle=STS_None;
		else
			tickstyle=STS_Ticks;

		tickstyle=STS_TicksNumbers;

		Setup(tickstyle,true,true,false,false,SRA_JumpTo,true,true);

		minimum=0;
		maximum=100;
		value=0;
		steps=1;
		numberofticks=11;
		tickdistance=10;
		numberedtickdistance=2.5;
		SlidingDuration=500;
		SmoothDrag=false;
		ButtonMoveTime=1000;
		FastMoveTime=500;

		numberformat="%.0f";

		SliderBase::ChangeEvent.DoubleLink(ChangeEvent);
	}
}
