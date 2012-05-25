#pragma once


#include "Interfaces/ITextbox.h"
#include "Textbox/TextboxBase.h"
#include "../Utils/Property.h"
#include <sstream>
#include "Interfaces/INumberbox.h"


namespace gge { namespace widgets {

	//this function will clear the characters that should not be in the given string
	//if the removed chars are before the location, this function should increment
	//removed before. This will ensure that the caret position remains  consistent
	template <class T_>
	void FixNumberString(std::string &number, int location, int &removedbefore, int base) {
		bool passedfirst=false;
		for(std::string::size_type i=0;i<number.length();i++) {
			char c=number[i];
			if(!( ( (c>='0' && c<='9') || (c>='a' && c<='a'+(base-11)) ) ||
				((c==' ' || c=='-' || c=='+')&&!passedfirst) )) {
					number.erase(i, 1);

					if((unsigned)location>i) {
						location--;
						removedbefore++;
					}

					i--;
			}
			else if(c!=' ' && c!='-' && c!='+') {
				passedfirst=true;
			}
		}
	}

	template <>
	inline void FixNumberString<float>(std::string &number, int location, int &removedbefore, int base) {
		bool passedfirst=false, passeddot=false;
		for(std::string::size_type i=0;i<number.length();i++) {
			char c=number[i];
			if(!( ( (c>='0' && c<='9') ) ||
				((c==' ' || c=='-' || c=='+')&&!passedfirst) || (c=='.'&&!passeddot) )) {
					number.erase(i, 1);

					if((unsigned)location>i) {
						location--;
						removedbefore++;
					}

					i--;
			}
			else if(c=='.') {
				passeddot=true;
			}
			else if(c!=' ' && c!='-' && c!='+') {
				passedfirst=true;
			}
		}
	}
	template <>
	inline void FixNumberString<double>(std::string &number, int location, int &removedbefore, int base) {
		bool passedfirst=false, passeddot=false;
		for(std::string::size_type i=0;i<number.length();i++) {
			char c=number[i];
			if(!( ( (c>='0' && c<='9') ) ||
				((c==' ' || c=='-' || c=='+')&&!passedfirst) || (c=='.'&&!passeddot) )) {
					number.erase(i, 1);

					if((unsigned)location>i) {
						location--;
						removedbefore++;
					}

					i--;
			}
			else if(c=='.') {
				passeddot=true;
			}
			else if(c!=' ' && c!='-' && c!='+') {
				passedfirst=true;
			}
		}
	}

	template <class T_, void (*Val_)(std::string&, int, int&,int)=FixNumberString<T_>, 
		T_ (*Conv_)(const std::string&,int)=utils::StrToNumber<T_> >
	class Numberbox : public INumberbox<T_>, public textbox::Base {
	public:
		Numberbox(const T_ &value=T_()) : Base(),
			ChangeEvent("ChangeEvent", this),
			INIT_PROPERTY(Numberbox, CaretLocation),
			INIT_PROPERTY(Numberbox, UseHex),
			INIT_PROPERTY(Numberbox, Prefix),
			INIT_PROPERTY(Numberbox, Suffix),
			AutoSelectAll(false), usehex(false)
		{
			this->Value=value;

			setupvscroll(false, false, false);
			
			ChangeEvent.DoubleLink(INumberbox<T_>::ChangeEvent);

			if(WR.Textboxes.Numberbox)
				setblueprint(*WR.Textboxes.Numberbox);
		}

		Numberbox &operator =(const T_ &s) {
			this->Value=s;

			return *this;
		}

		utils::EventChain<Numberbox> ChangeEvent;

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

		virtual bool KeyboardHandler(input::keyboard::Event::Type event, input::keyboard::Key Key) {
			if(this->KeyEvent(input::keyboard::Event(event, Key)))
				return true;
			
			return Base::KeyboardHandler(event,Key);
		}


		utils::NumericProperty<Numberbox, int> CaretLocation;
		utils::BooleanProperty<Numberbox> UseHex;
		utils::TextualProperty<Numberbox> Prefix, Suffix;

		bool AutoSelectAll;



	protected:


		virtual void textchanged() {
			std::string s=gettext();
			int movecaretby=0;
			Val_(s, getcaretlocation(), movecaretby, usehex ? 16 : 10);
			setcaretlocation(getcaretlocation()-movecaretby);
			if(s!=gettext())
				settext(s);

			value=Conv_(s, usehex ? 16 : 10);

			ChangeEvent();
		}

		virtual void setValue(const T_ &value) {
			this->value=value;
			ss.str("");
			ss<<value;

			settext(ss.str());

			ChangeEvent();
		}
		virtual T_ getValue() const {
			return value;
		}

		void setCaretLocation(const int &value) {
			Base::setcaretlocation(value);
		}
		int getCaretLocation() const {
			return Base::getcaretlocation();
		}

		void setUseHex(const bool &value) {
			if(UseHex!=value) {
				usehex = value;

				if(value)
					ss<<std::hex;
				else
					ss<<std::dec;

				setValue(this->value);
			}
		}
		bool getUseHex() const {
			return usehex;
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
			if(WR.Textboxes.Numberbox && !blueprintmodified)
				setblueprint(*WR.Textboxes.Numberbox);
		}

		T_ value;
		std::stringstream ss;
		bool usehex;

	};


}}