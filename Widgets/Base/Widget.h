#pragma once


#include <limits>
#include "Blueprint.h"
#include "..\..\Utils\Point2D.h"
#include "..\..\Utils\Size2D.h"
#include "../../Engine/Pointer.h"
#include "../Basic/WidgetLayer.h"


namespace gge { namespace widgets {

	class ContainerBase;

	extern utils::Collection<WidgetBase> DrawQueue;

	class WidgetBase {
		friend class ContainerBase;
		friend void Draw_Signal(IntervalObject *interval, void *data);
	public:


		WidgetBase() : Container(NULL),
			location(0,0),
			size(0,0),
			BaseLayer(NULL),
			wrapper(NULL),
			isvisible(true),
			isenabled(true),
			pointer(Pointer::None),
			GotFocus("GotFocus", this),
			LostFocus("LostFocus", this),
			waitingforredraw(false),
			mousetoken(0)
		{ }


		//Every widget should be given their own blueprints
		// otherwise an invalid cast will occur
		virtual void SetBlueprint(const Blueprint &bp)=0;



		virtual bool IsVisible() { return isvisible; }
		inline  bool IsHidden()  { return !IsVisible(); }
		virtual void Show(bool setfocus=false) { 
			isvisible=true;
			if(BaseLayer)
				BaseLayer->isVisible=isvisible;
			call_container_widget_visibility_change(true);
		}
		virtual void Hide() {
			isvisible=false; 
			if(BaseLayer)
				BaseLayer->isVisible=isvisible;
			call_container_widget_visibility_change(false);
		}
		//Virtual status of the following two functions might change
		void ToggleVisibility() { 
			if(!isvisible)	Show(); 
			else			Hide();  
		}
		void SetVisibility(bool visible) { 
			if(visible && !isvisible)		Show(); 
			else if(!visible && isvisible)	Hide(); 
		}


		virtual bool IsEnabled();
		inline  bool IsDisabled() { return !IsEnabled(); }
		virtual void Enable() { isenabled=true; }
		virtual void Disable();
		//!!Virtual status of the following two functions might change
		void ToggleEnabled() { 
			if(!isenabled)	Enable(); 
			else			Disable();
		}
		void SetEnabled(bool enabled) {
			if(enabled && !isenabled)		Enable();
			else if(!enabled && isenabled)	Disable();
		}



		virtual utils::Size GetSize() { return size; }
		int GetWidth() { return GetSize().Width; }
		int GetHeight() { return GetSize().Height; }
		virtual void Resize(utils::Size Size);
		void Resize(int W, int H) { Resize(utils::Size(W,H)); }
		void SetWidth(int W)  { Resize(W, size.Height); }
		void SetHeight(int H) { Resize(size.Width,  H); }



		virtual utils::Point GetLocation() { return location; }
		int GetX()    { return GetLocation().x; }
		int GetY()    { return GetLocation().y; }
		int GetLeft() { return GetLocation().x; }
		int GetTop()  { return GetLocation().y; }
		virtual void Move(utils::Point Location);
		void Move(int X, int Y) { Move(utils::Point(X,Y)); }
		void SetX(int X) { Move(utils::Point(X,location.y)); }
		void SetY(int Y) { Move(utils::Point(location.x,Y)); }
		void SetLeft(int X) { Move(utils::Point(X,location.y)); }
		void SetTop(int Y) { Move(utils::Point(location.x,Y)); }


		virtual utils::Bounds    GetBounds() { return utils::Bounds(GetLocation(), GetSize()); }
		virtual utils::Rectangle GetRectangle() { return utils::Rectangle(GetLocation(), GetSize()); }
		virtual void SetBounds(utils::Bounds b) { Move(b.TopLeft()); Resize(b.GetSize()); }
		virtual void SetRectangle(utils::Rectangle r) { Move(r.TopLeft()); Resize(r.GetSize()); }


		virtual bool IsFocused();
		virtual bool Focus() {
			call_container_setfocus();

			GotFocus();

			return true;
		}
		bool SetFocus(bool focus) {
			if(focus && !IsFocused()) {
				call_container_setfocus();
				return true;
			}
			else if(!focus && IsFocused())
				return RemoveFocus();
		}
		virtual bool RemoveFocus() {
			if(IsFocused()) {
				call_container_removefocus();
				return true;
			}
			else
				return false;
		}
		virtual void ForceRemoveFocus() {
			if(IsFocused())
				call_container_forceremovefocus();
		}


		virtual int  GetZOrder() {
			if(!BaseLayer)
				return 0;

			return BaseLayer->GetOrder();
		}
		virtual void SetZOrder(int Order) {
			if(BaseLayer)
				BaseLayer->SetOrder(Order);
		}
		virtual void ToTop() {
			if(BaseLayer)
				BaseLayer->OrderToTop();
		}
		virtual void ToBottom() {
			if(BaseLayer)
				BaseLayer->OrderToBottom();
		}

		virtual int  GetFocusOrder() {
			if(!wrapper)
				return 0;

			return wrapper->GetKey();
		}
		virtual void SetFocusOrder(int Order) {
			if(wrapper)
				wrapper->Reorder(Order);
		}
		void FocusOrderToTop() {
			if(wrapper)
				SetFocusOrder(wrapper->GetParent().LowestOrder()-1);
		}
		void FocusOrderToBottom() {
			if(wrapper)
				SetFocusOrder(wrapper->GetParent().HighestOrder()+1);
		}


		virtual ContainerBase *GetContainer() { return Container; }
		virtual bool HasContainer() { return Container!=NULL; }
		void SetContainer(ContainerBase &container);
		void SetContainer(ContainerBase *container) { if(!container) Detach(); else SetContainer(*container); }
		virtual void Detach();

		virtual WidgetLayer *GetBaseLayer() { return BaseLayer; }


		virtual Pointer::PointerType GetPointer() { return pointer; }
		virtual void SetPointer(Pointer::PointerType Pointer) { pointer=Pointer; }
		virtual void ResetPointer() { pointer=Pointer::None; }


		virtual bool KeyboardEvent(input::keyboard::Event::Type event, input::keyboard::Key Key) {
			return false;
		}

		virtual bool MouseEvent(input::mouse::Event::Type event, utils::Point location, int amount) { 
			if(input::mouse::Event::isClick(event) && !IsFocused())
				Focus();

			if(event==input::mouse::Event::Over && pointer!=Pointer::None)
				mousetoken=Pointers.Set(pointer);
			else if(event==input::mouse::Event::Out) {
				Pointers.Reset(mousetoken);
				mousetoken=0;
			}

			return !input::mouse::Event::isScroll(event);
		}


		virtual void Draw() {
			if(waitingforredraw) return;

			DrawQueue.Add(this);
			waitingforredraw=true;
		}

		virtual bool Accessed() {
			return Focus();
		}



		utils::EventChain<WidgetBase> GotFocus;
		utils::EventChain<WidgetBase> LostFocus;

		virtual ~WidgetBase() {
			DrawQueue.Remove(this);
			Detach();
		}


	protected:

		virtual void draw()=0;

		//TO BE CALLED BY CONTAINER
		virtual bool loosefocus(bool force) { LostFocus(); return true; }
		virtual bool locating(ContainerBase *container, int Order);
		virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order);
		virtual void accesskeystatechanged() { }
		virtual void containerenabledchanged(bool state) { }

		void locateto(ContainerBase* container, int Order, utils::SortedCollection<WidgetBase>::Wrapper * w);

		virtual bool detach(ContainerBase *container) {
			Container=NULL;
			utils::CheckAndDelete(BaseLayer);
			wrapper=NULL;

			return true;
		}
		
		
		//FRIEND HELPERS
		bool call_container_setfocus();
		void call_container_widget_visibility_change(bool state);
		void call_container_removefocus();
		void call_container_forceremovefocus();

		WidgetLayer   *BaseLayer;
		ContainerBase *Container;

		bool isvisible;
		bool isenabled;

		bool waitingforredraw;

		PointerCollection::Token mousetoken;

		utils::Point location;
		utils::Size  size;

		Pointer::PointerType pointer;

		utils::SortedCollection<WidgetBase>::Wrapper *wrapper;

		private:
			WidgetBase(const WidgetBase &wb);
	};

}}
