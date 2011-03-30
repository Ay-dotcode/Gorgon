#pragma once

#include <string>


#include "IWidgetObject.h"
#include "../Utils/Collection.h"

namespace gorgonwidgets {
	class IRadioButton {
	public:
		virtual IRadioButton	&Clear()=0;
		virtual IRadioButton	&Check()=0;
		virtual bool			 isChecked()=0;

		IRadioButton() : ChangeEvent("Change", this) { }

		virtual IWidgetObject &getWidget()=0;

		utils::Any RadioData;
		
		utils::EventChain<IRadioButton> ChangeEvent;
	};

	template<class T_>
	struct radiogroup_change_event {
		radiogroup_change_event(IRadioButton &Button, T_ Data) :
			Button(Button), Data(Data) {}

		IRadioButton &Button;
		T_ Data;
	};

	template<class T_=int>
	class RadioButtonGroup {
	public:
		utils::Collection<IRadioButton> Buttons;

		RadioButtonGroup() : ChangeEvent("Change", this), selected(NULL) { }

		void Radio_Change(utils::empty_event_params params, IRadioButton &caller,utils::Any data,string name) {
			RadioButtonGroup* rbg=dynamic_cast<RadioButtonGroup*>(data);

			if(caller.isChecked())
				rbg->activate(caller);
		}

		IRadioButton *Add(IRadioButton *radio) {
			Buttons.Add(radio);
			radio->ChangeEvent.Register(this, &RadioButtonGroup::Radio_Change,this);

			return radio;
		}
		IRadioButton &Add(IRadioButton &obj) { Add(&obj); return obj; }
		IRadioButton *Add(IRadioButton *obj, T_ data) {
			Add(obj);
			obj->RadioData=Any(data);

			return obj;
		}
		IRadioButton &Add(IRadioButton &obj, T_ data) { Add(&obj, data); return obj; }
		IRadioButton *getSelected() {
			return selected;
		}
		T_ getData() {
			if(selected)
				return (T_)selected->RadioData;
			else
				return T_();
		}
		operator T_() {
			if(selected)
				return (T_)selected->RadioData;
			else
				return T_();
		}
		void setSelected(IRadioButton *obj) {
			obj->Check();
		}
		
		void setSelected(T_ data) {
			foreach(IRadioButton, btn, Buttons) {
				if(btn->RadioData==data) {
					btn->Check();
					return;
				}
			}
		}

		utils::EventChain<RadioButtonGroup, radiogroup_change_event<T_> > ChangeEvent;



	protected:

		void activate(IRadioButton &button) {
			foreach(IRadioButton, btn, Buttons) {
				if(btn!=&button)
					btn->Clear();
				else
					btn->Check();
			}

			selected=&button;

			ChangeEvent(radiogroup_change_event<T_>(*getSelected(), getData()));
		}
		IRadioButton *selected;
	};
}
