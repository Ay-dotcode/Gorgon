#include "Checkbox.h"
#include "../Engine/Wave.h"

namespace gge {
	extern GGEMain *main;
}

namespace gorgonwidgets {
	CheckboxBase::CheckboxBase(CheckboxBP *BluePrint,IWidgetContainer &container,CheckboxTypes type) : 
		IWidgetObject(container),
		BluePrint(BluePrint),
		nextstate(CS_Normal), currentstate(CS_Normal), mstate(CS_Normal),
		temporalstate(false),
		NormalStyle(NULL), HoverStyle(NULL), PressedStyle(NULL),
		DisabledStyle(NULL), ToNextStyle(NULL),
		Normal2Hover(NULL), Normal2Pressed(NULL),
		Hover2Pressed(NULL), Hover2Normal(NULL),
		Pressed2Normal(NULL), Pressed2Hover(NULL),
		cNormalStyle(NULL), cHoverStyle(NULL), cPressedStyle(NULL),
		cDisabledStyle(NULL), cToNextStyle(NULL),
		cNormal2Hover(NULL), cNormal2Pressed(NULL),
		cHover2Pressed(NULL), cHover2Normal(NULL),
		cPressed2Normal(NULL), cPressed2Hover(NULL),
		type(type), checked(false), statetransition(false),
		mover(false), sticky(false),
		ChangeEvent("Change", this),
		textlayer(0, 0, 100, 100)
	{
		this->PointerType=BluePrint->PointerType;


		layer.Add(textlayer,0);
		int i;
		for(i=0;i<4;i++)
			memset(et[i],0,4*sizeof(CheckboxElement*));
		for(i=0;i<4;i++)
			memset(ct[i],0,4*sizeof(CheckboxElement*));

		Resize(0,0);
	}

	CheckboxBase::CheckboxBase(CheckboxBP *BluePrint,CheckboxTypes type) : 
		IWidgetObject(),
		BluePrint(BluePrint),
		nextstate(CS_Normal), currentstate(CS_Normal), mstate(CS_Normal),
		temporalstate(false),
		NormalStyle(NULL), HoverStyle(NULL), PressedStyle(NULL),
		DisabledStyle(NULL), ToNextStyle(NULL),
		Normal2Hover(NULL), Normal2Pressed(NULL),
		Hover2Pressed(NULL), Hover2Normal(NULL),
		Pressed2Normal(NULL), Pressed2Hover(NULL),
		cNormalStyle(NULL), cHoverStyle(NULL), cPressedStyle(NULL),
		cDisabledStyle(NULL), cToNextStyle(NULL),
		cNormal2Hover(NULL), cNormal2Pressed(NULL),
		cHover2Pressed(NULL), cHover2Normal(NULL),
		cPressed2Normal(NULL), cPressed2Hover(NULL),
		type(type), checked(false), statetransition(false),
		mover(false), sticky(false),
		ChangeEvent("Change", this),
		textlayer(0, 0, 100, 100)
	{
		this->PointerType=BluePrint->PointerType;

		layer.Add(textlayer,0);
		int i;
		for(i=0;i<4;i++)
			memset(et[i],0,4*sizeof(CheckboxElement*));
		for(i=0;i<4;i++)
			memset(ct[i],0,4*sizeof(CheckboxElement*));

		Resize(0,0);
	}

	CheckboxStates CheckboxBase::currentState() {
		return currentstate;
	}

	void CheckboxBase::Draw() {
		layer.Clear();
		if(!isvisible)
			return;
		if(!container)
			return;
		if(!container->isVisible())
			return;

		if(!isenabled) {
			if(DisabledStyle)
				DisabledStyle->Draw(layer,textlayer,text);
		} else {
			CheckboxElement *chk=DetermineElement(currentstate,nextstate);
			if(chk)
				chk->Draw(layer,textlayer,text);
		}

		ProgressionCheck();
	}

	void CheckboxBase::Resize(int Cx,int Cy) {
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

			if(this->type!=BluePrint->StyleGroups[i]->CheckboxType)
				v*=100;

			if(v<val) {
				val=v;
				id=i;
			}
		}

		SetStyles(BluePrint->StyleGroups[id]);
		if(Cx==0)
			Cx=BluePrint->StyleGroups[id]->PreferredWidth;
		if(Cy==0)
			Cy=BluePrint->StyleGroups[id]->PreferredHeight;

		IWidgetObject::Resize(Cx,Cy);
	}


	CheckboxBase	&CheckboxBase::SetStyles(CheckboxStyleGroup* grp) {
		if(grp->NormalStyle)
			NormalStyle=new CheckboxElement(*grp->NormalStyle);
		if(grp->HoverStyle)
			HoverStyle=new CheckboxElement(*grp->HoverStyle);
		if(grp->PressedStyle)
			PressedStyle=new CheckboxElement(*grp->PressedStyle);
		if(grp->DisabledStyle)
			DisabledStyle=new CheckboxElement(*grp->DisabledStyle);
		if(grp->Normal2Hover)
			Normal2Hover=new CheckboxElement(*grp->Normal2Hover);
		if(grp->Normal2Pressed)
			Normal2Pressed=new CheckboxElement(*grp->Normal2Pressed);
		if(grp->Hover2Pressed)
			Hover2Pressed=new CheckboxElement(*grp->Hover2Pressed);
		if(grp->Hover2Normal) {
			if(grp->Hover2Normal==grp->Normal2Hover)
				Hover2Normal=Normal2Hover;
			else
				Hover2Normal=new CheckboxElement(*grp->Hover2Normal);
		}
		if(grp->Pressed2Normal) {
			if(grp->Pressed2Normal==grp->Normal2Pressed)
				Pressed2Normal=Normal2Pressed;
			else
				Pressed2Normal=new CheckboxElement(*grp->Pressed2Normal);
		}
		if(grp->Pressed2Hover) {
			if(grp->Pressed2Hover==grp->Hover2Pressed)
				Pressed2Hover=Hover2Pressed;
			else
				Pressed2Hover=new CheckboxElement(*grp->Pressed2Hover);
		}
		if(grp->ToNextStyle)
			ToNextStyle=new CheckboxElement(*grp->ToNextStyle);

		RevHover2NormalState=grp->RevHover2NormalState;
		RevPressed2NormalState=grp->RevPressed2NormalState;
		RevPressed2HoverState=grp->RevPressed2HoverState;

		et[CS_Normal]  [CS_Normal]=NormalStyle;
		et[CS_Normal]  [CS_Hover]=Normal2Hover;
		et[CS_Normal]  [CS_Pressed]=Normal2Pressed;
		et[CS_Normal]  [CS_Disabled]=NULL;

		et[CS_Hover]   [CS_Normal]=Hover2Normal;
		et[CS_Hover]   [CS_Hover]=HoverStyle;
		et[CS_Hover]   [CS_Pressed]=Hover2Pressed;
		et[CS_Hover]   [CS_Disabled]=NULL;

		et[CS_Pressed] [CS_Normal]=Pressed2Normal;
		et[CS_Pressed] [CS_Hover]=Pressed2Hover;
		et[CS_Pressed] [CS_Pressed]=PressedStyle;
		et[CS_Pressed] [CS_Disabled]=NULL;

		et[CS_Disabled][CS_Normal]=NULL;
		et[CS_Disabled][CS_Hover]=NULL;
		et[CS_Disabled][CS_Pressed]=NULL;
		et[CS_Disabled][CS_Disabled]=DisabledStyle;


		if(grp->cNormalStyle)
			cNormalStyle=new CheckboxElement(*grp->cNormalStyle);
		if(grp->cHoverStyle)
			cHoverStyle=new CheckboxElement(*grp->cHoverStyle);
		if(grp->cPressedStyle)
			cPressedStyle=new CheckboxElement(*grp->cPressedStyle);
		if(grp->cDisabledStyle)
			cDisabledStyle=new CheckboxElement(*grp->cDisabledStyle);
		if(grp->cNormal2Hover)
			cNormal2Hover=new CheckboxElement(*grp->cNormal2Hover);
		if(grp->cNormal2Pressed)
			cNormal2Pressed=new CheckboxElement(*grp->cNormal2Pressed);
		if(grp->cHover2Pressed)
			cHover2Pressed=new CheckboxElement(*grp->cHover2Pressed);
		if(grp->cHover2Normal) {
			if(grp->cHover2Normal==grp->cNormal2Hover)
				cHover2Normal=cNormal2Hover;
			else
				cHover2Normal=new CheckboxElement(*grp->cHover2Normal);
		}
		if(grp->cPressed2Normal) {
			if(grp->cPressed2Normal==grp->cNormal2Pressed)
				cPressed2Normal=cNormal2Pressed;
			else
				cPressed2Normal=new CheckboxElement(*grp->cPressed2Normal);
		}
		if(grp->cPressed2Hover) {
			if(grp->cPressed2Hover==grp->cHover2Pressed)
				cPressed2Hover=cHover2Pressed;
			else
				cPressed2Hover=new CheckboxElement(*grp->cPressed2Hover);
		}
		if(grp->cToNextStyle)
			cToNextStyle=new CheckboxElement(*grp->cToNextStyle);

		cRevHover2NormalState=grp->cRevHover2NormalState;
		cRevPressed2NormalState=grp->cRevPressed2NormalState;
		cRevPressed2HoverState=grp->cRevPressed2HoverState;
		cRevToNextStyle=grp->cRevToNextStyle;

		ct[CS_Normal]  [CS_Normal]=cNormalStyle;
		ct[CS_Normal]  [CS_Hover]=cNormal2Hover;
		ct[CS_Normal]  [CS_Pressed]=cNormal2Pressed;
		ct[CS_Normal]  [CS_Disabled]=NULL;

		ct[CS_Hover]   [CS_Normal]=cHover2Normal;
		ct[CS_Hover]   [CS_Hover]=cHoverStyle;
		ct[CS_Hover]   [CS_Pressed]=cHover2Pressed;
		ct[CS_Hover]   [CS_Disabled]=NULL;

		ct[CS_Pressed] [CS_Normal]=cPressed2Normal;
		ct[CS_Pressed] [CS_Hover]=cPressed2Hover;
		ct[CS_Pressed] [CS_Pressed]=cPressedStyle;
		ct[CS_Pressed] [CS_Disabled]=NULL;

		ct[CS_Disabled][CS_Normal]=NULL;
		ct[CS_Disabled][CS_Hover]=NULL;
		ct[CS_Disabled][CS_Pressed]=NULL;
		ct[CS_Disabled][CS_Disabled]=cDisabledStyle;

		return *this;
	}

	CheckboxBase	&CheckboxBase::SimulateClicked() {
		Transition(CS_Pressed,false,false,!checked);
		Toggle(false);
		ClickEvent();

		return *this;
	}
	CheckboxBase	&CheckboxBase::SimulatePressed() {
		Transition(CS_Pressed);

		return *this;
	}

	CheckboxBase	&CheckboxBase::SimulateRelease() {
		Transition(prevstate);

		return *this;
	}

	CheckboxBase	&CheckboxBase::SimulateMouseOver() {
		if(isFocussed() && mstate==CS_Pressed) {
			Transition(CS_Pressed);
		}
		else {
			Transition(CS_Hover);
		}

		return *this;
	}

	CheckboxBase	&CheckboxBase::SimulateMouseOut() {
		if(!isFocussed()) {
			Transition(CS_Normal);
		} else if(nextstate=CS_Pressed) {
			Transition(CS_Hover);
		}

		return *this;
	}


	void CheckboxBase::Transition(CheckboxStates target,bool temporal,bool progression,int targetvalue) {
		if(statetransition && targetvalue==-1)
			return;

		if(targetvalue!=checked && targetvalue!=-1) {
			statetransition=true;
			statetransitiontarget=targetvalue;
			
			currentstate=target;
			nextstate=target;
		}

		bool playsound=false;
		if(currentstate==nextstate && nextstate==target)
			playsound=true;

		if(temporalstate) {
			currentstate=prevstate;
			temporalstate=false;
		}

		prevstate=currentstate;
		currentstate=nextstate;
		nextstate=target;
		CheckboxElement *elm=DetermineElement(currentstate,nextstate);
		if(elm && playsound)
			if(elm->Sound)
				elm->Sound->CreateWave()->Play();

		if(statetransition) {
			if(!elm){
				currentanimstart=main->CurrentTime;
				currentanimend=0;
			}
			else {
				elm->ReadyAnimation(false);
				currentanimstart=main->CurrentTime;
				currentanimend=currentanimstart+elm->Duration;
			}
		}

		if(currentstate!=nextstate) {
			if(elm) {
				currentanimstart=main->CurrentTime;

				if(elm==DetermineElement(prevstate,currentstate)) {
					int remaining=(int)currentanimend-currentanimstart;
					if(remaining<=0) {
						remaining=0;
						elm->ReadyAnimation(true);
					} else
						elm->Reverse();

					currentanimend=currentanimstart+elm->Duration-remaining;
				} else {
					elm->ReadyAnimation(false);
					currentanimend=main->CurrentTime+elm->Duration;
				}
			} else {
				prevstate=currentstate;
				currentstate=nextstate;
				elm=DetermineElement(currentstate,nextstate);
				if(temporal) {
					currentanimend=main->CurrentTime+CHECKBOX_CLICK_DOWNDURATION;
				}

				if(elm)
					if(elm->Sound)
						elm->Sound->CreateWave()->Play();
			}
		}

		temporalstate=temporal;
		Draw();
	}

	void CheckboxBase::ProgressionCheck() {
		if(main->CurrentTime>currentanimend) {
			if(statetransition) {
				statetransition=false;

				//determine current state
				if(this->isFocussed() || this->mover) {
					Transition(CS_Hover);
				} else
					Transition(CS_Normal);
			}

			if(currentstate!=nextstate) {
				if(temporalstate) {
					currentanimend=main->CurrentTime+CHECKBOX_CLICK_DOWNDURATION;
					temporalstate=false;
				} else {
					Transition(nextstate,false,true);
				}
			} else if(temporalstate) {
				Transition(CS_Hover,false,false,!checked);
				Toggle(false);
				temporalstate=false;
			}
		}
	}
	CheckboxElement *CheckboxBase::DetermineElement(gorgonwidgets::CheckboxStates currentstate, gorgonwidgets::CheckboxStates nextstate) {
		CheckboxElement *ret=NULL;

		if(statetransition) {
			if(statetransitiontarget)
				ret=ToNextStyle;
			else
				ret=cToNextStyle;
		} else {
			if(checked)
				ret=ct[currentstate][nextstate];
			else
				ret=et[currentstate][nextstate];
		}

		if(ret==NULL && currentstate==nextstate && !statetransition) {
			if(checked)
				ret=ct[0][0];
			else
				ret=et[0][0];
		}

		return ret;
	}
	void	CheckboxBase::SetBluePrint(IWidgetBluePrint *BP) {
		this->BluePrint=(CheckboxBP*)BP;
		Resize(this->width,this->height);
	}


	bool CheckboxBase::mouse_event(MouseEventType event,int x,int y) {
		if(!isvisible || !container->isVisible())
			return false;

		if(!isenabled)
			return true;

		switch(event) {
		case MOUSE_EVENT_OVER:
			SimulateMouseOver();
			mover=true;
			break;
		case MOUSE_EVENT_OUT:
			SimulateMouseOut();
			mover=false;
			break;
		case MOUSE_EVENT_LDOWN:
			this->SetFocus();

			mstate=CS_Pressed;
			SimulatePressed();
			break;
		case MOUSE_EVENT_LUP:
			SimulateRelease();
			mstate=CS_Normal;
			break;
		case MOUSE_EVENT_LCLICK:
			if(!sticky || !checked) {
				Transition(CS_Pressed,false,false,!checked);
				Toggle(false);
			}
			ClickEvent();
			
			break;
		}

		return true;
	}

	bool CheckboxBase::keyb_event(KeyboardEventType event,int keycode,KeyboardModifier modifier) {
		if(!isvisible)
			return false;

		if((modifier==KEYB_MOD_NONE || modifier==KEYB_MOD_ALTERNATIVE) && event==KEYB_EVENT_DOWN && keycode==13) {
			if(!sticky || !checked) {
				SimulateClicked();
			}
			return true;
		}
		if(modifier==KEYB_MOD_NONE && event==KEYB_EVENT_DOWN && keycode==32) {
			SimulatePressed();

			return true;
		}
		if(modifier==KEYB_MOD_NONE && event==KEYB_EVENT_UP && keycode==32) {
			SimulateRelease();
			if(!sticky || !checked) {
				Transition(CS_Pressed,false,false,!checked);
				Toggle(false);
			} else
				Transition(mover ? CS_Hover : CS_Normal);

			ClickEvent();

			return true;
		}
		return false;
	}
	void CheckboxBase::on_focus_event(bool state,IWidgetObject *related) {
		if(state && nextstate==CS_Normal) 
			Transition(CS_Hover);
		if(!state && nextstate==CS_Hover)
			Transition(CS_Normal);
	}

	CheckboxBase &CheckboxBase::Clear(bool animate) {
		if(checked) {
			Toggle(animate);
		}

		return *this;
	}
	CheckboxBase &CheckboxBase::Check(bool animate) {
		if(!checked) {
			Toggle(animate);
		}

		return *this;
	}
	CheckboxBase &CheckboxBase::SetChecked(bool checked) {
		if(checked)
			Check();
		else
			Clear();

		return *this;
	}
	CheckboxBase &CheckboxBase::Toggle(bool animate) {
		if(animate) {
			SimulateClicked();
		} else {

			checked=!checked;
			ChangeEvent();
		}

		return *this;
	}

	void RadioButton::RadioBtn_Change(empty_event_params p,CheckboxBase &object,Any data,string name) {
		IRadioButton::ChangeEvent();
	}
	RadioButton::RadioButton(CheckboxBP *BluePrint,IWidgetContainer &container) : 
		CheckboxBase(BluePrint,container,CT_RadioButton),
		ChangeEvent("Change", this)
	{
		CheckboxBase::ChangeEvent.Register(this, &RadioButton::RadioBtn_Change);
		IRadioButton::ChangeEvent=RadioButton::ChangeEvent;
		sticky=true;
	}
	RadioButton::RadioButton(CheckboxBP *BluePrint) : 
		CheckboxBase(BluePrint,CT_RadioButton),
		ChangeEvent("Change", this)
	{
		CheckboxBase::ChangeEvent.Register(this, &RadioButton::RadioBtn_Change);
		IRadioButton::ChangeEvent=RadioButton::ChangeEvent;
		sticky=true;
	}
	IRadioButton	&RadioButton::Clear(){
		CheckboxBase::Clear();
		return *(IRadioButton*)this;
	}
	IRadioButton	&RadioButton::Check(){
		CheckboxBase::Check();
		return *(IRadioButton*)this;
	}
	bool			 RadioButton::isChecked() {
		return checked;
	}
}