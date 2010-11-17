#include "Selectbar.h"

namespace gorgonwidgets {
	void Selectbar::base_change(slider_change_event_params p, SliderBase &slider, Any data, string eventname) {
		ChangeEvent(selectbar_change_params(getValue(), getData()));
	}
	
	Selectbar::Selectbar(gorgonwidgets::SliderBP *BP, gorgonwidgets::IWidgetContainer &container, SelectbarOrientations Orientation) : 
		SliderBase(BP,container,(SliderStyles)Orientation),
		ChangeEvent("Change", this)
	{
		init();
	}
	
	Selectbar::Selectbar(gorgonwidgets::SliderBP *BP, SelectbarOrientations Orientation) : 
		SliderBase(BP,(SliderStyles)Orientation),
		ChangeEvent("Change", this)
	{
		init();
	}

	void Selectbar::init() {
		Setup(STS_TicksNames,true,true,false,false,SRA_JumpTo,true,true);

		minimum=0;
		maximum=-1;
		value=0;
		steps=1;
		numberofticks=0;
		SmoothDrag=false;
		SlidingDuration=0;

		SliderBase::ChangeEvent.Register(this, &Selectbar::base_change);
	}
}
