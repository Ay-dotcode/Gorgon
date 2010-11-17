#include "Radiobar.h"

namespace gorgonwidgets {
	Radiobar::Radiobar(gorgonwidgets::SliderBP *BP, gorgonwidgets::IWidgetContainer &container) : 
		SliderBase(BP,container,SS_Bottom),
		ChangeEvent("Change", this)
	{
		init();
	}

	Radiobar::Radiobar(gorgonwidgets::SliderBP *BP) : 
		SliderBase(BP,SS_Bottom),
		ChangeEvent("Change", this)
	{
		init();
	}

	void Radiobar::init()  {
		Setup(STS_TicksNumbers,false,false,true,true,SRA_JumpTo,true,true);

		minimum=0;
		maximum=100;
		value=0;
		steps=1;
		numberofticks=11;
		tickdistance=10;
		buttonincrement=1;
		numberedtickdistance=5;
		SmoothDrag=false;

		numberformat="%.0f";

		SliderBase::ChangeEvent=ChangeEvent;
		}
}
