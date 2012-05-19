#pragma once


#include "Widget.h"
#include "../../Utils/SortedCollection.h"
#include <map>
#include "../../Utils/EventChain.h"
#include "Organizer.h"
#include "../Basic/WidgetLayer.h"
#include <limits>


namespace gge { namespace widgets {

	class ContainerBase {
		friend class WidgetBase;
	public:


		ContainerBase() : 
			Default(NULL),
			Cancel(NULL),
			Focused(NULL),
			PrevFocus(NULL),
			size(0,0),
			isvisible(true),
			isenabled(true),
			tabswitch(true),
			accesskeysenabled(true),
			organizer(NULL),
			UpHandler(NULL),
			organizing(false),
			ScrollingEvent("Scrolling", this)
		{ }


		virtual bool IsVisible() const { return isvisible; }
		inline  bool IsHidden()  const { return !IsVisible(); }
		virtual void Show(bool setfocus=false) { 
			isvisible=true;
			
			if(setfocus && !Focused) {
				bool done=false;
				if(PrevFocus)
					done=PrevFocus->Focus();
				if(!done && Widgets.GetCount())
					FocusFirst();
			}
		}
		virtual void Hide() {
			isvisible=false; 

			if(Focused)
				RemoveFocus();
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


		virtual bool IsEnabled()  const { return isenabled; }
		inline  bool IsDisabled() const { return !IsEnabled(); }
		virtual void Enable() { 
			if(!IsEnabled()) {
				isenabled=true;
				for(auto it=Widgets.First();it.IsValid();it.Next()) {
					call_widget_containerenabledchanged(*it, true);
				}
			} 
		}
		virtual void Disable() { 
			if(IsEnabled()) {
				isenabled=false;
				for(auto it=Widgets.First();it.IsValid();it.Next()) {
					call_widget_containerenabledchanged(*it, false);
				}
			}
		}
	//TODO !Virtual status of the following two functions might change
		void ToggleEnabled() { 
			if(!isenabled)	Enable(); 
			else			Disable();
		}
		void SetEnabled(bool enabled) {
			if(enabled && !isenabled)		Enable();
			else if(!enabled && isenabled)	Disable();
		}


		virtual utils::Size GetSize() { return size; }
		int GetWidth()   { return GetSize().Width; }
		int GetHeight()  { return GetSize().Height; }
		virtual void Resize(utils::Size Size) { 
			size=Size;
			Reorganize();
		}
		void Resize(int W, int H) { Resize(utils::Size(W,H)); }
		void SetWidth(int W)  { Resize(W, size.Height); }
		void SetHeight(int H) { Resize(size.Width,  H); }

		virtual utils::Size GetUsableSize() { return GetSize(); }
		int GetUsableWidth() { return GetUsableSize().Width; }
		int GetUsableHeight() { return GetUsableSize().Height; }

		virtual utils::Margins GetOverheadMargins() { 
			int w=(GetWidth()-GetUsableWidth())/2;
			int h=(GetHeight()-GetUsableHeight())/2;
			return utils::Margins(w,h,w,h); 
		}

		virtual utils::Point AbsoluteLocation() const {
			return utils::Point(0,0);
		}
		int AbsoluteLeft() const {
			return AbsoluteLocation().x;
		}
		int AbsoluteTop() const {
			return AbsoluteLocation().y;
		}


		virtual WidgetBase *GetFocused() const { return Focused; }
		virtual bool HasFocused() const { return Focused!=NULL; }
		//focuses to the first item that can take it, if no items can get focus returns false.
		virtual bool FocusFirst() {
			if(!isvisible || !isenabled)
				return false;

			for(auto it=Widgets.First();it.IsValid();it.Next()) {
				if(it->isvisible && it->isenabled) {
					if(it->Focus())
						return true;
				}
			}

			return false;
		}
		virtual bool FocusNext() {
			if(!isvisible || !isenabled)
				return false;

			for(auto it=Widgets.Find(Focused)+1;it.IsValid();it.Next()) {
				if(it->isvisible && it->isenabled) {
					if(it->Focus())
						return true;
				}
			}

			//roll again from bottom
			for(auto it=Widgets.First();it.IsValid() && it.CurrentPtr()!=Focused;it.Next()) {
				if(it->isvisible && it->isenabled) {
					if(it->Focus())
						return true;
				}
			}

			return false;
		}
		virtual bool FocusPrevious() {
			if(!isvisible || !isenabled)
				return false;

			for(auto it=Widgets.Find(Focused)-1;it.IsValid();it.Previous()) {
				if(it->isvisible && it->isenabled) {
					if(it->Focus())
						return true;
				}
			}

			//roll again from bottom
			for(auto it=Widgets.Last();it.IsValid() && &(*it)!=Focused;it.Previous()) {
				if(it->isvisible && it->isenabled) {
					if(it->Focus())
						return true;
				}
			}

			return false;
		}

		//Should remove currently focused item and should not set any other in place
		virtual bool RemoveFocus() {
			if(!focus_changing(NULL))
				return false;

			if(!call_widget_loosefocus(Focused))
				return false;

			PrevFocus=Focused;
			Focused=NULL;

			focus_changed(NULL);
			return true;
		}
		virtual void ForceRemoveFocus() {
			call_widget_loosefocus(Focused, true);

			PrevFocus=Focused;
			Focused=NULL;

			focus_changed(NULL);
		}

		
		virtual WidgetBase *GetDefault() const { return Default; }
		virtual bool HasDefault() const { return Default!=NULL; }
		virtual void SetDefault(WidgetBase &widget) { Default=&widget; }
		virtual void RemoveDefault() { Default=NULL; }
		virtual WidgetBase *GetCancel() const { return Cancel; }
		virtual bool HasCancel() const { return Cancel!=NULL; }
		virtual void SetCancel(WidgetBase &widget) { Cancel=&widget; }
		virtual void RemoveCancel() { Cancel=NULL; }


		bool IsTabSwitchEnabled() const { return tabswitch; }
		void EnableTabSwitch() { tabswitch=true; }
		void DisableTabSwitch() { tabswitch=false; }
		void ToggleTabSwitchEnabledState() { tabswitch=!tabswitch; }
		void SetTabSwitchEnabledState(bool state) { tabswitch = state; }

		//The ownership lies with the caller
		virtual WidgetLayer &CreateWidgetLayer(int Order=0) = 0;
		virtual LayerBase   &CreateBackgroundLayer() = 0;
		virtual WidgetLayer &CreateExtenderLayer() = 0;


		//A container or the target widget has right to reject add requests
		virtual bool AddWidget	 (WidgetBase &widget, int Order=std::numeric_limits<int>::min()) {
			if(!call_widget_locating(widget, Order))
				return false;

			if(Order==std::numeric_limits<int>::min())
				Order=Widgets.HighestOrder()+1;

			utils::SortedCollection<WidgetBase>::Wrapper *w=&Widgets.Add(widget, Order);

			call_widget_located(widget, w, Order);

			WidgetBoundsChanged();

			return true;
		}
		//Returns false if requested widget is an integral part of the container or
		//given widget simply does not exists
		virtual bool RemoveWidget(WidgetBase &widget) {
			auto it=Widgets.Find(widget);
			if(!it.IsValid())
				return false;

			if(!call_widget_detach(widget))
				return false;

			if(Focused==&widget)
				if(!FocusNext())
					RemoveFocus();
			if(Default==&widget)
				RemoveDefault();
			if(Cancel==&widget)
				RemoveCancel();
			if(UpHandler==&widget)
				UpHandler=NULL;

			Widgets.Remove(it);

			WidgetBoundsChanged();

			return true;
		}
		bool AddWidget	 (WidgetBase *widget, int Order=std::numeric_limits<int>::min()) { 
			if(widget==NULL)
				return false;

			return AddWidget(*widget, Order);	
		}
		bool RemoveWidget(WidgetBase *widget) { 
			if(widget==NULL)
				return false;

			return RemoveWidget(*widget); 
		}


		virtual bool IsActive() const = 0;
		virtual void Deactivate() = 0;


		virtual void RedrawAll() {
			if(!IsVisible())
				return;

			for(auto it=Widgets.Last();it.IsValid();it.Previous()) {
				if(it->IsVisible())
					it->Draw();
			}
		}
		virtual void Reorganize() {
			if(!organizing) {
				organizing=true;
				if(organizer) {
					organizer->Reorganize();
				}

				organizing=false;
			}
		}


		virtual void WidgetBoundsChanged() { 
			Reorganize();
		}

		
		//Widgets are responsible of removing previous access key if a single one is supported
		virtual void SetAccessKey  (WidgetBase &widget, input::keyboard::Key Key) { 
			WidgetBase *w=AccessKeys[Key];

			AccessKeys[Key]=&widget; 

			if(w)
				w->accesskeystatechanged();
		}
		virtual void ResetAccessKey(int Key) { AccessKeys[Key]=NULL; }
		virtual input::keyboard::Key  GetAccessKey(WidgetBase &widget) {
			for(auto it=AccessKeys.begin();it!=AccessKeys.end();++it) {
				if(it->second==&widget)
					return it->first;
			}

			return 0;
		}
		virtual WidgetBase *GetAccessTarget(int Key) const { if(AccessKeys.count(Key)) return AccessKeys.find(Key)->second; else return NULL; }
		bool IsAccessKeysEnabled() const { return accesskeysenabled; }
		void EnableAccessKeys() { 
			if(!accesskeysenabled) {
				accesskeysenabled=true;

				for(auto it=AccessKeys.begin();it!=AccessKeys.end();++it) {
					if(it->second)
						it->second->accesskeystatechanged();
				}
			}
		}
		void DisableAccessKeys() { 
			if(accesskeysenabled) {
				accesskeysenabled=false;

				for(auto it=AccessKeys.begin();it!=AccessKeys.end();++it) {
					if(it->second)
						it->second->accesskeystatechanged();
				}
			}
		}
		void ToggleAccessKeysEnabledState() { 
			if(!accesskeysenabled)
				EnableAccessKeys();
			else
				DisableAccessKeys(); 
		}
		void SetAccessKeysEnabledState(bool state) { 
			if(state)
				EnableAccessKeys();
			else
				DisableAccessKeys(); 
		}


		Organizer *GetOrganizer() const { return organizer; }
		bool HasOrganizer() const { return organizer!=NULL; }
		virtual void SetOrganizer(Organizer &organizer) {
			if(this->organizer) 
				this->organizer->SetAttached(NULL);

			this->organizer=&organizer;
			this->organizer->SetAttached(this);
			Reorganize();
		}
		void SetOrganizer(Organizer *organizer) { if(organizer) SetOrganizer(*organizer); else RemoveOrganizer(); }
		virtual void RemoveOrganizer() { 
			auto org=this->organizer;
			this->organizer=NULL;
			if(org) 
				org->SetAttached(NULL);  
		}


		//*Might* be useful
		//utils::EventChain<ContainerBase, input::keyboard::Event> KeyboardEvent;


		virtual ~ContainerBase() {
			for(auto it=Widgets.First();it.IsValid();it.Next()) {
				if(it->BoundToContainer)
					it.Delete();
			}
		}

		utils::SortedCollection<WidgetBase> Widgets;

		utils::EventChain<ContainerBase, bool&> ScrollingEvent;

	protected:

		virtual bool focus_changing(WidgetBase *newwidget) { return true; }
		virtual void focus_changed (WidgetBase *newwidget) { }

		bool PerformStandardKeyboardActions(input::keyboard::Event::Type event, input::keyboard::Key Key) {
			//Tab switch
			if(tabswitch) {
				if(event==input::keyboard::Event::Char && Key==input::keyboard::KeyCodes::Tab) {
					if(input::keyboard::Modifier::Current==input::keyboard::Modifier::None) {
						FocusNext();
						return true;
					}
					else if(input::keyboard::Modifier::Current==input::keyboard::Modifier::Shift) {
						FocusPrevious();
						return true;
					}
				}
			}

			if(accesskeysenabled) {
				//Enter/Esc -> Type of access keys
			//TODO !Might need to change to IButton
				if(Default && Default->IsVisible() && Default->IsEnabled() && event==input::keyboard::Event::Char && (!input::keyboard::Modifier::Check() || input::keyboard::Modifier::Current==input::keyboard::Modifier::Ctrl) && Key==input::keyboard::KeyCodes::Enter) {
					if(Default->Accessed())
						return true;
				}

				if(Cancel && Cancel->IsVisible() && Cancel->IsEnabled() && event==input::keyboard::Event::Char && !input::keyboard::Modifier::Check() && Key==input::keyboard::KeyCodes::Escape) {
					if(Cancel->Accessed())
						return true;
				}

				//Access keys
				if(event==input::keyboard::Event::Char && (!input::keyboard::Modifier::Check() || input::keyboard::Modifier::Current==input::keyboard::Modifier::Alt)) {
					if(AccessKeys[Key])
						if(AccessKeys[Key]->IsVisible() && AccessKeys[Key]->IsEnabled())
							if(AccessKeys[Key]->Accessed())
								return true;
				}
			}

			return false;
		}
		bool DistributeKeyboardEvent(input::keyboard::Event::Type event, input::keyboard::Key Key) {
			if(Focused) {
				if(event==input::keyboard::Event::Up && UpHandler) {
					UpHandler->KeyboardHandler(event, Key);
					UpHandler=NULL;
				}
				else if(Focused->IsVisible() && Focused->IsEnabled() && Focused->KeyboardHandler(event, Key)) {
					if(event==input::keyboard::Event::Down)
						UpHandler=Focused;

					return true;
				}
				else {
					//if(event==input::keyboard::Event::Down)
					//	UpHandler=NULL;
				}
			}

			return PerformStandardKeyboardActions(event, Key);
		}


		//TO BE CALLED BY WIDGET
		
		//if container is invisible, disabled, or object is not in this container or cannot be focused for any reason, return will be false
		bool setfocus(WidgetBase *widget) { 
			if(!isvisible || !isenabled)
				return false;

			auto it=Widgets.Find(widget);
			if(!it.IsValid())
				return false;
		
			if(!focus_changing(widget))
				return false;

			if(Focused && Focused!=widget)
				if(!call_widget_loosefocus(Focused))
					return false;

			Focused=widget;

			focus_changed(widget);

			return true;
		}

		void widget_visibility_change(WidgetBase *widget, bool state) {
			if(state==false && widget==Focused)
				FocusNext();

			Reorganize();
		}


		//FRIENDSHIP HELPERS

		bool call_widget_loosefocus(WidgetBase *widget, bool force=false) {
			if(!widget) return true;
			return widget->loosefocus(force);
		}

		bool call_widget_locating(WidgetBase &widget, int Order)  {
			return widget.locating(this,Order);
		}

		bool call_widget_detach(WidgetBase &widget)  {
			return widget.detach(this);
		}

		void call_widget_located(WidgetBase &widget, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order)  {
			return widget.located(this,w,Order);
		}

		void call_widget_accesskeystatechanged(WidgetBase &widget)  {
			widget.accesskeystatechanged();
		}

		void call_widget_containerenabledchanged(WidgetBase &widget, bool state) {
			widget.containerenabledchanged(state);
		}




		bool isvisible;
		bool isenabled;
		bool tabswitch;
		bool accesskeysenabled;
		bool organizing;
		utils::Size size;

		WidgetBase *Default;
		WidgetBase *Cancel;
		WidgetBase *Focused;
		WidgetBase *PrevFocus;
		WidgetBase *UpHandler;

		gge::widgets::Organizer *organizer;


		std::map<input::keyboard::Key, WidgetBase*> AccessKeys;

	};

	static ContainerBase &operator <<(ContainerBase &container, WidgetBase &widget) {
		widget.SetContainer(container);

		return container;
	}

}}
