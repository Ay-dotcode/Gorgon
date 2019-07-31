#include "WidgetBase.h"
#include "WidgetContainer.h"

namespace Gorgon { namespace UI {

	bool WidgetBase::Remove() {
		if(!parent)
			return true;

		return parent->Remove(*this);
	}

	bool WidgetBase::Focus() {
		if(!parent)
			return false;

		if(!allowfocus())
			return false;

		return parent->SetFocusTo(*this);
	}

	bool WidgetBase::Defocus() {
		if(!IsFocused() || !parent)
			return true;

		return parent->RemoveFocus();
	}

	WidgetContainer &WidgetBase::GetParent() const {
		if(parent == nullptr)
			throw std::runtime_error("Widget has no parent");

		return *parent;
	}

}
}