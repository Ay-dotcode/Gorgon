#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/ResourceFile.h"
#include "../Engine/GGEMain.h"
#include "../Engine/Layer.h"
#include "IWidgetContainer.h"
#include "CheckboxBase.h"
#include "RadioGroup.h"


namespace gorgonwidgets {

	class Checkbox : public CheckboxBase {
	public:

		Checkbox(CheckboxBP *BluePrint,IWidgetContainer &container) : 
		  CheckboxBase(BluePrint, container, CT_Checkbox) { }

		Checkbox(CheckboxBP *BluePrint) : 
		  CheckboxBase(BluePrint, CT_Checkbox) { }

		operator bool() { return isChecked(); }

		bool isChecked() {
			return checked;
		}

		Checkbox &Check() { SetCheckboxState(true); }
		Checkbox &Clear() { SetCheckboxState(false); }
		bool Toggle() { ToggleCheckbox(); return checked; }
		Checkbox &setChecked(bool Checked) { SetCheckboxState(Checked); }

	};

	class RadioButton : public CheckboxBase, public IRadioButton {
	public:
		RadioButton(CheckboxBP *BluePrint,IWidgetContainer &container);
		RadioButton(CheckboxBP *BluePrint);

		virtual IRadioButton	&Clear();
		virtual IRadioButton	&Check();
		virtual bool			 isChecked();
		bool Toggle() { ToggleCheckbox(); return checked; }

		virtual IWidgetObject &getWidget() { return *this; }


		EventChain<RadioButton, empty_event_params> ChangeEvent;
	protected:
		void RadioBtn_Change(empty_event_params p,CheckboxBase &object,Any data,string name);
	};


}
