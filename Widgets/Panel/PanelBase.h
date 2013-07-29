#pragma once


#include "../Base/Widget.h"
#include "../Interfaces/IScroller.h"
#include "PanelBlueprint.h"
#include <map>
#include <queue>
#include "../Main.h"
#include "../Basic/PetContainer.h"
#include "../Interfaces/IScroller.h"
#include "../Scrollbar.h"
#include "../StatefulLabel.h"
#include "../../Utils/OrderedCollection.h"
#include "../ExtenderLayer.h"

namespace gge { namespace widgets {
	namespace panel {
		class Base : public WidgetBase, public ContainerBase {
		public:

			Base();

			virtual ~Base() {
				clearcaches();
			}

			virtual bool IsVisible() const {
				return WidgetBase::IsVisible();
			}

			virtual void Show(bool setfocus=true) {
				ContainerBase::isvisible=true;
				WidgetBase::Show(setfocus);
				ContainerBase::Show();
			}

			virtual void Hide() {
				WidgetBase::Hide();
				ContainerBase::Hide();
			}


			virtual bool IsEnabled() const {
				return WidgetBase::IsEnabled();
			}

			virtual void Enable() {
				if(!IsEnabled()) {
					setstyle(widgets::Blueprint::Normal);
					WidgetBase::Enable();
					if(vscroll.bar.GetWidget())
						vscroll.bar.GetWidget()->Enable();

					for(auto it=Widgets.First();it.IsValid();it.Next()) {
						call_widget_containerenabledchanged(*it, true);
					}

					ContainerBase::isenabled=true;
				}
			}

			virtual void Disable() {
				if(IsEnabled()) {
					setstyle(widgets::Blueprint::Disabled);
					if(vscroll.bar.GetWidget())
						vscroll.bar.GetWidget()->Disable();

					WidgetBase::Disable();

					for(auto it=Widgets.First();it.IsValid();it.Next()) {
						call_widget_containerenabledchanged(*it, false);
					}

					ContainerBase::isenabled=false;
				}

			}


			virtual bool IsActive() const {
				return WidgetBase::IsFocused();
			}

			virtual LayerBase &CreateBackgroundLayer()  {
				LayerBase *layer=new LayerBase;
				background.Add(layer, background.SubLayers.HighestOrder()+1);

				return *layer;
			}

			virtual widgets::WidgetLayer &CreateWidgetLayer(int Order=0)  {
				widgets::WidgetLayer *layer=new widgets::WidgetLayer;
				widgetlayer.Add(layer, Order);

				return *layer;
			}

			virtual widgets::WidgetLayer &CreateExtenderLayer()  {
				widgets::WidgetLayer *layer=new widgets::WidgetLayer;
				extenderlayer.Add(layer, extenderlayer.SubLayers.LowestOrder()-1);

				return *layer;
			}

			using WidgetBase::ForceRemoveFocus;

			virtual void RedrawAll() {
				ContainerBase::RedrawAll();
				WidgetBase::Draw();
			}

			using WidgetBase::SetHeight;
			using WidgetBase::SetWidth;
			using WidgetBase::GetHeight;
			using WidgetBase::GetWidth;


			virtual void Resize(utils::Size Size) {
				unprepared=true;
				controls.Resize(Size);
				dialogcontrols.Resize(Size);
				WidgetBase::Resize(Size);

				adjustcontrols();
			}

			void Resize(int W, int H) {
				Resize(utils::Size(W,H));
			}

			virtual void SetContentSize(utils::Size Size) {
				Resize(Size+GetOverheadMargins());
			}

			void SetContentSize(int W, int H) {
				SetContentSize(utils::Size(W,H));
			}

			virtual void Draw() {
				unprepared=true;
				WidgetBase::Draw();
			}


			virtual utils::Size GetUsableSize() {
				prepare();
				return innerlayer.BoundingBox.GetSize();
			}

			virtual utils::Margins GetOverheadMargins() {
				prepare();
				return utils::Margins(
					innerlayer.BoundingBox.Left,
					innerlayer.BoundingBox.Top,
					GetWidth()-innerlayer.BoundingBox.Right,
					GetHeight()-innerlayer.BoundingBox.Bottom
				);
			}

			virtual bool RemoveFocus() {
				return WidgetBase::RemoveFocus();
			}

			virtual void Deactivate() {
				RemoveFocus();
			}

			void MoveBy(utils::Point amount) {
				Move(GetLocation()+amount);
			}

			using WidgetBase::Move;

			virtual void Move(utils::Point Location) {
				if(extenderlayercontainer)
					extenderlayercontainer->Move(Location);

				WidgetBase::Move(Location);
			}


			virtual bool Focus() {
				if(!Focused) {
					if(!allownofocus) {
						if(!FocusFirst())
							return false;
					}
					else
						FocusFirst();

					setstyle(widgets::Blueprint::Active);

					title.State=title.State_2;
					return WidgetBase::Focus();
				}
				else if(!IsFocused()) {
					setstyle(widgets::Blueprint::Active);

					bool ret=WidgetBase::Focus();
					if(ret) title.State=title.State_2;
					return ret;
				}
				else {
					title.State=title.State_2;
					return true;
				}
			}

			using WidgetBase::SetBlueprint;

			virtual void SetBlueprint(const widgets::Blueprint &bp) {
				blueprintmodified=true;
				setblueprint(bp);
			}

			virtual bool MouseHandler(input::mouse::Event::Type event, utils::Point location, int amount);

			virtual bool KeyboardHandler(input::keyboard::Event::Type event, input::keyboard::Key Key) {
				bool ret=ContainerBase::DistributeKeyboardEvent(event, Key);
				
				if(ret) return true;

				if(event==input::keyboard::Event::Char && (!input::keyboard::Modifier::Check() || input::keyboard::Modifier::Current==input::keyboard::Modifier::Alt)) {
					if(dialogcontrols.GetAccessKeys()[Key])
						if(dialogcontrols.GetAccessKeys()[Key]->IsVisible() && dialogcontrols.GetAccessKeys()[Key]->IsEnabled())
							if(dialogcontrols.GetAccessKeys()[Key]->Accessed())
								return true;
				}
				if(event==input::keyboard::Event::Char && (!input::keyboard::Modifier::Check() || input::keyboard::Modifier::Current==input::keyboard::Modifier::Alt)) {
					if(controls.GetAccessKeys()[Key])
						if(controls.GetAccessKeys()[Key]->IsVisible() && controls.GetAccessKeys()[Key]->IsEnabled())
							if(controls.GetAccessKeys()[Key]->Accessed())
								return true;
				}

				return false;
			}

			virtual void WidgetBoundsChanged() { 
				if(frozen) return;
				ContainerBase::WidgetBoundsChanged();

				adjustscrolls();
			}

			virtual utils::Point AbsoluteLocation();

			virtual utils::Size GetSize() {
				if(!bp)
					return WidgetBase::size;

				return utils::Size(WidgetBase::size.Width ? WidgetBase::size.Width : bp->DefaultSize.Width, WidgetBase::size.Height ? WidgetBase::size.Height : bp->DefaultSize.Height);
			}

		protected:
			virtual void setblueprint(const widgets::Blueprint &bp);

			void clearcaches();

			void adjustlayers();

			void adjustscrolls();

			void adjustcontrols();

			virtual int calculatevscrollback(int usableheight) {
				int height=usableheight;

				for(auto i=Widgets.First();i.IsValid();i.Next()) {
					int y=i->GetBounds().BottomRight().y;
					if(height<y)
						height=y;
				}

				return height;
			}

			void prepare();

			virtual void draw();

			void freeze() {
				frozen=true;
			}
			void unfreeze() {
				frozen=false;
				ContainerBase::WidgetBoundsChanged();

				adjustscrolls();
			}

			virtual void focus_changed(WidgetBase *newwidget);
			virtual bool loosefocus(bool force) {
				if(!Focused) {
					setstyle(widgets::Blueprint::Normal);
					title.State=title.State_1;
					return true;
				}

				if(force) {
					ContainerBase::ForceRemoveFocus();
					setstyle(widgets::Blueprint::Normal);
					title.State=title.State_1;
					return true;
				}
				else {
					if(ContainerBase::RemoveFocus()) {
						setstyle(widgets::Blueprint::Normal);
						title.State=title.State_1;
						return true;
					}
					else
						return false;
				}
			}

			virtual bool detach(ContainerBase *container) {
				WidgetBase::detach(container);
				
				utils::CheckAndDelete(extenderlayercontainer);
				innerlayer.parent=NULL;
				controls.BaseLayer.parent=NULL;
				dialogcontrols.BaseLayer.parent=NULL;
				overlayer.parent=NULL;
				extenderlayer.parent=NULL;

				return true;
			}

			virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
				WidgetBase::located(container, w, Order);

				BaseLayer->Add(innerlayer,1);
				BaseLayer->Add(dialogcontrols,0);
				BaseLayer->Add(controls,0);
				utils::CheckAndDelete(extenderlayercontainer);
				extenderlayercontainer=&container->CreateExtenderLayer();
				extenderlayercontainer->Add(extenderlayer, -1);
				extenderlayercontainer->Move(BaseLayer->BoundingBox.TopLeft());
				BaseLayer->Add(overlayer, -2);

				containerenabledchanged(container->IsEnabled());

				adjustcontrols();
			}

			virtual void containerenabledchanged(bool state) {
				if(style.from == widgets::Blueprint::Disabled || style.to == widgets::Blueprint::Disabled) {
					if(IsEnabled()) {
						setstyle(widgets::Blueprint::Normal);
					}					
				}
				else {
					if(!IsEnabled()) {
						setstyle(widgets::Blueprint::Disabled);
					}
				}

				if(vscroll.bar.GetWidget())
					vscroll.bar.GetWidget()->SetEnabled(state);
				for(auto it=Widgets.First();it.IsValid();it.Next())
					call_widget_containerenabledchanged(*it, state);
			}

			void setupvscroll(bool allow, bool show, bool autohide, bool dragscroll=false) {
				vscroll.allow=allow;
				vscroll.show=allow && show;
				vscroll.autohide=autohide;
				vscroll.dragscroll=allow & dragscroll;

				adjustscrolls();
			}

			void setallowmove(const bool &value) {
				allowmove=value;
			}
			bool getallowmove() const {
				return allowmove;
			}

			void setshowtitle(bool value) {
				if(value!=showtitle) {
					showtitle = value;
					title.SetVisibility(value);
					adjustcontrols();

					Reorganize();

					Draw();
				}
			}
			bool getshowtitle() const {
				return showtitle;
			}

			void settitle(const std::string &value) {
				if(title.Text!=value) {
					title.Text = value;
				}
			}
			std::string gettitle() const {
				return title.Text;
			}

			void seticon(graphics::RectangularGraphic2D *value) {
				title.Icon = value;
			}
			graphics::RectangularGraphic2D *geticon() const {
				return title.Icon;
			}

			void setallowresize(const bool &value) {
				allowresize=value;
			}
			bool getallowresize() const {
				return allowresize;
			}

			void setallownofocus(const bool &value) {
				allownofocus=value;
			}
			bool getallownofocus() const {
				return allownofocus;
			}

			void setpadding(const utils::Margins &value) {
				padding=value;

				Reorganize();

				Draw();			
			}
			utils::Margins getpadding() const {
				return padding;
			}

			void setallowvscroll(const bool &value) {
				if(vscroll.allow!=value) {
					vscroll.allow=value;

					if(!vscroll.allow && scroll.y) {
						vscrollto(0);
					}

					adjustscrolls();
				}
			}
			bool getallowvscroll() const {
				return vscroll.allow;
			}

			void setshowvscroll(bool value) {
				value=value && vscroll.allow;

				if(vscroll.show!=value) {
					vscroll.show=value;
					adjustscrolls();

					Reorganize();
				}
			}
			bool getshowvscroll() const {
				return vscroll.show;
			}

			void setsetautohidevscroll(const bool &value) {
				if(vscroll.autohide!=value) {
					vscroll.autohide=value;

					adjustscrolls();
				}
			}
			bool getsetautohidevscroll() const {
				return vscroll.autohide;
			}

			void setallowtabswitch(const bool &value) {
				tabswitch=value;
			}
			bool getallowtabswitch() const {
				return tabswitch;
			}

			void setdisplay(const bool &value) {
				if(display!=value) {
					display = value;
					Draw();
				}
			}
			bool getdisplay() const {
				return display;
			}

			/*void setshowtitlebtn(const bool &value) {
				if(showtitlebtn!=value) {
					showtitlebtn = value;
					adjustcontrols();
				}
			}
			bool getshowtitlebtn() const {
				return showtitlebtn;
			}

			void setshowdialogbtn(const bool &value) {
				if(showdialogbtn!=value) {
					showdialogbtn = value;
					adjustcontrols();
				}
			}
			bool getshowdialogbtn() const {
				return showdialogbtn;
			}*/

			BorderData *getouterborder() const { return outerborder; }
			BorderData *getinnerborder() const { return innerborder; }
			BorderData *getscrollingborder() const { return scrollingborder; }
			//BorderData *gettitleborder() const { return titleborder; }
			//BorderData *getdialogborder() const { return dialogborder; }
			
			utils::Margins getcontrolmargins() const { return controlmargins; }


			void placetitlebutton(WidgetBase &btn) {
				btn.SetContainer(controls);
				if(bp && dynamic_cast<Button*>(&btn))
					btn.SetBlueprint(bp->TitleButton);

				adjustcontrols();

				Reorganize();
			}

			void placedialogbutton(WidgetBase &btn) {
				btn.SetContainer(dialogcontrols);
				if(bp && dynamic_cast<Button*>(&btn))
					btn.SetBlueprint(bp->DialogButton);

				adjustcontrols();

				Reorganize();
			}

			void style_anim_finished();

			void setstyle(Blueprint::StyleType type);

			Scrollbar<> &getvscroller() {
				return vscroll.bar;
			}

			const Scrollbar<> &getvscroller() const {
				return vscroll.bar;
			}


			const Blueprint *bp;


			graphics::Basic2DLayer overlayer, innerlayer;
			LayerBase scrollinglayer, background;
			ExtenderLayer extenderlayer;
			graphics::Basic2DLayer widgetlayer;

			//for scrollbar, title buttons, etc
			PetContainer<Base> controls;
			//for dialog buttons
			PetContainer<Base> dialogcontrols;

			animation::Timer &getanimation(bool transition) {
				if(transition)
					return style_anim;
				else
					return idle_anim;
			}


			void vscrollto(int where) {
				vscroll.bar.Value=where;
			}

			void vscrollby(int amount) {
				vscroll.bar.Value+=amount*vscroll.bar.SmallChange;
			}

			int getvscroll() const { return vscroll.bar.Value; }

			virtual void vscroll_change() {
				//if(vscroll.bar.Value!=-scroll.y) {
					bool allow=true;
					ScrollingEvent(allow);
					if(!allow) {
						vscroll.bar.Value=-scroll.y;
						
						return;
					}
					scroll.y=-vscroll.bar.Value;
					adjustlayers();
				//}
			}

			//After any modification to this collection call adjust controls
			//To add a new button create it and send to placebutton function
			utils::OrderedCollection<WidgetBase> titlebuttons;
			utils::OrderedCollection<WidgetBase> dialogbuttons;

			StatefulLabel title;

			virtual void wr_loaded() {}
			bool blueprintmodified;

		private:
			bool allowmove;
			bool allowresize;
			bool allownofocus;
			bool display;
			bool frozen;

			bool showtitle;
			//bool showtitlebtn;
			//bool showdialogbtn;



			class cscroll {
			public:
				cscroll(bool allow) : allow(allow), show(allow), 
					dragscroll(false), autohide(true)
				{ }

				bool allow;
				bool show;
				bool dragscroll;//!
				bool autohide;

				Scrollbar<> bar;
			} vscroll/*, hscroll*/;

			bool unprepared;

			Blueprint::StyleMode style;
			Blueprint::StyleType next_style;

		//TODO Animations, idle, transition, animation finish function
			animation::Controller style_anim;
			animation::Timer	   idle_anim;
			bool style_anim_loop;


			bool move_mdown;
			bool move_ongoing;
			utils::Point move_mlocation;
			PointerCollection::Token move_pointer;


			utils::Margins padding;


			BorderData *outerborder;
			BorderData *innerborder; //!TODO: Draw
			BorderData *scrollingborder; //!TODO: Draw
			BorderData *titleborder; //!TODO: Draw
			BorderData *dialogborder; //!TODO: Draw
			BorderData *overlay;
			Byte Alpha, BGAlpha;




			std::map<resource::ResizableObjectProvider*, resource::ResizableObject*> ImageCache;
			std::map<BorderDataResource*, BorderData*> BorderCache;

			utils::Margins innermargins;
			utils::Margins scrollmargins;
			utils::Margins controlmargins;
			utils::Point   scroll;
			widgets::WidgetLayer *extenderlayercontainer;
		};
	}
}}

