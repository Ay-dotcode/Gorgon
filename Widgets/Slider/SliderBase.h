#pragma once

#include "SliderBluePrint.h"
#include "..\Base\Widget.h"
#include <map>
#include "..\Main.h"
#include "..\Basic\PetContainer.h"
#include "..\Button.h"
#include "..\..\Engine\Graphics.h"
#include "..\Interfaces\ISlider.h"

#ifndef LARGE_KEY_TIMEOUT_MULT
#	define LARGE_KEY_TIMEOUT_MULT	2
#endif


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

		//if you need to work with big ints (>16M), use long or unsigned, or manually set floattype to double
		//because the default float type for int is float and smooth sliding may not work for you.
		// RETAIN ABOVE COMMENT IN ALL CHILD CLASSES
		//Template parameter must allow +, -, / and (float) operators, for enums T_=int should be used
		// and enum should be mapped manually to ints
		template<class T_, class floattype=typename floattype<T_>::Type>
		class Base : public WidgetBase, public ISliderType, private animation::Base {
		public:


			Base(T_ value=T_(0), T_ minimum=T_(0), T_ maximum=T_(100), T_ steps=std::numeric_limits<T_>::epsilon(), T_ smallchange=T_(1), T_ largechange=T_(10)) : passivemode(false),
				orientation(Blueprint::Vertical), next_style(Blueprint::Style_None),
				value(value), minimum(minimum), maximum(maximum), steps(steps), autosize(AutosizeModes::Autosize),
				smallchange(smallchange), largechange(largechange), unprepared(false), buttonlayer(*this),
				rule_region(0,0,0,0), ticks_region(0,0,0,0), numbers_region(0,0,0,0), bp(NULL), rule_over(false),
				smoothvalue(floattype(value)), symbol_mdown(false), symbol_mover(false), value_over(false), golarge(false),
				upbutton(NULL), downbutton(NULL), key_repeat_timeout(200), goingup(false), goingdown(false), 
				indst_value(minimum),indst_smoothvalue(floattype(minimum)), cangetfocus(true),
				blueprintmodified(false)
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
				smooth.indst_controller.Pause();
				key_repeat.Pause();
				SetController(smooth.controller);

				symbollayer.MouseCallback.Set(*this, &Base::symbol_mouse);

				innerlayer.ClippingEnabled=true;

				smooth.targetvalue=value;

				WR.LoadedEvent.Register(this, &Base::wr_loaded);
			}

			using WidgetBase::SetBlueprint;

			virtual void SetBlueprint(const widgets::Blueprint &bp)  {
				blueprintmodified=true;
				setblueprint(bp);
			}

			virtual void setblueprint(const widgets::Blueprint &bp)  {
				this->bp=static_cast<const Blueprint*>(&bp);
				if(WidgetBase::size.Width==0)
					Resize(this->bp->DefaultSize);

				for(auto i=BorderCache.begin();i!=BorderCache.end();++i)
					if(i->second)
						i->second->DeleteAnimation();

				BorderCache.clear();

				for(auto i=ImageCache.begin();i!=ImageCache.end();++i)
					if(i->second)
						i->second->DeleteAnimation();

				ImageCache.clear();

				if(this->bp) {
					this->pointer=bp.Pointer;
				}

				setupbuttons();

				Draw();
			}
			virtual bool MouseEvent(input::mouse::Event::Type event, utils::Point location, int amount)  {
				if(passivemode) return true;

				WidgetBase::MouseEvent(event, location, amount);

				if(event==input::mouse::Event::Left_Click) {
					playsound(Blueprint::NotFocused, Blueprint::Focus_None, Blueprint::Down, Blueprint::Style_None);

					if(ishorizontal()) {
						int x;
						if(display.inverseaxis)
							x=value_end-location.x;
						else
							x=(location.x-value_start);

						if(numbers_region.isInside(location) && actions.number_click) {
							setvalue_smooth(gettextvalue(x));
							value_changed();
						}
						else if(ticks_region.isInside(location) && actions.tick_click) {
							setvalue_smooth(gettickvalue(x));
							value_changed();
						}
						else if(rule_region.isInside(location) && actions.rule_action!=NoAction) {
							setvalue_smooth(getrulevalue(x));
							value_changed();
						}
					}
					else {
						int y;
						if(display.inverseaxis)
							y=value_end-location.y;
						else
							y=(location.y-value_start);

						if(numbers_region.isInside(location) && actions.number_click) {
							setvalue_smooth(gettextvalue(y));
							value_changed();
						}
						else if(ticks_region.isInside(location) && actions.tick_click) {
							setvalue_smooth(gettickvalue(y));
							value_changed();
						}
						else if(rule_region.isInside(location) && actions.rule_action!=NoAction) {
							setvalue_smooth(getrulevalue(y));
							value_changed();
						}
					}
				}

				if(event==input::mouse::Event::Move) {
					if(ishorizontal()) {
						int x;
						if(display.inverseaxis)
							x=value_end-location.x;
						else
							x=(location.x-value_start);

						if(numbers_region.isInside(location) && actions.number_click) {
							hovervalue=gettextvalue(x);
							value_over=true;

							Draw();
						}
						else if(ticks_region.isInside(location) && actions.tick_click) {
							hovervalue=gettickvalue(x);
							value_over=true;

							Draw();
						}
						else if(rule_region.isInside(location)) {
							hovervalue=getrulevalue(x);
							value_over=true;

							Draw();
						}
					}
					else {
						int y;
						if(display.inverseaxis)
							y=value_end-location.y;
						else
							y=(location.y-value_start);


						if(numbers_region.isInside(location) && actions.number_click) {
							hovervalue=gettextvalue(y);
							value_over=true;

							Draw();
						}
						else if(ticks_region.isInside(location) && actions.tick_click) {
							hovervalue=gettickvalue(y);
							value_over=true;

							Draw();
						}
						else if(rule_region.isInside(location)) {
							hovervalue=getrulevalue(y);
							value_over=true;

							Draw();
						}
					}

					if(rule_region.isInside(location)) {
						if(!rule_over) {
							rule_over=true;
							Draw();
						}
					}
					else {
						if(rule_over) {
							rule_over=false;
							Draw();
						}
					}
				}

				if(event==input::mouse::Event::Over) {
					playsound(Blueprint::NotFocused, Blueprint::Focus_None, Blueprint::Normal, Blueprint::Hover);

				}

				if(event==input::mouse::Event::Out) {
					playsound(Blueprint::NotFocused, Blueprint::Focus_None, Blueprint::Hover, Blueprint::Normal);
					value_over=false;
					rule_over=false;

					Draw();
				}


				return WidgetBase::MouseEvent(event, location, amount);
			}

			virtual void Disable()  {
				if(IsEnabled()) {
					playsound(Blueprint::NotFocused, Blueprint::Focus_None, Blueprint::Normal, Blueprint::Disabled);
				}

				WidgetBase::Disable();
				buttonlayer.InformEnabledChange(false);

				setstyle(Blueprint::Disabled);
			}

			virtual void Enable()  {
				if(!IsEnabled()) {
					playsound(Blueprint::NotFocused, Blueprint::Focus_None, Blueprint::Disabled, Blueprint::Normal);
				}
				WidgetBase::Enable();

				buttonlayer.InformEnabledChange(true);


				if(symbol_mover)
					setstyle(Blueprint::Hover);
				else
					setstyle(Blueprint::Normal);
			}

			virtual bool Focus() {
				if(passivemode)
					return false;

				if(!cangetfocus)
					return false;

				if(!IsEnabled())
					return false;

				if(IsFocused())
					return true;

				playsound(Blueprint::NotFocused, Blueprint::Focused, Blueprint::Normal, Blueprint::Style_None);

				WidgetBase::Focus();
				setfocus(Blueprint::Focused);

				return true;
			}

			virtual void Draw() {
				unprepared=true;
				WidgetBase::Draw();
			}

			virtual utils::Size GetSize();

			virtual bool KeyboardEvent(input::keyboard::Event::Type event, input::keyboard::Key Key);

			virtual bool IsVertical() const { return isvertical(); }


		protected:
			enum RuleAction {
				NoAction,
				LargeChange,
				Goto
			};

			void playsound(Blueprint::FocusType focusfrom, Blueprint::FocusType focusto, Blueprint::StyleType from, Blueprint::StyleType to) {
				if(bp) {
					if(bp->Mapping[Blueprint::GroupMode(orientation, focusfrom, focusto)] &&
						bp->Mapping[Blueprint::GroupMode(orientation, focusfrom, focusto)]->Mapping[from][to] &&
						bp->Mapping[Blueprint::GroupMode(orientation, focusfrom, focusto)]->Mapping[from][to]->Sound) {
							WidgetBase::playsound(bp->Mapping[Blueprint::GroupMode(orientation, focusfrom, focusto)]->Mapping[from][to]->Sound);
					}
				}
			}


			bool blueprintmodified;

			virtual void wr_loaded() {

			}


			class numberformat {
			public:
				numberformat(std::string units="", std::string prefix="", int decimals=-1, bool hex=false) : units(units),
					prefix(prefix), decimals(decimals), hex(hex), separator(" - ")
				{ }

				numberformat(int decimals, std::string units="") : units(units),
					prefix(""), decimals(decimals), hex(false), separator(" - ")
				{ }

				bool operator ==(const numberformat &format) const {
					return units==format.units && prefix==format.prefix && decimals==format.decimals && hex==format.hex;
				}

				bool operator !=(const numberformat &format) const {
					return !this->operator ==(format);
				}

				std::string units;
				std::string prefix;
				std::string separator;
				int decimals;
				bool hex;
			};

			Button *upbutton, *downbutton;

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

				buttonlayer.InformEnabledChange(state);
			}

			virtual bool detach(ContainerBase *container)  {
				innerlayer.parent=NULL;
				overlayer.parent=NULL;
				symbollayer.parent=NULL;
				buttonlayer.BaseLayer.parent=NULL;
				return WidgetBase::detach(container);
			}

			virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
				WidgetBase::located(container, w, Order);
				if(BaseLayer) {
					BaseLayer->Add(innerlayer, 1);
					BaseLayer->Add(symbollayer, 0);
					BaseLayer->Add(buttonlayer, 0);
					BaseLayer->Add(overlayer, -1);
				}
				containerenabledchanged(container->IsEnabled());
			}

			virtual bool loosefocus(bool force)  {
				if(!IsFocused())
					return true;

				playsound(Blueprint::Focused, Blueprint::Focus_None, Blueprint::Normal, Blueprint::Style_None);

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
			void setupdisplay(bool symbol, bool rule, bool indicator, bool buttons, bool value, bool invertaxis=false, bool centerindicator=false, Alignment::Type valuelocation=Alignment::Middle_Center) {
				display.symbol=symbol;
				display.rule=rule;
				display.indicator=indicator;
				display.buttons=buttons;
				display.value=value;
				display.inverseaxis=invertaxis;
				display.centerindicator=centerindicator;
				this->valuelocation=valuelocation;

				Draw();
			}

			//speed is percent per second
			void setsmoothingmode(bool symbol, bool indicator, bool value, bool valuedisplay=true, float speed=100) {
				smooth.symbol=symbol;
				smooth.indicator=indicator;
				smooth.value=value;
				smooth.valuedisplay=valuedisplay;
				smooth.speed=speed;

				Draw();
			}

			//named locations disable regular ticks and numbers, but showticks and shownumbers will be used to determine
			//what will be drawn for named locations
			void setmarkers(bool ticks, bool numbers, bool namedlocations, floattype tickdistance=10, int numberdistance=2) {
				markers.ticks=ticks;
				markers.tickdistance=tickdistance;
				markers.numbers=numbers;
				markers.numberdistance=numberdistance;
				markers.namedlocations=namedlocations;

				Draw();
			}

			void setactions(bool symbol_drag, RuleAction rule_action, bool keyboard, bool tick_click=false, bool number_click=false) {
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

			void instantsetvalue(T_ value) {
				if(value<minimum) value=minimum;
				if(value>maximum) value=maximum;

				value=T_(utils::Round(float(value)/steps)*steps);
				smoothvalue=floattype(value);
				Draw();
			}
			//will not call value_changed
			//external use only, use setvalue_smooth for internal use
			void setvalue(T_ value) {
				if(value<minimum) value=minimum;
				if(value>maximum) value=maximum;

				value=T_(utils::Round(float(value)/steps)*steps);

				if(this->value!=value || smooth.targetvalue!=value) {
					this->value=value;
					if(smooth.issmooth()) {
						smooth.controller.ResetProgress();

						int duration = int(1000*((value-smoothvalue)/(maximum-minimum))*(100/smooth.speed));
						duration=abs(duration);

						if(duration>0) {
							smooth.stepvalue=(value-smoothvalue)/duration;
							smooth.sourcevalue=smoothvalue;
							smooth.targetvalue=value;

							smooth.controller.Play();
						}
						else {
							smooth.stepvalue=(value-smoothvalue)/duration;
							smooth.sourcevalue=smoothvalue;
							smooth.targetvalue=value;

							smooth.controller.Pause();
						}
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
				if(!smooth.value || smooth.issmooth()) {
					setvalue((value));

					value_changed();

					return;
				}
				if(value<minimum) value=minimum;
				if(value>maximum) value=maximum;

				value=T_(utils::Round(float(value)/steps)*steps);

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
					T_ d=indst_value-minimum;
					minimum=m;
					indst_value=minimum+d;

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
			bool ishorizontal() const {
				return (orientation==Blueprint::Top || orientation==Blueprint::Bottom || orientation==Blueprint::Horizontal);
			}
			bool isvertical() const {
				return (orientation==Blueprint::Left || orientation==Blueprint::Right || orientation==Blueprint::Vertical);
			}

			void setsmoothingspeed(float speed) {
				smooth.speed=speed;
			}
			float getsmoothingspeed() const {
				return smooth.speed;
			}

			T_ gettargetvalue() const {
				return smooth.targetvalue;
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

			void setshowvalue(bool value) {
				if(value!=display.value) {
					display.value=value;
					Draw();
				}
			}
			bool getshowvalue() const {
				return display.value;
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
				if(format!=value && markers.numbers) {
					format=value;

					Draw();
				}
			}
			numberformat getnumberformat() const {
				return format;
			}

			void setvalueformat(numberformat value) {
				if(valueformat!=value && display.value) {
					valueformat=value;

					Draw();
				}
			}
			numberformat getvalueformat() const {
				return valueformat;
			}

			void setvaluelocation(Alignment::Type value) {
				if(valuelocation!=value && display.value) {
					valuelocation=value;

					Draw();
				}
			}
			Alignment::Type getvaluelocation() const {
				return valuelocation;
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

			void setkeyrepeattimeout(const int &value) {
				key_repeat_timeout=value;
			}
			int getkeyrepeattimeout() const {
				return key_repeat_timeout;
			}

			void setaxisinverse(const bool &value) {
				if(display.inverseaxis!=value) {
					display.inverseaxis=value;

					Draw();
				}
			}
			bool getaxisinverse() const {
				return display.inverseaxis;
			}

			void setcenterindicator(const bool &value) {
				if(display.centerindicator!=value) {
					display.centerindicator=value;

					Draw();
				}
			}
			T_ getcenterindicator() const {
				return display.centerindicator;
			}

			void setindicatorstart(const T_ &value) {
				if(indst_value!=value) {
					indst_value=value;

					if(smooth.indicator && display.indicatorstart) {
						smooth.indst_controller.ResetProgress();

						int duration = int(1000*((indst_value-indst_smoothvalue)/(maximum-minimum))*(100/smooth.speed));
						duration=abs(duration);

						if(duration>0) {
							smooth.indst_stepvalue=(value-indst_smoothvalue)/duration;
							smooth.indst_sourcevalue=indst_smoothvalue;
							smooth.indst_targetvalue=indst_value;

							smooth.indst_controller.Play();
						}
					}
					else {
						indst_smoothvalue=floattype(value);
					}

					Draw();
				}
			}
			T_ getindicatorstart() const {
				return indst_value;
			}

			void setshowindicatorstart(const bool &value) {
				if(display.indicatorstart!=value) {
					display.indicatorstart=value;

					Draw();
				}
			}
			bool getshowindicatorstart() const {
				return display.indicatorstart;
			}

			void setcangetfocus(const bool &value) {
				cangetfocus=value;
				if(!cangetfocus && IsFocused())
					RemoveFocus();
			}
			bool getcangetfocus() const {
				return cangetfocus;
			}

			void smallincrease() {
				if(display.inverseaxis)
					setvalue_smooth(value-smallchange);
				else
					setvalue_smooth(value+smallchange);

				value_changed();

				Draw();
			}

			void smalldecrease() {
				if(display.inverseaxis)
					setvalue_smooth(value+smallchange);
				else
					setvalue_smooth(value-smallchange);

				value_changed();

				Draw();
			}

			void largeincrease() {
				if(display.inverseaxis)
					setvalue_smooth(value-largechange);
				else
					setvalue_smooth(value+largechange);

				value_changed();

				Draw();
			}

			void largedecrease() {
				if(display.inverseaxis)
					setvalue_smooth(value+largechange);
				else
					setvalue_smooth(value-largechange);

				value_changed();

				Draw();
			}

			void tomax() {
				if(display.inverseaxis) {
					setvalue_smooth(minimum);
				}
				else {
					setvalue_smooth(maximum);
				}

			}

			void tomin() {
				if(display.inverseaxis) {
					setvalue_smooth(maximum);
				}
				else {
					setvalue_smooth(minimum);
				}

			}

			void begin_goup() {
				goingup=true;
				goingdown=false;

				key_repeat.Play();
			}

			void begin_godown() {
				goingup=false;
				goingdown=true;

				key_repeat.Play();
			}

			void end_goup() {
				goingup=false;

				key_repeat.Pause();
			}

			void end_godown() {
				goingdown=false;

				key_repeat.Pause();
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
							int x=location.x-symbol_mdownpos.x+symbollayer.BoundingBox.Left-symbol_zero;
							if(display.inverseaxis)
								x=value_end-x;
							else
								x=x-value_start;

							v=(T_)utils::Round(x*valueperpixel+minimum);
						}
						else {
							int y=location.y-symbol_mdownpos.y+symbollayer.BoundingBox.Top-symbol_zero;
							if(display.inverseaxis)
								y=value_end-y;
							else
								y=y-value_start;

							v=(T_)utils::Round(y*valueperpixel+minimum);
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

			void upbutton_mouse(input::mouse::Event event) {
				if(event.event==input::mouse::Event::Left_Down) {
					Focus();

					if(!goingup) {
						smalldecrease();
						golarge=false;
						begin_goup();
					}
				}
				else if(event.event==input::mouse::Event::Right_Down) {
					Focus();

					if(!goingup) {
						largedecrease();
						golarge=true;
						begin_goup();
					}
				}
				else if(input::mouse::Event::isUp(event.event)) {
					end_goup();
				}
			}

			void downbutton_mouse(input::mouse::Event event) {
				Focus();

				if(event.event==input::mouse::Event::Left_Down) {
					if(!goingup) {
						smallincrease();
						golarge=false;
						begin_godown();
					}
				}
				else if(event.event==input::mouse::Event::Right_Down) {
					if(!goingup) {
						largeincrease();
						golarge=true;
						begin_godown();
					}
				}
				else if(input::mouse::Event::isUp(event.event)) {
					end_godown();
				}
			}

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
							value=T_(utils::Round(smoothvalue));

							value_changed();
						}
					}

					Draw();
				}

				if(!smooth.indst_controller.IsPaused()) {
					indst_smoothvalue=smooth.indst_sourcevalue+smooth.indst_stepvalue*smooth.indst_controller.GetProgress();

					if(smooth.indst_stepvalue>0 && indst_smoothvalue>=smooth.indst_targetvalue) {
						smooth.indst_controller.Pause();
						indst_smoothvalue=floattype(smooth.indst_targetvalue);
					}
					else if(smooth.indst_stepvalue<0 && indst_smoothvalue<=smooth.indst_targetvalue) {
						smooth.indst_controller.Pause();
						indst_smoothvalue=floattype(smooth.indst_targetvalue);
					}

					Draw();
				}

				if(golarge) {
					if(goingup) {
						while(key_repeat.GetProgress()>key_repeat_timeout*LARGE_KEY_TIMEOUT_MULT) {
							key_repeat.SetProgress(key_repeat.GetProgress()-key_repeat_timeout*LARGE_KEY_TIMEOUT_MULT);
							largedecrease();
						}
					}

					if(goingdown) {
						while(key_repeat.GetProgress()>key_repeat_timeout*LARGE_KEY_TIMEOUT_MULT) {
							key_repeat.SetProgress(key_repeat.GetProgress()-key_repeat_timeout*LARGE_KEY_TIMEOUT_MULT);
							largeincrease();
						}
					}
				}
				else {
					if(goingup) {
						while(key_repeat.GetProgress()>key_repeat_timeout) {
							key_repeat.SetProgress(key_repeat.GetProgress()-key_repeat_timeout);
							smalldecrease();
						}
					}

					if(goingdown) {
						while(key_repeat.GetProgress()>key_repeat_timeout) {
							key_repeat.SetProgress(key_repeat.GetProgress()-key_repeat_timeout);
							smallincrease();
						}
					}
				}

				return animation::ProgressResult::None;
			}

			void addnamedlocation(T_ location, const std::string &name="") {
				namedlocations[location]=name;
			}

			void removenamedlocation(T_ location) {
				namedlocations.erase(location);
			}

			T_ gettickvalue(int x);

			T_ gettextvalue(int x);

			T_ getrulevalue(int x);

			bool golarge;
			PetContainer<Base> buttonlayer;

			class csmooth {
			public:
				csmooth() : indicator(true), symbol(true), value(false), speed(100), stepvalue(0), valuedisplay(false), indst_stepvalue(0)
				{ }

				bool symbol;
				bool indicator;
				bool value;
				bool valuedisplay;
				float speed; //percent/sec
				animation::Controller controller;
				floattype stepvalue;
				floattype sourcevalue;
				T_ targetvalue;

				animation::Controller indst_controller;
				floattype indst_stepvalue;
				floattype indst_sourcevalue;
				T_ indst_targetvalue;

				bool issmooth() { return speed>0 && (symbol || indicator || value || valuedisplay); }
			};

			const csmooth &getsmoothinginfo() const {
				return smooth;
			}

		private:
			std::map<T_, std::string> namedlocations;

			animation::Controller &getanimation(Blueprint::TransitionType transition) {
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

			bool rule_over;


			int value_start;//pixel
			int value_end;
			floattype valueperpixel;

			bool value_over;
			T_ hovervalue;

			AutosizeModes::Type autosize;

			numberformat format;
			numberformat valueformat;

			T_ value;
			floattype smoothvalue;
			T_ minimum;
			T_ maximum;

			T_ smallchange;
			T_ largechange;

			T_ steps;

			T_ indst_value;
			floattype indst_smoothvalue;

			bool unprepared;

			Blueprint::OrientationType  orientation;

			Blueprint::FocusMode focus;
			Blueprint::StyleMode style;

			Blueprint::FocusType next_focus;
			Blueprint::StyleType next_style;

			animation::Controller focus_anim;
			animation::Controller style_anim;
			animation::Timer	   idle_anim;


			animation::Controller key_repeat;
			int key_repeat_timeout;
			bool goingup, goingdown;



			Placeholder *symbolp;
			Placeholder *tickp;
			Placeholder *textp;
			Placeholder *valuep;
			BorderData *border;
			BorderData *overlay;
			BorderData *rule;
			BorderData *indicator;
			BorderData *tickmarkborder;
			BorderData *ruleoverlay;
			animation::RectangularGraphic2DAnimation *symbol;
			animation::RectangularGraphic2DAnimation *tick;
			animation::RectangularGraphic2DAnimation *tick_hover;
			Font *font;
			Font *font_hover;
			Font *valuefont;
			Font *valuefont_ind;


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
					value(false),
					inverseaxis(false),
					centerindicator(false),
					indicatorstart(false)
				{ }
				

				bool 
					symbol,
					rule,
					indicator,
					buttons,
					value,
					inverseaxis,
					centerindicator,
					indicatorstart
				;
			} display;

			class cmarkers {
			public:
				cmarkers() : ticks(false), numbers(false), tickdistance(10), numberdistance(2), namedlocations(false)
				{ }

				bool ticks;
				bool numbers;
				floattype tickdistance;
				int numberdistance;
				bool namedlocations;


			} markers;

			csmooth smooth;

			class cactions {
			public:
				cactions() : symbol_drag(true), rule_action(Goto), tick_click(false), number_click(false), keyboard(true)
				{ }

				bool symbol_drag;
				RuleAction rule_action;
				bool tick_click;
				bool number_click;
				bool keyboard;
			} actions;

			bool passivemode;
			bool cangetfocus;
			Alignment::Type valuelocation;

			graphics::Colorizable2DLayer innerlayer;
			graphics::Colorizable2DLayer overlayer;
			WidgetLayer symbollayer;

			std::map<animation::RectangularGraphic2DSequenceProvider*, animation::RectangularGraphic2DAnimation*> ImageCache;
			std::map<BorderDataResource*, BorderData*> BorderCache;


			void setupbuttons();
			void drawtick(floattype v, utils::Point &location, bool reverse, utils::Size &size);
			void drawtext(floattype v, Alignment::Type align, const std::string &str, utils::Bounds &inner, utils::Point &location, bool reverse, utils::Size &size);
			int drawvalue(utils::Bounds &inner, int distance, bool reverse);
			void printvalue(int x, int y, int w);

		};

		template<class T_, class floattype>
		T_ gge::widgets::slider::Base<T_, floattype>::getrulevalue(int x) {

			if(actions.rule_action==LargeChange) {
				T_ v=(T_)utils::Round(x*valueperpixel)+minimum;
				if(v>value) {
					if(value<maximum)
						return value+largechange;
					else
						return value;
				}
				else {
					if(value>minimum)
						return value-largechange;
					else
						return value;
				}
			}
			else {
				T_ v=(T_)utils::Round(x*valueperpixel/steps)*steps+minimum;

				if(v<minimum) v=minimum;
				if(v>maximum) v=maximum;

				return v;
			}

		}

		template<class T_, class floattype>
		T_ gge::widgets::slider::Base<T_, floattype>::gettextvalue(int x) {
			if(markers.namedlocations) {
				T_ v=(T_)utils::Round(x*valueperpixel)+minimum;
				T_ mindist=maximum-minimum;
				T_ dval=v;

				for(auto i=namedlocations.begin();i!=namedlocations.end();++i) {
					T_ d=std::abs(i->first-v);
					if(d<mindist && i->second!="") {
						dval=i->first;
						mindist=d;
					}
				}

				return dval;
			}
			else {
				floattype i=markers.tickdistance*markers.numberdistance;
				floattype v=x*valueperpixel+minimum;
				v=utils::Round(v/i)*i;

				return T_(v);
			}
		}

		template<class T_, class floattype>
		T_ gge::widgets::slider::Base<T_, floattype>::gettickvalue(int x) {
			if(markers.namedlocations) {
				T_ v=(T_)utils::Round(x*valueperpixel)+minimum;
				T_ mindist=maximum-minimum;
				T_ dval=T_(v);

				for(auto i=namedlocations.begin();i!=namedlocations.end();++i) {
					T_ d=std::abs(i->first-v);
					if(d<mindist) {
						dval=i->first;
						mindist=d;
					}
				}

				return dval;
			}
			else {
				floattype i=markers.tickdistance;
				floattype v=x*valueperpixel+minimum;
				v=utils::Round(v/i)*i;

				return T_(v);
			}
		}

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

			utils::Size size=WidgetBase::size;

			if(size.Width==0)
				size.Width=bp->DefaultSize.Width;
			if(size.Height==0)
				size.Height=bp->DefaultSize.Height;

			if(autosize!=AutosizeModes::None) {
				if(ishorizontal()) {
					if(rule && display.rule) {
						int s=0;
						if(border) {
							s=border->BorderWidth.TotalX()+border->Padding.TotalX();
						}
						if(display.buttons) {
							if(upbutton)
								s+=upbutton->GetWidth();
							if(downbutton)
								s+=downbutton->GetWidth();
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
							h+=textp->GetSize(utils::Size(0,font->FontHeight()), utils::Size(0,font->FontHeight())).Height+textp->Margins.TotalY();
						else
							h+=font->FontHeight();
					}

					//value display
					if(display.value && valuefont && (Alignment::isTop(valuelocation)||Alignment::isBottom(valuelocation))) {
						h+=valuefont->FontHeight();
						if(valuep)
							h+=valuep->Margins.TotalY();
					}

					size.Height=h;
				}
				else {
					if(rule && display.rule) {
						int s=0;
						if(border) {
							s=border->BorderWidth.TotalY()+border->Padding.TotalY();
						}
						if(display.buttons) {
							if(upbutton)
								s+=upbutton->GetHeight();
							if(downbutton)
								s+=downbutton->GetHeight();
						}
						size.Height=rule->CalculateHeight(WidgetBase::size.Height-s)+s;
					}

					if(display.value && valuefont && (Alignment::isTop(valuelocation)||Alignment::isBottom(valuelocation))) {
						size.Height+=valuefont->FontHeight();
						if(valuep)
							size.Height+=valuep->Margins.TotalY();
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

					if(display.buttons) {
						int bw=0;
						if(upbutton) bw=upbutton->GetWidth();
						if(downbutton) bw=std::max(bw, downbutton->GetWidth());
						w=std::max(bw, w);
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
							w+=textp->GetSize(utils::Size(0,font->TextWidth(ss.str())), utils::Size(0,font->TextWidth(ss.str()))).Height+textp->Margins.TotalY();
						}
						else {
							w+=font->TextWidth(ss.str());
						}
					}

					//value display
					if(display.value && valuefont) {
						int vw=0;
						if(border)
							vw+=border->BorderWidth.TotalX()+border->Padding.TotalX();

						std::stringstream ss;
						if(valueformat.hex)
							ss<<std::hex;
						if(valueformat.decimals>-1)
							ss<<std::setprecision(valueformat.decimals);
						ss<<valueformat.prefix;
						ss<<maximum;
						ss<<valueformat.units;

						vw+=valuefont->TextWidth(ss.str());

						if(valuep)
							vw+=valuep->Margins.TotalY();

						if(vw>w)
							w=vw;
					}

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
			valuep=NULL;
			border=NULL;
			overlay=NULL;
			rule=NULL;
			indicator=NULL;
			ruleoverlay=NULL;
			symbol=NULL;
			tick=NULL;
			tick_hover=NULL;
			tickmarkborder=NULL;
			font=NULL;
			font_hover=NULL;
			valuefont=NULL;
			valuefont_ind=NULL;


			Blueprint::TransitionType transition;

			symbolp=bp->GetSymbolPlace(groups, style, transition);
			tickp=bp->GetTickmarkPlace(groups, Blueprint::Normal, transition);
			textp=bp->GetTextPlace(groups, Blueprint::Normal, transition);
			valuep=bp->GetValuePlace(groups, Blueprint::Normal, transition);
			font=bp->GetFont(groups, Blueprint::Normal, transition);
			valuefont=bp->GetValueFont(groups, Blueprint::Normal, transition);

			font_hover=bp->GetFont(groups, Blueprint::Hover, transition);
			valuefont_ind=bp->GetValueFont(groups, Blueprint::Hover, transition);


			bprovider=bp->GetOuterBorder(groups, Blueprint::Normal, transition);
			if(bprovider) {
				if(BorderCache[bprovider])
					border=BorderCache[bprovider];
				else {
					border=&bprovider->CreateResizableObject();
					BorderCache[bprovider]=border;
				}

				border->SetController(idle_anim);
			}

			bprovider=bp->GetOverlay(groups, Blueprint::Normal, transition);
			if(bprovider) {
				if(BorderCache[bprovider])
					overlay=BorderCache[bprovider];
				else {
					overlay=&bprovider->CreateResizableObject();
					BorderCache[bprovider]=overlay;
				}

				overlay->SetController(idle_anim);
			}

			bprovider=bp->GetRuler(groups,( IsEnabled() ? (rule_over ? Blueprint::Hover : Blueprint::Normal) : Blueprint::Disabled ), transition);
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

			bprovider=bp->GetRulerOverlay(groups, ( IsEnabled() ? (rule_over ? Blueprint::Hover : Blueprint::Normal) : Blueprint::Disabled ), transition);
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

			bprovider=bp->GetTickmarkBorder(groups, Blueprint::Normal, transition);
			if(bprovider) {
				if(BorderCache[bprovider])
					tickmarkborder=BorderCache[bprovider];
				else {
					tickmarkborder=&bprovider->CreateResizableObject();
					BorderCache[bprovider]=tickmarkborder;
				}

				tickmarkborder->SetController(idle_anim);
			}

			unprepared=false;
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


			//prepare();

			BaseLayer->BoundingBox.SetSize(GetSize());
			Bounds outer=Bounds(Point(0,0),BaseLayer->BoundingBox.GetSize());
			Bounds inner=outer;
			int	   distance=0;

			if(outer.Width()==0 || outer.Height()==0)
				return;

			if(border) {
				inner=border->ContentBounds(outer);
			}
			buttonlayer.BaseLayer.BoundingBox=outer;
			overlayer.BoundingBox=outer;


			//DRAWING BORDER
			if(border) {
				border->DrawIn(BaseLayer, outer);
			}

			int button_top=0;
			int button_bottom=0;

			//buttons
			if(display.buttons) {
				if(ishorizontal()) {
					if(upbutton) {
						upbutton->Move(inner.TopLeft());
						button_top=upbutton->GetWidth();
					}
					if(downbutton) {
						downbutton->Move(inner.Right-downbutton->GetWidth(), inner.Top);
						button_bottom=downbutton->GetWidth();
					}
					inner.Left+=button_top;
					inner.Right-=button_bottom;
				}
				else {
					if(upbutton) {
						upbutton->Move(inner.TopLeft());
						button_top=upbutton->GetHeight();
					}
					if(downbutton) {
						downbutton->Move(inner.Left, inner.Bottom-downbutton->GetHeight());
						button_bottom=downbutton->GetHeight();
					}
					inner.Top+=button_top;
					inner.Bottom-=button_bottom;
				}
			}

			innerlayer.BoundingBox=inner;


			//CALCULATE VALUE RANGE
			if(ishorizontal()) {
				if(rule) {
					value_start=inner.Left+rule->Margins.Left+rule->BorderWidth.Left;
					value_end=value_start+inner.Width()-(rule->Margins.TotalX()+rule->BorderWidth.TotalX());

					valueperpixel=floattype(maximum-minimum)/(inner.Width()-(rule->Margins.TotalX()+rule->BorderWidth.TotalX()));
				}
				else {
					value_start=inner.Left;
					value_end=value_start+inner.Width();

					valueperpixel=floattype(maximum-minimum)/inner.Width();
				}
			}
			else {
				if(rule) {
					value_start=inner.Top+rule->Margins.Top+rule->BorderWidth.Top;
					value_end=value_start+inner.Height()-(rule->Margins.TotalY()+rule->BorderWidth.TotalY());

					valueperpixel=floattype(maximum-minimum)/(inner.Height()-(rule->Margins.TotalY()+rule->BorderWidth.TotalY()));
				}
				else {
					value_start=inner.Top;
					value_end=value_start+inner.Height();

					valueperpixel=floattype(maximum-minimum)/inner.Height();
				}
			}
			if(display.centerindicator) {
				value_start+=(value_end-value_start)/2;
				valueperpixel*=2;
			}

			//ORIENTATION MODIFICATION
			graphics::SizeController2D szc;
			bool reverse;
			if(!(tick && markers.ticks && markers.tickdistance) && !(font && markers.numbers && markers.numberdistance)) {
				szc=graphics::SizeController2D::SingleMiddleCenter;
				reverse=false;
			}
			else if(orientation==Blueprint::Top || orientation==Blueprint::Left) {
				szc=graphics::SizeController2D::SingleBottomRight;
				reverse=true;
			}
			else {
				szc=graphics::SizeController2D::SingleTopLeft;
				reverse=false;
			}



			//DRAWING VALUE
			if(display.value && valuefont && Alignment::isTop(valuelocation)) {
				drawvalue(inner, distance, reverse);
			}
			if(display.value && valuefont && isvertical()) {
				if(Alignment::isBottom(valuelocation)) {
					inner.Bottom-=valuefont->FontHeight();
					if(valuep)
						inner.Bottom-=valuep->Margins.TotalY();
				}
				else {
					inner.Top+=valuefont->FontHeight();
					if(valuep)
						inner.Top+=valuep->Margins.TotalY();
				}
			}


			
			int ruledistance=distance;
			utils::Bounds rulebounds=inner;
			//DRAWING RULE
			if(display.rule && rule) {
				rulebounds=inner-rule->Margins;
				if(ishorizontal()) {
					if(reverse)
						rulebounds.Bottom-=distance;
					else
						rulebounds.Top+=distance;
				}
				else {
					if(reverse)
						rulebounds.Right-=distance;
					else
						rulebounds.Left+=distance;
				}
				rule->DrawIn(BaseLayer, szc, rulebounds);

				if(ishorizontal()) {
					rule_region=Bounds(inner.Left, distance, inner.Right, distance);
					distance+=rule->Margins.TotalY()+rule->CalculateHeight(szc,(inner-rule->Margins).Height());
					if(reverse) {
						rule_region.Top=inner.Bottom-distance;
						rule_region.Bottom=inner.Bottom;
					}
					else {
						rule_region.Bottom=distance;
					}
				}
				else {
					rule_region=Bounds(distance, inner.Top, distance, inner.Bottom);
					distance+=rule->Margins.TotalX()+rule->CalculateWidth(szc,(inner-rule->Margins).Width());
					if(reverse) {
						rule_region.Right=inner.Right-distance;
						rule_region.Right=inner.Right;
					}
					else {
						rule_region.Right=distance;
					}
				}


			}

			//DRAW INDICATOR
			if(display.indicator && indicator) {

				T_ indval=value;
				T_ indst=indst_value;
				if(smooth.indicator) {
					indval=(T_)smoothvalue;
					indst=(T_)indst_smoothvalue;
				}

				Bounds b = rulebounds+rule->Margins-(indicator->Margins)+indicator->BorderWidth;
				if(rule && display.rule) {
					b=b-rule->BorderWidth;
				}

				if(ishorizontal()) {
					int w=rulebounds.Width();
					int l=indicator->Margins.Left-indicator->BorderWidth.Left;
					int r=indicator->BorderWidth.TotalX()-indicator->Margins.TotalX();

					if(rule) {
						l+=rule->BorderWidth.Left+rule->Margins.Left;
						w-=rule->BorderWidth.TotalX();
					}

					if(display.indicatorstart) {
						if(display.inverseaxis) {
							if(indst>indval) {
								b.Right=l+w-(int)utils::Round(w*floattype(indval-minimum)/maximum)+r;
								b.Left=l+w-(int)utils::Round(w*floattype(indst-minimum)/maximum);
							}
							else {
								b.Right=l+w-(int)utils::Round(w*floattype(indst-minimum)/maximum)+r;
								b.Left=l+w-(int)utils::Round(w*floattype(indval-minimum)/maximum);
							}
						}
						else {
							if(indst>indval) {
								b.Left=l+(int)utils::Round(w*floattype(indval-minimum)/maximum);
								b.Right=l+(int)utils::Round(w*floattype(indst-minimum)/maximum)+r;
							}
							else {
								b.Left=l+(int)utils::Round(w*floattype(indst-minimum)/maximum);
								b.Right=l+(int)utils::Round(w*floattype(indval-minimum)/maximum)+r;
							}
						}
					}
					else {
						if(display.centerindicator) {
							b.Left=l-(int)utils::Round(w*floattype(indval-minimum)/maximum)/2+w/2;
							b.Right=l+(int)utils::Round(w*floattype(indval-minimum)/maximum)/2+w/2+r;
						}
						else if(display.inverseaxis) {
							b.Right=l+w+r;
							b.Left=l+w-(int)utils::Round(w*floattype(indval-minimum)/maximum);
						}
						else {
							b.Left=l;
							b.Right=l+(int)utils::Round(w*floattype(indval-minimum)/maximum)+r;
						}
					}
				}
				else {
					int h=rulebounds.Height();
					int t=indicator->Margins.Top-indicator->BorderWidth.Top;
					int bt=indicator->BorderWidth.TotalY()-indicator->Margins.TotalY();

					if(rule) {
						t+=rule->BorderWidth.Top+rule->Margins.Top;
						h-=rule->BorderWidth.TotalY();
					}

					if(display.indicatorstart) {
						if(display.inverseaxis) {
							if(indst>indval) {
								b.Bottom=t+h-(int)utils::Round(h*floattype(indval-minimum)/maximum)+bt;
								b.Top=t+h-(int)utils::Round(h*floattype(indst-minimum)/maximum);
							}
							else {
								b.Bottom=t+h-(int)utils::Round(h*floattype(indst-minimum)/maximum)+bt;
								b.Top=t+h-(int)utils::Round(h*floattype(indval-minimum)/maximum);
							}
						}
						else {
							if(indst>indval) {
								b.Top=t+(int)utils::Round(h*floattype(indval-minimum)/maximum);
								b.Bottom=t+(int)utils::Round(h*floattype(indst-minimum)/maximum)+bt;
							}
							else {
								b.Top=t+(int)utils::Round(h*floattype(indst-minimum)/maximum);
								b.Bottom=t+(int)utils::Round(h*floattype(indval-minimum)/maximum)+bt;
							}
						}
					}
					else {
						if(display.centerindicator) {
							b.Top=t-(int)utils::Round(h*floattype(indval-minimum)/maximum)/2+h/2;
							b.Bottom=t+(int)utils::Round(h*floattype(indval-minimum)/maximum)/2+h/2+bt;
						}
						else if(display.inverseaxis) {
							b.Bottom=t+h+bt;
							b.Top=t+h-(int)utils::Round(h*floattype(indval-minimum)/maximum);
						}
						else {
							b.Top=t;
							b.Bottom=t+(int)utils::Round(h*floattype(indval-minimum)/maximum)+bt;
						}
					}
				}

				BaseLayer->SetDrawMode(graphics::BasicSurface::Offscreen);
				indicator->DrawIn(BaseLayer, szc, b);
				BaseLayer->SetDrawMode(graphics::BasicSurface::OffscreenAlphaOnly);
				indicator->DrawIn(BaseLayer, szc, b);
				BaseLayer->SetDrawMode(graphics::BasicSurface::Normal);
			}

			//DRAW VALUE
			if(display.value && Alignment::isMiddle(valuelocation) && valuefont && valuefont_ind) {
				int x,y,w,h;
				if(rule && display.rule) {
					h=rule->CalculateHeight(szc, 0)-rule->Margins.TotalY()-rule->Padding.TotalY();
					if(valuep) {
						h-=valuep->Margins.TotalY();
					}
					y=(h-valuefont->FontBaseline())/2+rule->Margins.Top +rule->Padding.Top-innerlayer.BoundingBox.Top;
					x=rule->Margins.Left+rule->BorderWidth.Left+rule->Padding.Left-innerlayer.BoundingBox.Left;
					w=outer.Width()-rule->Margins.TotalX()-rule->BorderWidth.TotalX()-rule->Padding.TotalX();
				}
				else {
					h=rule->CalculateHeight(szc, 0);
					if(valuep) {
						h-=valuep->Margins.TotalY();
					}
					y=(h-valuefont->FontBaseline())/2-innerlayer.BoundingBox.Top;
					x=-innerlayer.BoundingBox.Left;
					w=outer.Width();
				}

				if(valuep) {
					x+=valuep->Margins.Left;
					y+=valuep->Margins.Top;
					w-=valuep->Margins.TotalX();
				}

				printvalue(x, y, w);
			}

			//DRAWING MARKER BORDER
			if(tickmarkborder) {
				int sz=0;
				if(markers.ticks && markers.tickdistance && tick) {
					utils::Size size=tick->GetSize();
					if(tickp) {
						size=tickp->GetSize(tick->GetSize(), inner.GetSize());
						size=size+utils::Size(tickp->Margins.TotalX(),tickp->Margins.TotalY());
					}

					if(ishorizontal())
						sz+=size.Height;
					else
						sz+=size.Width;
				}
				if(markers.numbers && markers.tickdistance && markers.numberdistance && font) {
					utils::Size size=utils::Size(0, font->FontHeight());
					if(tickp) {
						size=textp->GetSize(size, inner.GetSize());
						size=size+utils::Size(textp->Margins.TotalX(),textp->Margins.TotalY());
					}

					if(ishorizontal())
						sz+=size.Height;
					else
						sz+=size.Width;
				}

				if(sz>0) {
					if(ishorizontal()) {
						if(reverse) {
							int h=sz-tickmarkborder->Margins.TotalY();
							tickmarkborder->DrawIn(
								BaseLayer, 
								inner.Left+tickmarkborder->Margins.Left, 
								inner.Bottom-(distance+tickmarkborder->Margins.Bottom+h),
								inner.Width()-tickmarkborder->Margins.TotalX(),
								h
							);
						}
						else {
							tickmarkborder->DrawIn(
								BaseLayer, 
								inner.Left+tickmarkborder->Margins.Left, 
								inner.Top+distance+tickmarkborder->Margins.Top,
								inner.Width()-tickmarkborder->Margins.TotalX(),
								sz-tickmarkborder->Margins.TotalY()
							);
						}
					}
					else {
						if(reverse) {
							int w=sz-tickmarkborder->Margins.TotalX();
							tickmarkborder->DrawIn(
								BaseLayer, 
								inner.Right-(distance+tickmarkborder->Margins.Right+w),
								inner.Top+tickmarkborder->Margins.Top, 
								w,
								inner.Height()-tickmarkborder->Margins.TotalY()
							);
						}
						else {
							tickmarkborder->DrawIn(
								BaseLayer, 
								inner.Left+distance+tickmarkborder->Margins.Left,
								inner.Top+tickmarkborder->Margins.Top, 
								sz-tickmarkborder->Margins.TotalX(),
								inner.Height()-tickmarkborder->Margins.TotalY()
							);
						}
					}
				}
			}


			//DRAWING TICKS
			if(markers.ticks && markers.tickdistance && tick) {
				Point p(0,0);
				utils::Size size=tick->GetSize();
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

				Point location=p-Alignment::CalculateLocation(align, Bounds(Point(0,0),size), utils::Size(0,0));

				if(ishorizontal()) {
					location.y+=distance;

					ticks_region=Bounds(inner.Left, distance, inner.Right, distance+size.Height);
				}
				else {
					location.x+=distance;

					ticks_region=Bounds(distance, inner.Top, distance+size.Width, inner.Bottom);
				}

				floattype mn=ceil(minimum/markers.tickdistance)*markers.tickdistance;
				floattype st=floattype(mn), ed=floattype(maximum);

				if(tickp && rule) {
					bool insiderule=false;

					switch(orientation) {
					case Blueprint::Left:
						if(tickp->Margins.Right<-size.Height/2)
							insiderule=true;
						break;
					case Blueprint::Right:
					case Blueprint::Vertical:
						if(tickp->Margins.Left<-size.Height/2)
							insiderule=true;
						break;

					case Blueprint::Top:
						if(tickp->Margins.Bottom<-size.Height/2)
							insiderule=true;
						break;
					case Blueprint::Bottom:
					case Blueprint::Horizontal:
					default:
						if(tickp->Margins.Top<-size.Height/2)
							insiderule=true;
						break;
					}

					if(insiderule) {
						if(display.centerindicator) {
							ed-=markers.tickdistance;
						}
						else {
							st+=markers.tickdistance;
							ed-=markers.tickdistance;
						}
					}
				}

				if(markers.namedlocations) {
					for(auto i=namedlocations.begin();i!=namedlocations.end();++i) {
						drawtick(floattype(i->first), location, reverse, size);
					}
				} 
				else {
					for(floattype v=st;v<=ed;v+=markers.tickdistance) {
						drawtick(v, location, reverse, size);
					}
				}

				if(reverse) {
					if(ishorizontal()) {
						int h=ticks_region.Height();
						ticks_region.Top=innerlayer.BoundingBox.Height()-(ticks_region.Top+size.Height);
						ticks_region.SetHeight(h);
					}
					else {
						int w=ticks_region.Width();
						ticks_region.Left=innerlayer.BoundingBox.Width()-(ticks_region.Left+size.Width);
						ticks_region.SetWidth(w);
					}
				}

				if(tickp) {
					if(ishorizontal()) {
						distance+=tickp->Margins.TotalY()+size.Height;
						if(reverse)
							ticks_region.Top-=tickp->Margins.Top;
						else
							ticks_region.Bottom+=tickp->Margins.Bottom;
					}
					else {
						distance+=tickp->Margins.TotalX()+size.Width;
						if(reverse)
							ticks_region.Left-=tickp->Margins.Left;
						else
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

			//DRAWING NUMBERS
			if(markers.numbers && markers.tickdistance && markers.numberdistance && font) {
				Point p(0,0);
				utils::Size size=utils::Size(0, font->FontHeight());
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

				Point location=p-Alignment::CalculateLocation(align, Bounds(Point(0,0),size), utils::Size(0,0));

				if(ishorizontal()) {
					//location.x+=value_start;
					location.y+=distance;

					numbers_region=Bounds(inner.Left, distance, inner.Right, distance+size.Height);
				}
				else {
					//location.y+=value_start;
					location.x+=distance;

					numbers_region=Bounds(distance, inner.Top, distance+size.Width, inner.Bottom);
				}

				if(markers.namedlocations) {
					for(auto i=namedlocations.begin();i!=namedlocations.end();++i) {
						drawtext(floattype(i->first), align, i->second, inner, location, reverse, size);
					}
				}
				else {
					std::stringstream ss;
					if(format.hex)
						ss<<std::hex;
					if(format.decimals>-1)
						ss<<std::fixed<<std::setprecision(format.decimals);

					floattype mn=ceil(minimum/(markers.tickdistance*markers.numberdistance))*markers.tickdistance*markers.numberdistance;

					for(floattype v=(floattype)mn;v<=maximum;v+=markers.tickdistance*markers.numberdistance) {
						ss.str(string());
						ss<<format.prefix;
						ss<<v;
						ss<<format.units;
						std::string str=ss.str();

						drawtext(v, align, str, inner, location, reverse, size);

					}
				}

				if(reverse) {
					if(ishorizontal()) {
						int h=numbers_region.Height();
						numbers_region.Top=innerlayer.BoundingBox.Height()-(numbers_region.Top+size.Height);
						numbers_region.SetHeight(h);
					}
					else {
						int w=numbers_region.Width();
						numbers_region.Left=innerlayer.BoundingBox.Width()-(numbers_region.Left+size.Width);
						numbers_region.SetWidth(w);
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

			//DRAWING VALUE
			if(display.value && valuefont && Alignment::isBottom(valuelocation)) {
				distance = drawvalue(inner, distance, reverse);
			}


			//DRAWING RULE OVERLAY
			if(ruleoverlay && display.rule) {
				utils::Bounds b=rulebounds-ruleoverlay->Margins;
				if(rule)
					b=b+rule->Margins;

				ruleoverlay->DrawIn(overlayer, szc, b);
			}



			//DRAWING SYMBOL
			if(symbol && display.symbol) {
				Point p(0,0);
				utils::Size size=symbol->GetSize();
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

				Point location=p-Alignment::CalculateLocation(align, Bounds(Point(0,0),size), utils::Size(0,0));

				if(ishorizontal()) {
					symbol_zero=location.x;

					int x;
					T_ v;
					if(smooth.symbol)
						v=(T_)smoothvalue;
					else
						v=value;

					x=(int)utils::Round((v-minimum)/valueperpixel);

					if(display.inverseaxis) {
						location.x=location.x + (value_end-x);
					}
					else {
						location.x=location.x+x+value_start;
					}
				}
				else {
					symbol_zero=location.y;

					int y;
					T_ v;
					if(smooth.symbol)
						v=(T_)smoothvalue;
					else
						v=value;

					y=(int)utils::Round((v-minimum)/valueperpixel);

						if(display.inverseaxis) {
							location.y=location.y + (value_end-y);
						}
						else {
							location.y=location.y+y+value_start;
						}
				}

				if(reverse) {
					if(ishorizontal()) {
						location.y=inner.Bottom-(location.y+size.Height);
					} 
					else {
						location.x=inner.Right-(location.x+size.Width);
					}
				}

				symbollayer.Move(location);
				symbollayer.Resize(size);
				symbol->Draw(symbollayer,0,0);
			}

			//DRAWING OVERLAY
			if(overlay) {
				overlay->DrawIn(overlayer, outer);
			}


			//buttonlayer.Draw();

		} //end of method


		template<class T_, class floattype>
		void Base<T_, floattype>::setupbuttons() {
			if(display.buttons) {
				//utils::CheckAndDelete(upbutton);
				//utils::CheckAndDelete(downbutton);

				if(bp) {
					if(!upbutton)
						upbutton=new Button;

					if(bp->GetOrientationBaseGroup(orientation) && bp->GetOrientationBaseGroup(orientation)->UpButton)
						upbutton->SetBlueprint(*bp->GetOrientationBaseGroup(orientation)->UpButton);
					upbutton->SetContainer(buttonlayer);
					//upbutton->ClickEvent().Register(this,&Base::smalldecrease);
					upbutton->MouseEvent().Register(this,&Base::upbutton_mouse);

					if(!downbutton)
						downbutton=new Button;

					if(bp->GetOrientationBaseGroup(orientation) && bp->GetOrientationBaseGroup(orientation)->DownButton)
						downbutton->SetBlueprint(*bp->GetOrientationBaseGroup(orientation)->DownButton);
					downbutton->SetContainer(buttonlayer);
					//downbutton->ClickEvent().Register(this,&Base::smallincrease);
					downbutton->MouseEvent().Register(this,&Base::downbutton_mouse);
				}
			}
		}


		template<class T_, class floattype>
		bool Base<T_, floattype>::KeyboardEvent(input::keyboard::Event::Type event, input::keyboard::Key Key) {
			using namespace input::keyboard;

			if(!IsEnabled() || ispassive() || !actions.keyboard)
				return false;

			if(event==Event::Down && (Key==KeyCodes::Up || Key==KeyCodes::Left)) {
				if(!goingup) {
					smalldecrease();
					golarge=false;
					begin_goup();
				}

				return true;
			}
			if(event==Event::Up	&& (Key==KeyCodes::Up || Key==KeyCodes::Left)) {
				end_goup();

				return true;
			}

			if(event==Event::Down && (Key==KeyCodes::Down || Key==KeyCodes::Right)) {
				if(!goingdown) {
					smallincrease();
					golarge=false;
					begin_godown();
				}

				return true;
			}
			if(event==Event::Up	&& (Key==KeyCodes::Down || Key==KeyCodes::Right)) {
				end_godown();

				return true;
			}

			if(event==Event::Down && Key==KeyCodes::PageUp) {
				if(!goingup) {
					largedecrease();
					golarge=true;
					begin_goup();
				}

				return true;
			}
			if(event==Event::Up	&& Key==KeyCodes::PageUp) {
				end_goup();

				return true;
			}

			if(event==Event::Down && Key==KeyCodes::PageDown) {
				if(!goingdown) {
					largeincrease();
					golarge=true;
					begin_godown();
				}

				return true;
			}
			if(event==Event::Up	&& Key==KeyCodes::PageDown) {
				end_godown();

				return true;
			}

			if(event==Event::Up && Key==KeyCodes::Home) {
				tomin();

				return true;
			}

			if(event==Event::Up && Key==KeyCodes::End) {
				tomax();

				return true;
			}

			return false;
		}


		template<class T_, class floattype>
		void Base<T_, floattype>::drawtick(floattype v, utils::Point &location, bool reverse, utils::Size &size) {
			animation::RectangularGraphic2DAnimation *ttick=tick;
			if((v==smoothvalue && smooth.symbol) || (v==value) || (v==hovervalue && value_over)) {
				ttick=tick_hover;
			}
			if(ishorizontal()) {
				int x;
				if(display.inverseaxis) {
					x=location.x+( value_end-(int)utils::Round((v-minimum)/valueperpixel) )-innerlayer.BoundingBox.Left;
				}
				else {
					x=value_start+location.x+(int)utils::Round((v-minimum)/valueperpixel)-innerlayer.BoundingBox.Left;
				}

				if(display.centerindicator) {
					ttick->Draw(innerlayer, 
						x, 
						location.y-innerlayer.BoundingBox.Top
					);
					if(v!=minimum) {
						ttick->Draw(innerlayer, 
							2*value_start-(x-location.x)+location.x,
							location.y-innerlayer.BoundingBox.Top
						);
					}
				}
				else if(reverse) {
					ttick->Draw(innerlayer, 
						x, 
						innerlayer.BoundingBox.Height()-(location.y+size.Height)
					);
				}
				else {
					ttick->Draw(innerlayer, 
						x, 
						location.y-innerlayer.BoundingBox.Top
					);
				}
			}
			else {
				int y;
				if(display.inverseaxis) {
					y=location.y+( value_end-(int)utils::Round((v-minimum)/valueperpixel) )-innerlayer.BoundingBox.Top;
				}
				else {
					y=value_start+location.y+(int)utils::Round((v-minimum)/valueperpixel)-innerlayer.BoundingBox.Top;
				}

				if(reverse) {
					ttick->Draw(innerlayer, 
						innerlayer.BoundingBox.Width()-(location.x+size.Width),
						y
					);
				}
				else {
					ttick->Draw(innerlayer, 
						location.x-innerlayer.BoundingBox.Left, 
						y
					);
				}
			}
		}


		template<class T_, class floattype>
		void Base<T_, floattype>::drawtext(floattype v, Alignment::Type align, const std::string &str, utils::Bounds &inner, utils::Point &location, bool reverse, utils::Size &size) {
			Font *tfont=font;
			if((v==smoothvalue && smooth.symbol) || (v==value) || (v==hovervalue && value_over)) {
				tfont=font_hover;
			}
			TextAlignment::Type talign=TextAlignment::GetHorizontal(align);

			int toffset=0; //temp offset

			if(talign==TextAlignment::Center && ishorizontal()) {
				if(font->TextWidth(str)/2>value_start) {
					if(rule) {
						if(display.inverseaxis) {
							toffset=+(rule->BorderWidth.Right+rule->Margins.Right);
						}
						else {
							toffset=-(rule->BorderWidth.Left+rule->Margins.Left);
						}
					}

					if(display.inverseaxis) {
						talign=TextAlignment::Right;
					}
					else {
						talign=TextAlignment::Left;
					}
				}
				if(font->TextWidth(str)/2>inner.Right-(value_start+(maximum-minimum)/valueperpixel-inner.Left)) {
					if(rule && ishorizontal()) {
						if(display.inverseaxis) {
							toffset=-(rule->BorderWidth.Left+rule->Margins.Left);
						}
						else {
							toffset=+(rule->BorderWidth.Right+rule->Margins.Right);
						}
					}

					if(display.inverseaxis) {
						talign=TextAlignment::Left;
					}
					else {
						talign=TextAlignment::Right;
					}
				}
			}

			if(ishorizontal()) {
				int x;
				if(display.inverseaxis) {
					x=toffset+location.x+( value_end-(int)utils::Round((v-minimum)/valueperpixel) )-innerlayer.BoundingBox.Left;
				}
				else {
					x=toffset+value_start+location.x+(int)utils::Round((v-minimum)/valueperpixel)-innerlayer.BoundingBox.Left;
				}

				if(reverse) {
					tfont->Print(
						innerlayer, 
						x,
						innerlayer.BoundingBox.Height()-(location.y+size.Height),
						0, str, talign
					);
				}
				else {
					tfont->Print(
						innerlayer, 
						x, 
						location.y-innerlayer.BoundingBox.Top,
						0, str, talign
					);
				}
			}
			else {
				int y;
				if(display.inverseaxis) {
					y=toffset+location.y+( value_end-(int)utils::Round((v-minimum)/valueperpixel) )-innerlayer.BoundingBox.Top;
				}
				else {
					y=toffset+value_start+location.y+(int)utils::Round((v-minimum)/valueperpixel)-innerlayer.BoundingBox.Top;
				}

				if(reverse) {
					tfont->Print(
						innerlayer, 
						innerlayer.BoundingBox.Width()-(location.x+size.Width),
						y,
						0, str, talign
					);

				}
				else {
					tfont->Print(
						innerlayer, 
						location.x-innerlayer.BoundingBox.Left, 
						y,
						0, str, talign
					);
				}
			}
		}


		template<class T_, class floattype>
		int Base<T_, floattype>::drawvalue(utils::Bounds &inner, int distance, bool reverse) {
			int x=0,y=0, w=inner.Width();
			if(ishorizontal())
				y+=distance;
			else
				x+=distance;
			if(reverse) {
				if(ishorizontal())
					y=inner.Height()-y-valuefont->FontHeight();
			}
			if(valuep) {
				x+=valuep->Margins.Left;
				y+=valuep->Margins.Top;
				w-=valuep->Margins.TotalX();
			}

			printvalue(x, y, w);

			if(ishorizontal()) {
				distance+=valuefont->FontHeight();
				if(valuep)
					distance+=valuep->Margins.TotalY();
			}
			return distance;
		}


		template<class T_, class floattype>
		void Base<T_, floattype>::printvalue(int x, int y, int w) {
			std::stringstream ss;
			if(valueformat.hex)
				ss<<std::hex;
			if(valueformat.decimals>-1)
				ss<<std::fixed<<std::setprecision(valueformat.decimals);
			
			if(display.indicatorstart) {
				T_ v=value, indst=indst_value;

				if(smooth.valuedisplay) {
					v=T_(smoothvalue);
					indst=T_(indst_smoothvalue);
				}

				if(indst<v) {
					ss<<valueformat.prefix;
					ss<<indst;
					ss<<valueformat.units;
					ss<<valueformat.separator;
					ss<<valueformat.prefix;
					ss<<v;
					ss<<valueformat.units;
				}
				else {
					ss<<valueformat.prefix;
					ss<<v;
					ss<<valueformat.units;
					ss<<valueformat.separator;
					ss<<valueformat.prefix;
					ss<<indst;
					ss<<valueformat.units;
				}

			}
			else {
				ss<<valueformat.prefix;
				if(smooth.valuedisplay)
					ss<<T_(smoothvalue);
				else
					ss<<value;
				ss<<valueformat.units;
			}

			valuefont->Print(innerlayer, 
				x, y, w,
				ss.str(),
				TextAlignment::GetHorizontal(valuelocation)
			);
		}


	}

} }

