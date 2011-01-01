#pragma once

#include "../Engine/Layer.h"
#include "../Engine/Pointer.h"
#include "IWidgetContainer.h"
#include <string>
#include "../Utils/Any.h"
#include "../Utils/EventChain.h"

using namespace std;
using namespace gge;

namespace gorgonwidgets {

	struct keyboard_event_params {
		KeyboardEventType event;
		KeyboardModifier modifier;
		int keycode;
		bool &isused;

		keyboard_event_params(KeyboardEventType event, int keycode, KeyboardModifier modifier, bool &isused) :
			event(event), 
			modifier(modifier),
			keycode(keycode), 
			isused(isused)
		{}
	};

	class IWidgetContainer;

	class IWidgetObject {
		friend class IWidgetObject;
		friend class IWidgetContainer;
		friend class IWidgetBluePrint;
		friend bool fn_mouse_event(MouseEventType event,int x,int y,void *data);

	public:
		IWidgetObject();
		IWidgetObject(IWidgetContainer &container);
		virtual void			Enable() { isenabled=true; }
		virtual void			Disable() { isenabled=false; }
		virtual bool			isEnabled() { if(container) return isenabled && container->isEnabled(); else return isenabled; }

		virtual void			Show();
		virtual void			Hide();
		virtual bool			isVisible() { if(container) return isvisible && container->isVisible(); else return isvisible; }

		virtual void			SetText(string text) { this->text=text; }
		virtual string			GetText() { return text; }

		virtual void			Resize	(int Cx,int Cy);
		virtual void			Move	(int X,int Y);

		virtual void			SetFocus() { if(container) container->SetFocus(this); }
		virtual void			ZOrder(int Order=INT_MIN){ if(Order==INT_MIN) layer.OrderToTop(); else layer.setOrder(Order); order=layer.getOrder(); }
		virtual bool			isFocussed() { if(container) return &(container->getFocussed())==this && container->isActive(); else return false; }

		virtual void			SetPointer(Pointer *pointer) { pointer=pointer; }
		virtual void			ResetPointer() { pointer=NULL; }

		virtual IWidgetContainer *GetContainer() { return container; }

		virtual int				X() { return x; }
		virtual int				Y() { return y; }
		virtual int				Width() { return width; }
		virtual int				Height() { return height; }

		virtual int				getOrder() { return order; }
		virtual void			Draw()=0;

		virtual void			SetBluePrint(IWidgetBluePrint *BP)=0;
		virtual void			SetContainer(IWidgetContainer *container);
		virtual void			SetContainer(IWidgetContainer &container);
		virtual void			Detach();
	
		virtual bool keyb_event(KeyboardEventType event,int keycode,KeyboardModifier modifier)=0;
		virtual bool mouse_event(MouseEventType event,int x,int y)=0;

		EventChain<IWidgetObject, empty_event_params> ClickEvent;
		EventChain<IWidgetObject, empty_event_params> GotFocusEvent;
		EventChain<IWidgetObject, empty_event_params> LostFocusEvent;
		EventChain<IWidgetObject, empty_event_params> RelocateEvent;

		void *Data;

		WidgetLayer &getLayer() { return layer; }
		IWidgetContainer &getContainer() { return *container; }
		Pointer::PointerTypes PointerType;

		virtual void container_hide();

	protected:
		int keyboardeventid;
		Pointer *pointer;
		MouseEventToken mouseevent;
		bool isenabled,isvisible;
		int x,y,width,height;
		string text;
		WidgetLayer layer;
		int order;
		int pointerid;
		bool nofocus;

		virtual void on_focus_event(bool state,IWidgetObject *related)=0;
		void focus_event(bool state,IWidgetObject *related) { if(state) GotFocusEvent(); else LostFocusEvent(); on_focus_event(state,related); }

		IWidgetContainer *container;
	};
}
