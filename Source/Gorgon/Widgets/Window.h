#pragma once

#include "Panel.h"

namespace Gorgon { namespace Widgets {
    
    /**
     * This class will create in system window that can be placed anywhere
     * including panels. Once created, window will automatically place itself
     * into the active OS window. It supports title, if enabled, self movement 
     * and resizing.
     * 
     * When enabled, windows autohide their scrollbar as necessary.
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
        
        /// Locks the movement of the window.
        void LockMovement() {
            AllowMovement(false);
        }
        
        /// Allows movement of the window.
        void AllowMovement(bool allow = true);
        
        /// Returns whether the window can be moved.
        bool CanBeMoved() const {
            return allowmove;
        }
        
        virtual void EnableScroll(bool vertical, bool horizontal) override;
        
        TextualProperty<Window, std::string, &Window::GetTitle, &Window::SetTitle> Title;

    protected:
        virtual void updatescroll() override;
        
        virtual void updatecontent() override;
        
        void updatescrollvisibility();
        
        void mouse_down(UI::ComponentTemplate::Tag tag, Geometry::Point location, Input::Mouse::Button button);
        
        void mouse_up(UI::ComponentTemplate::Tag tag, Geometry::Point location, Input::Mouse::Button button);
        
        void mouse_move(UI::ComponentTemplate::Tag tag, Geometry::Point location);
        
    private:
        std::string title;
        bool allowmove = true;
        bool moving = false;
        Geometry::Point dragoffset;
    };
    
} }
