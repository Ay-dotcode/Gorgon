#include "SliderBase.h"
#include "../Engine/Wave.h"
#include "WidgetRegistry.h"
#ifndef SLIDER_TIMEOUT
#define SLIDER_TIMEOUT	250
#endif
#include "../Engine/GGEMain.h"


namespace gorgonwidgets {
	bool fn_rule_mouse(MouseEventType event,int x,int y,void *data) {
		return ((SliderBase*)data)->rule_mouse(event, x, y);
	}

	bool fn_symbol_mouse(MouseEventType event,int x,int y,void *data) {
		return ((SliderBase*)data)->symbol_mouse(event, x, y);
	}

	bool fn_tick_mouse(MouseEventType event,int x,int y,void *data) {
		return ((SliderBase*)data)->tick_mouse(event, x, y);
	}

	bool fn_text_mouse(MouseEventType event,int x,int y,void *data) {
		return ((SliderBase*)data)->text_mouse(event, x, y);
	}
	void fn_goUp(IntervalObject *interval, void *data) {
		((SliderBase*)data)->cancelclick=true;
		((SliderBase*)data)->Up();
		interval->Timeout=((SliderBase*)data)->intervaltimeout;
	}
	void fn_goDown(IntervalObject *interval, void *data) {
		((SliderBase*)data)->cancelclick=true;
		((SliderBase*)data)->Down();
		interval->Timeout=((SliderBase*)data)->intervaltimeout;
	}



	void SliderBase::btnUp_press(empty_event_params parameters, Button &object, Any data, string eventname) {
		goUp();
		cancelclick=false;
	}
	void SliderBase::btnUp_click(empty_event_params parameters, Button &object, Any data, string eventname) {
		if(!cancelclick)
			Up();
	}
	void SliderBase::btnUp_release(empty_event_params parameters, Button &object, Any data, string eventname) {
		stopUp();
	}
	void SliderBase::btnDown_press(empty_event_params parameters, Button &object, Any data, string eventname) {
		//data->Down();
		goDown();
		cancelclick=false;
	}
	void SliderBase::btnDown_click(empty_event_params parameters, Button &object, Any data, string eventname) {
		if(!cancelclick)
			Down();
	}
	void SliderBase::btnDown_release(empty_event_params parameters, Button &object, Any data, string eventname) {
		stopDown();
	}



	SliderBase::SliderBase(gorgonwidgets::SliderBP *BluePrint, IWidgetContainer &Container, SliderStyles Style) :
		IWidgetContainer(0, 0, 100, 100, 0),
		IWidgetObject(Container),
		BluePrint(BluePrint),
		NormalStyle(NULL), HoverStyle(NULL), PressedStyle(NULL),
		DisabledStyle(NULL),
		Normal2Hover(NULL), Normal2Pressed(NULL),
		Hover2Pressed(NULL), Hover2Normal(NULL),
		Pressed2Normal(NULL), Pressed2Hover(NULL),
		btnUp(NULL), btnDown(NULL),
		ChangeEvent("Change", this),
		minimum(0), maximum(100),
		value(0), steps(1),
		buttonincrement(1), largeincrement(10),
		numberofticks(11),
		numberedtickdistance(2),
		drawsymbol(true), drawrule(true), showbuttons(false),
		ruleaction(SRA_LargeIncrement), style(Style),
		OverlaySlidingDuration(500),
		SlidingDuration(300),
		OverlaySlidingDurationValueMod(500),
		SlidingDurationValueMod(500),
		ButtonMoveTime(2000),
		FastMoveTime(1000),
		SmoothDrag(false),
		godown(NULL), goup(NULL),
		mstate(SS_Normal),
		golarge(false), delayclicks(false),
		AlwaysShowCap(false),
		numberformat("%.0f"),
		rulelayer(0,0, 100,100), ticklayer(0,0, width,height),
		textlayer(0,0, width, height), textwlayer(0,0,Main.W, Main.H),
		overlayer(0,0, 100,100), symbollayer(0,0, 0,0)

	{
		init();
	}

	SliderBase::SliderBase(gorgonwidgets::SliderBP *BluePrint, SliderStyles Style) :
		IWidgetContainer(0, 0, 100, 100, 0),
		IWidgetObject(),
		BluePrint(BluePrint),
		NormalStyle(NULL), HoverStyle(NULL), PressedStyle(NULL),
		DisabledStyle(NULL),
		Normal2Hover(NULL), Normal2Pressed(NULL),
		Hover2Pressed(NULL), Hover2Normal(NULL),
		Pressed2Normal(NULL), Pressed2Hover(NULL),
		btnUp(NULL), btnDown(NULL),
		ChangeEvent("Change", this),
		minimum(0), maximum(100),
		value(0), steps(1),
		buttonincrement(1), largeincrement(10),
		numberofticks(11),
		numberedtickdistance(2),
		drawsymbol(true), drawrule(true), showbuttons(false),
		ruleaction(SRA_LargeIncrement), style(Style),
		OverlaySlidingDuration(500),
		SlidingDuration(300),
		OverlaySlidingDurationValueMod(500),
		SlidingDurationValueMod(500),
		ButtonMoveTime(2000),
		FastMoveTime(1000),
		SmoothDrag(false),
		godown(NULL), goup(NULL),
		mstate(SS_Normal),
		golarge(false), delayclicks(false),
		AlwaysShowCap(false),
		numberformat("%.0f"),
		rulelayer(0,0, 100,100), ticklayer(0,0, width,height),
		textlayer(0,0, width, height), textwlayer(0,0,Main.W, Main.H),
		overlayer(0,0, 100,100), symbollayer(0,0, 0,0)

	{
		init();
	}

	void SliderBase::init() {
		this->PointerType=BluePrint->PointerType;

		nextrulestate=currentrulestate=SS_Normal;
		nextsymbolstate=currentsymbolstate=SS_Normal;
		nexttickstate=currenttickstate=SS_Normal;


		layer.Add(BaseLayer, 0);

		textlayer.EnableClipping=true;
		layer.Add(textlayer, 2);

		ticklayer.EnableClipping=false;
		layer.Add(ticklayer, 3);

		symbollayer.EnableClipping=false;
		layer.Add(symbollayer, 1);
		symbolmover=new LayerMover(symbollayer);
		symbolmover->Setup(Point(0,0), Point(0,0), 0);

		layer.Add(rulelayer,3);

		overlayer.EnableClipping=true;
		overlayer.isVisible=false;
		layer.Add(overlayer,2);

		overlayresizer=new LayerResizer(overlayer);
		overlayresizer->Setup(gge::Rectangle(0,0,100,100),gge::Rectangle(0,0,100,100),0);


		if(style!=SliderStyles::SS_Horizontal && style!=SliderStyles::SS_Verticle)
			tickstyle=STS_None;
		else
			tickstyle=STS_Ticks;

		int i;
		for(i=0;i<4;i++)
			memset(et[i],0,4*sizeof(SliderElement*));

		SetBluePrint(BluePrint);


		btnUp=new Button(StyleGroup->Up, *this);
		btnDown=new Button(StyleGroup->Down, *this);

		btnUp->Hide();
		btnDown->Hide();

		rulemouseevent=rulelayer.RegisterMouseEvent(0,0,Main.W,Main.H,this,fn_rule_mouse,fn_rule_mouse,fn_rule_mouse,fn_rule_mouse,fn_rule_mouse,fn_rule_mouse)->Item;
		rulemouseevent->Enabled=false;

		symbolmouseevent=symbollayer.RegisterMouseEvent(0,0,Main.W,Main.H,this,fn_symbol_mouse,fn_symbol_mouse,fn_symbol_mouse,fn_symbol_mouse,fn_symbol_mouse,fn_symbol_mouse)->Item;
		symbolmouseevent->Enabled=false;

		tickmouseevent=ticklayer.RegisterMouseEvent(0,0,Main.W,Main.H,this,fn_tick_mouse,fn_tick_mouse,fn_tick_mouse,fn_tick_mouse,fn_tick_mouse,fn_tick_mouse)->Item;
		tickmouseevent->Enabled=false;

		textlayer.Add(textwlayer,0);
		textmouseevent=textwlayer.RegisterMouseEvent(0,0,Main.W,Main.H,this,fn_text_mouse,fn_text_mouse,fn_text_mouse,fn_text_mouse,fn_text_mouse,fn_text_mouse)->Item;
		textmouseevent->Enabled=false;

		btnUp->PressEvent.Register(this, &SliderBase::btnUp_press);
		btnUp->ReleaseEvent.Register(this, &SliderBase::btnUp_release);
		btnUp->ClickEvent.Register(this, &SliderBase::btnUp_click);
		btnDown->PressEvent.Register(this, &SliderBase::btnDown_press);
		btnDown->ReleaseEvent.Register(this, &SliderBase::btnDown_release);
		btnDown->ClickEvent.Register(this, &SliderBase::btnDown_click);

		SetStyles(StyleGroup);

		SetActive();

		allowtabswitch=false;
		allowdefaultactions=false;

		IWidgetObject::Resize(StyleGroup->PreferredWidth, StyleGroup->PreferredHeight);
	}

	void	SliderBase::Move(int X,int Y) {
		IWidgetObject::Move(X,Y);
		RelocateEvent();
	}

	void SliderBase::DetermineStyleGroup() {
		int id=-1,val=1000000,i,v;
		for(i=0;i<BluePrint->StyleGroups.getCount();i++) {
			if(BluePrint->StyleGroups[i]->Style==style) {
				if(BluePrint->StyleGroups[i]->PreferredWidth==0)
					v=1+abs(BluePrint->StyleGroups[i]->PreferredHeight-height);
				else if(BluePrint->StyleGroups[i]->PreferredHeight==0)
					v=abs(BluePrint->StyleGroups[i]->PreferredWidth-width)+1;
				else if(BluePrint->StyleGroups[i]->PreferredWidth==0 && BluePrint->StyleGroups[i]->PreferredHeight==0)
					v=1;
				else
					v=abs(BluePrint->StyleGroups[i]->PreferredWidth-width)+abs(BluePrint->StyleGroups[i]->PreferredHeight-height);

				if(v<val) {
					val=v;
					id=i;
				}
			}
		}

		if(id==-1) {
			for(i=0;i<BluePrint->StyleGroups.getCount();i++) {
				if( 
				 ((BluePrint->StyleGroups[i]->Style==SS_Top || BluePrint->StyleGroups[i]->Style==SS_Bottom || BluePrint->StyleGroups[i]->Style==SS_Horizontal) && 
				  (style==SS_Top || style==SS_Bottom || style==SS_Horizontal)) || 
				 ((BluePrint->StyleGroups[i]->Style==SS_Left || BluePrint->StyleGroups[i]->Style==SS_Right || BluePrint->StyleGroups[i]->Style==SS_Verticle) && 
				  (style==SS_Left || style==SS_Right || style==SS_Verticle)) ) {
					if(BluePrint->StyleGroups[i]->PreferredWidth==0)
						v=1+abs(BluePrint->StyleGroups[i]->PreferredHeight-height);
					else if(BluePrint->StyleGroups[i]->PreferredHeight==0)
						v=abs(BluePrint->StyleGroups[i]->PreferredWidth-width)+1;
					else if(BluePrint->StyleGroups[i]->PreferredWidth==0 && BluePrint->StyleGroups[i]->PreferredHeight==0)
						v=1;
					else
						v=abs(BluePrint->StyleGroups[i]->PreferredWidth-width)+abs(BluePrint->StyleGroups[i]->PreferredHeight-height);

					if(v<val) {
						val=v;
						id=i;
					}
				}
			}
		}

		if(id==-1) {
			for(i=0;i<BluePrint->StyleGroups.getCount();i++) {
				if(BluePrint->StyleGroups[i]->PreferredWidth==0)
					v=1+abs(BluePrint->StyleGroups[i]->PreferredHeight-height);
				else if(BluePrint->StyleGroups[i]->PreferredHeight==0)
					v=abs(BluePrint->StyleGroups[i]->PreferredWidth-width)+1;
				else if(BluePrint->StyleGroups[i]->PreferredWidth==0 && BluePrint->StyleGroups[i]->PreferredHeight==0)
					v=1;
				else
					v=abs(BluePrint->StyleGroups[i]->PreferredWidth-width)+abs(BluePrint->StyleGroups[i]->PreferredHeight-height);

				if(v<val) {
					val=v;
					id=i;
				}
			}
		}

		if(StyleGroup!=BluePrint->StyleGroups[id]) SetStyles(BluePrint->StyleGroups[id]);
	}

	void SliderBase::Resize(int Cx, int Cy) {
		DetermineStyleGroup();
		if(Cx==0)
			Cx=StyleGroup->PreferredWidth;
		if(Cy==0)
			Cy=StyleGroup->PreferredHeight;
		IWidgetObject::Resize(Cx,Cy);
		IWidgetContainer::Resize(Cy, Cy);

		setButtonPositions();
		setSymbolPosition(true);
	}

	void SliderBase::SetStyles(gorgonwidgets::SliderStyleGroup *grp) {
		StyleGroup=grp;
		if(grp->NormalStyle)
			NormalStyle=new SliderElement(*grp->NormalStyle);
		if(grp->HoverStyle)
			HoverStyle=new SliderElement(*grp->HoverStyle);
		if(grp->PressedStyle)
			PressedStyle=new SliderElement(*grp->PressedStyle);
		if(grp->DisabledStyle)
			DisabledStyle=new SliderElement(*grp->DisabledStyle);
		if(grp->Normal2Hover)
			Normal2Hover=new SliderElement(*grp->Normal2Hover);
		if(grp->Normal2Pressed)
			Normal2Pressed=new SliderElement(*grp->Normal2Pressed);
		if(grp->Hover2Pressed)
			Hover2Pressed=new SliderElement(*grp->Hover2Pressed);
		if(grp->Hover2Normal) {
			if(grp->Hover2Normal==grp->Normal2Hover)
				Hover2Normal=Normal2Hover;
			else
				Hover2Normal=new SliderElement(*grp->Hover2Normal);
		}
		if(grp->Pressed2Normal) {
			if(grp->Pressed2Normal==grp->Normal2Pressed)
				Pressed2Normal=Normal2Pressed;
			else
				Pressed2Normal=new SliderElement(*grp->Pressed2Normal);
		}
		if(grp->Pressed2Hover) {
			if(grp->Pressed2Hover==grp->Hover2Pressed)
				Pressed2Hover=Hover2Pressed;
			else
				Pressed2Hover=new SliderElement(*grp->Pressed2Hover);
		}

		RevHover2NormalState=grp->RevHover2NormalState;
		RevPressed2NormalState=grp->RevPressed2NormalState;
		RevPressed2HoverState=grp->RevPressed2HoverState;

		et[BS_Normal]  [BS_Normal]=NormalStyle;
		et[BS_Normal]  [BS_Hover]=Normal2Hover;
		et[BS_Normal]  [BS_Pressed]=Normal2Pressed;
		et[BS_Normal]  [BS_Disabled]=NULL;

		et[BS_Hover]   [BS_Normal]=Hover2Normal;
		et[BS_Hover]   [BS_Hover]=HoverStyle;
		et[BS_Hover]   [BS_Pressed]=Hover2Pressed;
		et[BS_Hover]   [BS_Disabled]=NULL;

		et[BS_Pressed] [BS_Normal]=Pressed2Normal;
		et[BS_Pressed] [BS_Hover]=Pressed2Hover;
		et[BS_Pressed] [BS_Pressed]=PressedStyle;
		et[BS_Pressed] [BS_Disabled]=NULL;

		et[BS_Disabled][BS_Normal]=NULL;
		et[BS_Disabled][BS_Hover]=NULL;
		et[BS_Disabled][BS_Pressed]=NULL;
		et[BS_Disabled][BS_Disabled]=DisabledStyle;

		if(btnUp) {
			btnUp->SetBluePrint(grp->Up);
			btnUp->Resize(StyleGroup->UpArrowSize.x, StyleGroup->UpArrowSize.y);
		}

		if(btnDown) {
			btnDown->SetBluePrint(grp->Down);
			btnDown->Resize(StyleGroup->DownArrowSize.x, StyleGroup->DownArrowSize.y);
			setButtonPositions();
		}
	}

	void SliderBase::SetBluePrint(IWidgetBluePrint *BP) {
		this->BluePrint=(SliderBP*)BP;
		DetermineStyleGroup();
		PointerType=Pointer::Arrow;
	}

	void SliderBase::Draw() {
		if(!isVisible()) return;

		LinkedListOrderedIterator<IWidgetObject> it=Subobjects;
		IWidgetObject *item;

		while(item=it) {
			if(item->isVisible())
				item->Draw();
		}

		layer.Clear();
		rulelayer.Clear();
		symbollayer.Clear();
		ticklayer.Clear();
		textlayer.Clear();
		overlayer.Clear();

		float valrat=(value-minimum)/(maximum-minimum);

		gge::Rectangle rulerect;
		SliderElement *ruleelm=DetermineRuleElement(currentrulestate,nextrulestate);
		SliderElement *symbolelm=DetermineSymbolElement(currentsymbolstate,nextsymbolstate);
		SliderElement *tickelm=DetermineTickElement(currentsymbolstate,nextsymbolstate);

		if(showbuttons) {
			btnUp->Draw();
			btnDown->Draw();
		}
		
		rulerect=RuleRectangle();

		if(drawrule) {
			rulelayer.X=rulerect.Left;
			rulelayer.Y=rulerect.Top;
			rulelayer.W=rulerect.Width;
			rulelayer.H=rulerect.Height;

			ruleelm->DrawRule(rulelayer);
		}

		if(drawoverlay) {
			overlayresizer->Progress();

			((Line *)ruleelm->Overlay)->DrawStart=AlwaysShowCap || !(style==SS_Left || style==SS_Right || style==SS_Verticle) || !(drawsymbol || tickstyle!=STS_None);
			((Line *)ruleelm->Overlay)->DrawEnd=AlwaysShowCap ||(style==SS_Left || style==SS_Right || style==SS_Verticle) || !(drawsymbol || tickstyle!=STS_None);
			ruleelm->DrawOverlay(overlayer);
		}


		int lastpos=0;
		int btnfinish=0;
		int rulefinish=0;
		int symbolfinish=0;

		switch(style) {
			case SS_Right:

				if(!drawrule && !drawsymbol) {
					rulefinish=ruleelm->RuleOffset;
					if(drawoverlay)
						rulefinish+=ruleelm->Overlay->Width()-tickelm->Tick->Texture.W;
				} else {
					if(showbuttons)
						btnfinish=max(btnDown->X() + btnDown->Width(), btnUp->X() + btnUp->Width());

					if(drawrule)
						rulefinish=rulelayer.X + rulelayer.W;

					if(drawsymbol)
						symbolfinish=symbollayer.X + symbolelm->Symbol->Width();
				}

				break;
			case SS_Left:

				if(!drawrule && !drawsymbol) {
					rulefinish=ruleelm->RuleOffset;
					if(drawoverlay)
						rulefinish+=ruleelm->Overlay->Width()-tickelm->Tick->Texture.W;
				} else {
					if(showbuttons)
						btnfinish=max(layer.W-btnDown->X(), layer.W-btnUp->X());

					if(drawrule)
						rulefinish=layer.W-rulelayer.X;

					if(drawsymbol)
						symbolfinish=layer.W-symbollayer.X;
				}

				break;
			case SS_Bottom:

				if(!drawrule && !drawsymbol) {
					rulefinish=ruleelm->RuleOffset;
					if(drawoverlay)
						rulefinish+=ruleelm->Overlay->Height()-tickelm->Tick->Texture.H;
				} else {
					if(showbuttons)
						btnfinish=max(btnDown->Y() + btnDown->Height(), btnUp->Y() + btnUp->Height());

					if(drawrule)
						rulefinish=rulelayer.Y + rulelayer.H;

					if(drawsymbol)
						symbolfinish=symbollayer.Y + symbolelm->Symbol->Height();
				}

				break;
			case SS_Top:

				if(!drawrule && !drawsymbol) {
					rulefinish=ruleelm->RuleOffset;
					if(drawoverlay)
						rulefinish+=ruleelm->Overlay->Height()-tickelm->Tick->Texture.H;
				} else {
					if(showbuttons)
						btnfinish=Max(BaseLayer.H-btnDown->Y(), BaseLayer.H-btnUp->Y());

					if(drawrule)
						rulefinish=BaseLayer.H-rulelayer.Y;

					if(drawsymbol)
						symbolfinish=BaseLayer.H-symbollayer.Y;
				}

				break;
		}

		lastpos=max(rulefinish, symbolfinish);

		float dist=0;
		if(tickstyle&STS_Ticks || tickstyle&STS_LocatedTicks) {
			switch(style) {
				case SS_Right:

					ticklayer.X=lastpos+tickelm->TickOffset.x;
					ticklayer.Y=rulerect.Top + tickelm->StartMargin+tickelm->TickOffset.y;
					ticklayer.W=tickelm->Tick->Texture.W;
					ticklayer.H=rulerect.Height-(tickelm->StartMargin+tickelm->EndMargin);

					lastpos=ticklayer.X+ticklayer.W;
					if(((float)ticklayer.H)/(numberofticks-1)>tickelm->MinTickDistance)
						dist=((float)ticklayer.H)/(numberofticks-1);

					break;
				case SS_Left:

					ticklayer.X=(layer.W-lastpos)-(tickelm->TickOffset.x+tickelm->Tick->Texture.W);
					ticklayer.Y=rulerect.Top + tickelm->StartMargin+tickelm->TickOffset.y;
					ticklayer.W=tickelm->Tick->Texture.W;
					ticklayer.H=rulerect.Height-(tickelm->StartMargin+tickelm->EndMargin);

					lastpos=layer.W-ticklayer.X;
					if(((float)ticklayer.H)/(numberofticks-1)>tickelm->MinTickDistance)
						dist=((float)ticklayer.H)/(numberofticks-1);

					break;
				case SS_Bottom:

					ticklayer.X=rulerect.Left + tickelm->StartMargin+tickelm->TickOffset.x;
					ticklayer.Y=lastpos + tickelm->TickOffset.y;
					ticklayer.W=rulerect.Width-(tickelm->StartMargin+tickelm->EndMargin);
					ticklayer.H=tickelm->Tick->Texture.H;

					lastpos=ticklayer.Y+ticklayer.H;
					if(((float)ticklayer.W)/(numberofticks-1)>tickelm->MinTickDistance)
						dist=((float)ticklayer.W)/(numberofticks-1);

					break;
				case SS_Top:
					ticklayer.Y=(layer.H-lastpos)-(tickelm->TickOffset.y+tickelm->Tick->Texture.H);
					ticklayer.X=rulerect.Left + tickelm->StartMargin+tickelm->TickOffset.x;
					ticklayer.H=tickelm->Tick->Texture.H;
					ticklayer.W=rulerect.Width-(tickelm->StartMargin+tickelm->EndMargin);

					lastpos=layer.H-ticklayer.Y;
					if(((float)ticklayer.W)/(numberofticks-1)>tickelm->MinTickDistance)
						dist=((float)ticklayer.W)/(numberofticks-1);

					break;
			}
		}

		if(tickstyle&STS_Ticks)
			ruleelm->DrawTickMarks(ticklayer, dist);
		if(tickstyle&STS_LocatedTicks)
			ruleelm->DrawLocatedTicks(ticklayer,dist, ticknames,minimum,maximum);

		if(tickstyle&STS_Ticks || tickstyle&STS_LocatedTicks) {
			switch(style) {
				case SS_Right:
				case SS_Left:
					ticklayer.H+=((ImageAnimation*)tickelm->Tick)->Height();
					break;

				case SS_Bottom:
				case SS_Top:
					ticklayer.W+=((ImageAnimation*)tickelm->Tick)->Width();
					break;
			}
		}



		if(tickstyle&STS_Numbers || tickstyle&STS_Names || tickstyle&STS_LocatedNames) {
			float sz=0;
			float d, nd;


			switch(style) {
				case SS_Right:
					textlayer.X=lastpos+tickelm->TextOffset.x;
					textlayer.Y=rulerect.Top + tickelm->StartMargin+tickelm->TextOffset.y;
					textlayer.W=layer.W-textlayer.X;
					textlayer.H=rulerect.Height-(tickelm->StartMargin+tickelm->EndMargin);

					sz=textlayer.H;
					break;
				case SS_Left:
					textlayer.W=layer.W-(lastpos+tickelm->TextOffset.x);
					textlayer.X=(layer.W-lastpos)-(tickelm->TextOffset.x+textlayer.W);
					textlayer.Y=rulerect.Top + tickelm->StartMargin+tickelm->TextOffset.y;
					textlayer.H=rulerect.Height-(tickelm->StartMargin+tickelm->EndMargin);

					sz=textlayer.H;
					break;
				case SS_Bottom:
					textlayer.X=rulerect.Left+ tickelm->StartMargin+tickelm->TextOffset.x;
					textlayer.Y=lastpos+tickelm->TextOffset.y;
					textlayer.W=rulerect.Width-(tickelm->StartMargin+tickelm->EndMargin);
					textlayer.H=layer.H-(lastpos+tickelm->TextOffset.y);

					sz=textlayer.W;

					break;
				case SS_Top:
					textlayer.H=tickelm->Font->FontHeight();
					textlayer.Y=(layer.H-lastpos)-(tickelm->TextOffset.y+textlayer.H);
					textlayer.X=rulerect.Left + tickelm->StartMargin+tickelm->TextOffset.x;
					textlayer.W=rulerect.Width-(tickelm->StartMargin+tickelm->EndMargin);

					sz=textlayer.W;

					break;
			}

			if(tickstyle&STS_Numbers) {
				d=numberedtickdistance*(sz/(numberofticks-1));
				nd=numberedtickdistance*((maximum-minimum)/(numberofticks-1));
				tickelm->DrawTickNumbers(textlayer, d, minimum, nd, maximum, numberformat);
			} else if(tickstyle&STS_Names && ticknames.getCount()) {
				d=sz/(ticknames.getCount()-1);
				tickelm->DrawTickNames(textlayer, d, ticknames,false,minimum, maximum);
			} else if(tickstyle&STS_LocatedNames && ticknames.getCount()) {
				tickelm->DrawTickNames(textlayer, 0, ticknames,true,minimum, maximum);
			}

			if(style==SS_Left || style==SS_Right)
				textlayer.H+=tickelm->Font->FontHeight();
		}

		if(drawsymbol)
			symbolelm->DrawSymbol(symbollayer);

		ProgressionCheck();
		BaseLayer.Clear();
	}

	void SliderBase::on_focus_event(bool state,IWidgetObject *related) {
		if(state && nexttickstate==SS_Normal) 
			TickTransition(SS_Hover);
		if(!state && nexttickstate==SS_Hover)
			TickTransition(SS_Normal);

		if(!isFocussed())
			IWidgetContainer::Deactivate();
	}

	bool SliderBase::keyb_event(KeyboardEventType event,int keycode,KeyboardModifier modifier) {
		if(passivemode) return false;
		if(event==KEYB_EVENT_DOWN && keycode==38) {
			goUp();
			cancelclick=false;

			return true;
		}
		if(event==KEYB_EVENT_UP && keycode==38) {
			stopUp();
			if(!cancelclick)
				Up();

			return true;
		}
		if(event==KEYB_EVENT_DOWN && keycode==40) {
			goDown();
			cancelclick=false;

			return true;
		}
		if(event==KEYB_EVENT_UP && keycode==40) {
			stopDown();
			if(!cancelclick)
				Down();

			return true;
		}
		if(event==KEYB_EVENT_DOWN && keycode==39) {
			goUp();
			cancelclick=false;

			return true;
		}
		if(event==KEYB_EVENT_UP && keycode==39) {
			stopUp();
			if(!cancelclick)
				Up();

			return true;
		}
		if(event==KEYB_EVENT_DOWN && keycode==37) {
			goDown();
			cancelclick=false;

			return true;
		}
		if(event==KEYB_EVENT_UP && keycode==37) {
			stopDown();
			if(!cancelclick)
				Down();

			return true;
		}
		if(event==KEYB_EVENT_DOWN && keycode==33) {
			golarge=true;
			goUp();
			cancelclick=false;

			return true;
		}
		if(event==KEYB_EVENT_UP && keycode==33) {
			stopUp();
			if(!cancelclick)
				Up();

			return true;
		}
		if(event==KEYB_EVENT_DOWN && keycode==34) {
			golarge=true;
			goDown();
			cancelclick=false;

			return true;
		}
		if(event==KEYB_EVENT_UP && keycode==34) {
			stopDown();
			if(!cancelclick)
				Down();

			return true;
		}

		return false;
	}

	SliderElement *SliderBase::DetermineRuleElement(SliderStates currentstate,SliderStates nextstate) {
		if(!isEnabled()) {
			DisabledStyle->setStyle(style);
			return DisabledStyle;
		}
		if(passivemode) {
			et[0][0]->setStyle(style);
			return et[0][0];
		}

		SliderElement *ret=NULL;
		ret=et[currentrulestate][nextrulestate];

		if(ret==NULL && currentrulestate==nextrulestate) {
			ret=et[0][0];
		}

		if(ret)
			ret->setStyle(style);
		return ret;
	}
	SliderElement *SliderBase::DetermineSymbolElement(SliderStates currentstate,SliderStates nextstate) {
		if(!isEnabled()) {
			DisabledStyle->setStyle(style);
			return DisabledStyle;
		}
		if(passivemode) {
			et[0][0]->setStyle(style);
			return et[0][0];
		}

		SliderElement *ret=NULL;
		ret=et[currentsymbolstate][nextsymbolstate];

		if(ret==NULL && currentsymbolstate==nextsymbolstate) {
			ret=et[0][0];
		}

		if(ret)
			ret->setStyle(style);
		return ret;
	}
	SliderElement *SliderBase::DetermineTickElement(SliderStates currentstate,SliderStates nextstate) {
		if(!isEnabled()) {
			DisabledStyle->setStyle(style);
			return DisabledStyle;
		}
		if(passivemode) {
			et[0][0]->setStyle(style);
			return et[0][0];
		}

		SliderElement *ret=NULL;
		ret=et[currenttickstate][nexttickstate];

		if(ret==NULL && currenttickstate==nexttickstate) {
			ret=et[0][0];
		}

		if(ret)
			ret->setStyle(style);
		return ret;
	}
	SliderElement *SliderBase::DetermineOverlayElement(SliderStates currentstate,SliderStates nextstate) {
		if(!isEnabled()) {
			DisabledStyle->setStyle(style);
			return DisabledStyle;
		}

		SliderElement *ret=NULL;
		ret=et[currentoverlaystate][nextoverlaystate];

		if(ret==NULL && currentoverlaystate==nextoverlaystate) {
			ret=et[0][0];
		}

		if(ret)
			ret->setStyle(style);
		return ret;
	}
	void SliderBase::RuleTransition(SliderStates target,bool temporal,bool progression) {
		bool playsound=false;
		if(currentrulestate==nextrulestate && nextrulestate==target)
			playsound=true;

		if(temporalrulestate) {
			currentrulestate=prevrulestate;
			temporalrulestate=false;
		}

		prevrulestate=currentrulestate;
		currentrulestate=nextrulestate;
		nextrulestate=target;
		SliderElement *elm=DetermineRuleElement(currentrulestate,nextrulestate);

		//if there is play state sound
		if(elm && playsound)
			if(elm->Sound)
				elm->Sound->CreateWave()->Play();

		if(currentrulestate!=nextrulestate) {
			if(elm) {
				currentruleanimstart=Main.CurrentTime;

				if(elm==DetermineRuleElement(prevrulestate,currentrulestate)) {
					int remaining=(int)currentruleanimend-currentruleanimstart;
					if(remaining<=0) {
						remaining=0;
						elm->ReadyAnimation(true);
					} else
						elm->Reverse();

					currentruleanimend=currentruleanimstart+elm->Duration-remaining;
				} else {
					elm->ReadyAnimation(false);
					currentruleanimend=Main.CurrentTime+elm->Duration;
				}
			} else {
				prevrulestate=currentrulestate;
				currentrulestate=nextrulestate;
				elm=DetermineRuleElement(currentrulestate,nextrulestate);
				if(temporal) {
					currentruleanimend=Main.CurrentTime+SLIDER_CLICK_DOWNDURATION;
				}

				if(elm)
					if(elm->Sound)
						elm->Sound->CreateWave()->Play();
			}
		}

		temporalrulestate=temporal;
	}
	void SliderBase::SymbolTransition(SliderStates target,bool temporal,bool progression) {
		bool playsound=false;
		if(currentsymbolstate==nextsymbolstate && nextsymbolstate==target)
			playsound=true;

		if(temporalsymbolstate) {
			currentsymbolstate=prevsymbolstate;
			temporalsymbolstate=false;
		}

		prevsymbolstate=currentsymbolstate;
		currentsymbolstate=nextsymbolstate;
		nextsymbolstate=target;
		SliderElement *elm=DetermineSymbolElement(currentsymbolstate,nextsymbolstate);

		//if there is play state sound
		if(elm && playsound)
			if(elm->Sound)
				elm->Sound->CreateWave()->Play();

		if(currentsymbolstate!=nextsymbolstate) {
			if(elm) {
				currentsymbolanimstart=Main.CurrentTime;

				if(elm==DetermineSymbolElement(prevsymbolstate,currentsymbolstate)) {
					int remaining=(int)currentsymbolanimend-currentsymbolanimstart;
					if(remaining<=0) {
						remaining=0;
						elm->ReadyAnimation(true);
					} else
						elm->Reverse();

					currentsymbolanimend=currentsymbolanimstart+elm->Duration-remaining;
				} else {
					elm->ReadyAnimation(false);
					currentsymbolanimend=Main.CurrentTime+elm->Duration;
				}
			} else {
				prevsymbolstate=currentsymbolstate;
				currentsymbolstate=nextsymbolstate;
				elm=DetermineSymbolElement(currentsymbolstate,nextsymbolstate);
				if(temporal) {
					currentsymbolanimend=Main.CurrentTime+SLIDER_CLICK_DOWNDURATION;
				}

				if(elm)
					if(elm->Sound)
						elm->Sound->CreateWave()->Play();
			}
		}

		temporalsymbolstate=temporal;
		setSymbolPosition();
	}
	void SliderBase::TickTransition(SliderStates target,bool temporal,bool progression) {
		bool playsound=false;
		if(currenttickstate==nexttickstate && nexttickstate==target)
			playsound=true;

		if(temporaltickstate) {
			currenttickstate=prevtickstate;
			temporaltickstate=false;
		}

		prevtickstate=currenttickstate;
		currenttickstate=nexttickstate;
		nexttickstate=target;
		SliderElement *elm=DetermineTickElement(currenttickstate,nexttickstate);

		//if there is play state sound
		if(elm && playsound)
			if(elm->Sound)
				elm->Sound->CreateWave()->Play();

		if(currenttickstate!=nexttickstate) {
			if(elm) {
				currenttickanimstart=Main.CurrentTime;

				if(elm==DetermineTickElement(prevtickstate,currenttickstate)) {
					int remaining=(int)currenttickanimend-currenttickanimstart;
					if(remaining<=0) {
						remaining=0;
						elm->ReadyAnimation(true);
					} else
						elm->Reverse();

					currenttickanimend=currenttickanimstart+elm->Duration-remaining;
				} else {
					elm->ReadyAnimation(false);
					currenttickanimend=Main.CurrentTime+elm->Duration;
				}
			} else {
				prevtickstate=currenttickstate;
				currenttickstate=nexttickstate;
				elm=DetermineTickElement(currenttickstate,nexttickstate);
				if(temporal) {
					currenttickanimend=Main.CurrentTime+SLIDER_CLICK_DOWNDURATION;
				}

				if(elm)
					if(elm->Sound)
						elm->Sound->CreateWave()->Play();
			}
		}

		temporaltickstate=temporal;
	}
	void SliderBase::OverlayTransition(SliderStates target,bool temporal,bool progression) {
		bool playsound=false;
		if(currentoverlaystate==nextoverlaystate && nextoverlaystate==target)
			playsound=true;

		if(temporaloverlaystate) {
			currentoverlaystate=prevoverlaystate;
			temporaloverlaystate=false;
		}

		prevoverlaystate=currentoverlaystate;
		currentoverlaystate=nextoverlaystate;
		nextoverlaystate=target;
		SliderElement *elm=DetermineOverlayElement(currentoverlaystate,nextoverlaystate);

		//if there is play state sound
		if(elm && playsound)
			if(elm->Sound)
				elm->Sound->CreateWave()->Play();

		if(currentoverlaystate!=nextoverlaystate) {
			if(elm) {
				currentoverlayanimstart=Main.CurrentTime;

				if(elm==DetermineOverlayElement(prevoverlaystate,currentoverlaystate)) {
					int remaining=(int)currentoverlayanimend-currentoverlayanimstart;
					if(remaining<=0) {
						remaining=0;
						elm->ReadyAnimation(true);
					} else
						elm->Reverse();

					currentoverlayanimend=currentoverlayanimstart+elm->Duration-remaining;
				} else {
					elm->ReadyAnimation(false);
					currentoverlayanimend=Main.CurrentTime+elm->Duration;
				}
			} else {
				prevoverlaystate=currentoverlaystate;
				currentoverlaystate=nextoverlaystate;
				elm=DetermineOverlayElement(currentoverlaystate,nextoverlaystate);
				if(temporal) {
					currentoverlayanimend=Main.CurrentTime+SLIDER_CLICK_DOWNDURATION;
				}

				if(elm)
					if(elm->Sound)
						elm->Sound->CreateWave()->Play();
			}
		}

		temporaloverlaystate=temporal;
	}
	void SliderBase::Setup(SliderTickSyle tickstyle, bool showsymbol, bool showrule, bool showbuttons, bool showoverlay, SliderRuleActions ruleaction,bool EnableSymbolDrag,bool EnableTickClick) {
		this->tickstyle=tickstyle;
		this->drawsymbol=showsymbol;
		this->drawrule=showrule;
		this->showbuttons=showbuttons;
		this->ruleaction=ruleaction;
		this->drawoverlay=showoverlay;
		overlayer.isVisible=drawoverlay;

		if(style==SS_Verticle||style==SS_Horizontal)
			this->tickstyle=STS_None;

		textlayer.EnableClipping= (style==SS_Left || style==SS_Right || style==SS_Verticle);


		if(showbuttons) {
			btnUp->Show();
			btnDown->Show();

			btnUp->Resize(StyleGroup->UpArrowSize.x,StyleGroup->UpArrowSize.y);
			btnDown->Resize(StyleGroup->DownArrowSize.x,StyleGroup->DownArrowSize.y);

			setButtonPositions();
		} else {
			btnUp->Hide();
			btnDown->Hide();
		}

		rulemouseevent->Enabled	  = showrule;
		symbolmouseevent->Enabled = EnableSymbolDrag && showsymbol;
		tickmouseevent->Enabled   = tickstyle!=STS_None && EnableTickClick;
		textmouseevent->Enabled   = (tickstyle&STS_Numbers || tickstyle&STS_Names) && EnableTickClick;

		setSymbolPosition(true);
	}
	void SliderBase::setButtonPositions() {
		switch(style) {
			case SS_Horizontal:
				btnDown->Move(0, (height-(StyleGroup->DownArrowSize.y+StyleGroup->ArrowOffset.y))/2+StyleGroup->ArrowOffset.y);
				btnUp->Move(width-StyleGroup->UpArrowSize.x, (height-(StyleGroup->DownArrowSize.y+StyleGroup->ArrowOffset.y))/2+StyleGroup->ArrowOffset.y);
				break;
			case SS_Top:
				btnDown->Move(0, height-StyleGroup->DownArrowSize.y-StyleGroup->ArrowOffset.y);
				btnUp->Move(width-StyleGroup->UpArrowSize.x, height-StyleGroup->DownArrowSize.y-StyleGroup->ArrowOffset.y);
				break;
			case SS_Bottom:
				btnDown->Move(0, StyleGroup->ArrowOffset.y);
				btnUp->Move(width-StyleGroup->UpArrowSize.x, StyleGroup->ArrowOffset.y);
				break;
			case SS_Right:
				btnUp->Move(StyleGroup->ArrowOffset.x, 0);
				btnDown->Move(StyleGroup->ArrowOffset.x, height-(StyleGroup->DownArrowSize.y));
				break;
			case SS_Left:
				btnUp->Move(width - (StyleGroup->ArrowOffset.x+StyleGroup->UpArrowSize.x), 0);
				btnDown->Move(width - (StyleGroup->ArrowOffset.x+StyleGroup->DownArrowSize.x), height-(StyleGroup->DownArrowSize.y));
				break;
			case SS_Verticle:
				btnUp->Move((width - (StyleGroup->UpArrowSize.x+StyleGroup->ArrowOffset.x))/2, 0);
				btnDown->Move((width - (StyleGroup->DownArrowSize.x+StyleGroup->ArrowOffset.x))/2, height-(StyleGroup->DownArrowSize.y));
				break;
		}
	}
	void SliderBase::setSymbolPosition(bool instant) {
		float valrat=(value-minimum)/(maximum-minimum);

		SliderElement *ruleelm=DetermineRuleElement(currentrulestate,nextrulestate);
		SliderElement *symbolelm=DetermineSymbolElement(currentsymbolstate,nextsymbolstate);

		gge::Rectangle rulesize=RuleRectangle();
		int h,w,p;
		Point pnt;
		float dist;

		if(drawsymbol) {
			symbolmover->Progress();
			symbollayer.W=symbolelm->Symbol->Width();
			symbollayer.H=symbolelm->Symbol->Height();
			switch(style) {
				case SS_Right:
				case SS_Left:
				case SS_Verticle:
					h=rulesize.Height - (symbolelm->StartMargin + symbolelm->EndMargin);
					p=h*valrat;
					p=h-p;
					p+= symbolelm->StartMargin + symbolelm->SymbolOffset.y + rulesize.Top;
					dist=(float)abs(p-symbollayer.Y)/h;

					break;
				case SS_Bottom:
				case SS_Top:
				case SS_Horizontal:
					w=rulesize.Width - (symbolelm->StartMargin + symbolelm->EndMargin);
					p=w*valrat;
					p+= symbolelm->StartMargin + symbolelm->SymbolOffset.x + rulesize.Left;
					dist=(float)abs(p-symbollayer.X)/w;

					break;
			}

			switch(style) {
				case SS_Right:
					pnt=Point(symbolelm->SymbolOffset.x, p);
					break;
				case SS_Left:
					pnt=Point(width-(symbolelm->Symbol->Width()+symbolelm->SymbolOffset.x), p);
					break;
				case SS_Horizontal:
					pnt=Point(p,(height-(symbolelm->SymbolOffset.y+symbolelm->Symbol->Height()))/2);
					break;
				case SS_Verticle:
					pnt=Point((width-(symbolelm->SymbolOffset.x+symbolelm->Symbol->Width()))/2, p);
					break;
				case SS_Bottom:
					pnt=Point(p,symbolelm->SymbolOffset.y);
					break;
				case SS_Top:
					pnt=Point(p,height-(symbolelm->SymbolOffset.y+symbolelm->Symbol->Height()));
					break;

			}

			if(instant)
				symbolmover->Setup(pnt, 0);
			else
				symbolmover->Setup(pnt, SlidingDuration+SlidingDurationValueMod*dist);
		}

		if(drawoverlay) {
			overlayresizer->Progress();
			gge::Rectangle r;
			switch(style) {
				case SS_Right:
				case SS_Verticle:
					r.Left=rulesize.Left+ruleelm->OverlayOffset.x;
					break;

				case SS_Left:
					r.Left=rulesize.Left-ruleelm->OverlayOffset.x;
					break;

				case SS_Bottom:
				case SS_Horizontal:
					r.Top=rulesize.Top+ruleelm->OverlayOffset.y;
					break;

				case SS_Top:
					r.Top=rulesize.Top-ruleelm->OverlayOffset.y;
					break;

			}
			switch(style) {
				case SS_Right:
				case SS_Left:
				case SS_Verticle:
					r.Width=ruleelm->Overlay->Width();
					r.Top=rulesize.Top+ruleelm->OverlayOffset.y;
					if(tickstyle!=STS_None || drawsymbol)
						r.Height=(rulesize.Height-(ruleelm->StartMargin+ruleelm->EndMargin))*valrat+ruleelm->StartMargin;
					else
						r.Height=rulesize.Height*valrat;

					r.Height+=ruleelm->OverlaySizeMod;
					if(AlwaysShowCap && (drawsymbol || tickstyle!=STS_None))
						r.Height+=((Line*)ruleelm->Overlay)->Start->Height();
					r.Top+=rulesize.Height-r.Height;
					dist=(float)abs(r.Height-overlayer.H)/rulesize.Height;

					break;

				case SS_Bottom:
				case SS_Top:
				case SS_Horizontal:
					r.Height=ruleelm->Overlay->Height();
					r.Left=rulesize.Left+ruleelm->OverlayOffset.x;
					if(tickstyle!=STS_None || drawsymbol)
						r.Width=(rulesize.Width-(ruleelm->StartMargin+ruleelm->EndMargin))*valrat+ruleelm->StartMargin;
					else
						r.Width=rulesize.Width*valrat;
					r.Width+=ruleelm->OverlaySizeMod;
					if(AlwaysShowCap && (drawsymbol || tickstyle!=STS_None))
						r.Width+=((Line*)ruleelm->Overlay)->End->Width();
					dist=(float)abs(r.Width-overlayer.W)/rulesize.Width;

					break;
			}

			if(instant)
				overlayresizer->Setup(r, 0);
			else
				overlayresizer->Setup(r, OverlaySlidingDuration+OverlaySlidingDurationValueMod*dist);
		}

	}
	bool SliderBase::mouse_event(MouseEventType event,int x,int y) {
		if(passivemode) return true;
		switch(event) {
		case MOUSE_EVENT_OVER:
			TickTransition(SS_Hover);
			break;
		case MOUSE_EVENT_OUT:
			if(!isFocussed()) 
				TickTransition(SS_Normal);

			break;
		}
		return true;
	}

	bool SliderBase::rule_mouse(MouseEventType event,int x,int y) {
		static int px, py;
		if(passivemode) return true;

		switch(event) {
		case MOUSE_EVENT_OVER:
			RuleTransition(SS_Hover);
			return mouse_event(event, x, y);
		case MOUSE_EVENT_OUT:
			RuleTransition(SS_Normal);
			return mouse_event(event, x, y);
		case MOUSE_EVENT_LDOWN:
			if(!nofocus)
				this->SetFocus();
			RuleTransition(SS_Pressed);

			if(ruleaction==SRA_JumpTo) {
				px=x;
				py=y;
				MoveSymbolTo(x,y);
			} else if(ruleaction=SRA_LargeIncrement) {
				golarge=true;

				if(CalcMoveSymbolTo(x,y)>value) {
					setValue(value+largeincrement);
					goUp();
				}
				else {
					setValue(value-largeincrement);
					goDown();
				}
			}

			mstate=SS_Pressed;
			break;
		case MOUSE_EVENT_LUP:
			if(!isFocussed())
				RuleTransition(SS_Normal);
			else
				RuleTransition(SS_Hover);

			if(goup)
				stopUp();
			if(godown)
				stopDown();

			mstate=SS_Normal;
			break;
		case MOUSE_EVENT_LCLICK:
			ClickEvent();
			
			break;
		case MOUSE_EVENT_MOVE:
			if(mstate==SS_Pressed) {
				if(ruleaction==SRA_JumpTo && (px!=x || py!=y))
					MoveSymbolTo(x,y);

				px=x;
				py=y;
			}

			break;
		}

		return true;
	}
	bool SliderBase::symbol_mouse(MouseEventType event,int x,int y) {
		static int px, py, ix, iy;
		if(passivemode) return true;

		switch(event) {
		case MOUSE_EVENT_OVER:
			SymbolTransition(SS_Hover);

			return mouse_event(event, x, y);
		case MOUSE_EVENT_OUT:
			SymbolTransition(SS_Normal);

			return mouse_event(event, x, y);
		case MOUSE_EVENT_LDOWN:
			if(!nofocus)
				this->SetFocus();
			SymbolTransition(SS_Pressed);

			ix=px=x;
			iy=py=y;

			mstate=SS_Pressed;
			break;
		case MOUSE_EVENT_LUP:
			SymbolTransition(SS_Hover);

			mstate=SS_Normal;
			break;
		case MOUSE_EVENT_LCLICK:
			ClickEvent();
			
			break;
		case MOUSE_EVENT_MOVE:
			if(mstate==SS_Pressed) {

				if(x!=px || y!=py)
					MoveSymbolToSymbol(x-ix, y-iy);


				px=x;
				py=y;
			}
			
			break;
		}

		return true;
	}
	bool SliderBase::tick_mouse(MouseEventType event,int x,int y) {
		static int px, py;
		if(passivemode) return true;

		switch(event) {
		case MOUSE_EVENT_OVER:
		case MOUSE_EVENT_OUT:
			return mouse_event(event, x, y);
		case MOUSE_EVENT_LDOWN:
			if(!nofocus)
				this->SetFocus();

			px=x;
			py=y;
			MoveSymbolToTick(x,y);

			mstate=SS_Pressed;
			break;
		case MOUSE_EVENT_LUP:
			if(goup)
				stopUp();
			if(godown)
				stopDown();

			mstate=SS_Normal;
			break;
		case MOUSE_EVENT_LCLICK:
			ClickEvent();
			
			break;
		case MOUSE_EVENT_MOVE:
			if(mstate==SS_Pressed) {
				if(px!=x || py!=y)
					MoveSymbolToTick(x,y);

				px=x;
				py=y;
			}

			break;
		}

		return true;
	}
	bool SliderBase::text_mouse(MouseEventType event,int x,int y) {
		static int px, py;
		if(passivemode) return true;

		switch(event) {
		case MOUSE_EVENT_OVER:
		case MOUSE_EVENT_OUT:
			return mouse_event(event, x, y);
		case MOUSE_EVENT_LDOWN:
			if(!nofocus)
				this->SetFocus();

			px=x;
			py=y;
			MoveSymbolToText(x,y);

			mstate=SS_Pressed;
			break;
		case MOUSE_EVENT_LUP:

			if(goup)
				stopUp();
			if(godown)
				stopDown();

			mstate=SS_Normal;
			break;
		case MOUSE_EVENT_LCLICK:
			ClickEvent();
			
			break;
		case MOUSE_EVENT_MOVE:
			if(mstate==SS_Pressed) {
				if(px!=x || py!=y)
					MoveSymbolToText(x,y);

				px=x;
				py=y;
			}

			break;
		}

		return true;
	}
	void SliderBase::SetFocus() {
		if(passivemode) return;
		IWidgetObject::SetFocus();

		if(nexttickstate==SS_Normal)
			TickTransition(SS_Hover);
	}

	void SliderBase::setValue(float value) { 
		if(value<minimum) value=minimum;
		if(value>maximum) value=maximum;
		value/=steps;
		value=Round(value);
		value*=steps;
		slider_change_event_params p;
		p.before=this->value;
		this->value=value;
		p.after=this->value;
		if(p.before!=p.after) {
			ChangeEvent(p);
		}

		
		float valrat=(value-minimum)/(maximum-minimum);
		if(valrat==prevrat)
			return;
		prevrat=valrat;

		setSymbolPosition();
	}

	void SliderBase::goUp() {
		if(goup) return;
		int time=SLIDER_TIMEOUT;
		if(golarge)
			intervaltimeout=(FastMoveTime/(maximum-minimum))*largeincrement;
		else
			intervaltimeout=(ButtonMoveTime/(maximum-minimum))*buttonincrement;

		if(!delayclicks)
			time=intervaltimeout;
		goup=Main.RegisterInterval(time, this, fn_goUp);
	}
	void SliderBase::stopUp() {
		Main.UnregisterInterval(goup);
		goup=NULL;
		golarge=false;
	}
	void SliderBase::goDown() {
		if(godown) return;
		int time=SLIDER_TIMEOUT;
		if(golarge)
			intervaltimeout=(FastMoveTime/(maximum-minimum))*largeincrement;
		else
			intervaltimeout=(ButtonMoveTime/(maximum-minimum))*buttonincrement;

		if(!delayclicks)
			time=intervaltimeout;
		godown=Main.RegisterInterval(time, this, fn_goDown);
	}
	void SliderBase::stopDown() {
		Main.UnregisterInterval(godown);
		godown=NULL;
		golarge=false;
	}
	gge::Rectangle SliderBase::RuleRectangle() {
		gge::Rectangle rulesize;

		SliderElement *ruleelm=DetermineRuleElement(currentrulestate, nextrulestate);

		if(showbuttons)
			rulesize=ruleelm->RuleRectangle(layer, StyleGroup->UpArrowSize, StyleGroup->DownArrowSize, StyleGroup->ArrowOffset);
		else
			rulesize=ruleelm->RuleRectangle(layer);

		return rulesize;
	}

	void SliderBase::MoveSymbolTo(int x, int y) {
		setValue(CalcMoveSymbolTo(x,y));
	}
	float SliderBase::CalcMoveSymbolTo(int x, int y) {
		SliderElement *symbolelm=DetermineSymbolElement(currentsymbolstate,nextsymbolstate);
		float v,position,h,w;
		gge::Rectangle rulesize=RuleRectangle();

		switch(style) {
			case SS_Right:
			case SS_Left:
			case SS_Verticle:
				position=y;
				position-=symbolelm->StartMargin;
				h=rulesize.Height - (symbolelm->StartMargin + symbolelm->EndMargin);

				position=h-position;

				v=position / h;
				return v*(maximum-minimum) + minimum;
			case SS_Top:
			case SS_Bottom:
			case SS_Horizontal:
			default:
				position=x;
				position-=symbolelm->StartMargin;
				w=rulesize.Width - (symbolelm->StartMargin + symbolelm->EndMargin);

				v=position / w;
				return v*(maximum-minimum) + minimum;
		}
	}
	void SliderBase::MoveSymbolToSymbol(int x, int y) {
		SliderElement *symbolelm=DetermineSymbolElement(currentsymbolstate,nextsymbolstate);
		gge::Rectangle rulesize=RuleRectangle();

		switch(style) {
			case SS_Right:
			case SS_Left:
			case SS_Verticle:
				y+=symbollayer.Y-rulesize.Top-symbolelm->SymbolOffset.y;
				break;
			case SS_Bottom:
			case SS_Top:
			case SS_Horizontal:
				x+=symbollayer.X-rulesize.Left-symbolelm->SymbolOffset.x;
				break;
		}
		setValue(CalcMoveSymbolTo(x,y));
		setSymbolPosition(!SmoothDrag);
	}
	void SliderBase::MoveSymbolToTick(int x, int y) {
		SliderElement *ruleelm=DetermineRuleElement(currentsymbolstate,nextsymbolstate);
		gge::Rectangle rulesize=RuleRectangle();

		switch(style) {
			case SS_Right:
			case SS_Left:
			case SS_Verticle:
				y+=ruleelm->StartMargin+ruleelm->TickOffset.y;
				break;
			case SS_Bottom:
			case SS_Top:
			case SS_Horizontal:
				x+=ruleelm->StartMargin+ruleelm->TickOffset.x;
				break;
		}
		setValue(CalcMoveSymbolTo(x,y));
	}
	void SliderBase::MoveSymbolToText(int x, int y) {
		SliderElement *ruleelm=DetermineRuleElement(currentsymbolstate,nextsymbolstate);
		gge::Rectangle rulesize=RuleRectangle();

		switch(style) {
			case SS_Right:
			case SS_Left:
			case SS_Verticle:
				y+=ruleelm->StartMargin+ruleelm->TextOffset.y;
				break;
			case SS_Bottom:
			case SS_Top:
			case SS_Horizontal:
				x+=ruleelm->StartMargin+ruleelm->TextOffset.x;
				break;
		}
		setValue(CalcMoveSymbolTo(x,y));
	}
	void SliderBase::ProgressionCheck() {
		if(Main.CurrentTime>currentruleanimend) {
			if(currentrulestate!=nextrulestate) {
				if(temporalrulestate) {
					currentruleanimend=Main.CurrentTime+SLIDER_CLICK_DOWNDURATION;
					temporalrulestate=false;
				} else {
					RuleTransition(nextrulestate,false,true);
				}
			} else if(temporalrulestate) {
				RuleTransition(prevrulestate,false,true);
				temporalrulestate=false;
			}
		}
		if(Main.CurrentTime>currentsymbolanimend) {
			if(currentsymbolstate!=nextsymbolstate) {
				if(temporalsymbolstate) {
					currentsymbolanimend=Main.CurrentTime+SLIDER_CLICK_DOWNDURATION;
					temporalsymbolstate=false;
				} else {
					SymbolTransition(nextsymbolstate,false,true);
				}
			} else if(temporalsymbolstate) {
				SymbolTransition(prevsymbolstate,false,true);
				temporalsymbolstate=false;
			}
		}
		if(Main.CurrentTime>currenttickanimend) {
			if(currenttickstate!=nexttickstate) {
				if(temporaltickstate) {
					currenttickanimend=Main.CurrentTime+SLIDER_CLICK_DOWNDURATION;
					temporaltickstate=false;
				} else {
					TickTransition(nexttickstate,false,true);
				}
			} else if(temporaltickstate) {
				TickTransition(prevtickstate,false,true);
				temporaltickstate=false;
			}
		}
		if(Main.CurrentTime>currentoverlayanimend) {
			if(currentoverlaystate!=nextoverlaystate) {
				if(temporaloverlaystate) {
					currentoverlayanimend=Main.CurrentTime+SLIDER_CLICK_DOWNDURATION;
					temporaloverlaystate=false;
				} else {
					OverlayTransition(nextoverlaystate,false,true);
				}
			} else if(temporaloverlaystate) {
				OverlayTransition(prevoverlaystate,false,true);
				temporaloverlaystate=false;
			}
		}
	}
}
