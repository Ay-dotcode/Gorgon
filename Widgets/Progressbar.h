#pragma once
#include "Interfaces\IProgressor.h"
#include "Slider\SliderBase.h"

namespace gge { namespace widgets {

	template<class T_=int>
	class Progressbar : public IProgressor<T_>, public slider::Base<T_> {
	public:
		Progressbar() : INIT_PROPERTY(Progressbar, AnimationDuration)
		{
			Base::setorientation(slider::Blueprint::Horizontal);
			Base::setupdisplay(false, true, true, false, false);
			Base::setsmoothingmode(false, true, false, true, 100);
			Base::setmarkers(false, false, false);
			setpassive();

			if(WR.Progressbar)
				setblueprint(*WR.Progressbar);
		}

		virtual WidgetBase *GetWidget() {
			return this;
		}

		template <class O_>
		Progressbar &operator =(const O_ &value) { 
			(Object.*setter)(value);

			return *this;
		}

		utils::NumericProperty<Progressbar, int> AnimationDuration;

	protected:
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

		virtual void wr_loaded() {
			if(WR.Progressbar && !blueprintmodified)
				setblueprint(*WR.Progressbar);
		}

	};

}}
