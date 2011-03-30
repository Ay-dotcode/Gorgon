#pragma once

#include "Progressbar.h"
#include "SliderBase.h"

namespace gorgonwidgets {
	enum HeatbarDirections {
		HBD_Left=SS_Left,
		HBD_Right=SS_Right
	};
	class Heatbar : public Progressbar {
	public:
		Heatbar(SliderBP *BP, gorgonwidgets::IWidgetContainer &container, HeatbarDirections direction);
		Heatbar(SliderBP *BP, HeatbarDirections direction);

		virtual void SetRange(float min, float max) { minimum=min; maximum=max; setNumberDistance(numberdistance); }
		void showNumbers() { tickstyle=(SliderTickSyle)(tickstyle|STS_Numbers); }
		void hideNumbers() { tickstyle=(SliderTickSyle)(tickstyle&~STS_Numbers); }
		void showTicks() { tickstyle=(SliderTickSyle)(tickstyle|STS_Ticks); }
		void hideTicks() { tickstyle=(SliderTickSyle)(tickstyle&~STS_Ticks); }

		void setNumberofTicks(int ticks) { numberofticks=ticks; setNumberDistance(numberdistance); }
		int getNumberofTicks() { return numberofticks; }
		void setNumberDistance(float distance) { numberedtickdistance=distance/((maximum-minimum)/(numberofticks-1)); numberdistance=distance; }
		void setNumberFormat(string Format) { numberformat=Format; }
		void setNumberFormat(int decimal, string unit="") { char tmp[50]; sprintf_s<50>(tmp, "%%.%if%s", decimal, unit.data()); setNumberFormat(tmp); }
		string getNumberFormat() { return numberformat; }

		bool isShowingNumbers() { return tickstyle&STS_Numbers!=0; }
		bool isShowingTicks() { return tickstyle&STS_Ticks!=0; }

	protected:
		float numberdistance;

		void init(HeatbarDirections direction);
	};
}