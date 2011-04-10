#include "IWidgetContainer.h"
#include "IWidgetObject.h"
#include "WidgetMain.h"
#include "../Engine/GGEMain.h"

namespace gge { namespace widgets {

	IWidgetContainer::IWidgetContainer(LayerBase &Parent, int X, int Y, int W, int H, int Order) :
		BackgroundRedrawEvent("BackgroudRedraw", this),Order(Order),
		Focussed(NULL),
		Default(NULL),
		Cancel(NULL),
		isactive(false), allowdefaultactions(true), allowtabswitch(true),
		isenabled(true), 
		BaseLayer(X,Y, W,H), 
		ExtenderLayer(0,0, Main.W,Main.H), ObjectLayer(0,0, W,H), BackgroundLayer(0,0, W,H)
	{
		BaseLayer.Add(ExtenderLayer, -1);
		BaseLayer.Add(ObjectLayer, 0);
		BaseLayer.Add(BackgroundLayer, 1);

		Parent.Add(BaseLayer);
	}


	IWidgetContainer::IWidgetContainer(int X, int Y, int W, int H, int Order) :
		BackgroundRedrawEvent("BackgroudRedraw", this),Order(Order),
		Focussed(NULL),
		Default(NULL),
		Cancel(NULL),
		isactive(false), allowdefaultactions(true), allowtabswitch(true),
		isenabled(true), 
		BaseLayer(X,Y, W,H), 
		ExtenderLayer(0,0, W,H), ObjectLayer(0,0, W,H), BackgroundLayer(0,0, W,H)
	{
		BaseLayer.Add(ExtenderLayer, -1);
		BaseLayer.Add(ObjectLayer, 0);
		BaseLayer.Add(BackgroundLayer, 1);
	}

	IWidgetContainer &IWidgetContainer::operator += (IWidgetObject &Widget) { AddWidget(Widget); return *this; }
	IWidgetContainer &IWidgetContainer::operator -= (IWidgetObject &Widget) { RemoveWidget(Widget); return *this; }

	void IWidgetContainer::Move(int x,int y){
		BaseLayer.X=x;
		BaseLayer.Y=y;
	}

	void IWidgetContainer::Hide(){
		BaseLayer.isVisible=false;

		this->Deactivate();
	
		utils::LinkedListIterator<IWidgetObject>it= Subobjects;
		IWidgetObject* widget;
		while(widget=it)
			widget->container_hide();
	}

	void IWidgetContainer::Resize(int Width,int Height) {
		BaseLayer.W=Width;
		BaseLayer.H=Height;

		ObjectLayer.W=Width;
		ObjectLayer.H=Height;

		BackgroundLayer.W=Width;
		BackgroundLayer.H=Height;
	}

	IWidgetObject *IWidgetContainer::AddWidget(IWidgetObject *Widget) {
		Widget->Detach();
		utils::LinkedListItem<IWidgetObject> *item=Subobjects.AddItem(Widget,Subobjects.HighestOrder()+1);
		Widget->container=this;

		ObjectLayer.Add(Widget->getLayer(),-item->getOrder());

		return Widget;
	}

	IWidgetObject *IWidgetContainer::RemoveWidget(IWidgetObject *Widget) { 
		Widget->container=NULL;
		ObjectLayer.SubLayers.Remove(Widget->getLayer()); 
		Subobjects.Remove(Widget); 

		return Widget;
	}

	IWidgetObject &IWidgetContainer::AddWidget(IWidgetObject &Widget) {
		Widget.Detach();
		utils::LinkedListItem<IWidgetObject> *item=Subobjects.AddItem(&Widget,Subobjects.HighestOrder()+1);
		Widget.container=this;

		ObjectLayer.Add(Widget.getLayer(),-item->getOrder());

		return Widget;
	}

	IWidgetObject &IWidgetContainer::RemoveWidget(IWidgetObject &Widget) { 
		Widget.container=NULL;
		ObjectLayer.SubLayers.Remove(Widget.getLayer()); 
		Subobjects.Remove(&Widget); 

		return Widget;
	}

	IWidgetObject *IWidgetContainer::AddDialog(IWidgetObject *Widget) {
		Widget->Detach();
		Widget->container=this;
		utils::LinkedListItem<IWidgetObject> *item=Subobjects.AddItem(Widget,Subobjects.HighestOrder()+1);

		DialogLayer->Add(Widget->getLayer(),-item->getOrder());

		return Widget;
	}

	IWidgetObject *IWidgetContainer::RemoveDialog(IWidgetObject *Widget) { 
		Widget->container=NULL;
		DialogLayer->SubLayers.Remove(Widget->getLayer()); 
		Subobjects.Remove(Widget); 

		return Widget;
	}

	IWidgetObject &IWidgetContainer::AddDialog(IWidgetObject &Widget) {
		return *AddDialog(&Widget);
	}

	IWidgetObject &IWidgetContainer::RemoveDialog(IWidgetObject &Widget) {
		Widget.container=NULL;
		DialogLayer->SubLayers.Remove(Widget.getLayer()); 
		Subobjects.Remove(&Widget); 

		return Widget;
	}

	void IWidgetContainer::SetFocus(IWidgetObject &Object) {
		SetFocus(&Object);
	}
	void IWidgetContainer::SetFocus(IWidgetObject *Object) {
		if(Object!=NULL && !isactive)
			this->Activate();

		IWidgetObject *obj=this->Focussed;
		this->Focussed=Object;

		if(obj)
			obj->focus_event(false,Object);
		if(Object)
			Object->focus_event(true,obj);
	}

	void IWidgetContainer::SetDefault(IWidgetObject *Object) {
		this->Default=Object;


	}

	void IWidgetContainer::SetCancel(IWidgetObject *Object) {
		this->Cancel=Object;
	}


	void IWidgetContainer::Activate() {
		isactive=true;
	}

	void IWidgetContainer::Deactivate() {
		if(Focussed)
			this->SetFocus(NULL);

		isactive=false;
	}

	void IWidgetContainer::FocusNext() {
		IWidgetObject *object=NULL;

		utils::LinkedListItem<IWidgetObject>* item=Subobjects.FindListItem(Focussed);

		if(item) {
			while(item->getOrderedNext()) {
				item=item->getOrderedNext();
				if(!item->Item->nofocus) {
					object=item->Item;
					break;
				}
			}

			if(!object) {
				foreach(IWidgetObject, item, Subobjects)
					if(!item->nofocus) {
						object=item;
						break;
					}
			}
		}		
		else
			object=Subobjects.getOrderedFirstItem();

		if(object)
			object->SetFocus();
		
	}
	void IWidgetContainer::FocusPrevious() {
		IWidgetObject *object=NULL;

		utils::LinkedListItem<IWidgetObject>* item=Subobjects.FindListItem(Focussed);

		if(item) {
			while(item->getOrderedPrevious()) {
				item=item->getOrderedPrevious();
				if(!item->Item->nofocus) {
					object=item->Item;
					break;
				}
			}

			if(!object) {
				utils::LinkedListOrderedIterator<IWidgetObject> it=Subobjects.GetReverseOrderedIterator();
				
				while(IWidgetObject *item=it)
					if(!item->nofocus) {
						object=item;
						break;
					}
			}
		}		
		else
			object=Subobjects.getOrderedLastItem();

		if(object)
			object->SetFocus();
	}

	bool IWidgetContainer::keyboard_event_subitems(input::KeyboardEvent::Type event,int keycode) {
		bool ret=false;
		if(this->Focussed) {
			if(this->Focussed->isEnabled() && this->Focussed->isVisible())
				ret=this->Focussed->keyboard(event,keycode);
		}

		return ret;
	}

	bool IWidgetContainer::keyboard_event_actions(input::KeyboardEvent::Type event,int keycode) {
		if(allowtabswitch) {
			//do default button handling
			if(event==input::KeyboardEvent::Char && keycode==9) {
				if(!input::KeyboardModifier::IsModified()) {
					FocusNext();
					return true;
				}
				else if(input::KeyboardModifier::Current==input::KeyboardModifier::Shift) {
					FocusPrevious();
					return true;
				}
			}
		}
	
		if(allowdefaultactions) {
			if(event==input::KeyboardEvent::Down && keycode==13 && !input::KeyboardModifier::Check()) {
				if(Default)
					return Default->keyboard(event,keycode);
			}

			if(event==input::KeyboardEvent::Down && keycode==27 && !input::KeyboardModifier::Check()) {
				if(Cancel)
					return Cancel->keyboard(event,13);

				this->Deactivate();
			}
		}

		return false;
	}

	bool IWidgetContainer::keyboard_event(input::KeyboardEvent::Type event,int keycode) {

		if(keyboard_event_subitems(event,keycode))
			return true;

		return keyboard_event_actions(event,keycode);
	}

} }
