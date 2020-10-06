#include "Composer.h"


namespace Gorgon { namespace Widgets {

    bool Composer::Activate() {
        if(!Focus())
            return false;

        FocusFirst();

        return true;
    }
    
    bool Composer::allowfocus() const {
        for(auto &w : widgets) {
            if(w.AllowFocus()) {
                return true;
            }
        }
        
        return false;
    }
    
    void Composer::focused() {
        if(!HasFocusedWidget())
            FocusFirst();
        WidgetBase::focused();
    }
    
    void Composer::focuslost() {
        RemoveFocus();
        WidgetBase::focuslost();
    }

    void Composer::focuschanged() { 
        if(HasFocusedWidget() && !IsFocused())
            Focus();
        else if(!HasFocusedWidget() && IsFocused())
            RemoveFocus();
    }

    
} }
