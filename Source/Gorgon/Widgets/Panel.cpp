#include "Panel.h"

namespace Gorgon { namespace Widgets {
    Panel::Panel(const UI::Template &temp) : 
    UI::ComponentStackWidget(temp) 
    {
        stack.SetOtherMouseEvent([this](UI::ComponentTemplate::Tag tag, Input::Mouse::EventType type, Geometry::Point, float amount) {
            if(type == Input::Mouse::EventType::Scroll_Vert) {
                ScrollBy((int)amount*15);
            }
        });

        stack.SetValueTransitionSpeed({1, 1, 0, 0});
    }

    
    bool Panel::Activate() {
        if(!Focus())
            return false;

        FocusFirst();

        return true;
    }

    
    bool Panel::KeyEvent(Input::Key key, float state) {
        return UI::WidgetContainer::KeyEvent(key, state);
    }

    
    bool Panel::CharacterEvent(Char c) {
        return UI::WidgetContainer::CharacterEvent(c);
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
    
    
    bool Panel::allowfocus() const {
        for(auto &w : widgets) {
            if(w.AllowFocus()) {
                return true;
            }
        }
        
        return false;
    }
    

    void Panel::focused() {
        FocusFirst();
    }
    

    void Panel::focuslost() {
        RemoveFocus();
    }

    
    Gorgon::Layer &Panel::getlayer() {
        return stack.GetLayerOf(stack.IndexOfTag(UI::ComponentTemplate::Contents));
    }


    void Panel::focuschanged() { 
        if(HasFocusedWidget() && !IsFocused())
            Focus();
        else if(!HasFocusedWidget() && IsFocused())
            RemoveFocus();
    }
    

    void Panel::ScrollTo(int x, int y, bool clip){
        auto b = stack.TagBounds(UI::ComponentTemplate::Contents);
        stack.SetValue(float(x) / b.Width(), float(y ) / b.Height());
    }
    
    
    Geometry::Point Panel::ScrollOffset() const {
        auto b = stack.TagBounds(UI::ComponentTemplate::Contents);
        auto val = stack.GetValue();

        return {(int)(val[0]*b.Width()), (int)(val[1]*b.Height())};
    }

}
}
