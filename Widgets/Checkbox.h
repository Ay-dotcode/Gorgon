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
			ChangeEvent("ChangeEvent", this),
			BiasToChecked(true)
		{
			Text=text;
			ChangeEvent.DoubleLink(ICheckbox::ChangeEvent);
			Autosize=AutosizeModes::Autosize;

			if(WR.Checkbox)
				setblueprint(*WR.Checkbox);
		}

		template<class T_>
		Checkbox(T_ fn, std::string text="") : Base(true, AutosizeModes::None, false, true, false),
			INIT_PROPERTY(Checkbox, Autosize),
			INIT_PROPERTY(Checkbox, TextWrap),
			INIT_PROPERTY(Checkbox, Accesskey),
			ChangeEvent("ClickEvent", this),
			BiasToChecked(true)
		{
			Text=text;
			ChangeEvent.DoubleLink(ICheckbox::ChangeEvent);

			ChangeEvent.Register(fn);
			Autosize=AutosizeModes::Autosize;

			if(WR.Checkbox)
				setblueprint(*WR.Checkbox);
		}
		

		void RemoveAccesskey() {
			Accesskey=0;
		}

		Checkbox &operator =(bool checked) {
			if(checked)
				setState(CheckboxState::Checked);
			else
				setState(CheckboxState::Unchecked);

			return *this;
		}

		virtual bool MouseHandler(input::mouse::Event::Type event, utils::Point location, int amount) { 
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
					ChangeEvent();
				}

				break;
			}

			return WidgetBase::MouseHandler(event, location, amount);
		}

		virtual bool KeyboardHandler(input::keyboard::Event::Type event, input::keyboard::Key Key) {
			if(!IsEnabled())
				return false;

			if(Key==input::keyboard::KeyCodes::Space && event==input::keyboard::Event::Down && !input::keyboard::Modifier::Check()) {
				Base::down();
				return true;
			}
			else if(Key==input::keyboard::KeyCodes::Space && event==input::keyboard::Event::Up && !input::keyboard::Modifier::Check()) {
				Base::up();
				change();
				ChangeEvent();

				return true;
			}


			return false;
		}

		virtual bool Accessed() {
			if(!IsEnabled())
				return false;

			Base::click();
			change();
			ChangeEvent();

			return true;
		}

		utils::EventChain<Checkbox> ChangeEvent;


		utils::Property<Checkbox, AutosizeModes::Type> Autosize;
		utils::NumericProperty<Checkbox, input::keyboard::Key> Accesskey;
		utils::BooleanProperty<Checkbox> TextWrap;

		//if true clicking on the checkbox while its state is unknown makes it checked
		//otherwise it will make it unchecked
		bool BiasToChecked;

	protected:

		virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
			Base::located(container, w, Order);

			if(container && getunderline())
				container->SetAccessKey(*this, getunderline());
		}

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
			if(Container) {
				Container->SetAccessKey(*this, key);
			}
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

		virtual void wr_loaded() {
			if(!blueprintmodified && WR.Checkbox)
				setblueprint(*WR.Checkbox);
		}
	};


}}