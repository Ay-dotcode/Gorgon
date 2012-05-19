#pragma once
#include "Interfaces/ISlider.h"
#include "Slider/SliderBase.h"

namespace gge { namespace widgets {

	template<class T_=int, class floattype=typename slider::floattype<T_>::Type>
	class Slider : public ISlider<T_>, public slider::Base<T_> {
	public:
		enum OrientationType {
			Bottom	= slider::Blueprint::Bottom,
			Top		= slider::Blueprint::Top,
			Left	= slider::Blueprint::Left,
			Right	= slider::Blueprint::Right,
		}; 

		typedef typename slider::Base<T_>::numberformat NumberFormatType;

		Slider() : ChangeEvent("ChangeEvent", this),
			INIT_PROPERTY(Slider, AnimationDuration),
			INIT_PROPERTY(Slider, Orientation),
			INIT_PROPERTY(Slider, ShowTicks),
			INIT_PROPERTY(Slider, TickDistance),
			INIT_PROPERTY(Slider, ShowNumbers),
			INIT_PROPERTY(Slider, NumberDistance),
			INIT_PROPERTY(Slider, Stepsize),
			INIT_PROPERTY(Slider, NumberFormat),
			INIT_PROPERTY(Slider, ValueAnimation)
		{
			slider::Base<T_>::setorientation(slider::Blueprint::Bottom);
			slider::Base<T_>::setupdisplay(true, true, false, false, false);
			slider::Base<T_>::setsmoothingmode(true, false, false, true, 100);
			slider::Base<T_>::setmarkers(true, true, false, 10, 5);
			slider::Base<T_>::setactions(true, slider::Base<T_>::Goto, true, true, true);
			slider::Base<T_>::setactive();

			ISlider<T_>::ChangeEvent.DoubleLink(ChangeEvent);

			if(WR.Slider)
				this->setblueprint(*WR.Slider);
		}

		virtual WidgetBase *GetWidget() {
			return this;
		}

		template <class O_>
		Slider &operator =(const O_ &value) { 
			(this->Object.*this->setter)(value);

			return *this;
		}

		utils::NumericProperty<Slider, int> AnimationDuration;
		utils::Property<Slider, OrientationType> Orientation;
		utils::BooleanProperty<Slider> ShowTicks;
		utils::NumericProperty<Slider, floattype> TickDistance;
		utils::BooleanProperty<Slider> ShowNumbers;
		//in terms of ticks (2 means every other tick)
		utils::NumericProperty<Slider, int> NumberDistance;
		utils::NumericProperty<Slider, T_> Stepsize;
		utils::Property<Slider, NumberFormatType> NumberFormat;
		utils::BooleanProperty<Slider> ValueAnimation;
		
		utils::EventChain<Slider> ChangeEvent;

	protected:



		//REQUIRED
		virtual T_ getValue() const {
			return slider::Base<T_>::getvalue();
		}
		void setValue(const T_ &value) {
			slider::Base<T_>::setvalue(value);
		}
		T_ getMin() const {
			return slider::Base<T_>::getmin();
		}
		void setMin(const T_ &value) {
			slider::Base<T_>::setmin(value);
		}
		T_ getMax() const {
			return slider::Base<T_>::getmax();
		}
		void setMax(const T_ &value) {
			slider::Base<T_>::setmax(value);
		}


		int getAnimationDuration() const {
			if(slider::Base<T_>::getsmoothingspeed()==0)
				return 0;

			return int(100000/slider::Base<T_>::getsmoothingspeed());
		}
		void setAnimationDuration(const int &value) {
			if(value==0)
				slider::Base<T_>::setsmoothingspeed(0);
			else
				slider::Base<T_>::setsmoothingspeed(100000.f/value);
		}

		OrientationType getOrientation() const {
			return OrientationType(slider::Base<T_>::getorientation());
		}
		void setOrientation(const OrientationType &orientation) {
			slider::Base<T_>::setaxisinverse(orientation==Left || orientation==Right);

			slider::Base<T_>::setorientation(slider::Blueprint::OrientationType(orientation));
		}

		void setShowTicks(const bool &value) {
			slider::Base<T_>::setshowticks(value);
		}
		bool getShowTicks() const {
			return slider::Base<T_>::getshowticks();
		}

		void setTickDistance(const floattype &value) {
			slider::Base<T_>::settickdistance(value);
			slider::Base<T_>::setlargechange(T_(value));
		}
		floattype getTickDistance() const {
			return slider::Base<T_>::gettickdistance();
		}

		void setShowNumbers(const bool &value) {
			slider::Base<T_>::setshownumbers(value);
		}
		bool getShowNumbers() const {
			return slider::Base<T_>::getshownumbers();
		}

		void setNumberDistance(const int &value) {
			slider::Base<T_>::setnumberdistance(value);
		}
		int getNumberDistance() const {
			return slider::Base<T_>::getnumberdistance();
		}

		void setValueAnimation(const bool &value) {
			slider::Base<T_>::setsmoothingmode(true, false, value, true, 100);
		}
		bool getValueAnimation() const {
			return slider::Base<T_>::getsmoothinginfo().value;
		}

		void setStepsize(const T_ &value) {
			slider::Base<T_>::setsteps(value);
			slider::Base<T_>::setsmallchange(std::max(value,T_(1)));
		}
		T_ getStepsize() const {
			return slider::Base<T_>::getsteps();
		}

		void setNumberFormat(const NumberFormatType &value) {
			slider::Base<T_>::setnumberformat(value);
		}
		NumberFormatType getNumberFormat() const {
			return slider::Base<T_>::getnumberformat();
		}

		virtual void wr_loaded() {
			if(WR.Slider && !this->blueprintmodified)
				this->setblueprint(*WR.Slider);
		}


		virtual void value_changed() {
			ChangeEvent();
		}

	};

}}
