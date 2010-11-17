#include "NamedSlider.h"

namespace gorgonwidgets {
	NamedSlider::NamedSlider(gorgonwidgets::SliderBP *BP, gorgonwidgets::IWidgetContainer &container, NamedSliderDirections direction) : 
		Slider(BP,container,(SliderStyles)direction), numberdistance(0)
	{
		init();
	}

	NamedSlider::NamedSlider(gorgonwidgets::SliderBP *BP, NamedSliderDirections direction) : 
		Slider(BP,(SliderStyles)direction), numberdistance(0)
	{
		init();
	}

	void NamedSlider::init() {
		Setup(STS_LocatedTicksNames,true,true,false,false,SRA_JumpTo,true,true);
	}
}
