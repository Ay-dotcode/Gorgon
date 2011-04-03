#include "NamedSlider.h"

namespace gge { namespace widgets {
	NamedSlider::NamedSlider(gge::widgets::SliderBP *BP, gge::widgets::IWidgetContainer &container, NamedSliderDirections direction) : 
		Slider(BP,container,(SliderStyles)direction), numberdistance(0)
	{
		init();
	}

	NamedSlider::NamedSlider(gge::widgets::SliderBP *BP, NamedSliderDirections direction) : 
		Slider(BP,(SliderStyles)direction), numberdistance(0)
	{
		init();
	}

	void NamedSlider::init() {
		Setup(STS_LocatedTicksNames,true,true,false,false,SRA_JumpTo,true,true);
	}
} }

