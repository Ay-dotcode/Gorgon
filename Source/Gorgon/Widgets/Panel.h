#pragma once

#include "../UI/ComponentStackWidget.h"
#include "../UI/WidgetContainer.h"
#include "../Property.h"
#include "../Input/KeyRepeater.h"
#include "Registry.h"
#include "Scrollbar.h"

namespace Gorgon { namespace Widgets {
    
    class Panel : public UI::ComponentStackWidget, public UI::WidgetContainer {
    public:
        
        Panel(const Panel &) = delete;
        
        explicit Panel(const UI::Template &temp);
        
        explicit Panel(Registry::TemplateType type = Registry::Panel_Regular) : Panel(Registry::Active()[type]) { }

        using Widget::Resize;
        
        using Widget::Remove;
        
        using WidgetContainer::Remove;
        
        virtual void Resize(const Geometry::Size &size) override;
        
        using Widget::Move;
        
        virtual void Move(const Geometry::Point &location) override;
        

        virtual bool Activate() override;


        virtual bool KeyEvent(Input::Key, float) override;


        virtual bool CharacterEvent(Char) override;


        virtual bool IsVisible() const override;


        virtual Geometry::Size GetInteriorSize() const override;


        virtual bool ResizeInterior(Geometry::Size size) override;
        
        /// Controls whether scrolling will be enabled vertically or horizontally.
        /// It is possible to use ScrollTo function without enabling scrolling but
        /// the user will not be able to scroll in this case. Enabling scrolling
        /// will also display a scrollbar. Depending on the theme it might not be
        /// visible until there is something to scroll. Disabling scrolling will
        /// not take the scroll position to top. 
        virtual void EnableScroll(bool vertical, bool horizontal);
        
        /// Whether vertical scrolling is enabled
        bool IsVerticalScrollEnabled() const {
            return vscroll;
        }
        
        /// Whether horizontal scrolling is enabled
        bool IsHorizontalScrollEnabled() const {
            return hscroll;
        }
        
        
        /// Scrolls the contents of the panel so that the given location will
        /// be at the top left. If clip is set, the scroll amount cannot go
        /// out of the scrolling region.
        void ScrollTo(Geometry::Point location, bool clip = false) {
            ScrollTo(location.X, location.Y, clip);
        }
        
        /// Scrolls the contents of the panel so that the given location will
        /// be at the top left. If clip is set, the scroll amount cannot go
        /// out of the scrolling region.
        void ScrollTo(int x, int y, bool clip = true);

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
        
        /// Sets the horizontal scroll distance per click in pixels. Default depends
        /// on the default size of the panel.
        void SetScrollDistance(int vert) {
            scrolldist.Y = vert;
        }
        
        /// Sets the scroll distance per click in pixels. Default depends
        /// on the default size of the panel.
        void SetScrollDistance(int hor, int vert) {
            SetScrollDistance({hor, vert});
        }
        
        /// Sets the scroll distance per click in pixels. Default depends
        /// on the default size of the panel.
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
        
        using Widget::EnsureVisible;
        
        bool EnsureVisible(const UI::Widget &widget) override;

        using Widget::Enable;
        using Widget::Disable;
        using Widget::ToggleEnabled;

        virtual void SetEnabled(bool value) override {
            if(value != IsEnabled()) {
                ComponentStackWidget::SetEnabled(value);
                distributeparentenabled(value);
            }
        }

        virtual bool IsEnabled() const override {
            return ComponentStackWidget::IsEnabled();
        }
        
        virtual UI::ExtenderRequestResponse RequestExtender(const Gorgon::Layer &self) override;
        
        /// The spacing should be left between widgets
        virtual int GetSpacing() const override;
        
        /// Returns the unit width for a widget. This size is enough to
        /// have a bordered icon. Widgets should be sized according to unit
        /// width and spacing. A single unit width would be too small for
        /// most widgets.
        virtual int GetUnitWidth() const override;
        
        /// Overrides default spacing and unitwidth
        void SetSizes(int spacing, int unitwidth) {
            this->spacing = spacing;
            this->unitwidth = unitwidth;
            issizesset = true;
        }
        
        /// Sets the unit size automatically. Full width will be at least
        /// given units wide. Returns remaining size.
        int AutomaticUnitSize(int spacing, int units = 6) {
            this->spacing   = spacing;
            this->unitwidth = ( GetInteriorSize().Width - spacing * (units-1) ) / units;
            issizesset      = true;
            
            return GetInteriorSize().Width - (this->unitwidth * units + this->spacing * (units-1));
        }
        
        /// Return to use default sizes
        void UseDefaultSizes() {
            issizesset = false;
        }
        
        /// Report mouse scroll. This function will be called automatically
        /// for regular mouse events. This function will return false if the
        /// given mouse event is not consumed.
        bool MouseScroll(Input::Mouse::EventType type, Geometry::Point location, float amount);
        
    protected:
        virtual bool allowfocus() const override;
        
        virtual void focused() override;

        virtual void focuslost() override;

        virtual Layer &getlayer() override;
        
        void focuschanged() override;
        
        virtual void childboundschanged(Widget *source) override;
        
        virtual void updatecontent();
        
        virtual void updatescroll();
        
        virtual void updatebars();
        
        bool updaterequired = false;
        
        void scrolltox(int x) {
            ScrollTo(x, ScrollOffset().Y);
        }
        
        void scrolltoy(int y) {
            ScrollTo(y);
        }
        
        Input::KeyRepeater repeater;
        
        FocusStrategy getparentfocusstrategy() const override {
            if(HasParent())
                return GetParent().CurrentFocusStrategy();
            else
                return AllowAll;
        }

        virtual void parentenabledchanged(bool state) override {
            ComponentStackWidget::parentenabledchanged(state);

            if(!state && IsEnabled())
                distributeparentenabled(state);
            else if(state && IsEnabled())
                distributeparentenabled(state);
        }
        
        virtual UI::Widget *createvscroll(const UI::Template &temp);
        
        virtual UI::Widget *createhscroll(const UI::Template &temp);
        
        int overscroll = 0;
        bool scrollclipped = true;
        Geometry::Point scrolldist = {80, 45};
        Geometry::Point scrolloffset = {0, 0};
        int scrollspeed = 500;
        Geometry::Point target = {0, 0};
        bool isscrolling = false;
        float scrollleftover = 0;
        bool vscroll = true, hscroll = false;
        
        int spacing   = 0;
        int unitwidth = 0;
        bool issizesset = false;
    };
    
} }

