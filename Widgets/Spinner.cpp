#include "Spinner.h"

namespace gorgonwidgets {
	void Spinner::attached_changed(text_change_event_params p, Textbox &textbox, Any data, string name) {
		if(textbox.GetText().length()>0) {
			const char *t=textbox.GetText().data();
			for(;*t!=0;t++)
				if(*t!='.' && (*t<'0' || *t>'9')) {
					setValue(atof(textbox.GetText().data()));
					return;
				}
			changeValue(atof(textbox.GetText().data()));
		}
	}

	void Spinner::attached_lostfocus(empty_event_params p, IWidgetObject &textbox, Any data, string name) {
		setValue(atof(textbox.GetText().data()));
	}

	void Spinner::attached_keyboard(keyboard_event_params p, Textbox &textbox, Any data, string name) {
		if(p.event==KEYB_EVENT_DOWN && p.keycode==38) {
			goUp();
			p.isused=true;
			cancelclick=false;
		}
		if(p.event==KEYB_EVENT_UP && p.keycode==38) {
			stopUp();
			if(!cancelclick)
				Up();
			p.isused=true;
		}
		if(p.event==KEYB_EVENT_DOWN && p.keycode==40) {
			goDown();
			p.isused=true;
			cancelclick=false;
		}
		if(p.event==KEYB_EVENT_UP && p.keycode==40) {
			stopDown();
			if(!cancelclick)
				Down();
			p.isused=true;
		}
	}

	Spinner::Spinner(gorgonwidgets::SliderBP *BP, gorgonwidgets::IWidgetContainer &container) : 
		SliderBase(BP, container, SS_Verticle),
		eventtoken(0), lostfocuseventtoken(0), keyboardeventtoken(0),
		attachedto(NULL),
		ChangeEvent("Change", this)
	{
		init();
	}

	Spinner::Spinner(gorgonwidgets::SliderBP *BP) : 
		SliderBase(BP, SS_Verticle),
		eventtoken(0), lostfocuseventtoken(0), keyboardeventtoken(0),
		attachedto(NULL),
		ChangeEvent("Change", this)
	{
		init();
	}

	void Spinner::init() {
		Setup(STS_None,false,false,true,false,SRA_None,false,false);

		minimum=0;
		maximum=100;
		value=0;
		steps=1;
		buttonincrement=1;

		SliderBase::ChangeEvent.DoubleLink(ChangeEvent);

		this->Hide();
		delayclicks=true;
	}

	void Spinner::AttachTo(gorgonwidgets::Textbox *textbox) {
		DetachFrom();

		attachedto=textbox;
		
		eventtoken=attachedto->ChangeEvent.Register(this, &Spinner::attached_changed);
		lostfocuseventtoken=attachedto->LostFocusEvent.Register(this, &Spinner::attached_lostfocus);
		keyboardeventtoken=attachedto->KeyboardEvent.Register(this, &Spinner::attached_keyboard);

		setValue(atof(attachedto->GetText().data()));

		Show();
		
		Reposition(); 
	}

	void Spinner::Reposition() {
		if(attachedto) {
			this->Resize(max(btnDown->Width(), btnUp->Width())+StyleGroup->ArrowOffset.x, max(attachedto->Height(), btnDown->Height()+btnUp->Height()+StyleGroup->ArrowOffset.y*2));
			Move(attachedto->X()+attachedto->Width(), attachedto->Y()+(attachedto->Height()-height)/2);
		}
	}
			
	void Spinner::DetachFrom() { 
		if(attachedto && eventtoken) { 
			attachedto->ChangeEvent.Unregister(eventtoken); 
			attachedto->LostFocusEvent.Unregister(lostfocuseventtoken); 
			attachedto=NULL; 
			eventtoken=0; 
			lostfocuseventtoken=0;
			keyboardeventtoken=0;
		} 
	}


	void Spinner::setValue(float value) {
		SliderBase::setValue(value);
		if(attachedto) { 
			char tmp[30]; 
			sprintf(tmp, numberformat.data(), this->value); 
			attachedto->SetText(tmp); 
		}
	}
}