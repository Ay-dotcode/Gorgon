#pragma once


#include <string>
#include "..\Base\Widget.h"
#include "..\..\Utils\Property.h"
#include "..\..\Utils\EventChain.h"
#include "..\Base\Container.h"
#include "..\..\Engine\Graphic2D.h"



namespace gge { namespace widgets {

	template<class T_>
	class INumberbox {
	public:

		INumberbox() : 
		  INIT_PROPERTY(INumberbox,Value),
			  changeevent("ChangeEvent", this),
			  KeyEvent("KeyEvent", this)
		  { }

		  utils::NumericProperty<INumberbox, T_> Value;


		  operator T_() const {
			  return Value;
		  }

		  INumberbox &operator =(const T_ &s) {
			  Text=s;

			  return *this;
		  }

		  virtual WidgetBase *GetWidget() {
			  return NULL;
		  }

		  utils::EventChain<INumberbox> &ChangeEvent() { return changeevent; }
		  utils::EventChain<INumberbox, input::keyboard::Event> KeyEvent;

	protected:
		//REQUIRED
		virtual void setValue(const T_ &value) = 0;
		virtual T_ getValue() const = 0;

		utils::EventChain<INumberbox> changeevent;
	};


} }
