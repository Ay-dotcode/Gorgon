#include "IWidgetObject.h"
#include "../Engine/GGEMain.h"

namespace gge { namespace widgets {

	bool fn_mouse_event(input::MouseEventType event,int x,int y,void *data) {
		IWidgetObject* iwo=(IWidgetObject*)data;
		if(iwo->isVisible()) {
			if(!iwo->isEnabled())
				return true;

			if(event==input::MOUSE_EVENT_OVER && iwo->pointerid)
				if(iwo->pointer)
					iwo->pointerid=Pointers.Set(iwo->pointer);
				else
					iwo->pointerid=Pointers.Set(iwo->PointerType);
			else if(event==input::MOUSE_EVENT_OUT && iwo->pointerid>0) {
				Pointers.Reset(iwo->pointerid);
				iwo->pointerid=0;
			}

			return iwo->mouse_event(event,x,y);
		}

		return false;
	}


	IWidgetObject::IWidgetObject() :
		text(""),
		x(0), y(0), width(0), height(0),
		isenabled(true), isvisible(true),
		order(0), nofocus(false),
		PointerType(Pointer::None), pointerid(0), pointer(NULL),
		ClickEvent("Click", this),
		GotFocusEvent("GotFocus", this),
		LostFocusEvent("LostFocus", this),
		RelocateEvent("Relocate", this),
		container(NULL), layer(x, y, width, height)
	{
		mouseevent=layer.RegisterMouseEvent(0 ,0, INT_MAX, INT_MAX, (void*)this, fn_mouse_event, fn_mouse_event, fn_mouse_event, fn_mouse_event, fn_mouse_event, fn_mouse_event, fn_mouse_event);
	}

	IWidgetObject::IWidgetObject(IWidgetContainer &container) :
		text(""),
		x(0), y(0), width(0), height(0),
		isenabled(true), isvisible(true),
		order(0), nofocus(false),
		PointerType(Pointer::None), pointerid(0), pointer(NULL),
		ClickEvent("Click", this),
		GotFocusEvent("GotFocus", this),
		LostFocusEvent("LostFocus", this),
		RelocateEvent("Relocate", this),
		container(&container), layer(x, y, width, height)
	{

		mouseevent=layer.RegisterMouseEvent(0 ,0, INT_MAX, INT_MAX, (void*)this, fn_mouse_event, fn_mouse_event, fn_mouse_event, fn_mouse_event, fn_mouse_event, fn_mouse_event, fn_mouse_event);

		container.AddWidget(this);
	}

	void IWidgetObject::Show() {
		isvisible=true;
		layer.isVisible=true;
		//this->Draw();


	}
	void IWidgetObject::Hide() {
		isvisible=false;
		layer.isVisible=false;

		if(pointerid) {
			Pointers.Reset(pointerid);
			pointerid=0;
		}

	}

	void IWidgetObject::Resize	(int Cx,int Cy) {
		if(Cx>0)
			width=Cx;
		if(Cy>0)
			height=Cy;

		layer.W=width;
		layer.H=height;

		RelocateEvent();
	}

	void IWidgetObject::Move	(int X,int Y) {
		x=X;
		y=Y;

		layer.X=X;
		layer.Y=Y;

		RelocateEvent();
	}

	void IWidgetObject::SetContainer(IWidgetContainer *container) {
		if(this->container) {
			this->container->RemoveWidget(this);
		}
		this->container=container;
		container->AddWidget(this);
	}

	void IWidgetObject::SetContainer(IWidgetContainer &container) {
		if(this->container) {
			this->container->RemoveWidget(this);
		}
		this->container=&container;
		container.AddWidget(this);
	}
	void IWidgetObject::Detach() {
		if(this->container) {
			container->RemoveWidget(this);
			container=NULL;
		}
	}
	

	void IWidgetObject::container_hide() {
		if(pointerid) {
			Pointers.Reset(pointerid);
			pointerid=0;
		}
	}


} }
