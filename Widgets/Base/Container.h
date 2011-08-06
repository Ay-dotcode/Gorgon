#pragma once


#include "Widget.h"
#include "..\..\Utils\SortedCollection.h"
#include <map>
#include "..\..\Utils\EventChain.h"
#include "Organizer.h"
#include "..\Basic\WidgetLayer.h"


namespace gge { namespace widgets {

	class ContainerBase {
		friend class WidgetBase;
	public:

		virtual bool IsVisible() { return isvisible; }
		inline  bool IsHidden()  { return !IsVisible(); }
		virtual void Show(bool setfocus=false) { 
			isvisible=true;
			
			if(setfocus && !Focussed) {
				bool done=false;
				if(PrevFocus)
					done=PrevFocus->Focus();
				if(!done && Widgets.getCount())
					FocusFirst();
			}
		}
		virtual void Hide() {
			isvisible=false; 

			if(Focussed)
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


		virtual bool IsEnabled()  { return isenabled; }
		inline  bool IsDisabled() { return !IsEnabled(); }
		virtual void Enable() { isenabled=true; }
		virtual void Disable() { isenabled=false; }
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
		virtual void Resize(utils::Size Size) { 
			size=Size;
			Reorganize();
		}
		void Resize(int W, int H) { Resize(utils::Size(W,H)); }
		void SetWidth(int W)  { Resize(W, size.Height); }
		void SetHeight(int H) { Resize(size.Width,  H); }


		virtual WidgetBase *GetFocussed() { return Focussed; }
		virtual bool HasFocussed() { return Focussed!=NULL; }
		//focuses to the first item that can take it, if no items can get focus returns false.
		virtual bool FocusFirst() {
			if(!isvisible || !isenabled)
				return false;

			for(auto it=Widgets.First();it.isValid();it.Next()) {
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

			for(auto it=Widgets.Find(Focussed)+1;it.isValid();it.Next()) {
				if(it->isvisible && it->isenabled) {
					if(it->Focus())
						return true;
				}
			}

			//roll again from bottom
			for(auto it=Widgets.First();it.isValid() && &(*it)!=Focussed;it.Next()) {
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

			for(auto it=Widgets.Find(Focussed)-1;it.isValid();it.Previous()) {
				if(it->isvisible && it->isenabled) {
					if(it->Focus())
						return true;
				}
			}

			//roll again from bottom
			for(auto it=Widgets.Last();it.isValid() && &(*it)!=Focussed;it.Previous()) {
				if(it->isvisible && it->isenabled) {
					if(it->Focus())
						return true;
				}
			}

			return false;
		}

		//Should remove currently focused item and should not set any other in place
		virtual bool RemoveFocus() {
			if(!call_widget_loosefocus(Focussed))
				return false;

			PrevFocus=Focussed;
			Focussed=NULL;

			return true;
		}
		virtual void ForceRemoveFocus() {
			call_widget_loosefocus(Focussed, true);

			PrevFocus=Focussed;
			Focussed=NULL;
		}

		
		//!!Might need to be changed to IButton or something similar
		//not a good idea to put ordinary widget here.
		virtual WidgetBase *GetDefault() { return Default; }
		virtual bool HasDefault() { return Default!=NULL; }
		virtual void SetDefault(WidgetBase &widget) { Default=&widget; }
		virtual void RemoveDefault() { Default=NULL; }
		virtual WidgetBase *GetCancel() { return Cancel; }
		virtual bool HasCancel() { return Cancel!=NULL; }
		virtual void SetCancel(WidgetBase &widget) { Cancel=&widget; }
		virtual void RemoveCancel() { Cancel=NULL; }


		bool IsTabSwitchEnabled() { return tabswitch; }
		void EnableTabSwitch() { tabswitch=true; }
		void DisableTabSwitch() { tabswitch=false; }
		void ToggleTabSwitchEnabledState() { tabswitch=!tabswitch; }
		void SetTabSwitchEnabledState(bool state) { tabswitch = state; }


		virtual WidgetLayer &CreateWidgetLayer(int Order=0) = 0;
		virtual LayerBase   &CreateBackgroundLayer(int Order=0) = 0;
		virtual LayerBase   &CreateExtenderLayer() = 0;


		//A container or the target widget has right to reject add requests
		virtual bool AddWidget	 (WidgetBase &widget, int Order=0) {
			if(!call_widget_locating(widget, Order))
				return false;

			utils::SortedCollection<WidgetBase>::Wrapper *w=&Widgets.Add(widget, Order);

			call_widget_located(widget, w, Order);

			Reorganize();

			return true;
		}
		//Returns false if requested widget is an integral part of the container or
		//given widget simply does not exists
		virtual bool RemoveWidget(WidgetBase &widget) {
			auto it=Widgets.Find(widget);
			if(!it.isValid())
				return false;

			if(!call_widget_detach(widget))
				return false;

			if(Focussed==&widget)
				if(!FocusNext())
					RemoveFocus();
			if(Default==&widget)
				RemoveDefault();
			if(Cancel==&widget)
				RemoveCancel();

			Widgets.Remove(it);

			Reorganize();

			return true;
		}
		bool AddWidget	 (WidgetBase *widget, int Order=0) { 
			if(widget==NULL)
				return false;

			return AddWidget(*widget, Order);	
		}
		bool RemoveWidget(WidgetBase *widget) { 
			if(widget==NULL)
				return false;

			RemoveWidget(*widget); 
		}


		virtual bool IsActive() = 0;
		virtual void Deactivate() = 0;


		virtual void Redraw() {
			for(auto it=Widgets.First();it.isValid();it.Next())
				it->Redraw();
		}
		virtual void Reorganize() {
			if(Organizer) {
				Organizer->Reorganize();
			}
		}


		virtual void WidgetBoundsChanged() { 
			Reorganize();
		}

		 
		virtual void RegisterAccessKey(WidgetBase &widget, int Key) { AccessKeys[Key]=&widget; }
		virtual void RemoveAccessKey  (WidgetBase &widget, int Key) { AccessKeys[Key]=NULL; }
		virtual int  GetAccessKey(WidgetBase &widget) {
			for(auto it=AccessKeys.begin();it!=AccessKeys.end();++it) {
				if(it->second==&widget)
					return it->first;
			}
		}
		virtual WidgetBase *GetAccessTarget(int Key) { return AccessKeys[Key]; }
		bool IsAccessKeysEnabled() { return accesskeysenabled; }
		void EnableAccessKeys() { accesskeysenabled=true; }
		void DisableAccessKeys() { accesskeysenabled=false; }
		void ToggleAccessKeysEnabledState() { accesskeysenabled=!accesskeysenabled; }
		void SetAccessKeysEnabledState(bool state) { accesskeysenabled=state; }


		Organizer *GetOrganizer() { return Organizer; }
		bool HasOrganizer() { return Organizer!=NULL; }
		virtual void SetOrganizer(Organizer &organizer) { 
			Organizer=&organizer;
			Reorganize();
		}
		void SetOrganizer(Organizer *organizer) { SetOrganizer(*organizer); }
		virtual void RemoveOrganizer() { Organizer=NULL; }


		//*Might* be useful
		//utils::EventChain<ContainerBase, input::keyboard::Event> KeyboardEvent;


		~ContainerBase();


	protected:

		virtual bool focus_changing(WidgetBase *newwidget) { return true; }
		virtual void focus_changed (WidgetBase *newwidget) { }

		bool PerformStandardKeyboardActions(input::keyboard::Event::Type event, int Key) {
			//Tab switch
			if(tabswitch) {
				if(event==input::keyboard::Event::Char && Key==input::keyboard::KeyCodes::Tab) {
					if(!input::keyboard::Modifier::Check()) {
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
				//!!Might need to change to IButton
				if(Default && Default->IsVisible() && Default->IsEnabled() && event==input::keyboard::Event::Char && Key==input::keyboard::KeyCodes::Enter) {
					if(Default->KeyboardEvent(input::keyboard::Event::Char, input::keyboard::KeyCodes::Enter))
						return true;
				}

				if(Cancel && Cancel->IsVisible() && Cancel->IsEnabled() && event==input::keyboard::Event::Char && Key==input::keyboard::KeyCodes::Escape) {
					if(Cancel->KeyboardEvent(input::keyboard::Event::Char, input::keyboard::KeyCodes::Enter))
						return true;
				}

				//Access keys
				if(event==input::keyboard::Event::Char && (!input::keyboard::Modifier::Check() || input::keyboard::Modifier::Current==input::keyboard::Modifier::Alt)) {
					if(AccessKeys[Key])
						if(AccessKeys[Key]->IsVisible() && AccessKeys[Key]->IsEnabled())
							if(AccessKeys[Key]->Focus())
								return true;
				}
			}

			return false;
		}
		bool DistributeKeyboardEvent(input::keyboard::Event::Type event, int Key) {
			if(Focussed) {
				if(Focussed->KeyboardEvent(event, Key))
					return true;
			}

			PerformStandardKeyboardActions(event, Key);
		}


		//TO BE CALLED BY WIDGET
		
		//if container is invisible, disabled, or object is not in this container or cannot be focused for any reason, return will be false
		bool setfocus(WidgetBase *widget) { 
			if(!isvisible || !isenabled)
				return false;

			auto it=Widgets.Find(widget);
			if(!it.isValid())
				return false;
		
			if(!focus_changing(widget))
				return false;

			if(Focussed)
				if(!call_widget_loosefocus(Focussed))
					return false;

			Focussed=widget;

			focus_changed(widget);

			return true;
		}

		void widget_visibility_change(WidgetBase *widget, bool state) {
			if(state==false && widget==Focussed)
				FocusNext();

			Reorganize();
		}


		//FRIENDSHIP HELPERS

		bool call_widget_loosefocus(WidgetBase *widget, bool force=false) {
			if(!widget) return true;
			return widget->loosefocus(force);
		}

		bool call_widget_locating(WidgetBase &widget, int Order)  {
			widget.locating(this,Order);
		}

		bool call_widget_detach(WidgetBase &widget)  {
			widget.detach(this);
		}

		void call_widget_located(WidgetBase &widget, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order)  {
			widget.located(this,w,Order);
		}




		bool isvisible;
		bool isenabled;
		bool tabswitch;
		bool accesskeysenabled;
		utils::Size size;

		WidgetBase *Default;
		WidgetBase *Cancel;
		WidgetBase *Focussed;
		WidgetBase *PrevFocus;

		widgets::Organizer *Organizer;


		utils::SortedCollection<WidgetBase> Widgets;
		std::map<int, WidgetBase*> AccessKeys;

	};

}}
