#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/ResourceFile.h"
#include "IWidgetContainer.h"
#include "CheckboxBP.h"
#include "../Engine/GGEMain.h"
#include "../Engine/Layer.h"


#define CHECKBOX_CLICK_DOWNDURATION	100

namespace gge { namespace widgets {
	class CheckboxElement;

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
		friend class CheckboxElement;
	public:
		CheckboxBase(CheckboxBP *BluePrint,IWidgetContainer &container,CheckboxTypes type);
		CheckboxBase(CheckboxBP *BluePrint,CheckboxTypes type);

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


		utils::EventChain<CheckboxBase> ChangeEvent;

	protected:
		int linesoverride;
		CheckboxElement::SymbolIconOrderConstants orderoverride;

		CheckboxBase	&ToggleCheckbox(bool animate=true);
		CheckboxBase	&SetCheckboxState(bool checked, bool animate=true);

		CheckboxStates	currentState();

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

		ImageAnimation *icon;

		CheckboxBP *BluePrint;
		CheckboxElement *et[4][4];
		CheckboxElement *ct[4][4];
		CheckboxElement *DetermineElement(CheckboxStates currentstate,CheckboxStates nextstate);
		void Transition(CheckboxStates target,bool temporal=false,bool progression=false,int targetvalue=-1);
		void ProgressionCheck();
		virtual void on_focus_event(bool state,IWidgetObject *related);
		
		virtual bool mouse_event(input::MouseEventType event,int x,int y);
		virtual bool keyb_event(input::KeyboardEventType event,int keycode,input::KeyboardModifier::Type modifier);
	};
} }
