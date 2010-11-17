#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/ResourceFile.h"
#include "CheckboxBP.h"
#include "../Engine/GGEMain.h"
#include "../Engine/Layer.h"
#include "IWidgetContainer.h"
#include "RadioGroup.h"

using namespace gge;
using namespace gre;

#define CHECKBOX_CLICK_DOWNDURATION	100

namespace gorgonwidgets {
	enum CheckboxStates {
		CS_Normal,
		CS_Disabled,
		CS_Pressed,
		CS_Hover
	};

	enum CheckboxStyles {
		CS_Empty,
		CS_Checked
	};

	class CheckboxBase : public IWidgetObject {
		friend class CheckboxBP;
	public:
		CheckboxBase(CheckboxBP *BluePrint,IWidgetContainer &container,CheckboxTypes type=CT_Checkbox);
		CheckboxBase(CheckboxBP *BluePrint,CheckboxTypes type=CT_Checkbox);

		CheckboxBase			&SetStyles(CheckboxStyleGroup* grp);

		CheckboxElement	*NormalStyle,
						*HoverStyle,
						*PressedStyle,
						*DisabledStyle,
						*Normal2Hover,
						*Normal2Pressed,
						*Hover2Pressed,
						*Hover2Normal,
						*Pressed2Normal,
						*Pressed2Hover,
						*ToNextStyle;

		CheckboxElement	*cNormalStyle,
						*cHoverStyle,
						*cPressedStyle,
						*cDisabledStyle,
						*cNormal2Hover,
						*cNormal2Pressed,
						*cHover2Pressed,
						*cHover2Normal,
						*cPressed2Normal,
						*cPressed2Hover,
						*cToNextStyle;

		bool			RevHover2NormalState,
						RevPressed2HoverState,
						RevPressed2NormalState;

		bool			cRevHover2NormalState,
						cRevPressed2HoverState,
						cRevPressed2NormalState,
						cRevToNextStyle;


		virtual void	Draw();
		virtual void	Resize(int Cx,int Cy);
		virtual void	SetBluePrint(IWidgetBluePrint *BP);

		CheckboxBase	&SimulateClicked();
		CheckboxBase	&SimulatePressed();
		CheckboxBase	&SimulateRelease();
		CheckboxBase	&SimulateMouseOver();
		CheckboxBase	&SimulateMouseOut();

		operator bool() { return isChecked(); }
		CheckboxBase &operator =(bool checked) { return SetChecked(checked); }


		CheckboxStates	currentState();

		virtual CheckboxBase	&Toggle(bool animate=true);
		CheckboxBase	&Check(bool animate=true);
		CheckboxBase	&Clear(bool animate=true);
		CheckboxBase	&SetChecked(bool checked);

		bool		isChecked() {
			return checked;
		}

		EventChain<CheckboxBase, empty_event_params> ChangeEvent;

	protected:
		CheckboxStates prevstate;
		CheckboxStates currentstate;
		CheckboxStates nextstate;
		CheckboxStates mstate;
		unsigned int currentanimstart;
		unsigned int currentanimend;
		CheckboxTypes type;
		bool sticky;

		bool checked;
		bool temporalstate;
		bool statetransition;
		bool statetransitiontarget;
		bool mover;
		Colorizable2DLayer textlayer;

		CheckboxBP *BluePrint;
		CheckboxElement *et[4][4];
		CheckboxElement *ct[4][4];
		CheckboxElement *DetermineElement(CheckboxStates currentstate,CheckboxStates nextstate);
		void Transition(CheckboxStates target,bool temporal=false,bool progression=false,int targetvalue=-1);
		void ProgressionCheck();
		virtual void on_focus_event(bool state,IWidgetObject *related);
		
		virtual bool mouse_event(MouseEventType event,int x,int y);
		virtual bool keyb_event(KeyboardEventType event,int keycode,KeyboardModifier modifier);
	};

	class RadioButton : public CheckboxBase, public IRadioButton {
	public:
		RadioButton(CheckboxBP *BluePrint,IWidgetContainer &container);
		RadioButton(CheckboxBP *BluePrint);

		virtual IRadioButton	&Clear();
		virtual IRadioButton	&Check();
		virtual bool			 isChecked();

		virtual IWidgetObject &getWidget() { return *this; }


		EventChain<RadioButton, empty_event_params> ChangeEvent;
	protected:
		void RadioBtn_Change(empty_event_params p,CheckboxBase &object,Any data,string name);
	};
}