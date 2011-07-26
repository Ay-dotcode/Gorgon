#include "Checkbox.h"


namespace gge { namespace widgets {


	void RadioButton::RadioBtn_Change() {
		IRadioButton::ChangeEvent();
	}
	RadioButton::RadioButton(CheckboxBP *BluePrint,IWidgetContainer &container) : 
		CheckboxBase(BluePrint,container,CT_RadioButton),
		ChangeEvent("Change", this)
	{
		CheckboxBase::ChangeEvent.Register(this, &RadioButton::RadioBtn_Change);
		IRadioButton::ChangeEvent.DoubleLink(RadioButton::ChangeEvent);
		sticky=true;
	}
	RadioButton::RadioButton(CheckboxBP *BluePrint) : 
		CheckboxBase(BluePrint,CT_RadioButton),
		ChangeEvent("Change", this)
	{
		CheckboxBase::ChangeEvent.Register(this, &RadioButton::RadioBtn_Change);
		IRadioButton::ChangeEvent.DoubleLink(RadioButton::ChangeEvent);
		sticky=true;
	}
	IRadioButton	&RadioButton::Clear(){
		CheckboxBase::SetCheckboxState(false);
		return *(IRadioButton*)this;
	}
	IRadioButton	&RadioButton::Check(){
		CheckboxBase::SetCheckboxState(true);
		return *(IRadioButton*)this;
	}
	bool			 RadioButton::isChecked() {
		return checked;
	}
} }
