#pragma once

#include "Progressbar.h"
#include "SliderBase.h"

namespace gorgonwidgets {
	enum PercentbarDirections {
		PBD_Top=SS_Top,
		PBD_Bottom=SS_Bottom
	};
	class Percentbar : public Progressbar {
	public:
		Percentbar(SliderBP *BP, gorgonwidgets::IWidgetContainer &container, PercentbarDirections direction);
		Percentbar(SliderBP *BP, PercentbarDirections direction);

		virtual void SetRange(float min, float max) { minimum=min; maximum=max; setNumberDistance(numberdistance); }
		void showNumbers() { tickstyle=(SliderTickSyle)(tickstyle|STS_Numbers); }
		void hideNumbers() { tickstyle=(SliderTickSyle)(tickstyle&~STS_Numbers); }
		void showTicks() { tickstyle=(SliderTickSyle)(tickstyle|STS_Ticks); }
		void hideTicks() { tickstyle=(SliderTickSyle)(tickstyle&~STS_Ticks); }

		void setNumberofTicks(int ticks) { numberofticks=ticks; setNumberDistance(numberdistance); }
		int getNumberofTicks() { return numberofticks; }
		void setNumberDistance(float distance) { numberedtickdistance=distance/((maximum-minimum)/(numberofticks-1)); numberdistance=distance; }
		void setNumberFormat(string Format) { numberformat=Format; }
		void setNumberFormat(int decimal, string unit="") { char tmp[50]; sprintf(tmp, "%%.%if%s", decimal, unit.data()); setNumberFormat(tmp); }
		string getNumberFormat() { return numberformat; }

		bool isShowingNumbers() { return tickstyle&STS_Numbers; }
		bool isShowingTicks() { return tickstyle&STS_Ticks; }
	protected:
		float numberdistance;

		void init(PercentbarDirections direction);
	};
}