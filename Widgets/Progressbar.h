#pragma once

#include "SliderBase.h"
#include "IProgressor.h"

namespace gorgonwidgets {
	class Progressbar : public SliderBase, public IProgressor {
	public:
		Progressbar(SliderBP *BP, gorgonwidgets::IWidgetContainer &container);
		Progressbar(SliderBP *BP);

		virtual void SetRange(float min, float max) { minimum=min; maximum=max; }
		virtual void setValue(float value) { SliderBase::setValue(value); }
		virtual int getValue() { return value; }

		void setMinimum(float Min) { minimum=Min; }
		void setMaximum(float Max) { maximum=Max; }
		float getMinimum() { return minimum; }
		float getMaximum() { return maximum; }

		float operator++ () {
			float v=getValue();
			setValue(v+1);

			return v;
		}

		float operator+= (float value) {
			float v=getValue();
			setValue(v+value);

			return v;
		}
		float operator =(float value) { setValue(value); return value; }
		int operator =(int value) { setValue(value); return value; }

		using SliderBase::Height;
		using SliderBase::Width;

		void setPercentage(float percent) { setValue(percent/100*(maximum-minimum)+minimum); }
		float getPercentage() { return 100*(value-minimum)/(maximum-minimum); }
		void setAnimationDuration(int Duration) { SlidingDuration=Duration; }

	protected:
		void init();
	};
}