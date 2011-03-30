#include "Button.h"
#include "../Engine/Wave.h"
#include <cstring>
#include "../Engine/GGEMain.h"

namespace gorgonwidgets {
	Button::Button(ButtonBP *BluePrint,IWidgetContainer &container) : 
		IWidgetObject(container),
		BluePrint(BluePrint),
		nextstate(BS_Normal), currentstate(BS_Normal),prevstate(BS_Normal),
		temporalstate(false),
		NormalStyle(NULL), HoverStyle(NULL), PressedStyle(NULL),
		DisabledStyle(NULL),
		Normal2Hover(NULL), Normal2Pressed(NULL),
		Hover2Pressed(NULL), Hover2Normal(NULL),
		Pressed2Normal(NULL), Pressed2Hover(NULL),
		mstate(BS_Normal), 
		Icon(NULL), HoverIcon(NULL),
		PressedIcon(NULL), DisabledIcon(NULL),
		PressEvent("Pressed", this),
		ReleaseEvent("Released", this),
		ClickEvent("Click", this), 
		textlayer(0,0, 100,100),iconlayer(0,0, 100,100)

	{
		PointerType=BluePrint->PointerType;

		textlayer.EnableClipping=true;
		layer.Add(textlayer, 0);
		layer.Add(iconlayer, 0);

		int i;
		for(i=0;i<4;i++)
			memset(et[i],0,4*sizeof(ButtonElement*));

		Resize(-1,-1);

		IWidgetObject::ClickEvent.DoubleLink(ClickEvent);
	}
	Button::Button(ButtonBP *BluePrint) : 
		IWidgetObject(),
		BluePrint(BluePrint),
		nextstate(BS_Normal), currentstate(BS_Normal),prevstate(BS_Normal),
		temporalstate(false),
		NormalStyle(NULL), HoverStyle(NULL), PressedStyle(NULL),
		DisabledStyle(NULL),
		Normal2Hover(NULL), Normal2Pressed(NULL),
		Hover2Pressed(NULL), Hover2Normal(NULL),
		Pressed2Normal(NULL), Pressed2Hover(NULL),
		mstate(BS_Normal), 
		Icon(NULL), HoverIcon(NULL),
		PressedIcon(NULL), DisabledIcon(NULL),
		PressEvent("Pressed", this),
		ReleaseEvent("Released", this),
		ClickEvent("Click", this), 
		textlayer(0,0, 100,100),iconlayer(0,0, 100,100)

	{
		PointerType=BluePrint->PointerType;

		textlayer.EnableClipping=true;
		layer.Add(textlayer, 0);
		layer.Add(iconlayer, 0);

		int i;
		for(i=0;i<4;i++)
			memset(et[i],0,4*sizeof(ButtonElement*));

		Resize(-1,-1);

		IWidgetObject::ClickEvent.DoubleLink(ClickEvent);
	}

	ButtonStates Button::currentState() {
		return currentstate;
	}

	void Button::Draw() {
		layer.Clear();
		if(!isvisible)
			return;
		if(!container)
			return;
		if(!container->isVisible())
			return;

		if(!isenabled) {
			if(DisabledStyle)
				DisabledStyle->Draw(layer,textlayer,iconlayer,text,DisabledIcon);
		} else {
			ButtonElement *btn=DetermineElement(currentstate,nextstate);
			if(btn) {
				ImageResource *icon;
				switch(currentstate) {
				case BS_Hover:
					if(HoverIcon==NULL)
						icon=Icon;
					else
						icon=HoverIcon;
					break;
				case BS_Pressed:
					if(PressedIcon==NULL)
						icon=Icon;
					else
						icon=PressedIcon;
					break;
				case BS_Normal:
					icon=Icon;
					break;
				}
				btn->Draw(layer,textlayer,iconlayer,text,icon);
			}
		}

		ProgressionCheck();
	}

	Button			&Button::SetStyles(ButtonStyleGroup* grp) {
		if(grp->NormalStyle)
			NormalStyle=new ButtonElement(*grp->NormalStyle);
		if(grp->HoverStyle)
			HoverStyle=new ButtonElement(*grp->HoverStyle);
		if(grp->PressedStyle)
			PressedStyle=new ButtonElement(*grp->PressedStyle);
		if(grp->DisabledStyle)
			DisabledStyle=new ButtonElement(*grp->DisabledStyle);
		if(grp->Normal2Hover)
			Normal2Hover=new ButtonElement(*grp->Normal2Hover);
		if(grp->Normal2Pressed)
			Normal2Pressed=new ButtonElement(*grp->Normal2Pressed);
		if(grp->Hover2Pressed)
			Hover2Pressed=new ButtonElement(*grp->Hover2Pressed);
		if(grp->Hover2Normal) {
			if(grp->Hover2Normal==grp->Normal2Hover)
				Hover2Normal=Normal2Hover;
			else
				Hover2Normal=new ButtonElement(*grp->Hover2Normal);
		}
		if(grp->Pressed2Normal) {
			if(grp->Pressed2Normal==grp->Normal2Pressed)
				Pressed2Normal=Normal2Pressed;
			else
				Pressed2Normal=new ButtonElement(*grp->Pressed2Normal);
		}
		if(grp->Pressed2Hover) {
			if(grp->Pressed2Hover==grp->Hover2Pressed)
				Pressed2Hover=Hover2Pressed;
			else
				Pressed2Hover=new ButtonElement(*grp->Pressed2Hover);
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

		return *this;
	}

	void Button::Resize(int Cx,int Cy) {
		int id,val=1000000,i,v;
		for(i=0;i<BluePrint->StyleGroups.getCount();i++) {
			if(BluePrint->StyleGroups[i]->PreferredWidth==0)
				v=1+abs(BluePrint->StyleGroups[i]->PreferredHeight-Cy);
			else if(BluePrint->StyleGroups[i]->PreferredHeight==0)
				v=abs(BluePrint->StyleGroups[i]->PreferredWidth-Cx)+1;
			else if(BluePrint->StyleGroups[i]->PreferredWidth==0 && BluePrint->StyleGroups[i]->PreferredHeight==0)
				v=1;
			else
				v=abs(BluePrint->StyleGroups[i]->PreferredWidth-Cx)+abs(BluePrint->StyleGroups[i]->PreferredHeight-Cy);

			if(v<val) {
				val=v;
				id=i;
			}

		}

		if(Cx==-1)
			Cx=BluePrint->StyleGroups[id]->PreferredWidth;

		if(Cy==-1)
			Cy=BluePrint->StyleGroups[id]->PreferredHeight;

		SetStyles(BluePrint->StyleGroups[id]);
		IWidgetObject::Resize(Cx,Cy);
	}


	Button	&Button::SimulateClicked() {
		Transition(BS_Pressed,true);
		ClickEvent();

		return *this;
	}
	Button	&Button::SimulatePressed() {
		Transition(BS_Pressed);
		PressEvent();

		return *this;
	}

	Button	&Button::SimulateRelease() {
		Transition(prevstate);
		ReleaseEvent();

		return *this;
	}

	Button	&Button::SimulateMouseOver() {
		if(isFocussed() && mstate==BS_Pressed) {
			Transition(BS_Pressed);
		}
		else {
			Transition(BS_Hover);
		}

		return *this;
	}

	Button	&Button::SimulateMouseOut() {
		if(!isFocussed()) {
			Transition(BS_Normal);
		} else if(nextstate=BS_Pressed) {
			Transition(BS_Hover);
		}

		return *this;
	}

	Button	&Button::SetDefault() {
		container->SetDefault(this);
		return *this;
	}

	Button	&Button::SetCancel() {
		container->SetCancel(this);
		return *this;
	}

	Button	&Button::SetNormal() {
		
		return *this;
	}

	void Button::Transition(ButtonStates target,bool temporal,bool progression) {
		bool playsound=false;
		if(currentstate==nextstate && nextstate==target)
			playsound=true;

		if(temporalstate) {
			currentstate=prevstate;
			temporalstate=false;
		}

		ButtonStates animprevstate=prevstate;
		prevstate=currentstate;
		currentstate=nextstate;
		nextstate=target;
		ButtonElement *elm=DetermineElement(currentstate,nextstate);

		//if there is play state sound
		if(elm && playsound)
			if(elm->Sound)
				elm->Sound->CreateWave()->Play();

		if(currentstate!=nextstate) {
			if(elm) {
				currentanimstart=Main.CurrentTime;

				if(elm==DetermineElement(prevstate,currentstate)) {
					int remaining=(int)currentanimend-currentanimstart;
					if(remaining<=0) {
						remaining=0;
						elm->ReadyAnimation(true);
					} else
						elm->Reverse();

					elm->Activate();

					if(elm->Duration==0) {
						elm->setLoop(false);
						currentanimend=currentanimstart+elm->getAnimationDuration()-remaining;
					} else {
						elm->setLoop(true);
						currentanimend=currentanimstart+elm->Duration-remaining;
					}
				} else {
					bool reverse=false;
					if(currentstate==BS_Hover && nextstate==BS_Normal && RevHover2NormalState)
						reverse=true;
					else if(currentstate==BS_Pressed && nextstate==BS_Hover && RevPressed2HoverState)
						reverse=true;
					else if(currentstate==BS_Pressed && nextstate==BS_Normal && RevPressed2NormalState)
						reverse=true;


					elm->ReadyAnimation(reverse);
					elm->Activate();
					currentanimstart=Main.CurrentTime;
					if(elm->Duration==0) {
						elm->setLoop(false);
						currentanimend=Main.CurrentTime+elm->getAnimationDuration();
					} else {
						elm->setLoop(true);
						currentanimend=Main.CurrentTime+elm->Duration;
					}
				}
			} else {
				prevstate=currentstate;
				currentstate=nextstate;
				elm=DetermineElement(currentstate,nextstate);
				elm->setLoop(true);
				elm->Activate();
				if(temporal) {
					currentanimend=Main.CurrentTime+BUTTON_CLICK_DOWNDURATION;
				}

				if(elm)
					if(elm->Sound)
						elm->Sound->CreateWave()->Play();
			}
		}

		temporalstate=temporal;
		Draw();
	}

	void Button::ProgressionCheck() {
		if(Main.CurrentTime>currentanimend) {
			if(currentstate!=nextstate) {
				if(temporalstate) {
					currentanimend=Main.CurrentTime+BUTTON_CLICK_DOWNDURATION;
					temporalstate=false;
				} else {
					Transition(nextstate,false,true);
				}
			} else if(temporalstate) {
				Transition(prevstate,false,true);
				temporalstate=false;
			}
		}
	}
	ButtonElement *Button::DetermineElement(gorgonwidgets::ButtonStates currentstate, gorgonwidgets::ButtonStates nextstate) {
		ButtonElement *ret=NULL;
		ret=et[currentstate][nextstate];

		if(ret==NULL && currentstate==nextstate) {
			ret=et[0][0];
		}

		return ret;
	}
	bool Button::keyb_event(KeyboardEventType event,int keycode,KeyboardModifier::Type modifier) {
		if(!isvisible)
			return false;

		if((modifier==KeyboardModifier::None || modifier==KeyboardModifier::Alternate) && event==KEYB_EVENT_DOWN && keycode==13) {
			SimulateClicked();
			return true;
		}
		if(modifier==KeyboardModifier::None && event==KEYB_EVENT_DOWN && keycode==32) {
			SimulatePressed();

			return true;
		}
		if(modifier==KeyboardModifier::None && event==KEYB_EVENT_UP && keycode==32) {
			SimulateRelease();
			ClickEvent();

			return true;
		}
		return false;
	}
	void Button::on_focus_event(bool state,IWidgetObject *related) {
		if(state && nextstate==BS_Normal) 
			Transition(BS_Hover);
		if(!state && nextstate==BS_Hover)
			Transition(BS_Normal);
	}
	void	Button::SetBluePrint(IWidgetBluePrint *BP) {
		this->BluePrint=(ButtonBP*)BP;
		Resize(this->width,this->height);
	}

	bool Button::mouse_event(MouseEventType event,int x,int y) {
		switch(event) {
		case MOUSE_EVENT_OVER:
			SimulateMouseOver();
			break;
		case MOUSE_EVENT_OUT:
			SimulateMouseOut();
			break;
		case MOUSE_EVENT_LDOWN:
			this->SetFocus();

			mstate=BS_Pressed;
			SimulatePressed();
			break;
		case MOUSE_EVENT_LUP:
			SimulateRelease();
			mstate=BS_Normal;
			break;
		case MOUSE_EVENT_LCLICK:
			ClickEvent();
			
			break;
		}

		return true;
	}
}