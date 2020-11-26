#pragma once

#include "Panel.h"

namespace Gorgon { namespace Widgets {
    
    /**
     * This class will create in system window that can be placed anywhere
     * including panels. Once created, window will automatically place itself
     * into the active OS window. It supports title, if enabled, self movement 
     * and resizing.
     */
    class Window : public Panel {
    public:
        
        Window(const Panel &) = delete;
        
        explicit Window(const UI::Template &temp, const std::string &title = "");
        
        explicit Window(Registry::TemplateType type) : Window(Registry::Active()[type], "") { }

        explicit Window(const std::string &title = "", Registry::TemplateType type = Registry::Window_Regular) : Window(Registry::Active()[type], title) { }
        
        
        /// Sets the window title to the given string. Title placement is controlled by
        /// the template and might not be visible.
        void SetTitle(const std::string &value);
        
        /// Returns the window title. Title placement is controlled by the template and 
        /// might not be visible.
        std::string GetTitle() const {
            return title;
        }
        
        TextualProperty<Window, std::string, &Window::GetTitle, &Window::SetTitle> Title;

    private:
        std::string title;
    };
    
} }
