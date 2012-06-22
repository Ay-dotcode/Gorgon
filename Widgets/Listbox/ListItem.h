#pragma once
#include "../Checkbox/CheckboxBase.h"
#include "IListItem.h"

namespace gge { namespace widgets {

	template<class T_=std::string, void(*CF_)(const T_ &, std::string &)=listbox::CastToString<T_ >>
	class ListItem;

	template<class T_, void(*CF_)(const T_ &, std::string &)>
	class ListItemModifier;

	template<class T_, void(*CF_)(const T_ &, std::string &)>
	class ListItem : public IListItem<T_, CF_>, public checkbox::Base {
		friend class listbox::Base<T_, CF_>;
		friend class ListItemModifier<T_, CF_>;
	public:

		template <class R_>
		ListItem(R_ *receiver, void(R_::*handler)(IListItem<T_, CF_>*,bool), const T_ &value) : 
			IListItem<T_, CF_>(value), checkbox::Base(true, AutosizeModes::None, false, false, true)
		{
			settoggle(receiver, handler);
		}

		template <class R_>
		ListItem(R_ *receiver, void(R_::*handler)(IListItem<T_, CF_>*,bool)) : 
			IListItem<T_, CF_>(T_()), checkbox::Base(true, AutosizeModes::None, false, false, true)
		{
			settoggle(receiver, handler);
		}


		//Drag
		operator T_() {
			return value;
		}

		void operator =(const T_ &value) {
			this->Value=value;
		}

		void operator =(const ListItem &value) {
			this->Value=value.value;
		}

		bool operator <(const ListItem &value) const {
			return this->value<value.value;
		}

		bool operator >(const ListItem &value) const {
			return this->value>value.value;
		}

		bool operator ==(const ListItem &value) const {
			return this->value==value.value;
		}


		virtual bool MouseHandler(input::mouse::Event::Type event, utils::Point location, int amount) { 
			using namespace input::mouse;

			if(Event::isDown(event)) {
				Focus();
			}

			switch (event) {
			case Event::Left_Down:
				Base::down();
				if(IsEnabled()) {
					if(this->notifier)
						this->notifier->Fire(this,true);
				}
				mdownlocation=location;
				break;
			case Event::Left_Up:
				Base::up();
				break;
			case Event::Over:
				Base::over();
				break;
			case Event::Move:
				if(this->allowdrag && PressedButtons==Event::Left && !IsDragging() && location.Distance(mdownlocation)>DragDistance) { 
					BeginDrag(*this);
					DragLocation=location;
				}
				break;
			case Event::Out:
				Base::out();
				break;
			case Event::Left_Click:

				break;
			}


			if(event==Event::Over && pointer!=Pointer::None)
				mousetoken=Pointers.Set(pointer);
			else if(event==Event::Out) {
				Pointers.Reset(mousetoken);
				mousetoken=0;
			}

			return !Event::isScroll(event);
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

				if(this->notifier)
					this->notifier->Fire(this,true);

				return true;
			}
			else if(Key==input::keyboard::KeyCodes::Space && event==input::keyboard::Event::Up && !input::keyboard::Modifier::Check()) {
				Base::up();

				if(this->notifier)
					this->notifier->Fire(this,true);

				return true;
			}


			return false;
		}

		bool IsSelected() {
			return Base::getstate()==2;
		}

		virtual bool Accessed() {
			if(!IsEnabled())
				return false;

			Base::click();
			if(this->notifier)
				this->notifier->Fire(this,true);

			return true;
		}

		virtual void Signal() {
			Base::click();
			if(this->notifier)
				this->notifier->Fire(this,false);
		}

		WidgetBase &GetWidget() {
			return *this;
		}

		virtual ~ListItem() {  }

	protected:
		template<class R_>
		void settoggle(R_ *receiver, void(R_::*handler)(IListItem<T_, CF_>*,bool)) {
			utils::CheckAndDelete(this->notifier);
			this->notifier=new prvt::listnotifyholder<T_,CF_,R_>(receiver, handler);
		}

		ListItem(const ListItem &li);

		virtual T_ getValue() const {
			return value;
		}

		void check() {
			Base::setstate(2);
		}

		void clear() {
			Base::setstate(1);
		}
		
		void set() {
			if(IsEnabled()) {
				if(this->notifier)
					this->notifier->Fire(this,true);
			}
		}

		virtual void setValue(const T_ &value) {
			this->value=value;
			std::string s;

			CF_(value,s);

			Base::settext(s);
		}

		T_ value;

		utils::Point mdownlocation;
		PointerCollection::Token dragtoken;
	};

	template<class T_, void(*CF_)(const T_ &, std::string &)>
	class ListItemModifier {
	protected:
		void callcheck(ListItem<T_, CF_> &li) { li.check(); }
		void callclear(ListItem<T_, CF_> &li) { li.clear(); }
		template <class R_>
		void callsettoggle(ListItem<T_, CF_> &li, R_ *target, void(R_::*handler)(IListItem<T_, CF_>*,bool)) {
			li.settoggle(target, handler);
		}
	};


}}