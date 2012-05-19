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

		IProgressor() : NumericProperty(this, &IProgressor::getValue, &IProgressor::setValue),
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
			(Object.*setter)(value);

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
		template<class T_>
		bool default_focusreceived(T_ *Container) {
			return false;
		}

	};

}}
