#include "WidgetContainer.h"

namespace Gorgon { namespace UI {
    
	bool WidgetContainer::Add(WidgetBase &widget) {
		if(widgets.Find(widget) != widgets.end())
			return true;

		if(!addingwidget(widget))
			return false;

		if(!widget.addingto(*this))
			return false;

		widget.addto(getlayer());
		widgets.Add(widget);

		widgetadded(widget);
		widget.addedto(*this);

		return true;
	}

	bool WidgetContainer::Insert(WidgetBase &widget, int index) {
		if(widgets.Find(widget) != widgets.end()) {
			ChangeFocusOrder(widget, index);
			ChangeZorder(widget, index);

			return true;
		}

		if(!addingwidget(widget))
			return false;

		if(!widget.addingto(*this))
			return false;

		//we are inserting before the focused widget
		if(index <= focusindex)
			focusindex++;

		widget.addto(getlayer());
		widget.setlayerorder(getlayer(), index);

		widgets.Insert(widget, index);

		widgetadded(widget);
		widget.addedto(*this);

		return true;
	}

	bool WidgetContainer::Remove(WidgetBase &widget) {
		auto pos = widgets.Find(widget);

		//not our widget
		if(pos == widgets.end())
			return true;

		if(!removingwidget(widget))
			return false;

		if(!widget.removingfrom())
			return false;

		ForceRemove(widget);

		return true;
	}

	void WidgetContainer::ForceRemove(WidgetBase &widget) {
		auto pos = widgets.Find(widget);

		//not our widget
		if(pos == widgets.end())
			return;

		if(focusindex == pos - widgets.begin())
			FocusNext();

		if(focusindex > pos - widgets.begin())
			focusindex--;

		widget.removefrom(getlayer());
		widgets.Remove(pos);

		widgetremoved(widget);
		widget.removed();
	}

	void WidgetContainer::ChangeFocusOrder(WidgetBase &widget, int order) {
		auto pos = widgets.Find(widget);

		//not our widget
		if(pos == widgets.end())
			return;

		//widget is moving across currently focused widget
		if(order <= focusindex && (pos-widgets.begin()) > focusindex) {
			focusindex++;
		}
		//widget is moving across currently focused widget
		else if(order >= focusindex && (pos-widgets.begin()) < focusindex) {
			focusindex--;
		}
		//this is the focused widget
		else if((pos-widgets.begin()) == focusindex) {
			focusindex = order;
		}

		widgets.MoveBefore(widget, order);
	}

	void WidgetContainer::ChangeZorder(WidgetBase &widget, int order) {
		auto &l = getlayer();
		auto pos = widgets.Find(widget);

		if(pos == widgets.end())
			return;

		widget.setlayerorder(l, order);
	}

	bool WidgetContainer::FocusFirst() {
		if(focused && !focused->canloosefocus())
			return false;

		//starting from the first, try to focus widgets in order
		focusindex = 0;
		for(auto &w : widgets) {
			if(w.allowfocus()) {
				auto prevfoc = focused;

				focused = &w;

				if(prevfoc)
					prevfoc->focuslost();

				w.focused();

				return true;
			}

			focusindex++;
		}

		//nothing can be focused
		focusindex = -1;
		return false;
	}

	bool WidgetContainer::FocusNext() {
		if(focused && !focused->canloosefocus())
			return false;

		for(int i=focusindex+1; i<widgets.GetSize(); i++) {
			if(widgets[i].allowfocus()) {
				auto prevfoc = focused;

				focused = &widgets[i];
				focusindex = i;

				if(prevfoc)
					prevfoc->focuslost();

				widgets[i].focused();

				return true;
			}
		}

		//not found, rollover
		for(int i=0; i<focusindex; i++) {
			if(widgets[i].allowfocus()) {
				auto prevfoc = focused;

				focused = &widgets[i];
				focusindex = i;

				if(prevfoc)
					prevfoc->focuslost();

				widgets[i].focused();


				return true;
			}
		}

		//nothing is found
		return false;
	}

	bool WidgetContainer::FocusPrevious() {
		if(focused && !focused->canloosefocus())
			return false;

		for(int i=focusindex-1; i>=0; i--) {
			if(widgets[i].allowfocus()) {
				auto prevfoc = focused;

				focused = &widgets[i];
				focusindex = i;

				if(prevfoc)
					prevfoc->focuslost();

				widgets[i].focused();


				return true;
			}
		}

		//not found, rollover
		for(int i=widgets.GetSize()-1; i>focusindex; i--) {
			if(widgets[i].allowfocus()) {
				auto prevfoc = focused;

				focused = &widgets[i];
				focusindex = i;

				if(prevfoc)
					prevfoc->focuslost();

				widgets[i].focused();


				return true;
			}
		}

		//nothing is found
		return false;
	}

	bool WidgetContainer::SetFocusTo(WidgetBase &widget) {
		auto pos = widgets.Find(widget);

		//not our widget
		if(pos == widgets.end())
			return false;

		//already focused, no need to check
		if(&widget == focused)
			return true;

		if(!widget.allowfocus())
			return false;

		if(focused && !focused->canloosefocus())
			return false;

		auto prevfoc = focused;

		focusindex = pos - widgets.begin();
		focused = &widget;

		if(prevfoc)
			prevfoc->focuslost();

		widget.focused();

		return true;
	}


	bool WidgetContainer::RemoveFocus() {
		if(!focused)
			return true;

		if(!focused->canloosefocus())
			return false;

		auto prevfoc = focused;

		focused = nullptr;
		focusindex = -1;

		prevfoc->focuslost();

		return true;
	}


	Gorgon::UI::WidgetBase & WidgetContainer::GetFocus() const {
		if(!focused)
			throw std::runtime_error("No widget is focused");

		return *focused;
	}

	bool WidgetContainer::handlestandardkey(Input::Key key) {
		namespace Keycodes = Input::Keyboard::Keycodes;

		if((key == Keycodes::Enter || key == Keycodes::Numpad_Enter) && 
		   (Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::Ctrl || Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::None)) {
			if(def)
				return def->Activate();
			
			return false;
		}
		else if(key == Keycodes::Escape && Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::None) {
			if(cancel)
				return cancel->Activate();

			return false;
		}
		else if(key == Keycodes::Tab) {
			if(Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::Shift)
				return FocusPrevious();

			if(Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::None)
				return FocusNext();
		}

		return false;
	}

	bool WidgetContainer::distributekeyevent(Input::Key key, float state, bool handlestandard) {
		if(focused && focused->KeyEvent(key, state))
			return true;

		if(handlestandard && state)
			return handlestandardkey(key);

		return false;
	}

	bool WidgetContainer::distributecharevent(Char c) {
		if(focused)
			return focused->CharacterEvent(c);

		return false;
	}

} }
