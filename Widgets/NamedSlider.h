#pragma once

#include "Slider.h"
#include "SliderBase.h"

namespace gorgonwidgets {
	enum NamedSliderDirections {
		NSD_Left=SS_Left,
		NSD_Right=SS_Right,
		NSD_Top=SS_Top,
		NSD_Bottom=SS_Bottom
	};
	class NamedSlider : public Slider {
	public:
		NamedSlider(SliderBP *BP, gorgonwidgets::IWidgetContainer &container, NamedSliderDirections direction);
		NamedSlider(SliderBP *BP, NamedSliderDirections direction);

		virtual void SetRange(float min, float max) { minimum=min; maximum=max; setNumberDistance(numberdistance); }
		void showIndicators() { tickstyle=(SliderTickSyle)(tickstyle|STS_LocatedNames); }

		void AddIndicator(string Text, float Value) { ticknames.Add(new SliderLocationName(Text, Value, NULL)); }
		void RemoveIndicator(float Value) {
			ticknames.ResetIteration();
			SliderLocationName *name;
			while(name=ticknames.next()) {
				if(name->value==Value) {
					ticknames.Remove(name);
					return;
				}
			}
		}
	protected:
		float numberdistance;

		void init();
	};
}
