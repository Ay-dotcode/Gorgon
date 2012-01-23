#pragma once


#include "Interfaces\IButton.h"
#include "Checkbox\CheckboxBase.h"
#include "..\Utils\Property.h"


namespace gge { namespace widgets {

	class Button : public IButton, public checkbox::Base {
	public:
		Button(std::string text="") : Base(true, AutosizeModes::None, false, false, true),
			INIT_PROPERTY(Button, Autosize),
			INIT_PROPERTY(Button, TextWrap),
			INIT_PROPERTY(Button, Accesskey),
			clickevent("ClickEvent", this)
		{
			Text=text;
			clickevent.DoubleLink(IButton::clickevent);
		}

		template<class T_>
		Button(T_ fn, std::string text="") : Base(true, AutosizeModes::None, true, false, true),
			INIT_PROPERTY(Button, Autosize),
			INIT_PROPERTY(Button, TextWrap),
			INIT_PROPERTY(Button, Accesskey),
			clickevent("ClickEvent", this)
		{
			Text=text;
			clickevent.DoubleLink(IButton::clickevent);

			clickevent.Register(fn);
		}

		Button &operator =(const std::string &s) {
			Text=s;

			return *this;
		}


		void RemoveAccesskey() {
			Accesskey=0;
		}

		void RemoveIcon() {
			Base::seticon(NULL);
		}

		virtual bool MouseEvent(input::mouse::Event::Type event, utils::Point location, int amount) { 
			//handle mouse events
			
			bool ret=false;

			if(input::mouse::Event::isDown(event)) {
				Focus();
				ret = true;
			}

			switch (event) {
			case input::mouse::Event::Left_Down:
				Base::down();
				ret = true;
				break;
			case input::mouse::Event::Left_Up:
				Base::up();
				ret = true;
				break;
			case input::mouse::Event::Over:
				Base::over();
				ret = true;
				break;
			case input::mouse::Event::Out:
				Base::out();
				ret = true;
				break;
			case input::mouse::Event::Left_Click:
				if(IsEnabled()) {
					playsound(Blueprint::NotFocused, Blueprint::Focus_None, 1,0,Blueprint::Down, Blueprint::Style_None);
					clickevent();
				}

				ret = true;
				break;
			}

			mouseevent(input::mouse::Event(event, location, amount));

			if(ret) 
				return true;
				
			return WidgetBase::MouseEvent(event, location, amount);
		}

		virtual bool KeyboardEvent(input::keyboard::Event::Type event, input::keyboard::Key Key) {
			if(!IsEnabled())
				return false;

			if(Key==input::keyboard::KeyCodes::Enter && event==input::keyboard::Event::Char && !input::keyboard::Modifier::Check()) {
				Fire();
				return true;
			}
			else if(Key==input::keyboard::KeyCodes::Space && event==input::keyboard::Event::Down && !input::keyboard::Modifier::Check()) {
				Base::down();
				return true;
			}
			else if(Key==input::keyboard::KeyCodes::Space && event==input::keyboard::Event::Up && !input::keyboard::Modifier::Check()) {
				Base::up();
				playsound(Blueprint::NotFocused, Blueprint::Focus_None, 1,0,Blueprint::Down, Blueprint::Style_None);
				clickevent();

				return true;
			}


			return false;
		}

		void Fire() {
			if(!IsEnabled())
				return;

			Base::click();
			playsound(Blueprint::NotFocused, Blueprint::Focus_None, 1,0,Blueprint::Down, Blueprint::Style_None);
			clickevent();
		}

		virtual bool Accessed() {
			if(!IsEnabled())
				return false;

			Fire();

			return true;
		}

		utils::Property<Button, AutosizeModes::Type> Autosize;
		utils::NumericProperty<Button, input::keyboard::Key> Accesskey;
		utils::BooleanProperty<Button> TextWrap;

		utils::EventChain<Button> &ClickEvent() {
			return clickevent;
		}

		utils::EventChain<Button, input::mouse::Event> &MouseEvent() {
			return mouseevent;
		}


		virtual WidgetBase *GetWidget() {
			return this;
		}

	protected:

		utils::EventChain<Button> clickevent;

		utils::EventChain<Button, input::mouse::Event> mouseevent;

		virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
			Base::located(container, w, Order);
			
			if(container && getunderline())
				container->SetAccessKey(*this, getunderline());
		}

		graphics::RectangularGraphic2D *getIcon() const {
			return Base::geticon();
		}

		void setIcon(graphics::RectangularGraphic2D *icon) {
			Base::seticon(icon);
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
	};


}}