#pragma once

#include "SliderBase.h"
#include "SliderBP.h"

namespace gorgonwidgets {
	class Slider : public SliderBase {
	public:
		Slider(gorgonwidgets::SliderBP *BP, gorgonwidgets::IWidgetContainer &container, SliderStyles Style);
		Slider(gorgonwidgets::SliderBP *BP, SliderStyles Style);

		void setValue(float value) { SliderBase::setValue(value); }
		float getValue() { return value; }

		void SetRange(float Min, float Max, float steps=-1) { setMinimum(Min); setMaximum(Max); if(steps>0) setSteps(steps); }
		void setMinimum(float Min) { minimum=Min; setTickDistance(tickdistance); buttonincrement=(maximum-minimum)/24; }
		void setMaximum(float Max) { maximum=Max; setTickDistance(tickdistance); buttonincrement=(maximum-minimum)/24; }
		void setSteps(float Step) { steps=Step; }
		void setTickDistance(float Distance) { 
			numberedtickdistance*=tickdistance/Distance; 
			tickdistance=Distance; 
			numberofticks=Round((maximum-minimum)/tickdistance+1); 
		}
		void setNumberDistance(float Distance) { numberedtickdistance=Distance/tickdistance; }
		void setNumberFormat(string Format) { numberformat=Format; }
		void setNumberFormat(int decimal, string unit="") { 
			std::ostringstream str;
			str<<"%."<<decimal<<"f"<<unit;
			setNumberFormat(str.str()); 
		}
		float getMinimum() { return minimum; }
		float getMaximum() { return maximum; }
		float getSteps() { return steps; }
		float getTickDistance() { return (maximum-minimum)/numberofticks; }
		float getNumberDistance() { return numberedtickdistance*tickdistance; }
		string getNumberFormat() { return numberformat; }

		void showNumbers() { tickstyle=(SliderTickSyle)(tickstyle|STS_Numbers); }
		void hideNumbers() { tickstyle=(SliderTickSyle)(tickstyle&~STS_Numbers); }
		void showTicks() { tickstyle=(SliderTickSyle)(tickstyle|STS_Ticks); }
		void hideTicks() { tickstyle=(SliderTickSyle)(tickstyle&~STS_Ticks); }

		bool isShowingNumbers() { return (tickstyle&STS_Numbers)!=0; }
		bool isShowingTicks() { return (tickstyle&STS_Ticks)!=0; }

		template<class T_>
		Slider &operator =(T_ val) {
			setValue(val);

			return *this;
		}

		operator float() {
			return getValue();
		}

		operator int() {
			return (int)getValue();
		}

		template<class T_>
		Slider &operator +=(T_ val) {
			setValue(getValue()+val);

			return *this;
		}

		template<class T_>
		Slider &operator -=(T_ val) {
			setValue(getValue()-val);

			return *this;
		}

		operator string() {
			stringstream ss;
			ss<<getValue();
			return ss.str();
		}

		utils::EventChain<Slider> ChangeEvent;

	protected:
		float tickdistance;

		void init();
	};
}
