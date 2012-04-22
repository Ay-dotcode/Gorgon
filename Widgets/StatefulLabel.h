#pragma once


#include "Interfaces\ILabel.h"
#include "Checkbox\CheckboxBase.h"
#include "..\Utils\Property.h"


namespace gge { namespace widgets {

	class StatefulLabel : public ILabel, public checkbox::Base {
	public:
		enum States {
			State_1	= 1,
			State_2,
			State_3
		};

		StatefulLabel(std::string text="") : Base(false, AutosizeModes::None, true, false, true),
			INIT_PROPERTY(StatefulLabel, Autosize),
			INIT_PROPERTY(StatefulLabel, TextWrap),
			INIT_PROPERTY(StatefulLabel, Accesskey),
			INIT_PROPERTY(StatefulLabel, Icon),
			INIT_PROPERTY(StatefulLabel, State)
		{
			Text=text;
			Autosize=AutosizeModes::Autosize;

			if(WR.Labels.Link)
				setblueprint(*WR.Labels.Link);
		}

		StatefulLabel &operator =(const std::string &s) {
			Text=s;

			return *this;
		}

		virtual bool Focus()  {
			return ILabel::default_focusreceived(Container);
		}


		virtual bool MouseHandler(input::mouse::Event::Type event, utils::Point location, int amount) { 
			return ILabel::default_mousehandler(event, location, amount);
		}

		void RemoveAccesskey() {
			Accesskey=0;
		}


		void RemoveIcon() {
			Base::seticon(NULL);
		}

		utils::Property<StatefulLabel, AutosizeModes::Type> Autosize;
		utils::NumericProperty<StatefulLabel, input::keyboard::Key> Accesskey;
		utils::BooleanProperty<StatefulLabel> TextWrap;
		utils::ReferenceProperty<StatefulLabel, graphics::RectangularGraphic2D> Icon;
		utils::Property<StatefulLabel, States> State;

	protected:

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

		void setState(const States &value) {
			Base::setstate(value);
		}
		States getState() const {
			return (States)Base::getstate();
		}

		virtual void wr_loaded() {
			if(!blueprintmodified && WR.Labels.Link)
				setblueprint(*WR.Labels.Link);
		}
	};


}}