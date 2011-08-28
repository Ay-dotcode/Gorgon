#pragma once
#include "Interfaces\IScroller.h"
#include "Slider\SliderBase.h"

namespace gge { namespace widgets {

	template<class T_=int, class floattype=typename slider::floattype<T_>::Type>
	class Scrollbar : public IScroller<T_>, public slider::Base<T_> {
	public:
		enum OrientationType {
			Vertical	= slider::Blueprint::Vertical,
			Horizontal	= slider::Blueprint::Horizontal,
		};

		typedef Base::numberformat NumberFormatType;

		Scrollbar() : changeevent("ChangeEvent", this),
			INIT_PROPERTY(Scrollbar, AnimationDuration),
			INIT_PROPERTY(Scrollbar, Orientation),
			INIT_PROPERTY(Scrollbar, LargeChange),
			INIT_PROPERTY(Scrollbar, SmallChange)
		{
			Base::setorientation(slider::Blueprint::Vertical);
			Base::setupdisplay(true, true, false, true, false);
			Base::setsmoothingmode(true, false, true, false, 100);
			Base::setmarkers(false, false, false);
			Base::setactions(true, Base::LargeChange, true);
			Base::setsmallchange(5);
			Base::setactive();

			IScroller::changeevent.DoubleLink(changeevent);
		}

		virtual WidgetBase *GetWidget() {
			return this;
		}

		template <class O_>
		Scrollbar &operator =(const O_ &value) { 
			(Object.*setter)(value);

			return *this;
		}

		utils::NumericProperty<Scrollbar, int> AnimationDuration;
		utils::Property<Scrollbar, OrientationType> Orientation;
		utils::NumericProperty<Scrollbar, T_> LargeChange;
		utils::NumericProperty<Scrollbar, T_> SmallChange;

		utils::EventChain<Scrollbar> &ChangeEvent() {
			return changeevent;
		}

	protected:

		utils::EventChain<Scrollbar> changeevent;


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
			Base::setorientation(slider::Blueprint::OrientationType(orientation));
		}

		void setLargeChange(const T_ &value) {
			Base::setlargechange(value);
		}
		T_ getLargeChange() const {
			return Base::getlargechange();
		}
		void setSmallChange(const T_ &value) {
			Base::setsmallchange(value);
		}
		T_ getSmallChange() const {
			return Base::getsmallchange();
		}



		virtual void value_changed() {
			changeevent();
		}

	};

}}
