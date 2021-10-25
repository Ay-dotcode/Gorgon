#pragma once

#include "../Layer.h"
#include "../Geometry/Point.h"
#include "../Input/Keyboard.h"
#include "../Property.h"
#include "../Geometry/PointProperty.h"
#include "../Geometry/SizeProperty.h"
#include "Dimension.h"

namespace Gorgon { namespace UI {

    class WidgetContainer;

    /**
    * This class is the base for all widgets. 
    */
    class Widget {
        friend class WidgetContainer;

    public:
        
        explicit Widget(const UnitSize &size) : Location(this), Size(this), Tooltip(this), size(size) {
        }
        
        Widget(Widget &&) = default;
        
        virtual ~Widget();
        
        /// Moves this widget to the given position. Widget might be
        /// moved by organizers.
        void Move(UnitDimension x, UnitDimension y) { Move({x, y}); }
        
        /// Moves this widget to the given position.
        void Move(const UnitPoint &value);

        /// Returns the location of the widget. This is the assigned location
        /// and may not reflect actual position. Widget might be moved by
        /// organizers.
        UnitPoint GetLocation() const {
            return location;
        }

        /// Returns the current location of the widget in pixels. This might
        /// be different than location that is set.
        virtual Geometry::Point GetCurrentLocation() const = 0;

        /// Changes the size of the widget.
        virtual void Resize(UnitDimension w, UnitDimension h) { Resize({w, h}); };

        /// Changes the size of the widget.
        virtual void Resize(const UnitSize &size);

        /// Returns the size of the widget
        UnitSize GetSize() const {
            return size;
        }

        /// Returns the current size of the widget in pixels. This might be
        /// different than the size that is set.
        virtual Geometry::Size GetCurrentSize() const = 0;
        
        /// Returns the bounds of the widget in pixels.
        Geometry::Bounds GetBounds() const { return {GetCurrentLocation(), GetCurrentSize()}; }
        
        /// Returns the width of the widget
        UnitDimension GetWidth() const { return GetSize().Width; }
        
        /// Returns the height of the widget
        UnitDimension GetHeight() const { return GetSize().Height; }
        
        /// Returns the width of the widget in pixels.
        int GetCurrentWidth() const { return GetCurrentSize().Width; }

        /// Returns the height of the widget in pixels.
        int GetCurrentHeight() const { return GetCurrentSize().Height; }

        /// Sets the width of the widget
        void SetWidth(UnitDimension width) { Resize(width, GetHeight()); }
        
        /// Sets the height of the widget
        void SetHeight(UnitDimension height) { Resize(GetWidth(), height); }
        
        /// Activates the widget. This might perform the action if the
        /// widget is a button, forward the focus if it is a label or
        /// focus if it is an input field.
        virtual bool Activate() = 0;

        /// Removes the widget from its parent. Returns true if the widget
        /// has no parent after the operation.
        bool Remove();
        
        /// If this widget can be focused currently
        bool AllowFocus() const { return allowfocus() && visible; }
        
        /// Transfers the focus to this widget. Returns true if the focus
        /// is actually transferred to this widget
        bool Focus();
        
        /// Removes the focus from this widget if this widget is focused.
        /// This function will not transfer the focus to another widget.
        bool Defocus();
        
        /// Returns if this widget is focused.
        bool IsFocused() const { return focus; }
        
        /// Shows this widget, widgets are visible by default.
        virtual void Show() { SetVisible(true); }
        
        /// Hides this widget, when hidden, widgets cannot gain focus
        virtual void Hide() { SetVisible(false); }
        
        /// Toggles the visibility state of the widget.
        void ToggleVisible() { SetVisible(!IsVisible ()); }
        
        /// Changes the visibility of the widget
        virtual void SetVisible(bool value);
        
        /// Returns if the widget is visible
        virtual bool IsVisible() const {
            return visible;
        }
        
        /// Ensures this widget is visible in its container by scrolling it
        /// into view. This function will not change visibility of the widget
        /// and will return false if the widget is not visible. This function 
        /// cannot be expected to take outside factors into account, such as 
        /// occlusion.
        bool EnsureVisible() const;

        /// Enables the widget so that the user can interact with it
        void Enable() { SetEnabled(true); }

        /// Disables the widget so that the user cannot interact with it
        void Disable() { SetEnabled(false); }

        /// Toggles enabled state of the widget
        void ToggleEnabled() { SetEnabled(!IsEnabled()); }

        /// Sets the enabled state of the widget
        virtual void SetEnabled(bool value) = 0;

        /// Returns whether the widget is enabled.
        virtual bool IsEnabled() const = 0;

        /// Returns if this widget has a parent
        bool HasParent() const { return parent != nullptr; }

        /// Returns the parent of this widget, throws if it does not have
        /// a parent.
        WidgetContainer &GetParent() const;
        
        /// Sets floating status of this widget. Floating widgets will not
        /// be moved or resized by organizers.
        virtual void SetFloating(bool value) {
            if(floating != value)
                boundschanged();
                
            floating = value;
        }
        
        /// Returns floating status of this widget. Floating widgets will not
        /// be moved or resized by organizers.
        bool IsFloating() const {
            return floating;
        }

        /// This function should be called whenever a key is pressed or released.
        virtual bool KeyPressed(Input::Key, float) { return false; }

        /// This function should be called whenever a character is received from
        /// operating system.
        virtual bool CharacterPressed(Char) { return false; }
        
        /// For widgets that supports it, this will trigger finalization the user
        /// interaction. This may cause widget fire change event or reorganize
        /// itself.
        virtual bool Done() { return true; }
        
        /// Sets the tooltip to the given value. This will immediately update the display if it is
        /// currently displayed.
        void SetTooltip(const std::string &value) {
            if(tooltip != value) {
                tooltip = value;
                TooltipChangedEvent();
            }
        }
        
        /// Removes the tooltip from this widget
        void RemoveTooltip() {
            SetTooltip("");
        }
        
        /// Returns the tooltip of this widget.
        std::string GetTooltip() const {
            return tooltip;
        }

        /// This event will be fired when the widget receives or looses focus.
        Event<Widget> FocusEvent = Event<Widget>{*this};

        /// This event will be fired when the area that the widget occupies on
        /// its container is changed. It will be fired when the widget is hidden
        /// or shown or its parent is changed. Movement, resize and parent change
        /// will not trigger this event if the widget is not visible. Similarly,
        /// if the object does not have a parent movement and resize will not
        /// trigger this event. Organizers use this event to rearrange widgets, 
        /// thus it is not advisable to remove all handlers from this event.
        Event<Widget> BoundsChangedEvent    = Event<Widget>{*this};
        
        /// This event will be fired when the mouse enters the widget area.
        Event<Widget> MouseEnterEvent       = Event<Widget>{*this};
        
        /// This event will be fired when the mouse exits the widget area.
        Event<Widget> MouseLeaveEvent       = Event<Widget>{*this};
        
        /// This event will be fired when the tooltip of the widget is changed
        Event<Widget> TooltipChangedEvent   = Event<Widget>{*this};
        
        /// This event will be fired before the widget is destroyed. This event 
        /// is fired in the middle of the destruction. It is not safe to access
        /// the widget at this point. Only the aliases to this widget should be
        /// invalidated in the event handlers registered to this function.
        Event<Widget> DestroyedEvent        = Event<Widget>{*this};
        
        Geometry::basic_PointProperty<Widget, UnitPoint, &Widget::GetLocation, &Widget::Move> Location;
        Geometry::basic_SizeProperty<Widget, UnitSize, &Widget::GetSize, &Widget::Resize> Size;
        TextualProperty<Widget, std::string, &Widget::GetTooltip, &Widget::SetTooltip> Tooltip;
        
        /// This is a debug feature
        void setname(std::string value) {
#ifndef NDEBUG
            dbgname = value;
#endif
        }
        
#ifndef NDEBUG
    std::string dbgname;
#endif

    protected:
        /// Should resize the widget in pixels
        virtual void resize(const Geometry::Size &size) = 0;

        /// Should move the widget in pixels
        virtual void move(const Geometry::Point &value) = 0;

        /// Called when it is about to be added to the given container
        virtual bool addingto(WidgetContainer &) { return true; }
        
        /// When called, widget should locate itself on to this layer.
        virtual void addto(Layer &layer) = 0;

        /// Called when this widget added to the given container
        virtual void addedto(WidgetContainer &container);

        /// When called, widget should remove itself from the given layer
        virtual void removefrom(Layer &layer) = 0;

        /// Called before this widget is removed from its parent.
        virtual bool removingfrom() { return true; }

        /// Called after this widget is removed from its parent.
        virtual void removed();

        /// When called, widget should reorder itself in layer hierarchy
        virtual void setlayerorder(Layer &layer, int order) = 0;

        /// Should return true if the widget can be focused
        virtual bool allowfocus() const { return true; }
        
        /// This is called after the focus is transferred to this widget.
        virtual void focused();
        
        /// Should return true if the widget can loose the focus right now.
        /// Even if you return false, you still might be forced to loose
        /// focus, even without this function getting called.
        virtual bool canloosefocus() const { return true; }
        
        /// This is called after the focus is lost. This is called even if
        /// focus removal is forced.
        virtual void focuslost();
        
        /// Call this function when the widget bounds is changed
        virtual void boundschanged();
        
        /// Call this function when the bounds of the parent is changed
        virtual void parentboundschanged();
        
        /// This function is called when the parent's enabled state changes.
        virtual void parentenabledchanged(bool /*state*/) { }
        
        /// This function is called when this widget is made or unmade default.
        virtual void setdefaultstate(bool /*default*/) { }
        
        /// This function is called when this widget is made or unmade cancel.
        virtual void setcancelstate(bool /*cancel*/) { }
        
        virtual void mouseenter();
        
        virtual void mouseleave();
        
        std::string tooltip;

        
    private:
        bool visible = true;
        bool enabled = true;
        bool focus   = false;
        bool floating= false;

        UnitPoint location;
        UnitSize  size;
        
        /// Never call this function
        virtual void hide() = 0;
        
        /// Never call this function
        virtual void show() = 0;
        
        WidgetContainer *parent = nullptr;
    };
    
    
}
namespace Widgets {
    using UI::Pixels;
    using UI::Percentage;
    using UI::Units;
    using UI::Dimension;
    using UI::UnitDimension;
}
}
