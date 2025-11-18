#pragma once


#include <string>
#include "../Base/Widget.h"
#include "../../Utils/Property.h"
#include "../../Utils/EventChain.h"
#include "../Base/Container.h"



namespace gge { namespace widgets {

	template<class T_=int>
	class IScroller : public utils::NumericProperty<IScroller<T_>, T_> {
	public:

		IScroller() : utils::NumericProperty<IScroller<T_>, T_>(this, &IScroller::getValue, &IScroller::setValue),
			Value(*this),
			ChangeEvent("ChangeEvent", this),			
			INIT_PROPERTY(IScroller, Min),
			INIT_PROPERTY(IScroller, Max)
		{ }

		//utils::NumericProperty<IScrollbar, T_> Value;
		utils::NumericProperty<IScroller, T_> Min;
		utils::NumericProperty<IScroller, T_> Max;

		virtual WidgetBase *GetWidget() {
			return NULL;
		}

		template <class O_>
		IScroller &operator =(const O_ &value) { 
			(this->Object.*this->setter)(value);

			return *this;
		}

		utils::EventChain<IScroller> ChangeEvent;

		utils::NumericProperty<IScroller, T_> &Value;

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
