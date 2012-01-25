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
		ListItem(R_ *receiver, void(R_::*handler)(IListItem*,bool), const T_ &value) : 
			IListItem(value), Base(true, AutosizeModes::None, false, false, true)
		{
			settoggle(receiver, handler);
		}

		template <class R_>
		ListItem(R_ *receiver, void(R_::*handler)(IListItem*,bool)) : 
			IListItem(T_()), Base(true, AutosizeModes::None, false, false, true)
		{
			settoggle(receiver, handler);
		}


		//Drag

		operator T_() {
			return value;
		}

		void operator =(const T_ &value) {
			Value=value;
		}

		void operator =(const ListItem &value) {
			Value=value.value;
		}

		bool operator <(const ListItem &value) {
			return this->value<value.value;
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
					if(notifier)
						notifier->Fire(this,true);
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

				if(notifier)
					notifier->Fire(this,true);

				return true;
			}
			else if(Key==input::keyboard::KeyCodes::Space && event==input::keyboard::Event::Up && !input::keyboard::Modifier::Check()) {
				Base::up();

				if(notifier)
					notifier->Fire(this,true);

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
			if(notifier)
				notifier->Fire(this,true);

			return true;
		}

		virtual void Signal() {
			Base::click();
			if(notifier)
				notifier->Fire(this,false);
		}

		WidgetBase &GetWidget() {
			return *this;
		}

		virtual ~ListItem() {  }

	protected:
		template<class R_>
		void settoggle(R_ *receiver, void(R_::*handler)(IListItem*,bool)) {
			utils::CheckAndDelete(notifier);
			notifier=new prvt::listnotifyholder<T_,CF_,R_>(receiver, handler);
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

		virtual void setValue(const T_ &value) {
			this->value=value;
			string s;

			CF_(value,s);

			Base::settext(s);
		}

		T_ value;

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