#pragma once


#include "..\Base\Widget.h"
#include "..\Base\Container.h"
#include "CheckboxBlueprint.h"
#include <map>
#include <queue>
#include "..\Main.h"

#ifndef CHECKBOX_CLICK_DOWNDURATION
#	define	CHECKBOX_CLICK_DOWNDURATION	75
#endif

namespace gge { namespace widgets {
	namespace checkbox {
		class Base : public WidgetBase {
		public:
			Base(bool cangetfocus, AutosizeModes::Type autosize,bool textwrap, bool drawsymbol,bool drawicon) : text(""),
				cangetfocus(cangetfocus), autosize(autosize), textwrap(textwrap), underline(),
				drawsymbol(drawsymbol), drawicon(drawicon), icon(NULL), currentstate(1),
				mouseover(false), mousedown(false), next_focus(Blueprint::FT_None), next_state(0), next_style(Blueprint::YT_None)
			{
				focus_anim.Pause();
				focus_anim.Finished.Register(this, &Base::focus_anim_finished);
				focus_anim.Paused.Register(this, &Base::focus_anim_finished);

				state_anim.Pause();
				state_anim.Finished.Register(this, &Base::state_anim_finished);
				state_anim.Paused.Register(this, &Base::state_anim_finished);

				style_anim.Pause();
				style_anim.Finished.Register(this, &Base::style_anim_finished);
				style_anim.Paused.Register(this, &Base::style_anim_finished);

				wait_timeout.Pause();
				wait_timeout.Paused.Register(this, &Base::up);

				innerlayer.EnableClipping=true;

				//!later: separate draw and object determination
				draw();
			}
		
			virtual void SetBlueprint(const widgets::Blueprint &bp)  {
				this->bp=static_cast<const Blueprint*>(&bp);
				if(GetSize().Width==0)
					Resize(this->bp->DefaultSize);

				Draw();
			}

			virtual bool Focus();

			virtual void Disable();

			virtual void Enable();

			//call calculate objects if this widget is waiting in the draw queue
			//to make sure everything object is ready
			virtual utils::Size GetSize();

		protected:
			virtual void draw();

			animation::AnimationController &getanimation(Blueprint::TransitionType transition) {
				if(transition==Blueprint::FocusTransition)
					return focus_anim;
				else if(transition==Blueprint::StateTransition)
					return state_anim;
				else
					return style_anim;
			}

			int lineheight(Blueprint::Line *line, int &prevymargin);

			void drawline(int id, Blueprint::TransitionType transition, int y, int reqh, int h, int &prevymargin);


			//SERVICES TO CLIENT
			void setstate(int state);
			int  getstate() const {
				return currentstate;
			}

			void setfocus(Blueprint::FocusType type);

			void setstyle(Blueprint::StyleType type);

			void down() {
				if(!IsEnabled()) return;

				mousedown=true;
				setstyle(Blueprint::Down);
			}
			void up() {
				if(!IsEnabled()) return;

				mousedown=false;
				if(mouseover)
					setstyle(Blueprint::Hover);
				else
					setstyle(Blueprint::Normal);
			}
			void click() {
				if(!IsEnabled()) return;

				down();

				if(style.to==Blueprint::YT_None) {
					next_style=Blueprint::Normal;
					triggerwait();
				}
				else
					up();
			}
			void triggerwait() {
				wait_timeout.ResetProgress();
				wait_timeout.Play();
				wait_timeout.SetPauseAt(CHECKBOX_CLICK_DOWNDURATION);
			}
			void over() {
				mouseover=true;
				if(!IsEnabled()) return;

				if(!mousedown)
					setstyle(Blueprint::Hover);
			}
			void out() {
				mouseover=false;
				if(!IsEnabled()) return;

				if(!mousedown)
					setstyle(Blueprint::Normal);
			}

			void setautosize(AutosizeModes::Type autosize) {
				this->autosize=autosize;

				Draw();

				Container->WidgetBoundsChanged();
			}
			AutosizeModes::Type getautosize() const {
				return autosize;
			}

			void settextwrap(bool textwrap) {
				this->textwrap=textwrap;

				if(autosize)
					Container->WidgetBoundsChanged();

				Draw();
			}
			bool gettextwrap() const {
				return textwrap;
			}

			void settext(const std::string &text) {
				this->text=text;

				Draw();
			}
			std::string gettext() const {
				return text;
			}

			void setunderline(input::keyboard::Key key) {
				underline=key;

				Draw();
			}
			input::keyboard::Key getunderline() const {
				return underline;
			}
			
			void seticon(graphics::RectangularGraphic2D *icon) {
				this->icon=icon;

				Draw();
			}
			graphics::RectangularGraphic2D *geticon() const {
				return this->icon;
			}


			//NOTIFICATIONS
			virtual void containerenabledchanged(bool state);

			virtual bool loosefocus(bool force);


			//TO BE OVERLOADED
			virtual void clickcompleted() { } //is this necessary?? probably no


			animation::AnimationController focus_anim;
			animation::AnimationController state_anim;
			animation::AnimationController style_anim;
			animation::AnimationController wait_timeout;

			bool focus_anim_loop;
			bool state_anim_loop;
			bool style_anim_loop;

			Placeholder *symbolp;
			Placeholder *textp;
			Placeholder *iconp;


			resource::ResizableObject *symbol;
			BorderData *border;
			BorderData *overlay;
			Font *font;

			Blueprint::Line *lines[4];

			virtual bool detach(ContainerBase *container);

			virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order);


		private:
			Blueprint::FocusMode focus;
			Blueprint::StateMode state;
			Blueprint::StyleMode style;

			Blueprint::StateType currentstate;

			Blueprint::FocusType next_focus;
			Blueprint::StateType next_state;
			Blueprint::StyleType next_style;

			std::map<resource::ResizableObjectProvider*, resource::ResizableObject*> ImageCache;
			std::map<BorderDataResource*, BorderData*> BorderCache;

			void focus_anim_finished();
			void state_anim_finished();
			void style_anim_finished();

			//mostly for access key, the given key, if found, will be underlined.
			//it will be drawn stretched (probably)
			input::keyboard::Key underline;
			bool cangetfocus;
			AutosizeModes::Type autosize;
			bool textwrap;
			bool drawsymbol;
			bool drawicon;

			bool mouseover;
			bool mousedown;

			graphics::RectangularGraphic2D *icon;

			graphics::Colorizable2DLayer innerlayer;

			std::string text;

			const Blueprint *bp;
		};
	}
}}
