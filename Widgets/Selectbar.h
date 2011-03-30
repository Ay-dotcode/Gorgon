#pragma once

#include "../Utils/Any.h"
#include "SliderBase.h"
#include "SliderBP.h"

namespace gorgonwidgets {
	enum SelectbarOrientations {
		SBO_Left=SS_Left,
		SBO_Right=SS_Right,
		SBO_Top=SS_Top,
		SBO_Bottom=SS_Bottom,
	};
	
	struct selectbar_change_params {
		string  value;
		utils::Any		data;
		selectbar_change_params(string value, utils::Any data) : value(value), data(data) {}
	};
	
	class Selectbar : public SliderBase {
	public:
		Selectbar(gorgonwidgets::SliderBP *BP, gorgonwidgets::IWidgetContainer &container, SelectbarOrientations Orientation);
		Selectbar(gorgonwidgets::SliderBP *BP, SelectbarOrientations Orientation);

		void setValue(string value) {  SliderBase::setValue((float)FindValue(value)); }
		string getValue() { return ticknames[(int)value]->name; }

		void showTicks() { tickstyle=(SliderTickSyle)(tickstyle|STS_Ticks); }
		void hideTicks() { tickstyle=(SliderTickSyle)(tickstyle&~STS_Ticks); }

		bool isShowingTicks() { return tickstyle&STS_Ticks; }

		void Add(string text, void *data=NULL) { numberofticks++; maximum++; ticknames.Add(new SliderLocationName(string(text), data)); SliderBase::setValue(value); }
		void Remove(string text) { numberofticks--; int i=FindValue(text); if(value>i) value--; maximum--; ticknames.Remove(i); SliderBase::setValue(value); }

		utils::Any getData() { return utils::Any(ticknames[(int)value]->data); }
		void setData(utils::Any data) { SliderBase::setValue((float)FindData(data)); }

		utils::EventChain<Selectbar, selectbar_change_params> ChangeEvent;
	protected:

		void base_change();

		int FindValue(string value) { int i=0; ticknames.ResetIteration(); SliderLocationName *s;while(s=ticknames.next()) { if(s->name==value) return i; i++; } }
		int FindData(utils::Any data) { int i=0; ticknames.ResetIteration(); SliderLocationName *s;while(s=ticknames.next()) { if(s->data==data) return i; i++; } }

		void init();
	};
}
