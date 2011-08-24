#pragma once


#include <string>
#include "..\Base\Widget.h"
#include "..\..\Utils\Property.h"
#include "..\..\Utils\EventChain.h"
#include "..\Base\Container.h"



namespace gge { namespace widgets {

	template<class T_>
	class ISlider : public utils::NumericProperty<ISlider<T_>, T_> {
	public:

		ISlider() : NumericProperty(this, &ISlider::getValue, &ISlider::setValue),
			Value(*this),
			changeevent("ChangeEvent", this),			
			INIT_PROPERTY(ISlider, Min),
			INIT_PROPERTY(ISlider, Max)
		{ }

		//utils::NumericProperty<ISlider, T_> Value;
		utils::NumericProperty<ISlider, T_> Min;
		utils::NumericProperty<ISlider, T_> Max;

		virtual WidgetBase *GetWidget() {
			return NULL;
		}

		template <class O_>
		ISlider &operator =(const O_ &value) { 
			(Object.*setter)(value);

			return *this;
		}

		utils::EventChain<ISlider> &ChangeEvent() {
			return changeevent;
		}

		NumericProperty<ISlider, T_> &Value;

	protected:
		//REQUIRED
		virtual T_ getValue() const = 0;
		virtual void setValue(const T_ &value) = 0;
		virtual T_ getMin() const = 0;
		virtual void setMin(const T_ &value) = 0;
		virtual T_ getMax() const = 0;
		virtual void setMax(const T_ &value) = 0;

		utils::EventChain<ISlider> changeevent;

		//SUPPLIED

	};

}}
