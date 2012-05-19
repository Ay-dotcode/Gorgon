#pragma once
#include "Interfaces/IScroller.h"
#include "Slider/SliderBase.h"

namespace gge { namespace widgets {

	template<class T_=int, class floattype=typename slider::floattype<T_>::Type>
	class Scrollbar : public IScroller<T_>, public slider::Base<T_> {
	public:
		enum OrientationType {
			Vertical	= slider::Blueprint::Vertical,
			Horizontal	= slider::Blueprint::Horizontal,
		};

		typedef typename slider::Base<T_>::numberformat NumberFormatType;

		Scrollbar() : ChangeEvent("ChangeEvent", this),
			INIT_PROPERTY(Scrollbar, AnimationDuration),
			INIT_PROPERTY(Scrollbar, Orientation),
			INIT_PROPERTY(Scrollbar, LargeChange),
			INIT_PROPERTY(Scrollbar, SmallChange),
			INIT_PROPERTY(Scrollbar, AllowFocus)
		{
			slider::Base<T_>::setorientation(slider::Blueprint::Vertical);
			slider::Base<T_>::setupdisplay(true, true, false, true, false);
			slider::Base<T_>::setsmoothingmode(true, false, true, false, 100);
			slider::Base<T_>::setmarkers(false, false, false);
			slider::Base<T_>::setactions(true, slider::Base<T_>::LargeChange, true);
			slider::Base<T_>::setsmallchange(18);
			slider::Base<T_>::setlargechange(120);
			slider::Base<T_>::setactive();

			IScroller<T_>::ChangeEvent.DoubleLink(ChangeEvent);

			if(WR.Scrollbar)
				this->setblueprint(*WR.Scrollbar);
		}

		virtual WidgetBase *GetWidget() {
			return this;	
		}

		template <class O_>
		Scrollbar &operator =(const O_ &value) { 
			(this->Object.*this->setter)(value);

			return *this;
		}

		utils::NumericProperty<Scrollbar, int> AnimationDuration;
		utils::Property<Scrollbar, OrientationType> Orientation;
		utils::NumericProperty<Scrollbar, T_> LargeChange;
		utils::NumericProperty<Scrollbar, T_> SmallChange;
		utils::BooleanProperty<Scrollbar> AllowFocus;

		utils::EventChain<Scrollbar> ChangeEvent;

	protected:



		//REQUIRED
		virtual T_ getValue() const {
			return slider::Base<T_>::getvalue();
		}
		void setValue(const T_ &value) {
			if(value!=slider::Base<T_>::gettargetvalue()) {
				slider::Base<T_>::setvalue(value);
				ChangeEvent();
			}
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
			return int(100000/slider::Base<T_>::getsmoothingspeed());
		}
		void setAnimationDuration(const int &value) {
			slider::Base<T_>::setsmoothingspeed(100000.f/value);
		}

		OrientationType getOrientation() const {
			return OrientationType(slider::Base<T_>::getorientation());
		}
		void setOrientation(const OrientationType &orientation) {
			slider::Base<T_>::setorientation(slider::Blueprint::OrientationType(orientation));
		}

		void setLargeChange(const T_ &value) {
			slider::Base<T_>::setlargechange(value);
		}
		T_ getLargeChange() const {
			return slider::Base<T_>::getlargechange();
		}
		void setSmallChange(const T_ &value) {
			slider::Base<T_>::setsmallchange(value);
		}
		T_ getSmallChange() const {
			return slider::Base<T_>::getsmallchange();
		}

		void setAllowFocus(const bool &value) {
			slider::Base<T_>::setcangetfocus(value);
		}
		bool getAllowFocus() const {
			return slider::Base<T_>::getcangetfocus();
		}


		virtual void wr_loaded() {
			if(WR.Scrollbar && !this->blueprintmodified)
				this->setblueprint(*WR.Scrollbar);
		}


		virtual void value_changed() {
			ChangeEvent();
		}

	};

}}
