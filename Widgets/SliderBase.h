#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/ResourceFile.h"
#include "Button.h"
#include "../Engine/GGEMain.h"
#include "../Engine/Layer.h"
#include "../Engine/Input.h"
#include "../Engine/GraphicLayers.h"
#include "../Effects/LayerMover.h"
#include "../Effects/LayerResizer.h"
#include "IWidgetContainer.h"
#include "SliderBP.h"
#include "Frame.h"

using namespace gge;
using namespace gre;
using namespace geffects;

#define SLIDER_CLICK_DOWNDURATION	100

namespace gorgonwidgets {
	enum SliderStates {
		SS_Normal,
		SS_Disabled,
		SS_Pressed,
		SS_Hover
	};

	enum SliderRuleActions {
		SRA_None	= 0,
		SRA_JumpTo	   ,
		SRA_LargeIncrement
	};

	enum SliderTickSyle {
		STS_None		= 0,
		STS_Ticks		= 1,
		STS_Numbers		= 2,
		STS_Names		= 4,
		STS_LocatedNames= 8,
		STS_LocatedTicks=16,
		STS_TicksNumbers= 3,
		STS_TicksNames	= 5,
		STS_TicksLocatedNames= 9,
		STS_TicksLocatedTicks=17,
		STS_LocatedTicksNames=24,
		STS_TicksLocatedTicksNames=25
	};

	struct slider_change_event_params {
		float before;
		float after;
	};

	class SliderBase : public IWidgetObject, IWidgetContainer
	{
		friend class SliderBP;
		friend bool fn_rule_mouse(MouseEventType event,int x,int y,void *data);
		friend bool fn_symbol_mouse(MouseEventType event,int x,int y,void *data);
		friend bool fn_text_mouse(MouseEventType event,int x,int y,void *data);
		friend bool fn_tick_mouse(MouseEventType event,int x,int y,void *data);
		friend void fn_goDown(IntervalObject *interval, void *data);
		friend void fn_goUp(IntervalObject *interval, void *data);

	public:

		SliderElement	*NormalStyle,
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
		virtual void	Move(int X,int Y);
		virtual void	SetBluePrint(IWidgetBluePrint *BP);
		virtual void	SetFocus();
		virtual bool    isVisible() { if(container) return IWidgetObject::isVisible() && container->isVisible(); else  return IWidgetObject::isVisible(); }

		virtual bool    isFocussed() { return IWidgetObject::isFocussed(); }
		virtual void	Activate() { on_focus_event(isFocussed(), NULL); }
		virtual void	Deactivate() { IWidgetContainer::Deactivate(); on_focus_event(isFocussed(), NULL); }
		virtual void	Enable() { IWidgetObject::isenabled=true; }
		virtual void	Disable() { IWidgetObject::isenabled=false; }
		virtual bool	isEnabled() { if(container) return IWidgetObject::isenabled && container->isEnabled(); else return IWidgetObject::isenabled; }

		using IWidgetObject::Height;
		using IWidgetObject::Width;
		using IWidgetObject::isEnabled;
		using IWidgetObject::isVisible;


		virtual void setStyle(SliderStyles Style) { style=Style; DetermineStyleGroup(); }

		int ButtonMoveTime;
		int FastMoveTime;
		bool SmoothDrag;
		bool AlwaysShowCap;

		virtual void Show() { IWidgetObject::Show(); }
		virtual void Hide() { IWidgetObject::Hide(); }

		//????virtual void SetSlidingDuration(int ms) { OverlaySlidingDuration=SlidingDuration=ms; }


		~SliderBase() { if(btnUp) delete btnUp; if(btnDown) delete btnDown; /* also delete style objects */}
	protected:
		void btnUp_press(empty_event_params parameters, Button &object, Any data, string eventname);
		void btnUp_release(empty_event_params parameters, Button &object, Any data, string eventname);
		void btnUp_click(empty_event_params parameters, Button &object, Any data, string eventname);
		void btnDown_press(empty_event_params parameters, Button &object, Any data, string eventname);
		void btnDown_release(empty_event_params parameters, Button &object, Any data, string eventname);
		void btnDown_click(empty_event_params parameters, Button &object, Any data, string eventname);

		EventChain<SliderBase, slider_change_event_params> ChangeEvent;

		int SlidingDuration;
		int SlidingDurationValueMod;
		int OverlaySlidingDuration;
		int OverlaySlidingDurationValueMod;

		void SetPassive() { passivemode=true; nofocus=true; }
		void SetActive() { passivemode=false; nofocus=false; }
		bool isPassive() { return passivemode; }

		SliderBase(SliderBP *BluePrint,IWidgetContainer &container, SliderStyles Style);
		SliderBase(SliderBP *BluePrint, SliderStyles Style);
		virtual void SetStyles(SliderStyleGroup* grp);

		virtual bool mouse_event(MouseEventType event,int x,int y);
		virtual bool rule_mouse(MouseEventType event,int x,int y);
		virtual bool symbol_mouse(MouseEventType event,int x,int y);
		virtual bool tick_mouse(MouseEventType event,int x,int y);
		virtual bool text_mouse(MouseEventType event,int x,int y);

		void Setup(SliderTickSyle tickstyle, bool showsymbol, bool showrule, bool showbuttons, bool showoverlay, SliderRuleActions ruleaction,bool EnableSymbolDrag,bool EnableTickClick);

		MouseEventObject *rulemouseevent;
		MouseEventObject *symbolmouseevent;
		MouseEventObject *tickmouseevent;
		MouseEventObject *textmouseevent;

		Button *btnUp, *btnDown;

		SliderStates prevrulestate;
		SliderStates currentrulestate;
		SliderStates nextrulestate;

		SliderStates prevsymbolstate;
		SliderStates currentsymbolstate;
		SliderStates nextsymbolstate;

		SliderStates prevtickstate;
		SliderStates currenttickstate;
		SliderStates nexttickstate;

		SliderStates prevoverlaystate;
		SliderStates currentoverlaystate;
		SliderStates nextoverlaystate;
		
		bool temporalrulestate;
		bool temporalsymbolstate;
		bool temporaltickstate;
		bool temporaloverlaystate;

		unsigned int currentruleanimstart;
		unsigned int currentruleanimend;
		unsigned int currentsymbolanimstart;
		unsigned int currentsymbolanimend;
		unsigned int currenttickanimstart;
		unsigned int currenttickanimend;
		unsigned int currentoverlayanimstart;
		unsigned int currentoverlayanimend;

		SliderStyles style;
		void DetermineStyleGroup();

		SliderStates mstate;

		float minimum, maximum, value;
		int numberofticks;

		float numberedtickdistance;

		float steps, buttonincrement, largeincrement;

		bool drawsymbol, drawrule, showbuttons,drawoverlay;
		SliderTickSyle tickstyle;
		string numberformat;
		SliderRuleActions ruleaction;

		virtual void setValue(float value);
		
		Collection<SliderLocationName> ticknames;

		Colorizable2DLayer textlayer;
		WidgetLayer textwlayer;
		WidgetLayer ticklayer;
		WidgetLayer	symbollayer;
		WidgetLayer	rulelayer;
		Basic2DLayer	overlayer;

		LayerMover *symbolmover;
		LayerResizer *overlayresizer;

		IntervalObject *goup;
		IntervalObject *godown;

		SliderBP *BluePrint;
		SliderStyleGroup *StyleGroup;
		SliderElement *et[4][4];

		SliderElement *DetermineRuleElement(SliderStates currentstate,SliderStates nextstate);
		SliderElement *DetermineSymbolElement(SliderStates currentstate,SliderStates nextstate);
		SliderElement *DetermineTickElement(SliderStates currentstate,SliderStates nextstate);
		SliderElement *DetermineOverlayElement(SliderStates currentstate,SliderStates nextstate);

		void SymbolTransition(SliderStates target,bool temporal=false,bool progression=false);
		void RuleTransition(SliderStates target,bool temporal=false,bool progression=false);
		void TickTransition(SliderStates target,bool temporal=false,bool progression=false);
		void OverlayTransition(SliderStates target,bool temporal=false,bool progression=false);
		void ProgressionCheck();

		virtual void on_focus_event(bool state,IWidgetObject *related);
		virtual bool keyb_event(KeyboardEventType event,int keycode,KeyboardModifier::Type modifier);

		virtual void setButtonPositions();
		virtual void setSymbolPosition(bool instant=false);

		void MoveSymbolTo(int x, int y);
		void MoveSymbolToSymbol(int x, int y);
		void MoveSymbolToTick(int x, int y);
		void MoveSymbolToText(int x, int y);
		float SliderBase::CalcMoveSymbolTo(int x, int y);
		gge::Rectangle RuleRectangle();

		bool golarge;
		int intervaltimeout;
		bool delayclicks;
		bool cancelclick;

		virtual void goUp();
		virtual void stopUp();
		virtual void goDown();
		virtual void stopDown();
		virtual void Up() { if(golarge) setValue(value+largeincrement); else setValue(value+buttonincrement); }
		virtual void Down() { if(golarge) setValue(value-largeincrement); else setValue(value-buttonincrement); }


		void init();
	private:
		bool passivemode;
		float prevrat;
	};
}
