#pragma once

#include "../UI/ComponentStackWidget.h"
#include "../UI/WidgetContainer.h"
#include "../Property.h"

namespace Gorgon { namespace Widgets {
    
    class Panel : public UI::ComponentStackWidget, public UI::WidgetContainer {
    public:
        
        Panel(const Panel &) = delete;
        
        Panel(Panel &&) = default;
        
        explicit Panel(const UI::Template &temp);
        

        virtual bool Activate() override;


        virtual bool KeyEvent(Input::Key, float) override;


        virtual bool CharacterEvent(Char) override;


        virtual bool IsVisible() const override;


        virtual Geometry::Size GetInteriorSize() const override;


        virtual bool ResizeInterior(Geometry::Size size) override;
        
        /// Scrolls the contents of the panel so that the given location will
        /// be at the top left. If clip is set, the scroll amount cannot go
        /// out of the scrolling region.
        void ScrollTo(Geometry::Point location, bool clip = false) {
            ScrollTo(location.X, location.Y, clip);
        }
        
        /// Scrolls the contents of the panel so that the given location will
        /// be at the top left. If clip is set, the scroll amount cannot go
        /// out of the scrolling region.
        void ScrollTo(int x, int y, bool clip);
        
        /// Scrolls the contents of the panel so that the given location will
        /// be at the top.
        void ScrollTo(int y, bool clip = true) {
            ScrollTo(stack.GetValue()[0]*100, y, clip);
        }
        
        /// Returns the current scroll offset
        Geometry::Point ScrollOffset() const {
            auto val = stack.GetValue();
            return {(int)(val[0]*100), (int)(val[1]*100)};
        }
        
        /// Sets the amount of extra scrolling distance after the bottom-most
        /// widget is completely visible in pixels. Default is 0.
        void SetOverscroll(int value) {
            overscroll = value;
        }
        
        /// Returns the amound of extra scrolling distance after the bottom-most
        /// widget is completely visible in pixels.
        int GetOverscroll() const {
            return overscroll;
        }

    protected:
        virtual bool allowfocus() const override;
        
        virtual void focused() override;


        virtual void focuslost() override;


        virtual Layer &getlayer() override;
        
        void focuschanged() override;
        
        virtual void boundschanged() override {
            if(HasOrganizer())
                GetOrganizer().Reorganize();
            
            WidgetBase::boundschanged();
        }
        
        int overscroll = 0;
    };
    
} }
