#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/ResourceFile.h"
#include "ButtonBP.h"
#include "../Engine/GGEMain.h"
#include "../Engine/Layer.h"
#include "../Engine/Input.h"
#include "../Engine/GraphicLayers.h"
#include "IWidgetContainer.h"


#define BUTTON_CLICK_DOWNDURATION	100

namespace gorgonwidgets {
	enum ButtonStates {
		BS_Normal,
		BS_Disabled,
		BS_Pressed,
		BS_Hover
	};

	class Button : public IWidgetObject
	{
		friend class ButtonBP;
	public:
		Button(ButtonBP *BluePrint,IWidgetContainer &container);
		Button(ButtonBP *BluePrint);

		Button			&SetStyles(ButtonStyleGroup* grp);

		ButtonElement	*NormalStyle,
						*HoverStyle,
						*PressedStyle,
						*DisabledStyle,
						*Normal2Hover,
						*Normal2Pressed,
						*Hover2Pressed,
						*Hover2Normal,
						*Pressed2Normal,
						*Pressed2Hover;

		bool			RevHover2NormalState,
						RevPressed2HoverState,
						RevPressed2NormalState;


		virtual void	Draw();
		virtual void	Resize(int Cx,int Cy);
		virtual void	SetBluePrint(IWidgetBluePrint *BP);

		Button	&SimulateClicked();
		Button	&SimulatePressed();
		Button	&SimulateRelease();
		Button	&SimulateMouseOver();
		Button	&SimulateMouseOut();

		ButtonStates	currentState();

		Button	&SetDefault();
		Button	&SetCancel();
		Button	&SetNormal();

		ImageResource *Icon;
		ImageResource *HoverIcon;
		ImageResource *PressedIcon;
		ImageResource *DisabledIcon;

		virtual bool mouse_event(input::MouseEventType event,int x,int y);

		utils::EventChain<Button> ClickEvent;
		utils::EventChain<Button> PressEvent;
		utils::EventChain<Button> ReleaseEvent;

	protected:
		ButtonStates prevstate;
		ButtonStates currentstate;
		ButtonStates nextstate;
		ButtonStates mstate;
		unsigned int currentanimstart;
		unsigned int currentanimend;
		bool temporalstate;
		Colorizable2DLayer textlayer,iconlayer;
		ButtonBP *BluePrint;
		ButtonElement *et[4][4];
		ButtonElement *DetermineElement(ButtonStates currentstate,ButtonStates nextstate);
		void Transition(ButtonStates target,bool temporal=false,bool progression=false);
		void ProgressionCheck();

		virtual void on_focus_event(bool state,IWidgetObject *related);
		virtual bool keyb_event(input::KeyboardEventType event,int keycode, input::KeyboardModifier::Type modifier);
	};
}