#pragma once


#include "..\Interfaces\ITextbox.h"
#include "..\Textbox\TextboxBase.h"
#include "..\Utils\Property.h"


namespace gge { namespace widgets {

	class PetTextbox : public ITextbox, public textbox::Base {
	public:
		PetTextbox(std::string text="") : Base(),
			changeevent("ChangeEvent", this),
			INIT_PROPERTY(PetTextbox, CaretLocation),
			INIT_PROPERTY(PetTextbox, Passive),
			INIT_PROPERTY(PetTextbox, Readonly),
			INIT_PROPERTY(PetTextbox, NoSelection),
			INIT_PROPERTY(PetTextbox, Prefix),
			INIT_PROPERTY(PetTextbox, Suffix),
			AutoSelectAll(false)
		{
			Text=text;
			changeevent.DoubleLink(ITextbox::changeevent);

			setupvscroll(false, false, false);
		}

		PetTextbox &operator =(const std::string &s) {
			Text=s;

			return *this;
		}

		utils::EventChain<PetTextbox> &ChangeEvent() {
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

		virtual bool MouseEvent(input::mouse::Event::Type event, utils::Point location, int amount) {
			if(MouseEventOccured(input::mouse::Event(event, location, amount)))
				return true;
			else
				return Base::MouseEvent(event, location, amount);
		}



		utils::NumericProperty<PetTextbox, int> CaretLocation;
		utils::BooleanProperty<PetTextbox> Passive;
		utils::BooleanProperty<PetTextbox> Readonly;
		utils::BooleanProperty<PetTextbox> NoSelection;
		utils::TextualProperty<PetTextbox> Prefix;
		utils::TextualProperty<PetTextbox> Suffix;
		bool AutoSelectAll;

		utils::ConsumableEvent<PetTextbox, input::mouse::Event> MouseEventOccured;

	protected:

		utils::EventChain<PetTextbox> changeevent;

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

		void setPassive(const bool &value) {
			setpassive(value);
		}
		bool getPassive() const {
			return getpassive();
		}

		void setReadonly(const bool &value) {
			setreadonly(value);
		}
		bool getReadonly() const {
			return getreadonly();
		}

		void setNoSelection(const bool &value) {
			setnoselection(value);
		}
		bool getNoSelection() const {
			return getnoselection();
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

	};


}}