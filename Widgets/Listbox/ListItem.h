#pragma once
#include "../Checkbox/CheckboxBase.h"
#include "IListItem.h"

namespace gge { namespace widgets {

	class ListItem;

	class ListItem : public IListItem, public checkbox::Base {
	public:

		ListItem(int index, std::function<void(IListItem&, int)> trigger) : 
			IListItem(index, trigger), checkbox::Base(true, AutosizeModes::None, false, false, true)
		{ }


		virtual bool MouseHandler(input::mouse::Event::Type event, utils::Point location, int amount) { 
			using namespace input::mouse;

			if(Event::isDown(event)) {

				Focus();
			}

			switch (event) {
			case Event::Left_Down:
				Base::down();
				if(IsEnabled()) {
					Trigger();
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
				if(mdownlocation.Distance(location)>DragDistance && PressedButtons==Event::Left && DragNotify) {
					DragNotify(*this, Index, mdownlocation);
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

			return !Event::isScroll(event) && !Event::isDrag(event);
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

				Trigger();

				return true;
			}
			else if(Key==input::keyboard::KeyCodes::Space && event==input::keyboard::Event::Up && !input::keyboard::Modifier::Check()) {
				Base::up();

				Trigger();

				return true;
			}


			return false;
		}

		virtual bool Accessed() {
			if(!IsEnabled())
				return false;

			Base::click();
			Trigger();

			return true;
		}

		WidgetBase &GetWidget() {
			return *this;
		}

		void SetIcon(graphics::RectangularGraphic2D *icon) {
			Base::seticon(icon);
		}

		void Select() {
			Base::setstate(2);
		}

		void Deselect() {
			Base::setstate(1);
		}

		void SetText(const std::string &text) {
			if(text!=gettext()) {
				settext(text);
			}
		}

		virtual ~ListItem() {  }

	protected:
		ListItem(const ListItem &li);


		utils::Point mdownlocation;
		PointerCollection::Token dragtoken;
	};
	
}}
