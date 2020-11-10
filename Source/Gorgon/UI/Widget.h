#pragma once

#include "../Layer.h"
#include "../Geometry/Point.h"
#include "../Input/Keyboard.h"
#include "../Geometry/PointProperty.h"
#include "../Geometry/SizeProperty.h"

namespace Gorgon { namespace UI {

    class WidgetContainer;

    /**
    * This class is the base for all widgets. 
    */
    class Widget {
    friend class WidgetContainer;
        //Non-virtual functions for visual studio

        void resize(const Geometry::Size &size) {
            Resize(size);
        }

        Geometry::Size getsize() const {
            return GetSize();
        }

        void move(const Geometry::Point &value) {
            Move(value);
        }

        Geometry::Point getlocation() const {
            return GetLocation();
        }
    public:
        
        Widget() : Location(this), Size(this) {
        }
        
        Widget(Widget &&) = default;
        
        virtual ~Widget() { }
        
        /// Moves this widget to the given position.
        void Move(int x, int y) { Move({x, y}); }
        
        /// Moves this widget to the given position.
        virtual void Move(const Geometry::Point &location) = 0;

        /// Returns the location of the widget
        virtual Geometry::Point GetLocation() const = 0;

        /// Changes the size of the widget.
        virtual void Resize(int w, int h) { Resize({w, h}); };

        /// Changes the size of the widget.
        virtual void Resize(const Geometry::Size &size) = 0;

        /// Returns the size of the widget
        virtual Geometry::Size GetSize() const = 0;
        
        /// Returns the bounds of the widget
        Geometry::Bounds GetBounds() const { return {GetLocation(), GetSize()}; }
        
        /// Returns the width of the widget
        int GetWidth() const { return GetSize().Width; }
        
        /// Returns the height of the widget
        int GetHeight() const { return GetSize().Height; }
        
        /// Sets the width of the widget
        void SetWidth(int width) { Resize(width, GetHeight()); }
        
        /// Sets the height of the widget
        void SetHeight(int height) { Resize(GetWidth(), height); }
        
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
        void Show() { SetVisible(true); }
        
        /// Hides this widget, when hidden, widgets cannot gain focus
        void Hide() { SetVisible(false); }
        
        /// Toggles the visibility state of the widget.
        void ToggleVisible() { SetVisible(!IsVisible()); }
        
        /// Changes the visibility of the widget
        void SetVisible(bool value);
        
        /// Returns if the widget is visible
        bool IsVisible() const {
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
        virtual void SetIsFloating(bool value) {
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
        virtual bool KeyEvent(Input::Key, float) { return false; }

        /// This function should be called whenever a character is received from
        /// operating system.
        virtual bool CharacterEvent(Char) { return false; }
        
        /// For widgets that supports it, this will trigger finalization the user
        /// interaction. This may cause widget fire change event or reorganize
        /// itself.
        virtual bool Done() { return true; }

        /// This event will be fired when the widget receives or looses focus.
        Event<Widget> FocusEvent = Event<Widget>{*this};

        /// This event will be fired when the area that the widget occupies on
        /// its container is changed. It will be fired when the widget is hidden
        /// or shown or its parent is changed. Movement, resize and parent change
        /// will not trigger this event if the widget is not visible. Similarly,
        /// if the object does not have a parent movement and resize will not
        /// trigger this event. Organizers use this event to rearrange widgets, 
        /// thus it is not advisable to remove all handlers from this event.
        Event<Widget> BoundsChangedEvent = Event<Widget>{*this};
        
        Geometry::PointProperty<Widget, &Widget::getlocation, &Widget::move> Location;
        Geometry::SizeProperty<Widget, &Widget::getsize, &Widget::resize> Size;
        
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

        /// This function is called when the parent's enabled state changes.
        virtual void parentenabledchanged(bool state) { }
        
    private:
        bool visible = true;
        bool enabled = true;
        bool focus   = false;
        bool floating= false;
        
        /// Never call this function
        virtual void hide() = 0;
        
        /// Never call this function
        virtual void show() = 0;
        
        WidgetContainer *parent = nullptr;
    };
    
    
} }
