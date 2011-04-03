#pragma once

#include "IWidgetObject.h"
#include "IWidgetContainer.h"
#include "IScroller.h"
#include "Rectangle.h"
#include "FrameBP.h"


namespace gge { namespace widgets {
	enum ScrollbarDisplayStates {
		SDS_Never,
		SDS_WhenNeeded,
		SDS_Always
	};

	class Frame : public IWidgetObject, public IWidgetContainer {
		friend bool frame_scroll(int amount, input::MouseEventType event, int x, int y, void *data);
	public:
		Frame(FrameBP &bp, IWidgetContainer &Container, int X=0, int Y=0, int W=100, int H=100, bool Dialog=false);
		Frame(FrameBP &bp, int X=0, int Y=0, int W=100, int H=100, bool Dialog=false);


		virtual void		Activate() { 
			isactive=true;
			IWidgetObject::SetFocus(); 
		}

		virtual void		SetFocus() { 
			isactive=true;
			IWidgetObject::SetFocus(); 
		}
		virtual void		Deactivate() {
			isactive=false;
		}

		
		using IWidgetObject::Enable;
		using IWidgetObject::Disable;
		using IWidgetObject::isEnabled;
		using IWidgetObject::X;
		using IWidgetObject::Y;
		using IWidgetObject::Width;
		using IWidgetObject::Height;

		WidgetLayer &getBaseLayer() { return BaseLayer; }

		virtual int			getUsableWidth() { return usable.Width; }
		virtual int			getUsableHeight() { return usable.Height; }

		using IWidgetObject::Show;
		using IWidgetObject::Hide;
		using IWidgetObject::isVisible;

		using IWidgetObject::Move;
		virtual void Resize(int Width,int Height) { 
			IWidgetObject::Resize(Width,Height);
			IWidgetContainer::Resize(Width,Height);

			adjustlocations(); 
		}
		using IWidgetObject::ZOrder;
		virtual void		Draw();
		virtual IWidgetObject *AddWidget(IWidgetObject *Widget) {
			IWidgetContainer::AddWidget(Widget);

			Widget->RelocateEvent.Register(this, &Frame::subobject_relocate);
			adjustlocations();

			return Widget; 
		}
		virtual IWidgetObject &AddWidget(IWidgetObject &Widget) { 
			IWidgetContainer::AddWidget(Widget);

			Widget.RelocateEvent.Register(this, &Frame::subobject_relocate);
			adjustlocations(); 

			return Widget; 
		}

		virtual void SetBluePrint(IWidgetBluePrint *BP);
		virtual void on_focus_event(bool state,IWidgetObject *related) { isactive=state; };
		
		virtual bool keyb_event(input::KeyboardEventType event,int keycode,input::KeyboardModifier::Type modifier) {
			bool ret=false;
			keyboard_event_params p(event, keycode, modifier,ret);
			KeypreviewEvent(p);
			if(ret) return true;

			ret=keyboard_event_subitems(event, keycode, modifier);
			if(ret && Focussed)
				ScrollIntoView(Focussed);

			if(ret)
				return true;

			KeyboardEvent(p);

			if(ret)
				return true;

			return keyboard_event_actions(event, keycode, modifier);
		}

		virtual bool mouse_event(input::MouseEventType event,int x,int y);


		void SetMargins(int margin) { SetMargins(Margins(margin, margin, margin, margin)); }
		void SetMargins(int left, int top, int right, int bottom) { SetMargins(Margins(left, top, right, bottom)); }
		void SetMargins(int horizontal, int verticle) { SetMargins(Margins(horizontal, verticle, horizontal, verticle)); }
		void SetMargins(Margins margins)	{	
			this->contentmargin=margins;

			adjustlocations();
		}


		Margins getMargins() { return contentmargin; }
		void SetVerticleScrollbar(IScroller &scrollbar);
		void SetVerticleScrollbar(IScroller *scrollbar) { SetVerticleScrollbar(*scrollbar); }
		virtual void SetFocus(IWidgetObject *Object) { if(Object) ScrollIntoView(Object); IWidgetContainer::SetFocus(Object); container->Activate(); }


		void setVerticleScrollbarState(ScrollbarDisplayStates State) { verticlescrollbarstate=State; adjustlocations(); }
		ScrollbarDisplayStates getVerticleScrollbarState() { return verticlescrollbarstate; }
		bool isVerticleScrollbarVisible() { if(verticlescrollbar) return verticlescrollbar->GetWidgetObject()->isVisible(); else return false; }

		void SetPassive() { noactivate=nofocus=true; }
		void SetActive() { noactivate=nofocus=false; }
		void ScrollIntoView(IWidgetObject *Object);
		virtual bool	isActive() { return isactive && container->isActive(); }

		Point Overhead() { 
			return Point(
				IWidgetObject::width-usable.Width,
				IWidgetObject::height-usable.Height
			);
		}

		virtual Size2D ContentSize() {
			int max_x=0,max_y=0;
			foreach(IWidgetObject, object, Subobjects) {
				if(object->X()+object->Width()>max_x) max_x=object->X()+object->Width();
				if(object->Y()+object->Height()>max_y) max_y=object->Y()+object->Height();
			}

			return Size2D(max_x, max_y);
		}


		utils::EventChain<Frame, keyboard_event_params> KeyboardEvent;
		utils::EventChain<Frame, keyboard_event_params> KeypreviewEvent;

		utils::EventChain<Frame> GotFocusEvent;
		utils::EventChain<Frame> LostFocusEvent;

		bool AllowDrag;

		~Frame() {
			if(normal)
				delete normal;
			if(active)
				delete active;
			if(verticlescrollbar)
				delete verticlescrollbar;
		}

	protected:
		ResizableRect *normal;
		ResizableRect *active;
		ResizableRect *innernormal;
		ResizableRect *inneractive;
		ResizableRect *scrollingnormal;
		ResizableRect *scrollingactive;

		bool verticlescrollbardisplayed;
		
		WidgetLayer		BoxLayer;
		Basic2DLayer	ScrollingLayer;

		Margins outerborderwidth;
		Margins scrollbarmargin;
		Margins innerbordermargin;
		Margins innerborderwidth;
		Margins scrollingbordermargin;
		Margins scrollingborderwidth;
		Margins contentmargin;

		IScroller *verticlescrollbar;

		ScrollbarDisplayStates verticlescrollbarstate;
		bool noactivate;
		int maxvscroll;

		Size2D usable;

		void init();

		void frame_verticlescroll(IScroller &object) {
			ScrollingLayer.Y=-object.getValue();
		}

		virtual void container_hide() {
			if(pointerid) {
				Pointers.Reset(pointerid);
				pointerid=0;
			}

	
			utils::LinkedListIterator<IWidgetObject>it= Subobjects;
			IWidgetObject* widget;
			while(widget=it)
				widget->container_hide();
		}


		virtual void adjustlocations();

		void subobject_relocate(IWidgetObject &object) {
			if(verticlescrollbar)
				if(&object==verticlescrollbar->GetWidgetObject())
					return;

			adjustlocations();
		}

	};
} }
