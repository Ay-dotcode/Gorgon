#pragma once


#include "Interfaces\IOption.h"
#include "Checkbox\CheckboxBase.h"
#include "..\Utils\Property.h"
#include "Interfaces\ICheckbox.h"


namespace gge { namespace widgets {

	template<class T_=int>
	class RadioButton : public IOption<T_>, public checkbox::Base {
	public:
		RadioButton(std::string text="") : Base(true, AutosizeModes::None, false, true, false),
			INIT_PROPERTY(RadioButton, Autosize),
			INIT_PROPERTY(RadioButton, TextWrap),
			INIT_PROPERTY(RadioButton, Accesskey),
			changeevent("ChangeEvent", this)
		{
			Text=text;
			changeevent.DoubleLink(IOption::changeevent);
			Autosize=AutosizeModes::Autosize;

			if(WR.RadioButton)
				setblueprint(*WR.RadioButton);
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

			return WidgetBase::MouseEvent(event, location, amount);
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

			if((Key==input::keyboard::KeyCodes::Up || Key==input::keyboard::KeyCodes::Left)  && event==input::keyboard::Event::Down && !input::keyboard::Modifier::Check()) {
				if(parent)
					parent->SetToPrev();
			}

			if((Key==input::keyboard::KeyCodes::Down || Key==input::keyboard::KeyCodes::Right)  && event==input::keyboard::Event::Down && !input::keyboard::Modifier::Check()) {
				if(parent)
					parent->SetToNext();
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

		utils::EventChain<RadioButton> &ChangeEvent() {
			return changeevent;
		}

		utils::Property<RadioButton, AutosizeModes::Type> Autosize;
		utils::NumericProperty<RadioButton, input::keyboard::Key> Accesskey;
		utils::BooleanProperty<RadioButton> TextWrap;

	protected:

		virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
			Base::located(container, w, Order);

			if(container && getunderline())
				container->SetAccessKey(*this, getunderline());
		}

		void change() {
			setState(CheckboxState::Checked);
		}

		utils::EventChain<RadioButton> changeevent;

		virtual void setState(const bool &state) {
			if(state) {
				Base::setstate(2);
			}
			else {
				Base::setstate(1);
			}
		}
		virtual bool getState() const {
			return Base::getstate()==2;
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
			if(!blueprintmodified && WR.RadioButton)
				setblueprint(*WR.RadioButton);
		}
	};

	template<class T_>
	class RadioGroup : public OptionGroup<T_, RadioButton<T_> > {
	public:

		RadioGroup &operator = (const T_ &value) { Set(value); }
	};

	template<class T_>
	static ContainerBase &operator <<(ContainerBase &container, RadioGroup<T_> &rg) {
		for(auto i=rg.First();i.IsValid();i.Next()) {
			container<<*i;
		}

		return container;
	}

	//template<class T_>
	//typedef OptionGroup<T_, RadioButton<T_> > RadioGroup;

}}