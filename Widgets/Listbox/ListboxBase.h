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
#include "../Organizers/MovingListOrganizer.h"
#include "../../Utils/StringUtils.h"

namespace gge { namespace widgets {
	class ListboxType {

	};

	namespace listbox {

		template<class T_>
		typename std::enable_if<utils::has_stringoperator<T_>::value>::type CastToString(const T_ &v, std::string &str) {
			str=(std::string)v;
		}
		template<class T_>
		typename std::enable_if<!utils::has_stringoperator<T_>::value>::type CastToString(const T_ &v, std::string &str) {
			std::stringstream ss;
			ss<<v;
			str=ss.str();
		}

		template<class T_>
		void EmptyStringConverter(const T_ &v, std::string &s) { s=""; }

		struct ListboxDragHandle : public gge::input::mouse::IDragData {
		public:
			ListboxDragHandle(int listboxid=0, int index=0) : listboxid(listboxid), index(index)
			{ }

			virtual int TypeID() const {
				return DragID;
			}

			int listboxid;

			int index;

			static const int DragID=13;
		};

		extern int nextindex;

		//forces subclasses to use item caching
		template<class R_>
		class Base : public WidgetBase, public ListboxType {
		public:

			Base() : bp(NULL),  controls(*this), autoheight(false), blueprintmodified(false),
				columns(1), allowreorder(false), dragtoken(0), prevheight(0), prevelmheight(0),
				elementcount(0), prevcols(1), id(nextindex), lastdragtime(0)
			{
				controls.AddWidget(panel);
				panel.Move(0,0);
				panel.SetOrganizer(organizer);
				panel.AllowTabSwitch=false;
				panel.GetVScroller().DisableAnimation();
				panel.ScrollingEvent.RegisterLambda([&]{
					if(this->organizer.SetVScroll(panel.GetVScroll())) {
						this->adjustitems();
					}
				});

				WR.LoadedEvent.Register(this, &Base::wr_loaded);
				triggerfn=[&](IListItem &item, int index) {
					this->trigger(dynamic_cast<R_&>(item), index+this->organizer.GetTop());
				};
				dragnotify=[&](IListItem &item, int index, gge::utils::Point location) {
					this->begindrag(item, index, location);
				};
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
						GetDraggedObject().TypeID()==ListboxDragHandle::DragID &&
						dynamic_cast<ListboxDragHandle&>(GetDraggedObject()).listboxid==id
					) {
						if(!dragtoken)
							dragtoken=Pointers.Set(Pointer::Drag);
						panelscroll=float(panel.GetVScroll());
						lastdragtime=0;
						return true;
					}
					else {
						return false;
					}
				}
				else if(event==Event::DragMove) {
					if(IsDragging()) {
						int vloc=location.y-panel.GetOverheadMargins().Top;
						if(vloc>panel.GetUsableHeight())
							vloc=panel.GetUsableHeight();

						if(vloc<panel.Widgets.First()->GetHeight()) {
							if(lastdragtime!=0) {
								unsigned t=Main.CurrentTime-lastdragtime;

								float v=1-(float)vloc/panel.Widgets.First()->GetHeight();
								float speed=std::pow(v, 0.4f)*8*panel.Widgets.First()->GetHeight();

								panelscroll-=speed*t/1000;
							}
							lastdragtime=Main.CurrentTime;
						}
						if(vloc>panel.GetUsableHeight()-panel.Widgets.First()->GetHeight()) {
							if(lastdragtime!=0) {
								unsigned t=Main.CurrentTime-lastdragtime;

								float v=1-(float)(panel.GetUsableHeight()-vloc)/panel.Widgets.First()->GetHeight();
								float speed=std::pow(v, 0.4f)*8*panel.Widgets.First()->GetHeight();

								panelscroll+=speed*t/1000;
							}
							lastdragtime=Main.CurrentTime;
						}

						ListboxDragHandle &h=dynamic_cast<ListboxDragHandle&>(GetDraggedObject());

						int l=h.index;

						location.x+=panel.Widgets.First()->GetWidth()/4;

						location.y+=panel.GetVScroll();

						int before=organizer.LogicalItemAt(location, true);
						if(before==-1) before=0;

						if(l!=before && l+1!=before && l>=0 && l<elementcount) {
							movebefore(l, before);

							if(before>l) {
								h.index=before-1;
							}
							else if(h.index>=elementcount){
								h.index=elementcount-1;
							}
							else {
								h.index=before;
							}
						}

						panel.SetVScroll(int(panelscroll));

						if(organizer.SetVScroll(panel.GetVScroll())) {
							adjustitems();
						}
					}
				}
				else if(event==Event::DragOut) {
					Pointers.Reset(dragtoken);
					dragtoken=0;
				}
				else if(event==Event::DragDrop) {
					Pointers.Reset(dragtoken);
					dragtoken=0;
				}


				return !Event::isScroll(event);
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

					adjustheight();
					Size.Height=panel.GetHeight();
				}
				else {
					panel.Resize(Size);
					controls.BaseLayer.Resize(Size);
				}

				if(BaseLayer)
					BaseLayer->Resize(controls.BaseLayer.BoundingBox.GetSize());

				this->panel.LargeScroll=this->panel.GetUsableSize().Height-this->panel.SmallScroll;

				checkelementlist();
			}

			virtual utils::Size GetSize() {
				if(!bp)
					return size;

				return utils::Size(size.Width ? size.Width : bp->DefaultSize.Width, size.Height ? size.Height : bp->DefaultSize.Height);
			}

			~Base() {
				if(dragtoken)
					Pointers.Reset(dragtoken);

				representations.Destroy();
			}

		protected:

			virtual void draw() {
			}

			//If the listbox supports reordering, it should supply this
			virtual void movebefore(unsigned item, unsigned before) {}

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

				if(BaseLayer) {
					BaseLayer->Add(controls);

					BaseLayer->Resize(controls.BaseLayer.BoundingBox.GetSize());

					BaseLayer->MouseCallback.Set(dynamic_cast<WidgetBase&>(*this), &WidgetBase::MouseHandler, input::mouse::Event::AllButOverCheck);
				}
			}

			void setautoheight(const bool &value) {
				if(autoheight!=value) {
					autoheight = value;
					adjustheight();

					checkelementlist();
				}
			}
			bool getautoheight() const {
				return autoheight;
			}
			void setcolumns(const int &value) {
				if(columns!=value) {
					columns = value;
					organizer.SetColumns(columns);
					adjustheight();

					checkelementlist();
				}
			}
			int  getcolumns() const {
				return columns;
			}
			void setallowreorder(const bool &value) {
				if(allowreorder!=value) {
					allowreorder = value;
					//for(auto it=panel.Widgets.First();it.IsValid();it.Next()) {
					//	dynamic_cast<IListItem*>(it.CurrentPtr())->SetDragAllowed(value);
					//}
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
					int elmheight=elementheight();
					int h=elmheight*elementcount;

					panel.SetHeight(200);
					h+=panel.GetHeight()-panel.GetUsableHeight();

					panel.SetHeight(std::min(h,sh));
					controls.BaseLayer.BoundingBox.SetHeight(std::min(h,sh));
				}
				else {
					panel.SetHeight(sh);
					controls.BaseLayer.BoundingBox.SetHeight(sh);
				}

				if(BaseLayer)
					BaseLayer->Resize(controls.BaseLayer.BoundingBox.GetSize());

				checkelementlist();
			}

			void checkelementlist() {
				int elmheight=elementheight();
				if(prevheight==panel.GetHeight() && prevelmheight==elmheight && columns==prevcols) return;

				prevheight=panel.GetHeight();

				int required=organizer.RequiredItems(elmheight);

				while(representations.GetCount()<required) {
					R_ *element=newelement(representations.GetCount(), triggerfn);
					element->SetBlueprint(bp->Item);
					element->DragNotify=dragnotify;
					representations.Add(element);
					panel.AddWidget(element->GetWidget());
					elementadded(*element);
				}

				organizer.Reorganize();
				adjustitems();
			}

			//*****
			//should be called
			void itemheightchanged() {
				adjustheight();
				if(!autoupdate) adjustitems();

				if(representations.GetCount()==0) {
					this->panel.SmallScroll=listbox::Base<R_>::bp->Item->DefaultSize.Height;
				}
				else {
					this->panel.SmallScroll=representations.First()->GetHeight();
				}
				this->panel.LargeScroll=this->panel.GetUsableSize().Height-this->panel.SmallScroll;
				this->panel.SmallScroll*=3;
			}
			void setitemcount(int count) {
				if(elementcount==count) return;

				int elmheight=elementheight();

				elementcount=count;
				organizer.SetLogicalCount(count);
				panel.LogicalHeight=organizer.GetLogicalHeight();

				this->adjustheight();
				/*if(!autoupdate)*/ adjustitems();
			}

			//******
			//could be implemented
			virtual R_ *newelement(int index, std::function<void(IListItem&, int)> trigger) { 
				auto newelm=new R_(index,trigger); 
				newelm->SetHeight(elementheight());

				return newelm;
			}
			//should return element height
			//element heights should be the same for everything to work smoothly
			virtual void elementadded(R_ &element) {}

			//*****
			//must be implemented
			virtual int elementheight() = 0;
			virtual void trigger(R_ &element, int index) = 0;
			//should set listitem values starting from the given start value
			virtual void adjustitems()=0;

			void begindrag(IListItem &item, int index, gge::utils::Point location) {
				if(!allowreorder) return;

				using namespace gge::input::mouse;

				BeginDrag(*new ListboxDragHandle(id, index+organizer.GetTop()));
				DragLocation=location;
			}


			const Blueprint *bp;

			PetContainer<Base> controls;

			ControlledPanel panel;

			bool blueprintmodified;

			virtual void wr_loaded() {
			}

			virtual void setblueprint(const widgets::Blueprint &bp);

			MovingListOrganizer organizer;

			gge::utils::OrderedCollection<R_> representations;

			bool autoupdate;

		private:
			bool autoheight;
			int columns;
			int prevheight;
			int prevelmheight;
			bool allowreorder;
			float panelscroll;
			PointerCollection::Token dragtoken;
			std::function<void(IListItem&, int)> triggerfn;
			std::function<void(IListItem&, int, gge::utils::Point)> dragnotify;
			int elementcount;
			int prevcols;
			int id;
			unsigned lastdragtime;
		};

		template<class R_>
		void Base<R_>::setblueprint(const widgets::Blueprint &bp) {
			if(this->bp==&bp)
				return;

			this->bp=static_cast<const Blueprint*>(&bp);

			if(this->bp->Panel)
				this->panel.SetBlueprint(*this->bp->Panel);

			if(size.Width==0) {
				this->panel.SetWidth(bp.DefaultSize.Width);
				this->controls.BaseLayer.BoundingBox.SetWidth(bp.DefaultSize.Width);
			}

			for(auto it=representations.First();it.IsValid();it.Next()) {
				it->SetBlueprint(*listbox::Base<R_>::bp->Item);
			}

			itemheightchanged();
			this->adjustheight();
		}

	}
}}

