#pragma once

#include "SliderBluePrint.h"
#include "..\Base\Widget.h"
#include <map>
#include "..\Main.h"


namespace gge { namespace widgets {

	namespace slider {

		#pragma region "Float types"
		template <class T_>
		struct floattype {
			typedef float Type;
		};

		template<>
		struct floattype<long> {
			typedef double Type;
		};

		template<>
		struct floattype<unsigned> {
			typedef double Type;
		};

		template<>
		struct floattype<unsigned long> {
			typedef double Type;
		};

		template<>
		struct floattype<long long> {
			typedef double Type;
		};

		template<>
		struct floattype<double> {
			typedef double Type;
		};

		template<>
		struct floattype<long double> {
			typedef long double Type;
		};
#pragma endregion

		//if you need to work with big ints (>16M), use long or unsigned, otherwise smooth sliding
		//will not work for you.
		// RETAIN ABOVE COMMENT IN ALL CHILD CLASSES
		//Template parameter must allow +, -, / and (float) operators, for enums T_=int should be used
		// and enum should be mapped manually to ints
		template<class T_, class floattype=typename floattype<T_>::Type>
		class Base : public WidgetBase, private animation::AnimationBase {
		public:


			Base(T_ value=T_(0), T_ minimum=T_(0), T_ maximum=T_(100), T_ steps=std::numeric_limits<T_>::epsilon(), T_ smallchange=T_(1), T_ largechange=T_(10)) : passivemode(false),
				orientation(Blueprint::Vertical), next_style(Blueprint::Style_None),
				value(value), minimum(minimum), maximum(maximum), steps(steps), autosize(AutosizeModes::Autosize),
				smallchange(smallchange), largechange(largechange), unprepared(false),
				rule_region(0,0,0,0), ticks_region(0,0,0,0), numbers_region(0,0,0,0),
				smoothvalue(floattype(value)), symbol_mdown(false), symbol_mover(false), valuehover(false)
			{
				if(this->steps==0)
					this->steps=1;

				focus_anim.Pause();
				focus_anim.Finished.Register(this, &Base::focus_anim_finished);
				focus_anim.Paused.Register(this, &Base::focus_anim_finished);

				style_anim.Pause();
				style_anim.Finished.Register(this, &Base::style_anim_finished);
				style_anim.Paused.Register(this, &Base::style_anim_finished);

				smooth.controller.Pause();
				SetController(smooth.controller);

				symbollayer.MouseCallback.Set(*this, &Base::symbol_mouse);

				innerlayer.EnableClipping=true;
			}


			virtual void SetBlueprint(const widgets::Blueprint &bp)  {
				this->bp=static_cast<const Blueprint*>(&bp);
				if(WidgetBase::size.Width==0)
					Resize(this->bp->DefaultSize);

				Draw();
			}

			virtual bool MouseEvent(input::mouse::Event::Type event, utils::Point location, int amount)  {
				if(passivemode) return true;

				WidgetBase::MouseEvent(event, location, amount);

				if(event==input::mouse::Event::Left_Click) {
					if(ishorizontal()) {
						if(rule_region.isInside(location)) {
							if(actions.rule_action==LargeChange) {
								if(valueperpixel*(location.x-value_start)+minimum>value) {
									setvalue_smooth(value+largechange);
									value_changed();
								}
								else if(valueperpixel*(location.x-value_start)+minimum<value) {
									setvalue_smooth(value-largechange);
									value_changed();
								}
							}
							else if(actions.rule_action==Goto) {
								setvalue_smooth(T_(valueperpixel*(location.x-value_start))+minimum);
								value_changed();
							}
						}
						if(ticks_region.isInside(location) && actions.tick_click) {
							floattype i=markers.tickdistance;
							floattype v=Round(valueperpixel*(location.x-value_start)/i)*i+minimum;
							v=Round(v/i)*i;

							setvalue_smooth(T_(v));
							value_changed();
						}
						if(numbers_region.isInside(location) && actions.number_click) {
							floattype i=markers.tickdistance*markers.numberdistance;
							floattype v=Round(valueperpixel*(location.x-value_start)/i)*i+minimum;
							v=Round(v/i)*i;

							setvalue_smooth(T_(v));
							value_changed();
						}
					}
				}
				else {
					//!
				}

				if(event==input::mouse::Event::Move) {
					if(ishorizontal()) {
						if(rule_region.isInside(location)) {
							if(actions.rule_action==Goto) {
								floattype v=valueperpixel*(location.x-value_start)+minimum;
								v=Round(v/steps)*steps;

								hovervalue=T_(v);
								valuehover=true;

								Draw();
							}
						}
						if(ticks_region.isInside(location) && actions.tick_click) {
							floattype i=markers.tickdistance;
							floattype v=Round(valueperpixel*(location.x-value_start)/i)*i+minimum;
							v=Round(v/i)*i;
							v=Round(v/steps)*steps;

							hovervalue=T_(v);
							valuehover=true;

							Draw();
						}
						if(numbers_region.isInside(location) && actions.number_click) {
							floattype i=markers.tickdistance*markers.numberdistance;
							floattype v=Round(valueperpixel*(location.x-value_start)/i)*i+minimum;
							v=Round(v/i)*i;
							v=Round(v/steps)*steps;

							hovervalue=T_(v);
							valuehover=true;

							Draw();
						}
					}
				}
				else {
					//!
				}

				if(event==input::mouse::Event::Out) {
					valuehover=false;

					Draw();
				}


				return true;
			}

			virtual void Disable()  {
				WidgetBase::Disable();

				setstyle(Blueprint::Disabled);
			}

			virtual void Enable()  {
				WidgetBase::Enable();

				if(symbol_mover)
					setstyle(Blueprint::Hover);
				else
					setstyle(Blueprint::Normal);
			}

			virtual bool Focus() {
				if(passivemode)
					return false;

				if(!IsEnabled())
					return false;

				if(IsFocussed())
					return true;

				WidgetBase::Focus();
				setfocus(Blueprint::Focused);

				return true;
			}

			virtual void Draw() {
				unprepared=true;
				WidgetBase::Draw();
			}

			virtual utils::Size GetSize();


		protected:
			enum RuleAction {
				NoAction,
				LargeChange,
				Goto
			};

			class numberformat {
			public:
				numberformat(std::string units="", std::string prefix="", int decimals=-1, bool hex=false) : units(units),
					prefix(prefix), decimals(decimals), hex(hex)
				{ }

				numberformat(int decimals, std::string units="") : units(units),
					prefix(""), decimals(decimals), hex(false)
				{ }

				bool operator ==(const numberformat &format) const {
					return units==format.units && prefix==format.prefix && decimals==format.decimals && hex==format.hex;
				}

				bool operator !=(const numberformat &format) const {
					return !this->operator ==(format);
				}

				std::string units;
				std::string prefix;
				int decimals;
				bool hex;
			};

			void prepare();

			virtual void draw();

			virtual void containerenabledchanged(bool state)  {
				if(style.from == Blueprint::Disabled || style.to == Blueprint::Disabled) {
					if(IsEnabled()) {
						if(symbol_mover)
							setstyle(Blueprint::Hover);
						else
							setstyle(Blueprint::Normal);
					}					
				}
				else {
					if(!IsEnabled()) {
						setstyle(Blueprint::Disabled);
					}
				}
			}

			virtual bool detach(ContainerBase *container)  {
				innerlayer.parent=NULL;
				overlayer.parent=NULL;
				symbollayer.parent=NULL;
				return WidgetBase::detach(container);
			}

			virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
				WidgetBase::located(container, w, Order);
				if(BaseLayer) {
					BaseLayer->Add(innerlayer, 1);
					BaseLayer->Add(symbollayer, 0);
					BaseLayer->Add(overlayer, -1);
				}
			}

			virtual bool loosefocus(bool force)  {
				if(!IsFocussed())
					return true;

				if(!symbol_mdown) {
					setfocus(Blueprint::NotFocused);
					return true;
				}
				else
					return false;
			}


			void setfocus(Blueprint::FocusType type);

			void setstyle(Blueprint::StyleType type);

			
			//will be called to validate the value, will only be called when
			// value is changed by local actions (mouse or keyboard handled by
			// this class)
			virtual void value_changed() { }

			//subject to change
			void setupdisplay(bool symbol, bool rule, bool indicator, bool buttons, bool value, Alignment::Type valuelocation=Alignment::Middle_Center) {
				display.symbol=symbol;
				display.rule=rule;
				display.indicator=indicator;
				display.buttons=buttons;
				display.value=value;
				this->valuelocation=valuelocation;

				Draw();
			}

			//speed is percent per second
			void setsmoothingmode(bool symbol, bool indicator, bool value, float speed=100) {
				smooth.symbol=symbol;
				smooth.indicator=indicator;
				smooth.value=value;
				smooth.speed=speed;

				Draw();
			}

			void setmarkers(bool ticks, bool numbers, floattype tickdistance=10, int numberdistance=2) {
				markers.ticks=ticks;
				markers.tickdistance=tickdistance;
				markers.numbers=numbers;
				markers.numberdistance=numberdistance;

				Draw();
			}

			void setactions(bool symbol_drag, RuleAction rule_action, bool tick_click, bool number_click) {
				actions.symbol_drag=symbol_drag;
				actions.rule_action=rule_action;
				actions.tick_click=tick_click;
				actions.number_click=number_click;
			}

			void setactive() {
				passivemode=false;
				Draw();
			}
			void setpassive() {
				passivemode=true;
				Draw();
			}
			bool ispassive() {
				return passivemode;
			}

			//will not call value_changed
			//external use only, use setvalue_smooth for internal use
			void setvalue(T_ value) {
				if(value<minimum) value=minimum;
				if(value>maximum) value=maximum;

				value=T_(Round(float(value)/steps)*steps);

				if(this->value!=value) {
					this->value=value;
					if(smooth.issmooth()) {
						smooth.controller.ResetProgress();

						int duration = int(1000*((value-smoothvalue)/(maximum-minimum))*(100/smooth.speed));
						duration=abs(duration);

						smooth.stepvalue=(value-smoothvalue)/duration;
						smooth.sourcevalue=smoothvalue;
						smooth.targetvalue=value;


						smooth.controller.Play();
					}
					else {
						smoothvalue=floattype(value);
					}
					Draw();
				}
			}
			//will not call value_changed, caller is responsible for it
			//all internal value changes should use this
			void setvalue_smooth(T_ value) {
				if(!smooth.value) {
					setvalue((value));
					return;
				}
				if(value<minimum) value=minimum;
				if(value>maximum) value=maximum;

				value=T_(Round(float(value)/steps)*steps);

				if(smooth.targetvalue!=value) {
					smooth.controller.ResetProgress();

					int duration = int(1000*((value-smoothvalue)/(maximum-minimum))*(100/smooth.speed));
					duration=abs(duration);

					smooth.stepvalue=(value-smoothvalue)/duration;
					smooth.sourcevalue=smoothvalue;
					smooth.targetvalue=value;


					smooth.controller.Play();

					Draw();
				}
			}
			T_ getvalue() const {
				return value;
			}

			void setmax(T_ m) {
				if(value>m) value=m;
				if(smoothvalue>m) smoothvalue=floattype(m);
				if(smooth.targetvalue>m) smooth.targetvalue=m;

				if(maximum!=m) {
					maximum=m;

					Draw();
				}
			}
			T_ getmax() const {
				return maximum;
			}

			void setmin(T_ m) {
				if(value<m) value=m;
				if(smoothvalue<m) smoothvalue=floattype(m);
				if(smooth.targetvalue<m) smooth.targetvalue=m;

				if(minimum!=m) {
					minimum=m;

					Draw();
				}
			}
			T_ getmin() const {
				return minimum;
			}

			void setorientation(Blueprint::OrientationType orientation) {
				this->orientation=orientation;
				Draw();
			}
			Blueprint::OrientationType getorientation() const {
				return orientation;
			}
			bool ishorizontal() {
				return (orientation==Blueprint::Top || orientation==Blueprint::Bottom || orientation==Blueprint::Horizontal);
			}
			bool isvertical() {
				return (orientation==Blueprint::Left || orientation==Blueprint::Right || orientation==Blueprint::Vertical);
			}

			void setsmoothingspeed(float speed) {
				smooth.speed=speed;
			}
			float getsmoothingspeed() const {
				return smooth.speed;
			}

			void setshowticks(bool ticks) {
				if(ticks!=markers.ticks) {
					markers.ticks=ticks;
					Draw();
				}
			}
			bool getshowticks() const {
				return markers.ticks;
			}

			void settickdistance(floattype distance) {
				if(markers.tickdistance!=distance) {
					markers.tickdistance=distance;
					Draw();
				}
			}
			floattype gettickdistance() const {
				return markers.tickdistance;
			}

			void setshownumbers(bool numbers) {
				if(numbers!=markers.numbers) {
					markers.numbers=numbers;
					Draw();
				}
			}
			bool getshownumbers() const {
				return markers.numbers;
			}

			void setnumberdistance(int distance) {
				if(markers.numberdistance!=distance) {
					markers.numberdistance=distance;
					Draw();
				}
			}
			int getnumberdistance() const {
				return markers.numberdistance;
			}

			void setsmallchange(T_ smallchange) {
				this->smallchange=smallchange;
			}
			T_ getsmallchange() const {
				return smallchange;
			}

			void setlargechange(T_ largechange) {
				this->largechange=largechange;
			}
			T_ getlargechange() const {
				return largechange;
			}

			void setsteps(T_ steps) {
				if(steps==0)
					steps=1;

				this->steps=steps;

				T_ v=value;
				setvalue(v);
				if(value!=v)
					value_changed();
			}
			T_ getsteps() const {
				return steps;
			}

			void setnumberformat(numberformat value) {
				if(format!=value) {
					format=value;

					Draw();
				}
			}
			numberformat getnumberformat() const {
				return format;
			}

			void setautosize(const AutosizeModes::Type &value) {
				autosize=value;

				if(Container)
					Container->WidgetBoundsChanged();

				Draw;
			}
			AutosizeModes::Type getautosize() const {
				return autosize;
			}


			bool symbol_mouse(input::mouse::Event::Type event, utils::Point location, int amount) {
				if(passivemode)
					return true;

				if(!actions.symbol_drag)
					return true;

				if(event==input::mouse::Event::Left_Down) {
					if(!IsEnabled()) return true;

					symbol_mdownpos=location;
					symbol_mdown=true;

					setstyle(Blueprint::Down);

					Focus();
				}
				else if(event==input::mouse::Event::Move && symbol_mdown) {
					if(!IsEnabled()) return true;

					if(location!=symbol_mdownpos) {
						T_ v;
						if(ishorizontal()) {
							v=(T_)Round((location.x-symbol_mdownpos.x+symbollayer.BoundingBox.Left-symbol_zero)*valueperpixel+minimum);
						}
						else {
							v=(T_)Round((location.y-symbol_mdownpos.y+symbollayer.BoundingBox.Top-symbol_zero)*valueperpixel+minimum);
						}

						setvalue(v);
						value_changed();

						Draw();
					}
				}
				else if(event==input::mouse::Event::Left_Up) {
					if(!IsEnabled()) return true;

					if(symbol_mover)
						setstyle(Blueprint::Hover);
					else
						setstyle(Blueprint::Normal);

					symbol_mdown=false;
				}
				else if(event==input::mouse::Event::Over) {
					symbol_mover=true;

					if(!IsEnabled()) return true;

					if(!symbol_mdown)
						setstyle(Blueprint::Hover);
				}
				else if(event==input::mouse::Event::Out) {
					symbol_mover=false;

					if(!IsEnabled()) return true;

					if(!symbol_mdown)
						setstyle(Blueprint::Normal);
				}

				return true;
			}

		private:
			animation::AnimationController &getanimation(Blueprint::TransitionType transition) {
				if(transition==Blueprint::FocusTransition)
					return focus_anim;
				else
					return style_anim;
			}

			void focus_anim_finished();
			void style_anim_finished();

			utils::Point symbol_mdownpos;
			bool symbol_mdown;
			bool symbol_mover;
			int symbol_zero;


			int value_start;//pixel
			float valueperpixel;

			bool valuehover;
			T_ hovervalue;

			AutosizeModes::Type autosize;

			numberformat format;

			T_ value;
			typename floattype smoothvalue;
			T_ minimum;
			T_ maximum;

			T_ smallchange;
			T_ largechange;

			T_ steps;

			bool unprepared;

			Blueprint::OrientationType  orientation;

			Blueprint::FocusMode focus;
			Blueprint::StyleMode style;

			Blueprint::FocusType next_focus;
			Blueprint::StyleType next_style;

			animation::AnimationController focus_anim;
			animation::AnimationController style_anim;
			animation::AnimationTimer	   idle_anim;


			Placeholder *symbolp;
			Placeholder *tickp;
			Placeholder *textp;
			BorderData *border;
			BorderData *overlay;
			BorderData *rule;
			BorderData *indicator;
			BorderData *ruleoverlay;
			animation::RectangularGraphic2DAnimation *symbol;
			animation::RectangularGraphic2DAnimation *tick;
			animation::RectangularGraphic2DAnimation *tick_hover;
			Font *font;
			Font *font_hover;


			utils::Bounds rule_region, ticks_region, numbers_region;

			bool focus_anim_loop;
			bool style_anim_loop;

			const Blueprint *bp;

			class cdisplay {
			public:
				cdisplay() : symbol(true),
					rule(true),
					indicator(false),
					buttons(true),
					value(false)
				{ }
				

				bool 
					symbol,
					rule,
					indicator,
					buttons,
					value
				;
			} display;

			class cmarkers {
			public:
				cmarkers() : ticks(false), numbers(false), tickdistance(10), numberdistance(2)
				{ }

				bool ticks;
				bool numbers;
				floattype tickdistance;
				int numberdistance;
			} markers;

			class csmooth {
			public:
				csmooth() : indicator(true), symbol(true), value(false), speed(100), stepvalue(0)
				{ }

				bool symbol;
				bool indicator;
				bool value;
				float speed; //percent/sec
				animation::AnimationController controller;
				floattype stepvalue;
				floattype sourcevalue;
				T_ targetvalue;

				bool issmooth() { return symbol || indicator || value; }
			} smooth;

			class cactions {
			public:
				cactions() : symbol_drag(true), rule_action(Goto), tick_click(false), number_click(false)
				{ }

				bool symbol_drag;
				RuleAction rule_action;
				bool tick_click;
				bool number_click;
			} actions;

			bool passivemode;
			Alignment::Type valuelocation;

			graphics::Colorizable2DLayer innerlayer;
			graphics::Colorizable2DLayer overlayer;
			WidgetLayer symbollayer;

			std::map<animation::RectangularGraphic2DSequenceProvider*, animation::RectangularGraphic2DAnimation*> ImageCache;
			std::map<BorderDataResource*, BorderData*> BorderCache;

			virtual animation::ProgressResult::Type Progress() {
				if(!smooth.controller.IsPaused()) {
					smoothvalue=smooth.sourcevalue+smooth.stepvalue*smooth.controller.GetProgress();

					if(smooth.stepvalue>0 && smoothvalue>=smooth.targetvalue) {
						smooth.controller.Pause();
						smoothvalue=floattype(smooth.targetvalue);
					}
					else if(smooth.stepvalue<0 && smoothvalue<=smooth.targetvalue) {
						smooth.controller.Pause();
						smoothvalue=floattype(smooth.targetvalue);
					}

					if(smooth.value) {
						if(value!=smoothvalue) {
							value=T_(Round(smoothvalue));

							value_changed();
						}
					}

					Draw();
				}

				return animation::ProgressResult::None;
			}
		};


		template<class T_, class floattype>
		void gge::widgets::slider::Base<T_, floattype>::style_anim_finished() {
			if(style.to==Blueprint::Style_None && style_anim_loop)
				style_anim.ResetProgress();

			if(style.to==Blueprint::Style_None && next_style==Blueprint::Style_None)
				return;

			style_anim.Pause();

			if(style.to!=Blueprint::Style_None) {
				style.from=style.to;
				style.to=Blueprint::Style_None;
			}

			if(next_style!=Blueprint::Style_None) {
				Blueprint::StyleType v=next_style, t=Blueprint::Style_None;

				if(style.from!=Blueprint::Normal) {
					if(next_style==Blueprint::Disabled) {
						v=Blueprint::Normal;
						t=next_style;
					}
				}
				else if(style.from==Blueprint::Disabled) {
					if(next_style!=Blueprint::Normal) {
						v=Blueprint::Normal;
						t=next_style;
					}
				}

				next_style=t;
				setstyle(v);
			}

			Draw();
		}


		template<class T_, class floattype>
		void gge::widgets::slider::Base<T_, floattype>::focus_anim_finished() {
			if(focus.to==Blueprint::Focus_None && focus_anim_loop)
				focus_anim.ResetProgress();

			if(focus.to==Blueprint::Focus_None)
				return;

			focus_anim.Pause();
			focus.from=focus.to;
			focus.to=Blueprint::Focus_None;

			if(next_focus!=Blueprint::Focus_None) {
				Blueprint::FocusType v=next_focus;
				next_focus=Blueprint::Focus_None;

				setfocus(v);
			}

			Draw();
		}

		template<class T_, class floattype>
		void Base<T_, floattype>::setfocus(Blueprint::FocusType type) {
			if(!bp) {
				focus.from=type;
				return;
			}

			if(focus.from!=type || focus.to!=Blueprint::Focus_None) {
				Blueprint::AnimationInfo info;
				if(focus.from==type)
					info=bp->HasFocusAnimation(orientation, Blueprint::FocusMode(focus.to, type));
				else
					info=bp->HasFocusAnimation(orientation, Blueprint::FocusMode(focus.from, type));

				if(info) {
					if(focus.from==type) {
						if(info.direction==focus_anim.GetSpeed()) {
							next_focus=type;
							return;
						}
						else
							focus.from=focus.to;
					}
					else if(focus.to==type) {
						next_focus=Blueprint::Focus_None;

						return;
					}

					if(info.duration==-2) {
						info.duration=-1;
						focus_anim_loop=true;
					}
					else if(info.duration==0) {
						focus.from=type;
						Draw();

						return;
					}
					else
						focus_anim_loop=false;

					if(info.direction==Blueprint::Forward)
						focus_anim.SetPauseAt(info.duration);
					else
						focus_anim.SetPauseAt(0);

					focus_anim.SetSpeed((float)info.direction);
					focus_anim.ClearFinished();
					focus_anim.Resume();

					focus.to=type;
				}
				else
					focus.from=type;
			}

			Draw();
		}

		template<class T_, class floattype>
		void gge::widgets::slider::Base<T_, floattype>::setstyle(Blueprint::StyleType type) {
			if(!bp) {
				style.from=type;
				return;
			}

			if(style.from!=type || style.to!=Blueprint::Style_None) {
				if(style.from!=type && style.to!=type && style.to!=Blueprint::Style_None) {
					next_style=type;
					return;
				}
				if(style.from==Blueprint::Disabled && type!=Blueprint::Normal || style.from!=Blueprint::Normal && type==Blueprint::Disabled) {
					next_style=type;
					type=Blueprint::Normal;
				}
				Blueprint::AnimationInfo info;
				if(style.from==type)
					info=bp->HasStyleAnimation(orientation, focus,Blueprint::StyleMode(style.to, type));
				else
					info=bp->HasStyleAnimation(orientation, focus,Blueprint::StyleMode(style.from, type));

				if(info) {
					if(style.from==type) {
						if(info.direction==style_anim.GetSpeed()) {
							next_style=type;
							return;
						}
						else
							style.from=style.to;
					}
					else if(style.to==type) {
						next_style=Blueprint::Style_None;

						return;
					}

					if(info.duration==-2) {
						info.duration=-1;
						style_anim_loop=true;
					}
					else
						style_anim_loop=false;

					if(info.direction==Blueprint::Forward)
						style_anim.SetPauseAt(info.duration);
					else
						style_anim.SetPauseAt(0);

					style_anim.SetSpeed((float)info.direction);
					style_anim.ClearFinished();
					style_anim.Resume();

					style.to=type;
				}
				else {
					style.from=type;
					style.to=Blueprint::Style_None;

					if(next_style!=Blueprint::Style_None) {
						Blueprint::StyleType v=next_style, t=Blueprint::Style_None;

						if(style.from!=Blueprint::Normal) {
							if(next_style==Blueprint::Disabled) {
								v=Blueprint::Normal;
								t=next_style;
							}
						}
						else if(style.from==Blueprint::Disabled) {
							if(next_style!=Blueprint::Normal) {
								v=Blueprint::Normal;
								t=next_style;
							}
						}

						next_style=t;
						setstyle(v);
					}
				}
			}

			Draw();
		}


		template<class T_, class floattype>
		utils::Size gge::widgets::slider::Base<T_, floattype>::GetSize() {

			if(!bp)
				return WidgetBase::size;

			prepare();

			Size size=WidgetBase::size;
			if(autosize!=AutosizeModes::None) {
				if(ishorizontal()) {
					if(rule && display.rule) {
						int s=0;
						if(border) {
							s=border->BorderWidth.TotalX()+border->Padding.TotalX();
						}
						size.Width=rule->CalculateWidth(WidgetBase::size.Width-s)+s;
					}

					int h=0;

					if(border) {
						h+=border->BorderWidth.TotalY()+border->Padding.TotalY();
					}

					if(rule && display.rule) {
						h+=rule->CalculateHeight()+rule->Margins.TotalY();
					}

					if(tick && markers.ticks) {
						if(tickp)
							h+=tickp->GetSize(tick->GetSize(), tick->GetSize()).Height+tickp->Margins.TotalY();
						else
							h+=tick->GetHeight();
					}

					if(font && markers.numbers) {
						if(textp)
							h+=textp->GetSize(Size(0,font->FontHeight()), Size(0,font->FontHeight())).Height+textp->Margins.TotalY();
						else
							h+=font->FontHeight();
					}

					//value display
					//if()

					size.Height=h;
				}
				else {
					if(rule && display.rule) {
						int s=0;
						if(border) {
							s=border->BorderWidth.TotalY()+border->Padding.TotalY();
						}
						size.Height=rule->CalculateHeight(WidgetBase::size.Width-s)+s;
					}

					int w=0;

					if(border) {
						w+=border->BorderWidth.TotalX()+border->Padding.TotalX();
					}

					if(rule && display.rule) {
						w+=rule->CalculateWidth()+rule->Margins.TotalX();
					}

					if(tick && markers.ticks) {
						if(tickp)
							w+=tickp->GetSize(tick->GetSize(), tick->GetSize()).Width+tickp->Margins.TotalX();
						else
							w+=tick->GetWidth();
					}

					if(font && markers.numbers) {
						std::stringstream ss;
						if(format.hex)
							ss<<std::hex;
						if(format.decimals>-1)
							ss<<std::setprecision(format.decimals);
						ss.str(string());
						ss<<format.prefix;
						ss<<maximum;
						ss<<format.units;

						if(textp) {
							w+=textp->GetSize(Size(0,font->TextWidth(ss.str())), Size(0,font->TextWidth(ss.str()))).Height+textp->Margins.TotalY();
						}
						else {
							w+=font->TextWidth(ss.str());
						}
					}

					//value display
					//if()

					size.Width=w;
				}
			}

			if(autosize==AutosizeModes::GrowOnly) {
				if(size.Width<WidgetBase::size.Width)
					size.Width=WidgetBase::size.Width;
				if(size.Height<WidgetBase::size.Height)
					size.Height=WidgetBase::size.Height;
			}

			return size;
		}

		template<class T_, class floattype>
		void gge::widgets::slider::Base<T_, floattype>::prepare() {
			if(!unprepared)
				return;

			if(!bp)
				return;

			Blueprint::Group **groups = new Blueprint::Group *[3];
			if(ispassive()) {
				groups[0]=bp->GetOrientationBaseGroup(orientation);
				groups[1]=NULL;
				groups[2]=NULL;
			} 
			else {
				bp->GetAlternatives(groups, orientation, focus);
			}

			BorderDataResource		*bprovider;
			animation::RectangularGraphic2DSequenceProvider *iprovider;

			symbolp=NULL;
			tickp=NULL;
			textp=NULL;
			border=NULL;
			overlay=NULL;
			rule=NULL;
			indicator=NULL;
			ruleoverlay=NULL;
			symbol=NULL;
			tick=NULL;
			tick_hover=NULL;
			font=NULL;
			font_hover=NULL;


			Blueprint::TransitionType transition;

			symbolp=bp->GetSymbolPlace(groups, style, transition);
			tickp=bp->GetTickmarkPlace(groups, Blueprint::Normal, transition);
			textp=bp->GetTextPlace(groups, Blueprint::Normal, transition);
			font=bp->GetFont(groups, Blueprint::Normal, transition);

			font_hover=bp->GetFont(groups, Blueprint::Hover, transition);


			bprovider=bp->GetOuterBorder(groups, style, transition);
			if(bprovider) {
				if(BorderCache[bprovider])
					border=BorderCache[bprovider];
				else {
					border=&bprovider->CreateResizableObject();
					BorderCache[bprovider]=border;
				}

				border->SetController(idle_anim);
			}

			bprovider=bp->GetOverlay(groups, style, transition);
			if(bprovider) {
				if(BorderCache[bprovider])
					overlay=BorderCache[bprovider];
				else {
					overlay=&bprovider->CreateResizableObject();
					BorderCache[bprovider]=overlay;
				}

				overlay->SetController(idle_anim);
			}

			//!detect ruler mouse over
			bprovider=bp->GetRuler(groups, Blueprint::Normal, transition);
			if(bprovider) {
				if(BorderCache[bprovider])
					rule=BorderCache[bprovider];
				else {
					rule=&bprovider->CreateResizableObject();
					BorderCache[bprovider]=rule;
				}

				rule->SetController(idle_anim);
			}

			bprovider=bp->GetIndicator(groups, Blueprint::Normal, transition);
			if(bprovider) {
				if(BorderCache[bprovider])
					indicator=BorderCache[bprovider];
				else {
					indicator=&bprovider->CreateResizableObject();
					BorderCache[bprovider]=indicator;
				}

				indicator->SetController(idle_anim);
			}

			bprovider=bp->GetRulerOverlay(groups, Blueprint::Normal, transition);
			if(bprovider) {
				if(BorderCache[bprovider])
					ruleoverlay=BorderCache[bprovider];
				else {
					ruleoverlay=&bprovider->CreateResizableObject();
					BorderCache[bprovider]=ruleoverlay;
				}

				ruleoverlay->SetController(idle_anim);
			}

			iprovider=bp->GetSymbol(groups, style, transition);
			if(iprovider) {
				if(ImageCache[iprovider])
					symbol=ImageCache[iprovider];
				else {
					symbol=&iprovider->CreateAnimation();
					ImageCache[iprovider]=symbol;
				}

				symbol->SetController(getanimation(transition));
			}

			iprovider=bp->GetTickmark(groups, Blueprint::Normal, transition);
			if(iprovider) {
				if(ImageCache[iprovider])
					tick=ImageCache[iprovider];
				else {
					tick=&iprovider->CreateAnimation();
					ImageCache[iprovider]=tick;
				}

				tick->SetController(idle_anim);
			}

			iprovider=bp->GetTickmark(groups, Blueprint::Hover, transition);
			if(iprovider) {
				if(ImageCache[iprovider])
					tick_hover=ImageCache[iprovider];
				else {
					tick_hover=&iprovider->CreateAnimation();
					ImageCache[iprovider]=tick_hover;
				}

				tick_hover->SetController(idle_anim);
			}
		}

		template<class T_, class floattype>
		void Base<T_, floattype>::draw() {
			using namespace gge::utils;
			using namespace gge::animation;
			using namespace gge::resource;

			if(!BaseLayer)
				return;

			BaseLayer->Clear();
			innerlayer.Clear();
			overlayer.Clear();
			symbollayer.Clear();

			if(!bp)
				return;


			prepare();

			BaseLayer->BoundingBox.SetSize(GetSize());
			Bounds outer=Bounds(Point(0,0),BaseLayer->BoundingBox.GetSize());
			Bounds inner=outer;
			int	   distance=0;

			if(border) {
				inner=border->ContentBounds(outer);
			}
			if(symbol && symbolp && display.symbol) {
				if(symbolp->Margins.Top<0)
					inner.Top-=symbolp->Margins.Top;
			}
			innerlayer.BoundingBox=inner;
			overlayer.BoundingBox=outer;


			//DRAWING BORDER
			if(border) {
				border->DrawIn(BaseLayer, outer);
			}


			//CALCULATE VALUE RANGE
			if(ishorizontal()) {
				if(rule) {
					value_start=inner.Left+rule->Margins.Left+rule->BorderWidth.Left;

					valueperpixel=floattype(maximum-minimum)/(inner.Width()-(rule->Margins.TotalX()+rule->BorderWidth.TotalX()));
				}
				else {
					value_start=inner.Left;

					valueperpixel=floattype(maximum-minimum)/inner.Width();
				}
			}
			else {
				if(rule) {
					value_start=inner.Top+rule->Margins.Top+rule->BorderWidth.Top;

					valueperpixel=floattype(maximum-minimum)/inner.Height()-(rule->Margins.TotalY()+rule->BorderWidth.TotalY());
				}
				else {
					value_start=inner.Top;

					valueperpixel=floattype(maximum-minimum)/inner.Height();
				}
			}


			//check and draw value



			//DRAWING RULE
			SizeController2D szc;
			if(orientation==Blueprint::Top || orientation==Blueprint::Left) {
				szc=SizeController2D::SingleBottomRight;
			}
			else {
				szc=SizeController2D::SingleTopLeft;
			}
			if(display.rule && rule) {
				if(rule) {
					rule->DrawIn(BaseLayer, szc, inner-rule->Margins);

					switch(orientation) {
						case Blueprint::Bottom:
						case Blueprint::Top:
						case Blueprint::Horizontal:
							rule_region=Bounds(inner.Left, distance, inner.Right, distance);
							distance+=rule->Margins.TotalY()+rule->CalculateHeight(szc,(inner-rule->Margins).Height());
							rule_region.Bottom=distance;
							break;
						case Blueprint::Right:
						case Blueprint::Left:
						case Blueprint::Vertical:
							rule_region=Bounds(distance, inner.Top, distance, inner.Top);
							distance+=rule->Margins.TotalX()+rule->CalculateHeight(szc,(inner-rule->Margins).Width());
							rule_region.Right=distance;
							break;
					}
				}

				//!!check and draw value

				if(display.indicator && indicator) {
					Bounds b = inner-(rule->BorderWidth+indicator->Margins);
					b=b+indicator->BorderWidth;
					if(ishorizontal()) {
						b.SetWidth(int((b.Width()-indicator->BorderWidth.TotalX())*floattype(smoothvalue-minimum)/maximum)+indicator->BorderWidth.TotalX());
					}
					else {
						b.SetHeight(int((b.Height()-indicator->BorderWidth.TotalY())*floattype(smoothvalue-minimum)/maximum)+indicator->BorderWidth.TotalY());
					}

					BaseLayer->SetDrawMode(graphics::BasicSurface::AlphaOnly);
					indicator->DrawIn(BaseLayer, szc, b);
					BaseLayer->SetDrawMode(graphics::BasicSurface::UseDestinationAlpha);
					indicator->DrawIn(BaseLayer, szc, b);
					BaseLayer->SetDrawMode(graphics::BasicSurface::Normal);
				}
			}
			else {

				//!!check and draw value

				if(display.indicator) {
					Bounds b = inner;
					b=b+indicator->BorderWidth-indicator->Margins;
					if(ishorizontal()) {
						b.SetWidth(int((b.Width()-indicator->BorderWidth.TotalX())*(smoothvalue-minimum)/maximum)+indicator->BorderWidth.TotalX());
					}
					else {
						b.SetHeight(int((b.Height()-indicator->BorderWidth.TotalY())*(smoothvalue-minimum)/maximum)+indicator->BorderWidth.TotalY());
					}

					BaseLayer->SetDrawMode(graphics::BasicSurface::AlphaOnly);
					indicator->DrawIn(BaseLayer, szc, b);
					BaseLayer->SetDrawMode(graphics::BasicSurface::UseDestinationAlpha);
					indicator->DrawIn(BaseLayer, szc, b);
					BaseLayer->SetDrawMode(graphics::BasicSurface::Normal);
				}
			}

			//DRAWING TICKS
			if(markers.ticks && markers.tickdistance && tick) {
				Point p(0,0);
				Size size=tick->GetSize();
				Alignment::Type align;
				if(orientation==Blueprint::Bottom || orientation==Blueprint::Horizontal)
					align=Alignment::Top_Center;
				else if(orientation==Blueprint::Top)
					align=Alignment::Bottom_Center;
				else if(orientation==Blueprint::Left)
					align=Alignment::Middle_Right;
				else
					align=Alignment::Middle_Left;

				if(tickp) {
					p=Point(tickp->Margins.Left-tickp->Margins.Right, tickp->Margins.Top);
					size=tickp->GetSize(tick->GetSize(), inner.GetSize());
					align=tickp->Align;
				}

				Point location=p-Alignment::CalculateLocation(align, Bounds(Point(0,0),size), Size(0,0));

				if(ishorizontal()) {
					location.x+=value_start;
					location.y+=distance;

					ticks_region=Bounds(inner.Left, distance, inner.Right, distance+size.Height);
				}
				else {
					location.y+=value_start;
					location.x+=distance;

					ticks_region=Bounds(distance, inner.Top, distance+size.Width, inner.Bottom);
				}

				floattype mn=ceil(minimum/markers.tickdistance)*markers.tickdistance;
				for(floattype v=mn;v<=maximum;v+=markers.tickdistance) {
					animation::RectangularGraphic2DAnimation *ttick=tick;
					if((v==smoothvalue && smooth.symbol) || (v==value) || (v==hovervalue && valuehover)) {
						ttick=tick_hover;
					}
					if(ishorizontal()) {
						ttick->Draw(innerlayer, Point(location.x+int((v-minimum)/valueperpixel), location.y)-innerlayer.BoundingBox.TopLeft());
					}
					else {
						ttick->Draw(BaseLayer, Point(location.x, location.y+int((v-minimum)/valueperpixel))-innerlayer.BoundingBox.TopLeft());
					}
				}

				if(tickp) {
					if(ishorizontal()) {
						distance+=tickp->Margins.TotalY()+size.Height;
						ticks_region.Bottom+=tickp->Margins.Bottom;
					}
					else {
						distance+=tickp->Margins.TotalX()+size.Width;
						ticks_region.Right+=tickp->Margins.Right;
					}
				}
				else {

					if(ishorizontal())
						distance+=size.Height;
					else
						distance+=size.Width;
				}
			}

			//!!numbers
			if(markers.numbers && markers.tickdistance && markers.numberdistance && font) {
				Point p(0,0);
				Size size=Size(0, font->FontHeight());
				Alignment::Type align;
				if(orientation==Blueprint::Bottom || orientation==Blueprint::Horizontal)
					align=Alignment::Top_Center;
				else if(orientation==Blueprint::Top)
					align=Alignment::Bottom_Center;
				else if(orientation==Blueprint::Left)
					align=Alignment::Middle_Right;
				else
					align=Alignment::Middle_Left;

				if(textp) {
					p=Point(textp->Margins.Left-textp->Margins.Right, textp->Margins.Top-textp->Margins.Bottom);
					size=textp->GetSize(size, inner.GetSize());
					align=textp->Align;
				}

				Point location=p-Alignment::CalculateLocation(align, Bounds(Point(0,0),size), Size(0,0));

				if(ishorizontal()) {
					location.x+=value_start;
					location.y+=distance;

					numbers_region=Bounds(inner.Left, distance, inner.Right, distance+size.Height);
				}
				else {
					location.y+=value_start;
					location.x+=distance;

					numbers_region=Bounds(distance, inner.Top, distance+size.Width, inner.Bottom);
				}

				stringstream ss;
				if(format.hex)
					ss<<std::hex;
				if(format.decimals>-1)
					ss<<std::setprecision(format.decimals);

				floattype mn=ceil(minimum/(markers.tickdistance*markers.numberdistance))*markers.tickdistance*markers.numberdistance;

				for(floattype v=(floattype)mn;v<=maximum;v+=markers.tickdistance*markers.numberdistance) {
					Font *tfont=font;
					if((v==smoothvalue && smooth.symbol) || (v==value) || (v==hovervalue && valuehover)) {
						tfont=font_hover;
					}
					ss.str(string());
					ss<<format.prefix;
					ss<<v;
					ss<<format.units;

					TextAlignment::Type talign=TextAlignment::GetHorizontal(align);

					int toffset=0; //temp offset

					if(talign==TextAlignment::Center) {
						if(v==mn) {
							if(font->TextWidth(ss.str())/2>value_start) {
								if(rule && ishorizontal())
									toffset=-(rule->BorderWidth.Left+rule->Margins.Left);
								else if(rule && isvertical())
									toffset=-(rule->BorderWidth.Top+rule->Margins.Top);

								talign=TextAlignment::Left;
							}
						}
						if(v==maximum) {
							if(font->TextWidth(ss.str())/2>inner.Right-(value_start+(maximum-minimum)/valueperpixel-inner.Left)) {
								if(rule && ishorizontal())
									toffset=+(rule->BorderWidth.Right+rule->Margins.Right);
								else if(rule && isvertical())
									toffset=+(rule->BorderWidth.Bottom+rule->Margins.Bottom);

								talign=TextAlignment::Right;
							}
						}
					}

					if(ishorizontal()) {
						tfont->Print(
							innerlayer, Point(location.x+int((v-minimum)/valueperpixel)+toffset, location.y)-innerlayer.BoundingBox.TopLeft(),
							0, ss.str(), talign
						);
					}
					else {
						tfont->Print(
							innerlayer, Point(location.x, location.y+int((v-minimum)/valueperpixel)+toffset)-innerlayer.BoundingBox.TopLeft(),
							0, ss.str(), talign
						);
					}
				}

				if(textp) {
					if(ishorizontal()) {
						distance+=textp->Margins.TotalY()+size.Height;
						numbers_region.Bottom+=textp->Margins.Bottom;
					}
					else {
						distance+=textp->Margins.TotalX()+size.Width;
						numbers_region.Right+=textp->Margins.Right;
					}
				}
				else {
					if(ishorizontal())
						distance+=size.Height;
					else
						distance+=size.Width;
				}
			}


			//!!check and draw value


			//DRAWING RULE OVERLAY
			if(ruleoverlay && display.rule) {
				ruleoverlay->DrawIn(overlayer, inner-ruleoverlay->Margins);
			}



			//DRAWING SYMBOL
			if(symbol && display.symbol) {
				Point p(0,0);
				Size size=symbol->GetSize();
				Alignment::Type align;
				if(orientation==Blueprint::Bottom || orientation==Blueprint::Horizontal)
					align=Alignment::Top_Center;
				else if(orientation==Blueprint::Top)
					align=Alignment::Bottom_Center;
				else if(orientation==Blueprint::Left)
					align=Alignment::Middle_Right;
				else
					align=Alignment::Middle_Left;

				if(symbolp) { 
					p=Point(symbolp->Margins.Left-symbolp->Margins.Right, symbolp->Margins.Top-symbolp->Margins.Bottom);

					size=symbolp->GetSize(symbol->GetSize(), inner.GetSize());
					align=symbolp->Align;
				}

				Point location=p-Alignment::CalculateLocation(align, Bounds(Point(0,0),size), Size(0,0));

				if(ishorizontal()) {
					location.x+=value_start;

					symbol_zero=location.x;

					if(smooth.symbol)
						location.x+=int((smoothvalue-minimum)/valueperpixel);
					else
						location.x+=int((value-minimum)/valueperpixel);
				}
				else {
					location.y+=value_start;

					symbol_zero=location.y;

					if(smooth.symbol)
						location.y+=int((smoothvalue-minimum)/valueperpixel);
					else
						location.y+=int((value-minimum)/valueperpixel);
				}
				symbollayer.Move(location);
				symbollayer.Resize(size);
				symbol->Draw(symbollayer,0,0);
			}

			//DRAWING OVERLAY
			if(overlay) {
				overlay->DrawIn(overlayer, outer);
			}

		} //end of method





	}

} }
