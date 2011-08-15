#pragma once


#include "Interfaces\ILabel.h"
#include "Checkbox\CheckboxBase.h"
#include "..\Utils\Property.h"


namespace gge { namespace widgets {

	class Label : public ILabel, public checkbox::Base {
	public:
		Label(std::string text="") : Base(false, AutosizeModes::None, true, false, true),
			INIT_PROPERTY(Label, Autosize),
			INIT_PROPERTY(Label, TextWrap),
			INIT_PROPERTY(Label, Accesskey)
		{
			Text=text;
		}

		Label &operator =(const std::string &s) {
			Text=s;

			return *this;
		}

		virtual bool Focus()  {
			return ILabel::default_focusreceived(Container);
		}


		virtual bool MouseEvent(input::mouse::Event::Type event, utils::Point location, int amount) { 
			return ILabel::default_mousehandler(event, location, amount);
		}

		void RemoveAccesskey() {
			Accesskey=0;
		}


		const graphics::RectangularGraphic2D *GetIcon() const {
			return Base::geticon();
		}

		void SetIcon(const graphics::RectangularGraphic2D *icon) {
			Base::seticon(icon);
		}

		void SetIcon(const graphics::RectangularGraphic2D &icon) {
			Base::seticon(&icon);
		}
		
		void RemoveIcon() {
			Base::seticon(NULL);
		}

		utils::Property<Label, AutosizeModes::Type> Autosize;
		utils::NumericProperty<Label, input::keyboard::Key> Accesskey;
		utils::BooleanProperty<Label> TextWrap;

	protected:

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

		virtual std::string gettext() const {
			return Base::gettext();
		}

		virtual void settext(const std::string &text)  {
			Base::settext(text);
		}
	};


}}