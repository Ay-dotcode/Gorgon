#pragma once


#include <string>
#include "..\Base\Widget.h"
#include "..\..\Utils\Property.h"
#include "..\..\Utils\EventChain.h"
#include "..\Base\Container.h"
#include "..\..\Engine\Graphic2D.h"



namespace gge { namespace widgets {

	class ITextbox {
	public:

		ITextbox() : 
		  INIT_PROPERTY(ITextbox,Text),
			  changeevent("ChangeEvent", this)
		  { }

		  utils::TextualProperty<ITextbox> Text;


		  operator std::string() const {
			  return Text;
		  }

		  ITextbox &operator =(const std::string &s) {
			  Text=s;

			  return *this;
		  }

		  virtual WidgetBase *GetWidget() {
			  return NULL;
		  }

		  utils::EventChain<ITextbox> &ChangeEvent() { return changeevent; }

	protected:
		//REQUIRED
		virtual void setText(const std::string &text) = 0;
		virtual std::string getText() const = 0;

		utils::EventChain<ITextbox> changeevent;
	};


} }
