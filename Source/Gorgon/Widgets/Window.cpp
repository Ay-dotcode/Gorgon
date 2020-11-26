#include "Window.h"


namespace Gorgon { namespace Widgets {
   
    Window::Window(const UI::Template &temp, const std::string &title) : 
        Panel(temp),
        Title(this),
        title(title)
    {
        EnableScroll(false, false);
        stack.SetData(UI::ComponentTemplate::Title, title);
    }
    
    void Window::SetTitle(const std::string &value) {
        title = value;
        stack.SetData(UI::ComponentTemplate::Title, title);
    }

    
} }
