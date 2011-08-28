#pragma once
#include "Interfaces\ISlider.h"
#include "Slider\SliderBase.h"

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

		typedef Base::numberformat NumberFormatType;

		Slider() : changeevent("ChangeEvent", this),
			INIT_PROPERTY(Slider, AnimationDuration),
			INIT_PROPERTY(Slider, Orientation),
			INIT_PROPERTY(Slider, ShowTicks),
			INIT_PROPERTY(Slider, TickDistance),
			INIT_PROPERTY(Slider, ShowNumbers),
			INIT_PROPERTY(Slider, NumberDistance),
			INIT_PROPERTY(Slider, Stepsize),
			INIT_PROPERTY(Slider, NumberFormat)
		{
			Base::setorientation(slider::Blueprint::Bottom);
			Base::setupdisplay(true, true, false, false, false);
			Base::setsmoothingmode(true, false, false, true, 100);
			Base::setmarkers(true, true, false, 10, 5);
			Base::setactions(true, Base::Goto, true, true, true);
			Base::setactive();

			ISlider::changeevent.DoubleLink(changeevent);
		}

		virtual WidgetBase *GetWidget() {
			return this;
		}

		template <class O_>
		Slider &operator =(const O_ &value) { 
			(Object.*setter)(value);

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
		
		utils::EventChain<Slider> &ChangeEvent() {
			return changeevent;
		}

	protected:

		utils::EventChain<Slider> changeevent;


		//REQUIRED
		virtual T_ getValue() const {
			return Base::getvalue();
		}
		void setValue(const T_ &value) {
			Base::setvalue(value);
		}
		T_ getMin() const {
			return Base::getmin();
		}
		void setMin(const T_ &value) {
			Base::setmin(value);
		}
		T_ getMax() const {
			return Base::getmax();
		}
		void setMax(const T_ &value) {
			Base::setmax(value);
		}


		int getAnimationDuration() const {
			return int(100000/Base::getsmoothingspeed());
		}
		void setAnimationDuration(const int &value) {
			Base::setsmoothingspeed(100000.f/value);
		}

		OrientationType getOrientation() const {
			return OrientationType(Base::getorientation());
		}
		void setOrientation(const OrientationType &orientation) {
			Base::setaxisinverse(orientation==Left || orientation==Right);

			Base::setorientation(slider::Blueprint::OrientationType(orientation));
		}

		void setShowTicks(const bool &value) {
			Base::setshowticks(value);
		}
		bool getShowTicks() const {
			return Base::getshowticks();
		}

		void setTickDistance(const floattype &value) {
			Base::settickdistance(value);
			Base::setlargechange(T_(value));
		}
		floattype getTickDistance() const {
			return Base::gettickdistance();
		}

		void setShowNumbers(const bool &value) {
			Base::setshownumbers(value);
		}
		bool getShowNumbers() const {
			return Base::getshownumbers();
		}

		void setNumberDistance(const int &value) {
			Base::setnumberdistance(value);
		}
		int getNumberDistance() const {
			return Base::getnumberdistance();
		}

		void setStepsize(const T_ &value) {
			Base::setsteps(value);
			Base::setsmallchange(max(value,1));
		}
		T_ getStepsize() const {
			return Base::getsteps();
		}

		void setNumberFormat(const NumberFormatType &value) {
			Base::setnumberformat(value);
		}
		NumberFormatType getNumberFormat() const {
			return Base::getnumberformat();
		}


		virtual void value_changed() {
			changeevent();
		}

	};

}}
