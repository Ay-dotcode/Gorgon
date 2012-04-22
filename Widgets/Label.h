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
			INIT_PROPERTY(Label, Accesskey),
			INIT_PROPERTY(Label, Icon)
		{
			Text=text;
			Autosize=AutosizeModes::Autosize;

			if(WR.Label)
				setblueprint(*WR.Label);
		}

		Label &operator =(const std::string &s) {
			Text=s;

			return *this;
		}

		virtual bool Focus()  {
			return false;//ILabel::default_focusreceived(Container);
		}

		virtual bool Accessed() {
			auto it=Container->Widgets.Find(this);
			it.Next();
			if(it.IsValid())
				it->Focus();

			return true;
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

		utils::Property<Label, AutosizeModes::Type> Autosize;
		utils::NumericProperty<Label, input::keyboard::Key> Accesskey;
		utils::BooleanProperty<Label> TextWrap;
		utils::ReferenceProperty<Label, graphics::RectangularGraphic2D> Icon;

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

		virtual void wr_loaded() {
			if(!blueprintmodified && WR.Label)
				setblueprint(*WR.Label);
		}

	};


}}