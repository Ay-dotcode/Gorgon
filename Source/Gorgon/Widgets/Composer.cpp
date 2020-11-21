#include "Composer.h"
#include "Registry.h"


namespace Gorgon { namespace Widgets {

    bool Composer::Activate() {
        if(!Focus())
            return false;

        FocusFirst();

        return true;
    }
    
    bool Composer::allowfocus() const {
        if(CurrentFocusStrategy() == Deny)
            return false;

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
        Widget::focused();
    }
    
    void Composer::focuslost() {
        RemoveFocus();
        Widget::focuslost();
    }

    void Composer::focuschanged() { 
        if(HasFocusedWidget() && !IsFocused())
            Focus();
        else if(!HasFocusedWidget() && IsFocused())
            RemoveFocus();
    }

    void Composer::hide() {
        base.Hide();
        if (HasParent())
            boundschanged();
    }

    void Composer::show() {
        base.Show();
        if (HasParent())
            boundschanged();
    }

    void Composer::Resize(const Geometry::Size &size) {
        base.Resize(size);
        if (IsVisible() && HasParent())
            boundschanged();

    }
    void Composer::Move(const Geometry::Point &location) {
        base.Move(location);
        if (IsVisible() && HasParent())
            boundschanged();
    }


    UI::ExtenderRequestResponse Composer::RequestExtender(const Layer &self) {
        if(HasParent()) {
            auto
            ans = GetParent().RequestExtender(self);

            if(ans.Extender) {
                if(!ans.Transformed)
                    ans.CoordinatesInExtender += GetLocation();

                return ans;
            }
        }

        return {
            false, this, self.GetLocation()};
    }

    int Composer::GetSpacing() const {
        if(issizesset) {
            return spacing;
        }
        else {
            return Widgets::Registry::Active().GetSpacing();
        }
    }

    int Composer::GetUnitWidth() const {
        if(issizesset) {
            return unitwidth;
        }
        else {
            return Widgets::Registry::Active().GetUnitWidth();
        }
    }
    

    void Composer::SetWidthInUnits(int n) {
        int w, s;

        if(HasParent()) {
            w = GetParent().GetUnitWidth();
            s = GetParent().GetSpacing();
        }
        else {
            w = Registry::Active().GetUnitWidth();
            s = Registry::Active().GetSpacing();
        }

        SetWidth(w * n + s * (n - 1));
    }

} }
