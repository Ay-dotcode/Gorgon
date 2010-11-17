#pragma once

#include "SliderBase.h"
#include "SliderBP.h"

namespace gorgonwidgets {
	class Radiobar : public SliderBase {
	public:
		Radiobar(gorgonwidgets::SliderBP *BP, gorgonwidgets::IWidgetContainer &container);
		Radiobar(gorgonwidgets::SliderBP *BP);

		void setValue(float value) { SliderBase::setValue(value); }
		float getValue() { return value; }

		void SetRange(float Min, float Max, float steps=-1) { setMinimum(Min); setMaximum(Max); if(steps>0) setSteps(steps); }
		void setMinimum(float Min) { minimum=Min; setTickDistance(tickdistance); }
		void setMaximum(float Max) { maximum=Max; setTickDistance(tickdistance); }
		void setSteps(float Step) { steps=Step; }
		void setNumberofTicks(int ticks) { setTickDistance((maximum-minimum)/(numberofticks-1)); }
		void setTickDistance(float Distance) { numberedtickdistance*=tickdistance/Distance; tickdistance=Distance; numberofticks=(maximum-minimum)/tickdistance+1; }
		void setNumberDistance(float Distance) { numberedtickdistance=Distance/tickdistance; }
		void setNumberFormat(string Format) { numberformat=Format; }
		void setNumberFormat(int decimal, string unit="") { char tmp[50]; sprintf(tmp, "%%.%if%s", decimal, unit.data()); setNumberFormat(tmp); }
		float getMinimum() { return minimum; }
		float getMaximum() { return maximum; }
		float getSteps() { return steps; }
		float getTickDistance() { return (maximum-minimum)/numberofticks; }
		float getNumberDistance() { return numberedtickdistance*tickdistance; }
		string getNumberFormat() { return numberformat; }

		void showNumbers() { tickstyle=(SliderTickSyle)(tickstyle|STS_Numbers); }
		void hideNumbers() { tickstyle=(SliderTickSyle)(tickstyle&~STS_Numbers); }
		void showButtons() { Setup(STS_TicksNumbers,false,false,true,true,SRA_JumpTo,true,true); }
		void hideButtons() { Setup(STS_TicksNumbers,false,false,false,true,SRA_JumpTo,true,true); }
		void showTicks() { tickstyle=(SliderTickSyle)(tickstyle|STS_Ticks); }
		void hideTicks() { tickstyle=(SliderTickSyle)(tickstyle&~STS_Ticks); }

		bool isShowingNumbers() { return tickstyle&STS_Numbers; }
		bool isShowingTicks() { return tickstyle&STS_Ticks; }
		bool isShowingButtons() { return showbuttons; }

		EventChain<Radiobar, slider_change_event_params> ChangeEvent;

	protected:
		float tickdistance;

		void init();
	};
}
