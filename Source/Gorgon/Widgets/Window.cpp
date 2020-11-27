#include "Window.h"


namespace Gorgon { namespace Widgets {
   
    Window::Window(const UI::Template &temp, const std::string &title) : 
        Panel(temp),
        Title(this),
        title(title)
    {
        stack.SetData(UI::ComponentTemplate::Title, title);
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
        
        if(val[3] == 1 || val[3] == 0 || !hscroll) {
            stack.RemoveCondition(UI::ComponentCondition::HScroll);
        }
        else {
            stack.AddCondition(UI::ComponentCondition::HScroll);
        }
        if(val[4] == 1 || val[4] == 0 || !vscroll) {
            stack.RemoveCondition(UI::ComponentCondition::VScroll);
        }
        else {
            stack.AddCondition(UI::ComponentCondition::VScroll);
        }
    }
    
} }
