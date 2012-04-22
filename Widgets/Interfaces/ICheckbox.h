#pragma once


#include <string>
#include "..\Base\Widget.h"
#include "..\..\Utils\Property.h"
#include "..\..\Utils\EventChain.h"
#include "..\Base\Container.h"
#include "..\..\Engine\Graphic2D.h"



namespace gge { namespace widgets {

	namespace CheckboxState {
		enum Type {
			Unchecked= 0,
			Checked  = 1,
			Unknown  = 2
		};
	}

	class ICheckbox {
	public:

		ICheckbox() : ChangeEvent("ChangeEvent", this),
			INIT_PROPERTY(ICheckbox,Text),
			INIT_PROPERTY(ICheckbox,State)
		  { }

		  utils::TextualProperty<ICheckbox> Text;
		  utils::Property<ICheckbox, CheckboxState::Type> State;

		  void Check() {
			  State=CheckboxState::Checked;
		  }
		  void Uncheck() {
			  State=CheckboxState::Unchecked;
		  }
		  operator bool() {
			  return State!=CheckboxState::Unchecked;
		  }

		  utils::EventChain<ICheckbox> ChangeEvent;

	protected:
		//REQUIRED
		virtual void setState(const CheckboxState::Type &state) = 0;
		virtual CheckboxState::Type getState() const = 0;
		virtual void setText(const std::string &text) = 0;
		virtual std::string getText() const = 0;

	};


} }
