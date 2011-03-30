#include "Scrollbar.h"

namespace gorgonwidgets {
	void Scrollbar::slider_changed() {
		IScroller::ChangeEvent();
	}
	Scrollbar::Scrollbar(gorgonwidgets::SliderBP *BP, gorgonwidgets::IWidgetContainer &container, ScrollbarAlignments Alignment) : 
		SliderBase(BP,container,(SliderStyles)Alignment), alignment(Alignment)
	{
		init();
	}
	Scrollbar::Scrollbar(gorgonwidgets::SliderBP *BP, ScrollbarAlignments Alignment) : 
		SliderBase(BP,(SliderStyles)Alignment), alignment(Alignment)
	{
		init();
	}

	void Scrollbar::setMaximum(int Max) { 
		int m=maximum; 
		maximum=Max; 

		if(alignment==SS_Verticle) 
			setValue(value+(maximum-m)); 

		if(value>maximum)
			setValue(maximum);
		if(value<minimum)
			setValue(minimum);


		FastMoveTime=1000*maximum/500;
		ButtonMoveTime=1000*maximum/100;
		SlidingDuration=FastMoveTime/2;
	}

	void Scrollbar::setValue(float value) {
		if(this->value != value)
			IScroller::ChangeEvent();

		SliderBase::setValue(value);
	}
	
	void Scrollbar::init() {
		Setup(STS_None,true,true,true,false,SRA_LargeIncrement,true,false);

		minimum=0;
		maximum=100;
		steps=1;
		buttonincrement=5;
		largeincrement=50;
		SmoothDrag=false;
		SlidingDuration=100;
		SlidingDurationValueMod=0;

		SliderBase::ChangeEvent.Register(this, &Scrollbar::slider_changed);

		setMaximum(100);
		setValue(0);
	}
}
