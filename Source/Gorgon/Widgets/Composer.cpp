#include "Composer.h"
#include "Registry.h"


namespace Gorgon { namespace Widgets {

    Composer::Composer(const UI::UnitSize &size) :
        Widget(Pixels(0, 0)) //Resize is needed to convert unitsize to pixels
    {
        Resize(size);
        base.Add(inputlayer);
        
        inputlayer.SetOver([this]{
            mouseenter();
        });
        
        inputlayer.SetOut([this] {
            mouseleave();
        });
    }
    
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

    void Composer::resize(const Geometry::Size &size) {
        base.Resize(size);
        if (IsVisible() && HasParent())
            boundschanged();

    }
    void Composer::move(const Geometry::Point &location) {
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
                    ans.CoordinatesInExtender += GetCurrentLocation();

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

    int Composer::GetUnitSize() const {
        if(issizesset) {
            return unitwidth;
        }
        else {
            return Widgets::Registry::Active().GetUnitSize();
        }
    }
    

    bool ComponentStackComposer::Activate() {
        if(!Focus())
            return false;

        FocusFirst();

        return true;
    }

    bool ComponentStackComposer::allowfocus() const {
        if(CurrentFocusStrategy() == Deny)
            return false;

        for(auto &w : widgets) {
            if(w.AllowFocus()) {
                return true;
            }
        }
        
        return false;
    }
    
    void ComponentStackComposer::focused() {
        if(!HasFocusedWidget())
            FocusFirst();
        Widget::focused();
    }
    
    void ComponentStackComposer::focuslost() {
        RemoveFocus();
        Widget::focuslost();
    }

    void ComponentStackComposer::focuschanged() { 
        if(HasFocusedWidget() && !IsFocused())
            Focus();
        else if(!HasFocusedWidget() && IsFocused())
            RemoveFocus();
    }

    void ComponentStackComposer::resize(const Geometry::Size &size) {
        ComponentStackWidget::resize(size);
        
        if(HasOrganizer())
            GetOrganizer().Reorganize();
        
        childboundschanged(nullptr);
        distributeparentboundschanged();
    }
    
    void ComponentStackComposer::move(const Geometry::Point &location) {
        ComponentStackWidget::move(location);
        
        distributeparentboundschanged();
    }

    UI::ExtenderRequestResponse ComponentStackComposer::RequestExtender(const Layer &self) {
        if(HasParent()) {
            auto
            ans = GetParent().RequestExtender(self);

            if(ans.Extender) {
                if(!ans.Transformed)
                    ans.CoordinatesInExtender += GetCurrentLocation();

                return ans;
            }
        }

        return {
            false, this, self.GetLocation()};
    }

    int ComponentStackComposer::GetSpacing() const {
        if(issizesset) {
            return spacing;
        }
        else {
            return Widgets::Registry::Active().GetSpacing();
        }
    }

    int ComponentStackComposer::GetUnitSize() const {
        if(issizesset) {
            return unitwidth;
        }
        else {
            return Widgets::Registry::Active().GetUnitSize();
        }
    }

} }
