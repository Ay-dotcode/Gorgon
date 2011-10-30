#pragma once


#include "..\Base\Widget.h"
#include "..\Interfaces\IScroller.h"
#include "..\Interfaces\IButton.h"
#include "PanelBlueprint.h"
#include <map>
#include <queue>
#include "..\Main.h"
#include "..\Basic\PetContainer.h"
#include "..\Interfaces\IScroller.h"
#include "..\Scrollbar.h"

namespace gge { namespace widgets {
	namespace panel {
		class Base : public WidgetBase, public ContainerBase {
		public:

			Base() : innermargins(0),
				allownofocus(false), allowmove(false), allowresize(false),
				controls(*this),
				bp(NULL), next_style(widgets::Blueprint::Style_None),
				move_mdown(false), move_ongoing(false), padding(5),
				move_pointer(PointerCollection::NullToken), scroll(0,0),
				vscroll(true), scrollmargins(0)
			{
				innerlayer.Add(scrollinglayer);
				scrollinglayer.Add(background, 1);
				scrollinglayer.Add(widgetlayer, 0);
				scrollinglayer.Add(extenderlayer, -1);
				innerlayer.EnableClipping=true;

				style_anim.Pause();
				style_anim.Finished.Register(this, &Base::style_anim_finished);
				style_anim.Paused.Register(this, &Base::style_anim_finished);

				vscroll.bar.Hide();
				vscroll.bar.SetContainer(controls);
				vscroll.bar.AllowFocus=false;
				vscroll.bar.SmallChange=60;
				vscroll.bar.LargeChange=120;
				vscroll.bar.ChangeEvent().Register(this, &Base::vscroll_change);
			}

			virtual bool IsVisible() {
				return WidgetBase::IsVisible();
			}

			virtual void Show(bool setfocus=true) {
				WidgetBase::Show(setfocus);
				ContainerBase::Show();
			}

			virtual void Hide() {
				ContainerBase::Hide();
				WidgetBase::Hide();
			}


			virtual bool IsEnabled() {
				return WidgetBase::IsEnabled();
			}

			virtual void Enable() {
				if(!IsEnabled()) {
					setstyle(widgets::Blueprint::Normal);
					WidgetBase::Enable();

					for(auto it=Widgets.First();it.isValid();it.Next()) {
						call_widget_containerenabledchanged(*it, true);
					}
				}
			}

			virtual void Disable() {
				if(IsEnabled()) {
					setstyle(widgets::Blueprint::Disabled);

					WidgetBase::Disable();

					for(auto it=Widgets.First();it.isValid();it.Next()) {
						call_widget_containerenabledchanged(*it, true);
					}
				}

			}


			virtual bool IsActive() {
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

			virtual void Resize(utils::Size Size) {
				WidgetBase::Resize(Size);

				adjustscrolls();
			}

			void Resize(int W, int H) {
				WidgetBase::Resize(utils::Size(W,H));
			}

			virtual void Draw() {
				unprepared=true;
				WidgetBase::Draw();
			}

			virtual utils::Size GetSize() {
				return WidgetBase::GetSize();
			}

			virtual utils::Size GetUsableSize() {
			//TODO ?
				return innerlayer.BoundingBox.GetSize();
			}

			virtual bool RemoveFocus() {
				return WidgetBase::RemoveFocus();
			}

			virtual void Deactivate() {
				WidgetBase::RemoveFocus();
			}

			void MoveBy(utils::Point amount) {
				Move(GetLocation()+amount);
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

					return WidgetBase::Focus();
				}
				else if(!IsFocused()) {
					setstyle(widgets::Blueprint::Active);

					return WidgetBase::Focus();
				}
				else
					return true;
			}

			virtual void SetBlueprint(const widgets::Blueprint &bp) {
				this->bp=static_cast<const Blueprint*>(&bp);
				if(WidgetBase::size.Width==0)
					Resize(this->bp->DefaultSize);

				for(auto i=BorderCache.begin();i!=BorderCache.end();++i)
					utils::CheckAndDelete(i->second);

				for(auto i=ImageCache.begin();i!=ImageCache.end();++i)
					utils::CheckAndDelete(i->second);

				if(this->bp)
					if(this->bp->Scroller)
						vscroll.bar.SetBlueprint(*this->bp->Scroller);

				if(this->bp) {
					this->pointer=bp.Pointer;
				}

				Draw();
			}

			virtual bool MouseEvent(input::mouse::Event::Type event, utils::Point location, int amount);

			virtual bool KeyboardEvent(input::keyboard::Event::Type event, input::keyboard::Key Key) {
				return ContainerBase::DistributeKeyboardEvent(event, Key);
			}

			virtual void WidgetBoundsChanged() { 
				ContainerBase::WidgetBoundsChanged();

				adjustscrolls();
			}

		protected:
			void adjustlayers();

			void adjustscrolls();

			void prepare();

			virtual void draw();

			virtual void focus_changed(WidgetBase *newwidget);
			virtual bool loosefocus(bool force) {
				if(!Focused) {
					setstyle(widgets::Blueprint::Normal);
					return true;
				}

				if(force) {
					ContainerBase::ForceRemoveFocus();
					setstyle(widgets::Blueprint::Normal);
					return true;
				}
				else {
					if(ContainerBase::RemoveFocus()) {
						setstyle(widgets::Blueprint::Normal);
						return true;
					}
					else
						return false;
				}
			}

			virtual bool detach(ContainerBase *container) {
				innerlayer.parent=NULL;
				controls.BaseLayer.parent=NULL;
				overlayer.parent=NULL;

				return true;
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
			}

			virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
				WidgetBase::located(container, w, Order);
				
				BaseLayer->Add(innerlayer,1);
				BaseLayer->Add(controls,0);
				BaseLayer->Add(overlayer, -1);

				containerenabledchanged(container->IsEnabled());

				adjustscrolls();
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
			LayerBase scrollinglayer, background, widgetlayer, extenderlayer;

			//for scrollbar, title buttons, etc
			PetContainer<Base> controls;


			animation::AnimationTimer &getanimation(bool transition) {
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

			virtual void vscroll_change() {
				if(vscroll.bar.Value!=-scroll.y) {
					scroll.y=-vscroll.bar.Value;
					adjustlayers();
				}
			}

		private:
			bool allowmove;
			bool allowresize;
			bool allownofocus;


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
			animation::AnimationController style_anim;
			animation::AnimationTimer	   idle_anim;
			bool style_anim_loop;


			bool move_mdown;
			bool move_ongoing;
			utils::Point move_mlocation;
			PointerCollection::Token move_pointer;


			utils::Margins padding;


			BorderData *outerborder;
			BorderData *overlay;
			Byte Alpha, BGAlpha;




			std::map<resource::ResizableObjectProvider*, resource::ResizableObject*> ImageCache;
			std::map<BorderDataResource*, BorderData*> BorderCache;

			utils::Margins innermargins;
			utils::Margins scrollmargins;
			utils::Point   scroll;
		};
	}
}}

