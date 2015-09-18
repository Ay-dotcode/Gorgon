#pragma once


#include <string>
#include "../Base/Widget.h"
#include "../../Utils/Property.h"
#include "../../Utils/EventChain.h"
#include "../Base/Container.h"



namespace gge { namespace widgets {

	template<class T_>
	class IProgressor : public utils::NumericProperty<IProgressor<T_>, T_> {
	public:

		IProgressor() : utils::NumericProperty<IProgressor<T_>, T_>(this, &IProgressor::getValue, &IProgressor::setValue),
			INIT_PROPERTY(IProgressor, Min),
			Value(*this),
			INIT_PROPERTY(IProgressor, Max)
		{ }

		//utils::NumericProperty<IProgressor, T_> Value;
		utils::NumericProperty<IProgressor, T_> Min;
		utils::NumericProperty<IProgressor, T_> Max;
		utils::NumericProperty<IProgressor, T_> &Value;

		virtual WidgetBase *GetWidget() {
			return NULL;
		}

		template <class O_>
		IProgressor &operator =(const O_ &value) { 
			(this->Object.*this->setter)(value);

			return *this;
		}


	protected:
		//REQUIRED
		virtual T_ getValue() const = 0;
		virtual void setValue(const T_ &value) = 0;
		virtual T_ getMin() const = 0;
		virtual void setMin(const T_ &value) = 0;
		virtual T_ getMax() const = 0;
		virtual void setMax(const T_ &value) = 0;

		//SUPPLIED
		template<class C_>
		bool default_focusreceived(C_ *Container) {
			return false;
		}

	};

}}
