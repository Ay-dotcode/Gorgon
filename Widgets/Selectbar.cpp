#include "Selectbar.h"

namespace gge { namespace widgets {
	void Selectbar::base_change() {
		ChangeEvent(selectbar_change_params(getValue(), getData()));
	}
	
	Selectbar::Selectbar(gge::widgets::SliderBP *BP, gge::widgets::IWidgetContainer &container, SelectbarOrientations Orientation) : 
		SliderBase(BP,container,(SliderStyles)Orientation),
		ChangeEvent("Change", this)
	{
		init();
	}
	
	Selectbar::Selectbar(gge::widgets::SliderBP *BP, SelectbarOrientations Orientation) : 
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
} }

