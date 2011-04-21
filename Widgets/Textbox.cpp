#include "Textbox.h"
#include "../Engine/Wave.h"
#include "../Resource/SoundResource.h"
#include "../Engine/GGEMain.h"

using namespace gge::resource;

namespace gge { namespace widgets {
	Textbox::Textbox(TextboxBP *BluePrint,IWidgetContainer &container) : 
		IWidgetObject(container),
		NormalStyle(NULL), HoverStyle(NULL),
		ActiveStyle(NULL), DisabledStyle(NULL),
		Normal2Hover(NULL), Normal2Active(NULL),
		Hover2Active(NULL), Hover2Normal(NULL),
		Active2Normal(NULL), Active2Hover(NULL),
		BluePrint(BluePrint),
		nextstate(TS_Normal), currentstate(TS_Normal),
		caretposition(0), selectstart(0), selectend(0),
		isactive(false),
		scroll(0),
		AlwaysShowSelection(true),
		AutoSelectAll(false),
		mousedown(false),
		ChangeEvent("Change",this),
		KeyboardEvent("Keyboard",this),
		textlayer(0,0,width,height)
	{
		this->PointerType=BluePrint->PointerType;
		Caret=new ImageAnimation(BluePrint->Caret);

		textlayer.EnableClipping=true;
		layer.Add(textlayer, 0);
		layer.EnableClipping=true;

		int i;
		for(i=0;i<4;i++)
			memset(et[i],0,4*sizeof(TextboxElement*));

	}

	Textbox::Textbox(TextboxBP *BluePrint) : 
		IWidgetObject(),
		NormalStyle(NULL), HoverStyle(NULL),
		ActiveStyle(NULL), DisabledStyle(NULL),
		Normal2Hover(NULL), Normal2Active(NULL),
		Hover2Active(NULL), Hover2Normal(NULL),
		Active2Normal(NULL), Active2Hover(NULL),
		BluePrint(BluePrint),
		nextstate(TS_Normal), currentstate(TS_Normal),
		caretposition(0), selectstart(0), selectend(0),
		isactive(false),
		scroll(0),
		AlwaysShowSelection(true),
		AutoSelectAll(false),
		mousedown(false),
		ChangeEvent("Change",this),
		KeyboardEvent("Keyboard",this),
		textlayer(0,0,width,height)
	{
		this->PointerType=BluePrint->PointerType;
		Caret=new ImageAnimation(BluePrint->Caret);

		textlayer.EnableClipping=true;
		layer.Add(textlayer, 0);
		layer.EnableClipping=true;

		int i;
		for(i=0;i<4;i++)
			memset(et[i],0,4*sizeof(TextboxElement*));

	}

	void	Textbox::SetBluePrint(IWidgetBluePrint *BP) {
		this->BluePrint=(TextboxBP*)BP;
		Resize(this->width,this->height);
	}


	TextboxStates Textbox::currentState() {
		return currentstate;
	}

	void Textbox::SetText(string text) {
		IWidgetObject::SetText(text);
		if(caretposition>text.length())
			caretposition=text.length();
	}


	Textbox &Textbox::SetCaretPosition(int Position) {
		caretposition=Position;

		if(caretposition>(int)text.length())
			caretposition=text.length();

		if(caretposition<0)
			caretposition=0;

		selectstart=selectend=caretposition;

		//Draw();

		return *this;
	}

	Textbox &Textbox::SetSelection(int start,int end) {

		if(start<0)
			start=0;
		if(start>(int)text.length())
			start=(int)text.length();
		if(end<0)
			end=0;
		if(end>(int)text.length())
			end=(int)text.length();

		if(start>end) {
			int t=start;
			start=end;
			end=t;
		}

		selectstart=start;
		selectend=end;

		caretposition=end;

		return *this;
	}

	Textbox &Textbox::SelectAll() {
		selectstart=0;
		selectend=(int)text.length();
		
		caretposition=selectend;

		return *this;
	}
	
	void Textbox::Draw() {
		layer.Clear();
		if(!isvisible || !container->isVisible())
			return;

		ProgressionCheck();
		if(!isEnabled()) {
			if(DisabledStyle)
				DisabledStyle->Draw(layer,textlayer,text);
		} else {
			TextboxElement *txt=DetermineElement(currentstate,nextstate);
			if(txt)
				txt->Draw(layer,textlayer,text);
		}
	}

	Textbox			&Textbox::SetStyles(TextboxStyleGroup* grp) {
		if(grp->NormalStyle) {
			NormalStyle=new TextboxElement(*grp->NormalStyle);
			NormalStyle->Parent=this;
		}
		if(grp->HoverStyle) {
			HoverStyle=new TextboxElement(*grp->HoverStyle);
			HoverStyle->Parent=this;
		}
		if(grp->ActiveStyle) {
			ActiveStyle=new TextboxElement(*grp->ActiveStyle);
			ActiveStyle->Parent=this;
		}
		if(grp->DisabledStyle) {
			DisabledStyle=new TextboxElement(*grp->DisabledStyle);
			DisabledStyle->Parent=this;
		}
		if(grp->Normal2Hover) {
			Normal2Hover=new TextboxElement(*grp->Normal2Hover);
			Normal2Hover->Parent=this;
		}
		if(grp->Normal2Active) {
			Normal2Active=new TextboxElement(*grp->Normal2Active);
			Normal2Active->Parent=this;
		}
		if(grp->Hover2Active) {
			Hover2Active=new TextboxElement(*grp->Hover2Active);
			Hover2Active->Parent=this;
		}
		if(grp->Hover2Normal) {
			if(grp->Hover2Normal==grp->Normal2Hover)
				Hover2Normal=Normal2Hover;
			else
				Hover2Normal=new TextboxElement(*grp->Hover2Normal);

			Hover2Normal->Parent=this;
		}
		if(grp->Active2Normal) {
			if(grp->Active2Normal==grp->Normal2Active)
				Active2Normal=Normal2Active;
			else
				Active2Normal=new TextboxElement(*grp->Active2Normal);

			Active2Normal->Parent=this;
		}
		if(grp->Active2Hover) {
			if(grp->Active2Hover==grp->Hover2Active)
				Active2Hover=Hover2Active;
			else
				Active2Hover=new TextboxElement(*grp->Active2Hover);

			Active2Hover->Parent=this;
		}

		RevHover2NormalState=grp->RevHover2NormalState;
		RevActive2NormalState=grp->RevActive2NormalState;
		RevActive2HoverState=grp->RevActive2HoverState;

		et[TS_Normal]  [TS_Normal]=NormalStyle;
		et[TS_Normal]  [TS_Hover]=Normal2Hover;
		et[TS_Normal]  [TS_Active]=Normal2Active;
		et[TS_Normal]  [TS_Disabled]=NULL;

		et[TS_Hover]   [TS_Normal]=Hover2Normal;
		et[TS_Hover]   [TS_Hover]=HoverStyle;
		et[TS_Hover]   [TS_Active]=Hover2Active;
		et[TS_Hover]   [TS_Disabled]=NULL;

		et[TS_Active]  [TS_Normal]=Active2Normal;
		et[TS_Active]  [TS_Hover]=Active2Hover;
		et[TS_Active]  [TS_Active]=ActiveStyle;
		et[TS_Active]  [TS_Disabled]=NULL;

		et[TS_Disabled][TS_Normal]=NULL;
		et[TS_Disabled][TS_Hover]=NULL;
		et[TS_Disabled][TS_Active]=NULL;
		et[TS_Disabled][TS_Disabled]=DisabledStyle;

		return *this;
	}

	void Textbox::Resize(int Cx,int Cy) {
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

		SetStyles(BluePrint->StyleGroups[id]);
		IWidgetObject::Resize(Cx,BluePrint->StyleGroups[id]->PreferredHeight);
	}


	Textbox	&Textbox::SimulateActivate() {
		Transition(TS_Active);
		
		return *this;
	}

	Textbox	&Textbox::SimulateMouseOver() {
		Transition(TS_Hover);
		
		return *this;
	}

	Textbox	&Textbox::SimulateMouseOut() {
		if(isactive)
			Transition(TS_Active);
		else
			Transition(TS_Normal);
		
		return *this;
	}

	void Textbox::Transition(TextboxStates target,bool progression) {
		bool playsound=false;
		if(isFocussed()) {
			if(target!=TS_Active && target!=TS_Disabled)
				return;
		}
		if(currentstate==nextstate && nextstate==target)
			playsound=true;

		prevstate=currentstate;
		currentstate=nextstate;
		nextstate=target;
		TextboxElement *elm=DetermineElement(currentstate,nextstate);
		if(elm && playsound)
			if(elm->Sound)
				elm->Sound->CreateWave()->Play();

		if(currentstate!=nextstate) {
			if(elm) {
				currentanimstart=Main.CurrentTime;

				if(elm==DetermineElement(prevstate,currentstate)) {
					unsigned int remaining=currentanimend-currentanimstart;

					elm->Reverse();
					currentanimend=currentanimstart+elm->Duration-remaining;
				} else {
					currentanimend=Main.CurrentTime+elm->Duration;
				}
			} else {
				currentstate=nextstate;
				elm=DetermineElement(currentstate,nextstate);

				if(elm)
					if(elm->Sound)
						elm->Sound->CreateWave()->Play();
			}
		}

		Draw();
	}

	void Textbox::ProgressionCheck() {
		if(Main.CurrentTime>currentanimend) {
			if(currentstate!=nextstate) {
				Transition(nextstate,true);
			}
		}
	}
	TextboxElement *Textbox::DetermineElement(gge::widgets::TextboxStates currentstate, gge::widgets::TextboxStates nextstate) {
		TextboxElement *ret=NULL;
		ret=et[currentstate][nextstate];
		if(ret==NULL && currentstate==nextstate) {
			ret=et[0][0];
		}

		return ret;
	}
	bool Textbox::mouse(input::MouseEventType event,int x,int y) {
		static int firstselect;
		if(!isvisible || !container->isVisible())
			return false;

		if(!isenabled)
			return true;

		TextboxElement *elm=DetermineElement(currentstate,nextstate);

		switch(event) {
		case input::MOUSE_EVENT_OVER:
			SimulateMouseOver();
			break;
		case input::MOUSE_EVENT_OUT:
			SimulateMouseOut();
			break;
		case input::MOUSE_EVENT_LDOWN:
			firstselect=selectend=selectstart=caretposition=elm->detectChar(x);
			mousedown=true;
			break;
		case input::MOUSE_EVENT_LUP:
			selectend=caretposition=elm->detectChar(x);
			if(selectend<firstselect) {
				selectstart=selectend;
				selectend=firstselect;
			}
			mousedown=false;
			this->SetFocus();
			break;
		case input::MOUSE_EVENT_MOVE:
			if(mousedown) {
				selectend=caretposition=elm->detectChar(x);

				if(selectend<firstselect) {
					selectstart=selectend;
					selectend=firstselect;
				}
			}

			break;
		case input::MOUSE_EVENT_LCLICK:
			ClickEvent();

			break;
		}

		return true;
	}

	bool Textbox::keyboard(input::KeyboardEvent::Type event,int keycode) {
		if(!isvisible)
			return false;

		bool isused=false;
		KeyboardEvent(keyboard_event_params(event, keycode, input::KeyboardModifier::Current, isused));

		if(isused)
			return true;

		if(event==input::KeyboardEvent::Char) {
			switch(keycode) {
			case 8: //backspace
				if(selectstart!=selectend) {
					string txt=this->GetText();
					text_change_event_params pars;
					pars.prev=txt;
					txt.erase(selectstart,selectend-selectstart);
					this->SetText(txt);
					pars.current=txt;
					caretposition=selectend=selectstart;

					ChangeEvent(pars);
				}
				else if(caretposition) {
					string txt=this->GetText();
					text_change_event_params pars;
					pars.prev=txt;
					caretposition--;
					txt.erase(caretposition,1);
					this->SetText(txt);
					pars.current=txt;

					ChangeEvent(pars);
				}
				return true;
			default:
				if(keycode>=32) {
					text_change_event_params pars;
					pars.prev=this->GetText();

					if(selectstart!=selectend) {
						string txt=pars.prev;
						txt.erase(selectstart,selectend-selectstart);
						this->SetText(txt);
						pars.current=txt;
						caretposition=selectend=selectstart;
					}

					char t[2];
					sprintf_s<2>(t,"%c",keycode);
					this->SetText(this->GetText().insert(caretposition,t));
					SetCaretPosition(caretposition+1);
					pars.current=this->GetText();

					ChangeEvent(pars);
					return true;
				}
			}
		}
		if(event==input::KeyboardEvent::Down) {
			if( !input::KeyboardModifier::Check() ) {
				switch(keycode) {
				case 35: //End
					SetCaretPosition(text.length());
					return true;
				case 36: //Home
					SetCaretPosition(0);
					return true;
				case 37: //left
					SetCaretPosition(--caretposition);
					return true;
				case 39: //right
					SetCaretPosition(++caretposition);
					return true;
				case 46:  //del
					if(selectstart!=selectend) {
						string txt=this->GetText();
						text_change_event_params pars;
						pars.prev=txt;
						txt.erase(selectstart,selectend-selectstart);
						this->SetText(txt);
						pars.current=txt;
						caretposition=selectend=selectstart;

						ChangeEvent(pars);
					}
					else {
						string txt=this->GetText();
						text_change_event_params pars;
						pars.prev=txt;
						txt.erase(caretposition,1);
						this->SetText(txt);
						pars.current=this->GetText();

						ChangeEvent(pars);
					}
					return true;
				}
			}
		} else {
			if(input::KeyboardModifier::Current==input::KeyboardModifier::Shift) {
				switch(keycode) {
				case 35: //End
					if(caretposition==selectend) {
						caretposition=(int)text.length();

						selectend=caretposition;
					} else {
						caretposition=(int)text.length();

						selectstart=selectend;
						selectend=caretposition;
					}
					return true;
				case 36: //Home
					if(caretposition==selectstart) {
						caretposition=0;

						selectstart=caretposition;
					} else {
						caretposition=0;

						selectend=selectstart;
						selectstart=caretposition;
					}
					return true;
				case 37: //left
					if(caretposition==selectstart) {
						if(caretposition)
							caretposition--;

						selectstart=caretposition;
					} else {
						if(caretposition)
							caretposition--;

						selectend=caretposition;
					}
					return true;
				case 39: //right
					if(caretposition==selectend) {
						if(caretposition<(int)text.length())
							caretposition++;

						selectend=caretposition;
					} else {
						if(caretposition<(int)text.length())
							caretposition++;

						selectstart=caretposition;
					}
					return true;
				}
			}
		}

		return false;
	}

	void Textbox::on_focus_event(bool state,IWidgetObject *related) {
		if(AutoSelectAll && !isactive && state)
			SelectAll();

		isactive=state;
		if(isactive)
			Transition(TS_Active);
		else
			if(currentstate==TS_Active)
				Transition(TS_Normal);
		Draw();
	}
} }
