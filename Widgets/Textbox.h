#pragma once


#include "Interfaces/ITextbox.h"
#include "Textbox/TextboxBase.h"
#include "../Utils/Property.h"


namespace gge { namespace widgets {

	class Textbox : public ITextbox, public textbox::Base {
	public:
		Textbox(std::string text="") : Base(),
			ChangeEvent("ChangeEvent", this),
			INIT_PROPERTY(Textbox, CaretLocation),
			INIT_PROPERTY(Textbox, Readonly),
			INIT_PROPERTY(Textbox, Prefix),
			INIT_PROPERTY(Textbox, Suffix),
			AutoSelectAll(false)
		{
			Text=text;
			ChangeEvent.DoubleLink(ITextbox::ChangeEvent);

			setupvscroll(false, false, false);

			if(WR.Textbox)
				setblueprint(*WR.Textbox);
		}

		Textbox &operator =(const std::string &s) {
			Text=s;

			return *this;
		}

		utils::EventChain<Textbox> ChangeEvent;

		void SelectAll() {
			Base::setselection(0, gettext().length());
		}

		void Select(int start, int end) {
			Base::setselection(start, end);
		}

		virtual bool Focus() {
			if(!Base::Focus())
				return false;

			if(AutoSelectAll)
				SelectAll();

			return true;
		}


		virtual WidgetBase *GetWidget() {
			return this;
		}

		utils::NumericProperty<Textbox, int> CaretLocation;
		utils::BooleanProperty<Textbox> Readonly;
		bool AutoSelectAll;
		utils::TextualProperty<Textbox> Prefix, Suffix;


	protected:


		virtual void textchanged() {
			ChangeEvent();
		}

		virtual std::string getText() const {
			return Base::gettext();
		}
		virtual void setText(const std::string &text)  {
			Base::settext(text);
		}

		void setCaretLocation(const int &value) {
			Base::setcaretlocation(value);
		}
		int getCaretLocation() const {
			return Base::getcaretlocation();
		}

		void setReadonly(const bool &value) {
			setreadonly(value);
		}
		bool getReadonly() const {
			return getreadonly();
		}

		void setPrefix(const std::string &value) {
			setprefix(value);
		}
		std::string getPrefix() const {
			return getprefix();
		}

		void setSuffix(const std::string &value) {
			setsuffix(value);
		}
		std::string getSuffix() const {
			return getsuffix();
		}

		virtual void wr_loaded() {
			if(WR.Textbox && !blueprintmodified)
				setblueprint(*WR.Textbox);
		}

	};


}}