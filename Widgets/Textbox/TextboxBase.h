#pragma once


#include "..\Base\Widget.h"
#include "..\Interfaces\IScroller.h"
#include "TextboxBlueprint.h"
#include <map>
#include <queue>
#include "..\Main.h"
#include "..\Basic\PetContainer.h"
#include "..\Interfaces\IScroller.h"
#include "..\Scrollbar.h"

namespace gge { namespace widgets {
	namespace textbox {
		class Base : public WidgetBase {
		public:

			Base() : controls(*this),
				bp(NULL), next_style(widgets::Blueprint::Style_None),
				scroll(0,0), vscroll(true), mhover(false), caretlocation(0), selectionstart(0),
				caret(NULL), selection(NULL), textlocation(0,0), mdown(false)
			{
				innerlayer.EnableClipping=true;

				style_anim.Pause();
				style_anim.Finished.Register(this, &Base::style_anim_finished);
				style_anim.Paused.Register(this, &Base::style_anim_finished);

				vscroll.bar.Hide();
				vscroll.bar.SetContainer(controls);
				vscroll.bar.AllowFocus=false;
				vscroll.bar.ChangeEvent().Register(this, &Base::vscroll_change);
			}

			virtual bool IsVisible() {
				return WidgetBase::IsVisible();
			}


			virtual bool IsEnabled() {
				return WidgetBase::IsEnabled();
			}

			virtual void Enable();

			virtual void Disable();

			virtual void Draw() {
				unprepared=true;
				WidgetBase::Draw();
			}

			virtual bool Focus();

			using WidgetBase::SetBlueprint;

			virtual void SetBlueprint(const widgets::Blueprint &bp);

			virtual bool MouseEvent(input::mouse::Event::Type event, utils::Point location, int amount);

			virtual bool KeyboardEvent(input::keyboard::Event::Type event, input::keyboard::Key Key);

		protected:
			void adjustscrolls();

			void prepare();

			virtual void draw();

			virtual bool loosefocus(bool force);

			virtual bool detach(ContainerBase *container) {
				innerlayer.parent=NULL;
				overlayer.parent=NULL;
				controls.BaseLayer.parent=NULL;

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
				BaseLayer->Add(controls, 0);
				BaseLayer->Add(overlayer, -1);

				//adjustscrolls();
			}

			void setupvscroll(bool allow, bool show, bool autohide) {
				vscroll.allow=allow;
				vscroll.show=allow && show;
				vscroll.autohide=autohide;

				adjustscrolls();
			}

			void settext(const std::string &value) {
				if(text!=value) {
					int l=text.length();

					text=value;

					if(caretlocation==l) {
						setcaretlocation(value.length());
					}

					Draw();
				}
			}
			std::string gettext() const {
				return text;
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

			void setcaretlocation(int location) {
				if(location<0) location=0;
				if(location>(int)text.length()) location=(int)text.length();

				if(caretlocation!=location) {
					selectionstart=location;

					caretlocation=location;

					Draw();
				}
			}
			int getcaretlocation() const {
				return caretlocation;
			}

			void setselection(int start, int end) {
				if(start<0) start=0;
				if(start>(int)text.length()) start=(int)text.length();
				if(end<0) end=0;
				if(end>(int)text.length()) end=(int)text.length();

				if(selectionstart!=start || caretlocation!=end) {
					selectionstart=start;
					caretlocation=end;

					Draw();
				}
			}
			int getselectionstart() const {
				return selectionstart;
			}

			void setprefix(const std::string &value) {
				if(prefix!=value) {
					prefix = value;
					
					Draw();
				}
			}
			std::string getprefix() const {
				return prefix;
			}

			void setsuffix(const std::string &value) {
				if(suffix!=value) {
					suffix = value;

					Draw();
				}
			}
			std::string getsuffix() const {
				return suffix;
			}

			virtual void textchanged() {}
			//validates user input. only partial string is sent
			virtual void validatetext(std::string &str) {}


			void style_anim_finished();

			void setstyle(Blueprint::StyleType type);


			const Blueprint *bp;


			graphics::Basic2DLayer overlayer;
			graphics::Colorizable2DLayer innerlayer;

			//for scrollbar
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

					Draw();
				}
			}

			void playsound(Blueprint::StyleType stylefrom, Blueprint::StyleType styleto);

		private:
			class cscroll {
			public:
				cscroll(bool allow) : allow(allow), show(allow), 
					autohide(true)
				{ }

				bool allow;
				bool show;
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

			animation::RectangularGraphic2DAnimation *caret;



			BorderData *outerborder;
			BorderData *overlay;
			BorderData *selection;

			int caretlocation;
			int selectionstart;

			utils::Point textlocation;

			bool mhover;
			bool mdown;

			Font *font;

			std::string text, prefix, suffix;


			//?
			std::map<resource::ResizableObjectProvider*, resource::ResizableObject*> ImageCache;
			std::map<BorderDataResource*, BorderData*> BorderCache;

			utils::Point   scroll;
		};
	}
}}

