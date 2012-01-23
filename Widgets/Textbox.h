#pragma once


#include "Interfaces\ITextbox.h"
#include "Textbox\TextboxBase.h"
#include "..\Utils\Property.h"


namespace gge { namespace widgets {

	class Textbox : public ITextbox, public textbox::Base {
	public:
		Textbox(std::string text="") : Base(),
			changeevent("ChangeEvent", this),
			INIT_PROPERTY(Textbox, CaretLocation),
			AutoSelectAll(false)
		{
			Text=text;
			changeevent.DoubleLink(ITextbox::changeevent);

			setupvscroll(false, false, false);
		}

		Textbox &operator =(const std::string &s) {
			Text=s;

			return *this;
		}

		utils::EventChain<Textbox> &ChangeEvent() {
			return changeevent;
		}

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
		bool AutoSelectAll;


	protected:

		utils::EventChain<Textbox> changeevent;

		virtual void textchanged() {
			changeevent();
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

	};


}}