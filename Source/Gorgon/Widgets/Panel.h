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
        
        using WidgetBase::Resize;
        
        virtual void Resize(Geometry::Size size) override;
        

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
            ScrollTo(ScrollOffset().X, y, clip);
        }

        /// Scrolls the contents an additional amount.
        void ScrollBy(int y, bool clip = true) {
            ScrollTo(ScrollOffset().X, ScrollOffset().Y + y, clip);
        }

        /// Scrolls the contents an additional amount.
        void ScrollBy(int x, int y, bool clip = true) {
            ScrollTo(ScrollOffset().X + x, ScrollOffset().Y + y, clip);
        }
        
        /// Returns the current scroll offset
        Geometry::Point ScrollOffset() const;
        
        /// Returns the current maximum scroll offset
        Geometry::Point MaxScrollOffset() const;
        
        /// Sets the amount of extra scrolling distance after the bottom-most
        /// widget is completely visible in pixels. Default is 0. Does not
        /// apply if everything is visible.
        void SetOverscroll(int value);
        
        /// Returns the amount of extra scrolling distance after the bottom-most
        /// widget is completely visible in pixels.
        int GetOverscroll() const {
            return overscroll;
        }
        
        /// Sets the horizontal scroll distance per click in pixels. Default is 45px.
        void SetScrollDistance(int vert) {
            scrolldist.Y = vert;
        }
        
        /// Sets the scroll distance per click in pixels. Default is 80, 45px.
        void SetScrollDistance(int hor, int vert) {
            SetScrollDistance({hor, vert});
        }
        
        /// Sets the scroll distance per click in pixels. Default is 80, 45px.
        void SetScrollDistance(Geometry::Point dist) {
            scrolldist = dist;
        }
        
        /// Returns the scroll distance per click
        Geometry::Point GetScrollDistance() const {
            return scrolldist;
        }
        
        /// Disables smooth scrolling of the panel
        void DisableSmoothScroll() {
            SetSmoothScrollSpeed(0);
        }
        
        /// Adjusts the smooth scrolling speed of the panel. Given value is
        /// in pixels per second, default value is 250.
        void SetSmoothScrollSpeed(int value);
        
        /// Returns the smooth scrolling speed of the panel. If smooth scroll
        /// is disabled, this value will be 0.
        int GetSmoothScrollSpeed() const {
            return scrollspeed;
        }
        
        /// Returns if the smooth scroll is enabled.
        bool IsSmoothScrollEnabled() const {
            return scrollspeed != 0;
        }
        
    protected:
        virtual bool allowfocus() const override;
        
        virtual void focused() override;


        virtual void focuslost() override;


        virtual Layer &getlayer() override;
        
        void focuschanged() override;
        
        virtual void childboundschanged(WidgetBase *source) override;
        
        void updatecontent();
        
        bool updaterequired = false;
        
        int overscroll = 0;
        bool scrollclipped = true;
        Geometry::Point scrolldist = {80, 45};
        int scrollspeed = 250;
    };
    
} }
