#include "Window.h"


namespace Gorgon { namespace Widgets {
   
    Window::Window(const UI::Template &temp, const std::string &title) : 
        Panel(temp),
        Title(this),
        title(title)
    {
        stack.SetData(UI::ComponentTemplate::Title, title);
        
        stack.SetMouseDownEvent([this](auto tag, auto location, auto button) { mouse_down(tag, location, button); });
        stack.SetMouseUpEvent([this](auto tag, auto location, auto button) { mouse_up(tag, location, button); });
        stack.SetMouseMoveEvent([this](auto tag, auto location) { mouse_move(tag, location); });
        
        updatescrollvisibility();
    }
    
    void Window::SetTitle(const std::string &value) {
        title = value;
        stack.SetData(UI::ComponentTemplate::Title, title);
    }
    
    void Window::EnableScroll(bool vertical, bool horizontal) {
        Panel::EnableScroll(vertical, horizontal);
        
        updatescrollvisibility();
    }
    
    void Window::updatescroll() {
        Panel::updatescroll();
        updatescrollvisibility();
    }
    
    void Window::updatecontent() {
        Panel::updatecontent();
        updatescrollvisibility();
    }
    
    void Window::updatescrollvisibility() {
        auto val = stack.GetTargetValue();
        
        if(val[2] == 1 || val[2] == 0 || !hscroll) {
            stack.RemoveCondition(UI::ComponentCondition::HScroll);
        }
        else {
            stack.AddCondition(UI::ComponentCondition::HScroll);
        }
        if(val[3] == 1 || val[3] == 0 || !vscroll) {
            stack.RemoveCondition(UI::ComponentCondition::VScroll);
        }
        else {
            stack.AddCondition(UI::ComponentCondition::VScroll);
        }
    }
    
    void Window::mouse_down(UI::ComponentTemplate::Tag tag, Geometry::Point location, Input::Mouse::Button button) {
        if(tag == UI::ComponentTemplate::NoTag) {
            if(stack.IndexOfTag(UI::ComponentTemplate::DragTag) == -1)
                tag = UI::ComponentTemplate::DragTag;
            else {
                int ind = stack.ComponentAt(location);
                
                if(ind != -1)
                    tag = stack.GetTemplate(ind).GetTag();
            }
        }
        
        if(button == Input::Mouse::Button::Left && allowmove && tag == UI::ComponentTemplate::DragTag) {
            moving = true;
            dragoffset = location;
        }
        
        Focus();
    }
    
    void Window::mouse_up(UI::ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button button) {
        if(button == Input::Mouse::Button::Left) {
            moving = false;
        }
    }
    
    void Window::mouse_move(UI::ComponentTemplate::Tag, Geometry::Point location) {
        if(moving && location != dragoffset) {
            Move(GetLocation() + location-dragoffset);
        }
    }
    

    void Window::AllowMovement(bool allow) {
        if(allowmove == allow) 
            return;
        
        allowmove = allow;
        if(moving)
            mouse_up(UI::ComponentTemplate::NoTag, {0, 0}, Input::Mouse::Button::Left);
    }

} }
