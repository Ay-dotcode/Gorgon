#include "Widget.h"
#include "WidgetContainer.h"

namespace Gorgon { namespace UI {

	bool Widget::Remove() {
		if(!parent)
			return true;

		return parent->Remove(*this);
	}

	bool Widget::Focus() {
		if(!parent)
			return false;

		if(!allowfocus() || !visible || !enabled)
			return false;

		return parent->SetFocusTo(*this);
	}

	bool Widget::Defocus() {
		if(!IsFocused() || !parent)
			return true;

		return parent->RemoveFocus();
	}

	WidgetContainer &Widget::GetParent() const {
		if(parent == nullptr)
			throw std::runtime_error("Widget has no parent");

		return *parent;
	}


    void Widget::SetVisible(bool value) {
        if(visible != value) {
            visible = value;
            
            if(value)
                show();
            else {
                if(IsFocused() && HasParent())
                    GetParent().FocusPrevious();
                
                Defocus();
                hide();
            }
            
            if(parent)
                parent->childboundschanged(this);
        }
    }
    
    
    bool Widget::EnsureVisible() const {
        if(!parent)
            return false;
        
        return parent->EnsureVisible(*this);
    }

    void Widget::boundschanged(){
        if(parent)
            parent->childboundschanged(this);
        
        BoundsChangedEvent();
    }


    /// Called when this widget added to the given container
    void Widget::addedto(WidgetContainer &container) {
        if(parent == &container)
            return;

        parent = &container;

        if(IsVisible())
            boundschanged();

        parentenabledchanged(parent->IsEnabled());
    }

    void Widget::removed(){
        if(!parent)
            return;
        
        parent->childboundschanged(this);

        parent = nullptr;
        
        if(IsVisible())
            boundschanged(); 
    }
    

    void Widget::focuslost() {
        focus = false;
        FocusEvent();
    }


    void Widget::focused() {
        focus = true;
        FocusEvent();
    }


    Widget::~Widget() {
        if(HasParent()) {
            GetParent().deleted(this);
        }
    }

} }
