#pragma once


#include "../Base/Widget.h"
#include "../Interfaces/IScroller.h"
#include "ListboxBlueprint.h"
#include <map>
#include <queue>
#include "../Main.h"
#include "../Basic/PetContainer.h"
#include "../Interfaces/IScroller.h"
#include "../Scrollbar.h"
#include "../Panel/ControlledPanel.h"
#include "IListItem.h"
#include "../Organizers/ListOrganizer.h"

namespace gge { namespace widgets {
	class ListboxType {

	};

	namespace listbox {

		template<class T_>
		void CastToString(const T_ &v, std::string &str) {
			str=(std::string)v;
		}

		template<class T_, void(*CF_)(const T_ &, std::string &)=CastToString<T_> >
		class Base : public WidgetBase, public ListboxType {
		public:

			Base() : bp(NULL),  controls(*this), autoheight(false), blueprintmodified(false), KeepItems(false),
				columns(1), allowreorder(false), dragtoken(0)
			{
				controls.AddWidget(panel);
				panel.Move(0,0);
				panel.SetOrganizer(organizer);
				panel.AllowTabSwitch=false;

				WR.LoadedEvent.Register(this, &Base::wr_loaded);
			}


			virtual bool IsEnabled() {
				return WidgetBase::IsEnabled();
			}

			virtual void Enable() {
				controls.Enable();
				WidgetBase::Enable();
			}

			virtual void Disable() {
				controls.Disable();
				WidgetBase::Disable();
			}

			virtual void Draw() {
				panel.Draw();
				WidgetBase::Draw();
			}

			virtual bool Focus() {
				if(IsEnabled() && IsVisible())
					panel.Focus();

				return WidgetBase::Focus();
			}

			virtual void SetBlueprint(const widgets::Blueprint &bp) {
				blueprintmodified=true;
				setblueprint(bp);
			}

			virtual bool MouseHandler(input::mouse::Event::Type event, utils::Point location, int amount) {
				using namespace input::mouse;
				if(event==Event::DragOver) {
					if(
						IsDragging() && 
						GetDraggedObject().TypeID()==IListItem<T_, CF_>::DragID && 
						panel.Widgets.FindLocation(dynamic_cast<IListItem<T_, CF_>&>(GetDraggedObject()).GetWidget())!=-1
					) {
						if(!dragtoken)
							dragtoken=Pointers.Set(Pointer::Drag);
						return true;
					}
					else {
						return false;
					}
				}
				else if(event==Event::DragMove) {
					//location+=DragLocation;
					if(IsDragging()) {
						IListItem<T_, CF_> &l=dynamic_cast<IListItem<T_, CF_>&>(GetDraggedObject());
						
						//location.y-=l.GetWidget().GetHeight()/2;
						//location.x-=l.GetWidget().GetWidth()/2;

						IListItem<T_, CF_> *before=NULL;
						for(auto w=panel.Widgets.First();w.IsValid();w.Next()) {
							if(w->GetY()+w->GetHeight()>location.y && w->GetX()+w->GetWidth()>location.x) {
								before=dynamic_cast<IListItem<T_, CF_>*>(w.CurrentPtr());
								break;
							}
						}
						movebefore(l, before);
					}
				}
				else if(event==Event::DragDrop) {
					Pointers.Reset(dragtoken);
					dragtoken=0;
				}
					
				
				return !Event::isScroll(event);
			}

			virtual bool KeyboardHandler(input::keyboard::Event::Type event, input::keyboard::Key Key) {
				return panel.KeyboardHandler(event, Key);
			}
			using WidgetBase::Resize;
			virtual void Resize(utils::Size Size) {
				WidgetBase::Resize(Size);
				if(Size.Width==0 && bp)
					Size.Width=bp->DefaultSize.Width;
				if(Size.Height==0 && bp)
					Size.Height=bp->DefaultSize.Height;

				if(autoheight) {
					panel.SetWidth(Size.Width);
					controls.BaseLayer.BoundingBox.SetWidth(Size.Width);
				}
				else {
					panel.Resize(Size);
					controls.BaseLayer.Resize(Size);
				}

				if(BaseLayer)
					BaseLayer->Resize(controls.BaseLayer.BoundingBox.GetSize());
			}

			virtual utils::Size GetSize() {
				if(!bp)
					return size;

				return utils::Size(size.Width ? size.Width : bp->DefaultSize.Width, size.Height ? size.Height : bp->DefaultSize.Height);
			}

			~Base() {
				if(!KeepItems)
					panel.Widgets.Destroy();
				
				if(dragtoken)
					Pointers.Reset(dragtoken);
			}
			
			bool KeepItems;

		protected:

			virtual void add(IListItem<T_, CF_> &item) {
				panel.AddWidget(item.GetWidget());
				item.setallowdrag(allowreorder);
				if(autoheight)
					adjustheight();
			}

			virtual void remove(IListItem<T_, CF_> &item) {
				item.GetWidget().Detach();
				if(autoheight)
					adjustheight();
			}

			virtual void draw() {
			}

			//If the listbox supports reordering, it should supply this
			virtual void movebefore(IListItem<T_, CF_> &item, IListItem<T_, CF_> *before) {}

			virtual bool loosefocus(bool force) {
				if(force) {
					panel.ForceRemoveFocus();
					return true;
				}
				else
					return panel.RemoveFocus();
			}

			virtual bool detach(ContainerBase *container) {
				controls.BaseLayer.parent=NULL;

				return true;
			}

			virtual void containerenabledchanged(bool state) {
				controls.InformEnabledChange(state);
			}

			virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
				WidgetBase::located(container, w, Order);

				BaseLayer->Add(controls);

				if(BaseLayer)
					BaseLayer->Resize(controls.BaseLayer.BoundingBox.GetSize());

				BaseLayer->MouseCallback.Set(dynamic_cast<WidgetBase&>(*this), &WidgetBase::MouseHandler, input::mouse::Event::AllButOverCheck);
			}

			void setautoheight(const bool &value) {
				if(autoheight!=value) {
					autoheight = value;
					adjustheight();
				}
			}
			bool getautoheight() const {
				return autoheight;
			}
			void setcolumns(const int &value) {
				if(columns!=value) {
					columns = value;
					organizer.SetColumns(columns);
					if(autoheight)
						adjustheight();
				}
			}
			int getcolumns() const {
				return columns;
			}
			void setallowreorder(const bool &value) {
				if(allowreorder!=value) {
					allowreorder = value;
					for(auto it=panel.Widgets.First();it.IsValid();it.Next()) {
						dynamic_cast<IListItem<T_, CF_>*>(it.CurrentPtr())->setallowdrag(value);
					}
				}
			}
			bool getallowreorder() const {
				return allowreorder;
			}

			void adjustheight() {
				int sh=size.Height;

				if(sh==0 && bp)
					sh=bp->DefaultSize.Height;

				if(autoheight) {
					panel.SetHeight(200);
					int h=0;
					for(auto it=panel.Widgets.First();it.IsValid();it.Next()) {
						if(it->IsVisible())
							h+=it->GetHeight();
					}
					h+=panel.GetHeight()-panel.GetUsableHeight();
					panel.SetHeight(std::min(h+1,sh));
					controls.BaseLayer.BoundingBox.SetHeight(std::min(h+1,sh));
				}
				else {
					panel.SetHeight(sh);
					controls.BaseLayer.BoundingBox.SetHeight(sh);
				}

				if(BaseLayer)
					BaseLayer->Resize(controls.BaseLayer.BoundingBox.GetSize());
			}


			const Blueprint *bp;

			PetContainer<Base> controls;

			ControlledPanel panel;

			bool blueprintmodified;

			virtual void wr_loaded() {
			}

			virtual void setblueprint(const widgets::Blueprint &bp);
		

			ListOrganizer organizer;
		private:
			bool autoheight;
			int columns;
			bool allowreorder;
			PointerCollection::Token dragtoken;
		};

		template<class T_, void(*CF_)(const T_ &, std::string &)>
		void Base<T_, CF_>::setblueprint(const widgets::Blueprint &bp) {
			if(this->bp==&bp)
				return;

			this->bp=static_cast<const Blueprint*>(&bp);

			if(this->bp->Panel)
				panel.SetBlueprint(*this->bp->Panel);

			if(size.Width==0) {
				panel.SetWidth(bp.DefaultSize.Width);
				controls.BaseLayer.BoundingBox.SetWidth(bp.DefaultSize.Width);
			}

			adjustheight();
		}

	}
}}

