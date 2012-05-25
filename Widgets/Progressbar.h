#pragma once
#include "Interfaces/IProgressor.h"
#include "Slider/SliderBase.h"

namespace gge { namespace widgets {

	template<class T_=int>
	class Progressbar : public IProgressor<T_>, public slider::Base<T_> {
	public:
		Progressbar() : INIT_PROPERTY(Progressbar, AnimationDuration)
		{
			slider::Base<T_>::setorientation(slider::Blueprint::Horizontal);
			slider::Base<T_>::setupdisplay(false, true, true, false, false);
			slider::Base<T_>::setsmoothingmode(false, true, false, true, 100);
			slider::Base<T_>::setmarkers(false, false, false);
			this->setpassive();

			if(WR.Progressbar)
				this->setblueprint(*WR.Progressbar);
		}

		virtual WidgetBase *GetWidget() {
			return this;
		}

		template <class O_>
		Progressbar &operator =(const O_ &value) { 
			(this->Object.*this->setter)(value);

			return *this;
		}

		void SetValue(const T_ &value) {
			slider::Base<T_>::instantsetvalue(value);
		}

		utils::NumericProperty<Progressbar, int> AnimationDuration;

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
			return int(100000/slider::Base<T_>::getsmoothingspeed());
		}
		void setAnimationDuration(const int &value) {
			slider::Base<T_>::setsmoothingspeed(100000.f/value);
		}

		virtual void wr_loaded() {
			if(WR.Progressbar && !this->blueprintmodified)
				this->setblueprint(*WR.Progressbar);
		}

	};

}}
