#include "Panel.h"

namespace Gorgon { namespace Widgets {

	bool Panel::Activate() {
		if(!Focus())
			return false;

		FocusFirst();

		return true;
	}

	bool Panel::KeyEvent(Input::Key key, float state) {
		UI::WidgetContainer::KeyEvent(key, state);
	}

	bool Panel::CharacterEvent(Char c) {
		UI::WidgetContainer::CharacterEvent(c);
	}

	bool Panel::IsVisible() const {
		return stack.IsVisible();
	}

	Geometry::Size Panel::GetInteriorSize() const {
		return stack.TagBounds(UI::ComponentTemplate::Contents).GetSize();
	}

	bool Panel::ResizeInterior(Geometry::Size size) {
		auto tb = stack.TagBounds(UI::ComponentTemplate::Contents).GetSize();
		stack.Resize(size + stack.GetSize() - tb);

		return stack.TagBounds(UI::ComponentTemplate::Contents).GetSize() == size;
	}

	void Panel::focused() {
		FocusFirst();
	}

	void Panel::focuslost() {
		RemoveFocus();
	}

	Gorgon::Layer &Panel::getlayer() {
		throw std::logic_error("The method or operation is not implemented.");
	}

} }