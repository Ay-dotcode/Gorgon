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

		if(!allowfocus() || !visible || !enabled)
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


    void WidgetBase::SetVisible(bool value) {
        if(visible != value) {
            visible = value;
            
            if(value)
                show();
            else {
                Defocus();
                hide();
            }
            
            if(parent)
                parent->childboundschanged(this);
        }
    }
    
    
    bool WidgetBase::EnsureVisible() const {
        if(!parent)
            return false;
        
        return parent->EnsureVisible(*this);
    }

    void WidgetBase::boundschanged(){
        if(parent)
            parent->childboundschanged(this);
        
        BoundsChangedEvent();
    }


    /// Called when this widget added to the given container
    void WidgetBase::addedto(WidgetContainer &container) {
        if(parent == &container)
            return;

        parent = &container;

        if(IsVisible())
            boundschanged();

        parentenabledchanged(parent->IsEnabled());
    }

    void WidgetBase::removed(){
        if(!parent)
            return;
        
        parent->childboundschanged(this);

        parent = nullptr;
        
        if(IsVisible())
            boundschanged(); 
    }
    

    void WidgetBase::focuslost() {
        focus = false;
        FocusEvent();
    }


    void WidgetBase::focused() {
        focus = true;
        FocusEvent();
    }

} }
