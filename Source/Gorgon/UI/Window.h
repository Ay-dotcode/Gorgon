#pragma once

#include "../Utils/Assert.h"
#include "../Event.h"
#include "../Main.h"
#include "../Layer.h"
#include "../Input/Layer.h"
#include "../Input/Keyboard.h"
#include "../Graphics/Layer.h"
#include "../Window.h"
#include "LayerAdapter.h"

#include "WidgetContainer.h"
#include "TooltipManager.h"

#include <stdexcept>


namespace Gorgon { namespace UI {

    /**
    * UI window allows programmers to create an OS window that will accept
    * widgets and has the ability to run by its own.
    */
    class Window : public Gorgon::Window, public Runner, public WidgetContainer {
    public:

        Window() : Gorgon::Window() {
        }
        /// Creates a new window
        /// @param  rect the position and the **interior** size of the window unless
        ///         use outer metrics is set to true
        /// @param  name of the window
        /// @param  visible after creation, window will be visible or invisible depending
        ///         on this value. 
        Window(Geometry::Rectangle rect, const std::string &name, bool allowresize = false, bool visible = true):
            Gorgon::Window(rect, name, allowresize, visible) 
        {
            init();
        }

        /// Creates a new window
        /// @param  rect the position and the **interior** size of the window unless
        ///         use outer metrics is set to true
        /// @param  name of the window
        /// @param  visible after creation, window will be visible or invisible depending
        ///         on this value. 
        Window(Geometry::Rectangle rect, const char *name, bool allowresize = false, bool visible = true):
            Gorgon::Window(rect, name, allowresize, visible)
        {
            init();
        }

        /// Creates a new window at the center of the screen
        /// @param  size of the window
        /// @param  name of the window
        /// @param  title of the window
        /// @param  visible after creation, window will be visible or invisible depending
        ///         on this value. 
        Window(const Geometry::Size &size, const std::string &name, const std::string &title, bool allowresize = false, bool visible = true):
            Gorgon::Window(size, name, title, allowresize, visible) 
        {
            init();
        }

        /// Creates a new window at the center of the screen
        /// @param  size of the window
        /// @param  name of the window
        /// @param  title of the window
        /// @param  visible after creation, window will be visible or invisible depending
        ///         on this value. 
        Window(const Geometry::Size &size, const char *name, const char *title, bool allowresize = false, bool visible = true):
            Gorgon::Window(size, name, title, allowresize, visible) 
        {
            init();
        }

        /// Creates a new window at the center of the screen
        /// @param  size of the window
        /// @param  name of the window
        /// @param  visible after creation, window will be visible or invisible depending
        ///         on this value. 
        Window(const Geometry::Size &size, const std::string &name, bool allowresize = false, bool visible = true):
            Gorgon::Window(size, name, allowresize, visible) 
        {
            init();
        }

        /// Creates a new window at the center of the screen
        /// @param  size of the window
        /// @param  name of the window
        /// @param  visible after creation, window will be visible or invisible depending
        ///         on this value. 
        Window(const Geometry::Size &size, const char *name, bool allowresize = false, bool visible = true):
            Gorgon::Window(size, name, allowresize, visible) 
        {
            init();
        }

        /// Creates a new window at the center of the screen
        /// @param  monitor that the window will be centered on
        /// @param  size of the window
        /// @param  name of the window
        /// @param  visible after creation, window will be visible or invisible depending
        ///         on this value. 
        Window(const WindowManager::Monitor &monitor, const Geometry::Size &size, const std::string &name, bool allowresize = false, bool visible = true):
            Gorgon::Window(monitor, size, name, allowresize, visible) 
        {
            init();
        }

        /// Creates a new window at the center of the screen
        /// @param  monitor that the window will be centered on
        /// @param  size of the window
        /// @param  name of the window
        /// @param  visible after creation, window will be visible or invisible depending
        ///         on this value. 
        Window(const WindowManager::Monitor &monitor, const Geometry::Size &size, const char *name, bool allowresize = false, bool visible = true):
            Gorgon::Window(monitor, size, name, allowresize, visible) 
        {
            init();
        }

        /// Creates a fullscreen window. Fullscreen windows do not have chrome and covers
        /// entire screen, including any panels it contains.
        Window(const FullscreenTag &tag, const WindowManager::Monitor &monitor, const std::string &name, const std::string &title = "") :
            Gorgon::Window(tag, monitor, name, title)
        {
            init();
        }

        /// Creates a fullscreen window. Fullscreen windows do not have chrome and covers
        /// entire screen, including any panels it contains.
        Window(const FullscreenTag &tag, const std::string &name, const std::string &title = ""):
            Gorgon::Window(tag, name, title) 
        {
            init();
        }

        /// Copy constructor is not allowed
        Window(const Window &) = delete;

        virtual ~Window() {
            KeyEvent.Unregister(inputtoken);
            CharacterEvent.Unregister(chartoken);
            delete extenderlayer;
        }
        
        Window(Window &&other);
        
        Window &operator=(Window &&other);
        
        virtual Geometry::Size GetInteriorSize() const override {
            return Gorgon::Window::GetSize();
        }
        
        virtual bool IsDisplayed () const override {
            return Gorgon::Window::IsVisible();
        }
        
        using Gorgon::Window::Resize;
        
        virtual void Resize(const Geometry::Size &size) override {
            Gorgon::Window::Resize(size);
            
            distributeparentboundschanged();
        }
        
        virtual bool ResizeInterior(Geometry::Size size) override {
            Gorgon::Window::Resize(size);
            
            return GetSize() == size;
        }
        
        /// Closes the window, returning the execution to the
        /// point where Run function is called. It allows current
        /// frame to be completed before quiting.
        virtual void Quit() override {
            quiting = true;
        }

        virtual void Run() override {
            while(!quiting) {
                Gorgon::NextFrame();
            }

            Gorgon::Window::Close();
        }

        virtual void Step() override {
            Gorgon::NextFrame();
        }
        
        /// Window does not do any scrolling, thus cannot ensure visibility
        bool EnsureVisible(const UI::Widget &) override {
            return true;
        }
        
        virtual ExtenderRequestResponse RequestExtender(const Gorgon::Layer &self) override {
            return {true, &extenderadapter, self.TranslateToTopLevel(), GetSize()};
        }
        
        /// The spacing should be left between widgets
        virtual int GetSpacing() const override;
        
        using WidgetContainer::GetUnitSize;
        
        /// Returns the unit width for a widget. This size is enough to
        /// have a bordered icon. Widgets should be sized according to unit
        /// width and spacing. A single unit width would be too small for
        /// most widgets.
        virtual int GetUnitSize() const override;
        
        
        /// Overrides default spacing and unitwidth
        void SetSizes(int spacing, int unitwidth) {
            this->spacing = spacing;
            this->unitwidth = unitwidth;
            issizesset = true;
        }
        
        /// Sets the unit size automatically. Full width will be at least
        /// given units wide. Returns remaining size.
        int AutomaticUnitSize(int spacing, int units = 6) override {
            this->spacing   = spacing;
            this->unitwidth = ( GetInteriorSize().Width - spacing * (units-1) ) / units;
            
            return GetInteriorSize().Width - (this->unitwidth * units + this->spacing * (units-1));
        }
        
        /// Return to use default sizes
        void UseDefaultSizes() {
            issizesset = false;
        }

        /// Returns a container that can be used to place windows
        WidgetContainer &WindowContainer() {
            return windowadapter;
        }

        /// Returns a container that can be used to place dialog windows. Dialog windows stays above
        /// bars (status, menu, etc...)
        WidgetContainer &DialogContainer() {
            return dialogadapter;
        }

        /// Returns a container that can be used to place bars like statusbar, menubar, or taskbar.
        /// Bar container stays above windows
        WidgetContainer &BarContainer() {
            return baradapter;
        }

        /// Returns a container that can be used to place widgets under the graphical contents of
        /// the window.
        WidgetContainer &UnderContainer() {
            return dialogadapter;
        }
        
        virtual void Destroy() override;
        
        TooltipManager Tooltips = TooltipManager{*this};
        
        using WidgetContainer::Add;
        using Gorgon::Window::Add;
        using Gorgon::Window::KeyEvent;
        using Gorgon::Window::CharacterEvent;

    protected:
        virtual Gorgon::Layer &getlayer() override {
            return *widgetlayer;
        }

        decltype(KeyEvent)::Token keyinit();

        decltype(CharacterEvent)::Token charinit();
        
        void init();
        
        void focuschangedin(LayerAdapter &cont);

        virtual void focuschanged() override {
            if(HasFocusedWidget() && focusedadapter) {
                focusedadapter->RemoveFocus();
                focusedadapter = nullptr;
            }
        }

    private:
        bool quiting = false;
        LayerAdapter extenderadapter, windowadapter, baradapter, dialogadapter, underadapter;
        Graphics::Layer *extenderlayer = nullptr;
        Graphics::Layer *dialoglayer = nullptr;
        Graphics::Layer *barlayer = nullptr;
        Graphics::Layer *windowlayer = nullptr;
        Graphics::Layer *widgetlayer = nullptr;
        Graphics::Layer *underlayer = nullptr;

        LayerAdapter *focusedadapter = nullptr;

        decltype(KeyEvent)::Token inputtoken = keyinit(); //to initialize token after window got constructed
        decltype(CharacterEvent)::Token chartoken = charinit(); //to initialize token after window got constructed
        
        int spacing   = 0;
        int unitwidth = 0;
        bool issizesset = false;
    };
    
} }

