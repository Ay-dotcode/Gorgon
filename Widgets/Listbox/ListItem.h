#pragma once
#include "../Checkbox/CheckboxBase.h"
#include "IListItem.h"

namespace gge { namespace widgets {

	template<class T_=std::string, void(*CF_)(const T_ &, std::string &)=listbox::CastToString<T_ >>
	class ListItem;

	template<class T_, void(*CF_)(const T_ &, std::string &)>
	class ListItemModifier {
	protected:
		void callcheck(ListItem<T_, CF_> &li);
		void callclear(ListItem<T_, CF_> &li);
		void callsettoggle(ListItem<T_, CF_> &li, typename IListItem<T_, CF_>::ToggleNotifyFunction &toggle);
	};

	template<class T_, void(*CF_)(const T_ &, std::string &)>
	class ListItem : public IListItem<T_, CF_>, public checkbox::Base {
		friend class listbox::Base<T_, CF_>;
		friend class ListItemModifier<T_, CF_>;
	public:


		ListItem(ToggleNotifyFunction &toggle, const T_ &value) : IListItem(value), Base(true, AutosizeModes::None, false, false, true),
			toggle(toggle)
		{ }

		ListItem(ToggleNotifyFunction &toggle) : IListItem(T_()), Base(true, AutosizeModes::None, false, false, true),
			toggle(toggle)
		{ }

		//Drag



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
					toggle(this);
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

				toggle(this);

				return true;
			}
			else if(Key==input::keyboard::KeyCodes::Space && event==input::keyboard::Event::Up && !input::keyboard::Modifier::Check()) {
				Base::up();

				toggle(this);

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
			toggle(this);

			return true;
		}


		WidgetBase &GetWidget() {
			return *this;
		}


	protected:
		ToggleNotifyFunction &toggle;

		void settoggle(ToggleNotifyFunction &toggle) {
			this->toggle=toggle;
		}

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
	void ListItemModifier<T_,CF_>::callcheck(ListItem<T_, CF_> &li) { li.check(); }

	template<class T_, void(*CF_)(const T_ &, std::string &)>
	void ListItemModifier<T_,CF_>::callclear(ListItem<T_, CF_> &li) { li.clear(); }

	template<class T_, void(*CF_)(const T_ &, std::string &)>
	void ListItemModifier<T_,CF_>::callsettoggle(ListItem<T_, CF_> &li, typename IListItem<T_, CF_>::ToggleNotifyFunction &toggle) {
		this->toggle=toggle;
	}


}}