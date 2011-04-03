#pragma once

#include "Progressbar.h"
#include "SliderBase.h"

namespace gge { namespace widgets {
	enum IndicatorbarDirections {
		IBD_Left=SS_Left,
		IBD_Right=SS_Right,
		IBD_Top=SS_Top,
		IBD_Bottom=SS_Bottom
	};
	class Indicatorbar : public Progressbar {
	public:
		Indicatorbar(SliderBP *BP, gge::widgets::IWidgetContainer &container, IndicatorbarDirections direction);
		Indicatorbar(SliderBP *BP, IndicatorbarDirections direction);

		virtual void SetRange(float min, float max) { minimum=min; maximum=max; setNumberDistance(numberdistance); }
		void showNumbers() { tickstyle=(SliderTickSyle)(tickstyle|STS_Numbers); }
		void showIndicators() { tickstyle=(SliderTickSyle)(tickstyle|STS_LocatedNames); }
		void hideText() { tickstyle=(SliderTickSyle)((tickstyle&~STS_Numbers)&~STS_LocatedNames); }

		void showTicks() { tickstyle=(SliderTickSyle)(tickstyle|STS_Ticks); }
		void hideTicks() { tickstyle=(SliderTickSyle)(tickstyle&~STS_Ticks); }

		void showIndicatorMarks() { tickstyle=(SliderTickSyle)(tickstyle|STS_LocatedTicks); }
		void hideIndicatorMarks() { tickstyle=(SliderTickSyle)(tickstyle&~STS_LocatedTicks); }

		void setNumberofTicks(int ticks) { numberofticks=ticks; setNumberDistance(numberdistance); }
		int getNumberofTicks() { return numberofticks; }
		void setNumberDistance(float distance) { numberedtickdistance=distance/((maximum-minimum)/(numberofticks-1)); numberdistance=distance; }
		void setNumberFormat(string Format) { numberformat=Format; }
		void setNumberFormat(int decimal, string unit="") { char tmp[50]; sprintf(tmp, "%%.%if%s", decimal, unit.data()); setNumberFormat(tmp); }
		string getNumberFormat() { return numberformat; }

		bool isShowingNumbers() { return tickstyle&STS_Numbers; }
		bool isShowingIndicators() { return tickstyle&STS_LocatedNames; }
		bool isShowingTicks() { return tickstyle&STS_Ticks; }

		void AddIndicator(string Text, float Value) { ticknames.Add(new SliderLocationName(Text, Value, NULL)); }

	protected:
		float numberdistance;

		void init(IndicatorbarDirections direction);
	};
} }
