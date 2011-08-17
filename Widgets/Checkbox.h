#pragma once


#include "Interfaces\ICheckbox.h"
#include "Checkbox\CheckboxBase.h"
#include "..\Utils\Property.h"


namespace gge { namespace widgets {

	class Checkbox : public ICheckbox, public checkbox::Base {
	public:
		Checkbox(std::string text="") : Base(true, AutosizeModes::None, false, true, false),
			INIT_PROPERTY(Checkbox, Autosize),
			INIT_PROPERTY(Checkbox, TextWrap),
			INIT_PROPERTY(Checkbox, Accesskey),
			changeevent("ChangeEvent", this),
			BiasToChecked(true)
		{
			Text=text;
			changeevent.DoubleLink(ICheckbox::changeevent);
		}

		template<class T_>
		Checkbox(T_ fn, std::string text="") : Base(true, AutosizeModes::None, false, true, false),
			INIT_PROPERTY(Checkbox, Autosize),
			INIT_PROPERTY(Checkbox, TextWrap),
			INIT_PROPERTY(Checkbox, Accesskey),
			changeevent("ClickEvent", this),
			BiasToChecked(true)
		{
			Text=text;
			changeevent.DoubleLink(ICheckbox::clickevent);

			changeevent.Register(fn);
		}
		

		void RemoveAccesskey() {
			Accesskey=0;
		}


		virtual bool MouseEvent(input::mouse::Event::Type event, utils::Point location, int amount) { 
			//handle mouse events

			if(input::mouse::Event::isDown(event)) {
				Focus();
			}

			switch (event) {
			case input::mouse::Event::Left_Down:
				Base::down();
				break;
			case input::mouse::Event::Left_Up:
				Base::up();
				break;
			case input::mouse::Event::Over:
				Base::over();
				break;
			case input::mouse::Event::Out:
				Base::out();
				break;
			case input::mouse::Event::Left_Click:
				if(IsEnabled()) {
					change();
					changeevent();
				}

				break;
			}

			return !input::mouse::Event::isScroll(event);
		}

		virtual bool KeyboardEvent(input::keyboard::Event::Type event, input::keyboard::Key Key) {
			if(!IsEnabled())
				return false;

			if(Key==input::keyboard::KeyCodes::Space && event==input::keyboard::Event::Down && !input::keyboard::Modifier::Check()) {
				Base::down();
				return true;
			}
			else if(Key==input::keyboard::KeyCodes::Space && event==input::keyboard::Event::Up && !input::keyboard::Modifier::Check()) {
				Base::up();
				change();
				changeevent();

				return true;
			}


			return false;
		}

		virtual bool Accessed() {
			if(!IsEnabled())
				return false;

			Base::click();
			change();
			changeevent();

			return true;
		}

		utils::Property<Checkbox, AutosizeModes::Type> Autosize;
		utils::NumericProperty<Checkbox, input::keyboard::Key> Accesskey;
		utils::BooleanProperty<Checkbox> TextWrap;

		//if true clicking on the checkbox while its state is unknown makes it checked
		//otherwise it will make it unchecked
		bool BiasToChecked;

	protected:

		void change() {
			switch(Base::getstate()) {
			case 1:
				setState(CheckboxState::Checked);
				break;
			case 2:
				setState(CheckboxState::Unchecked);
				break;
			case 3:
			default:
				if(BiasToChecked)
					setState(CheckboxState::Checked);
				else
					setState(CheckboxState::Unchecked);

				break;
			}
		}

		utils::EventChain<Checkbox> changeevent;

		virtual void setState(const CheckboxState::Type &state) {
			switch(state) {
			case CheckboxState::Unchecked:
				Base::setstate(1);
				break;
			case CheckboxState::Checked:
				Base::setstate(2);
				break;
			case CheckboxState::Unknown:
			default:
				Base::setstate(3);
				break;
			}
		}
		virtual CheckboxState::Type getState() const {
			switch(Base::getstate()) {
			case 1:
				return CheckboxState::Unchecked;
			case 2:
				return CheckboxState::Checked;
			case 3:
			default:
				return CheckboxState::Unknown;
			}
		}

		AutosizeModes::Type getAutosize() const {
			return Base::getautosize();
		}

		void setAutosize(const AutosizeModes::Type &autosize) {
			Base::setautosize(autosize);
		}

		bool getTextWrap() const {
			return Base::gettextwrap();
		}

		void setTextWrap(const bool &wrap) {
			Base::settextwrap(wrap);
		}

		input::keyboard::Key getAccesskey() const {
			return Base::getunderline();
		}

		void setAccesskey(const input::keyboard::Key &key) {
			Container->SetAccessKey(*this, key);
			Base::setunderline(key);
		}

		virtual void accesskeystatechanged() {
			Base::setunderline(Container->GetAccessKey(*this));
		}

		virtual std::string getText() const {
			return Base::gettext();
		}

		virtual void setText(const std::string &text)  {
			Base::settext(text);
		}
	};


}}