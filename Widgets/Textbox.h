#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/ResourceFile.h"
#include "TextboxBP.h"
#include "../Engine/GGEMain.h"
#include "../Engine/GraphicLayers.h"
#include "IWidgetContainer.h"

using namespace gge;
using namespace gre;

namespace gorgonwidgets {
	enum TextboxStates {
		TS_Normal,
		TS_Disabled,
		TS_Active,
		TS_Hover
	};

	struct text_change_event_params {
		string prev;
		string current;
	};

	class Textbox : public IWidgetObject
	{
		friend class TextboxBP;
		friend class TextboxElement;
	public:
		Textbox(TextboxBP *BluePrint,IWidgetContainer &container);
		Textbox(TextboxBP *BluePrint);

		Textbox			&SetStyles(TextboxStyleGroup* grp);

		TextboxElement	*NormalStyle,
						*HoverStyle,
						*ActiveStyle,
						*DisabledStyle,
						*Normal2Hover,
						*Normal2Active,
						*Hover2Active,
						*Hover2Normal,
						*Active2Normal,
						*Active2Hover;

		bool			RevHover2NormalState,
						RevActive2HoverState,
						RevActive2NormalState;


		virtual void	Draw();
		virtual void	Resize(int Cx,int Cy);
		virtual void	SetBluePrint(IWidgetBluePrint *BP);

		Textbox	&SimulateActivate();
		Textbox	&SimulateRelease();
		Textbox	&SimulateMouseOver();
		Textbox	&SimulateMouseOut();

		Textbox &SetCaretPosition(int Position);
		Textbox &SetSelection(int start,int end);
		Textbox &SelectAll();

		TextboxStates	currentState();
		ImageAnimation *Caret;

		virtual	void	SetText(string text);

		EventChain<Textbox, text_change_event_params> ChangeEvent;
		EventChain<Textbox, keyboard_event_params> KeyboardEvent;


		bool AlwaysShowSelection;
		bool AutoSelectAll;

	protected:
		TextboxStates prevstate;
		TextboxStates currentstate;
		TextboxStates nextstate;
		unsigned int currentanimstart;
		unsigned int currentanimend;
		Colorizable2DLayer textlayer;

		TextboxBP *BluePrint;
		int caretposition;
		int selectstart;
		int selectend;
		bool isactive;
		int scroll;
		virtual void on_focus_event(bool state,IWidgetObject *related);

		TextboxElement *et[4][4];
		TextboxElement *DetermineElement(TextboxStates currentstate,TextboxStates nextstate);
		void Transition(TextboxStates target,bool progression=false);
		void ProgressionCheck();
		
		virtual bool mouse_event(MouseEventType event,int x,int y);
		virtual bool keyb_event(KeyboardEventType event,int keycode,KeyboardModifier::Type modifier);

		bool mousedown;

	};
}