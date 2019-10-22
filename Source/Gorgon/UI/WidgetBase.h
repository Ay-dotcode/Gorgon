#pragma once

#include "../Layer.h"
#include "../Geometry/Point.h"
#include "../Input/Keyboard.h"

namespace Gorgon { namespace UI {

    class WidgetContainer;

    /**
    * This class is the base for all widgets. 
    */
    class WidgetBase {
    friend class WidgetContainer;
    public:
        
        virtual ~WidgetBase() { }
        
        /// Moves this widget to the given position.
        void Move(int x, int y) { Move({x, y}); }
        
        /// Moves this widget to the given position.
        virtual void Move(Geometry::Point location) = 0;

        /// Returns the location of the widget
        virtual Geometry::Point GetLocation() const = 0;

        /// Changes the size of the widget.
        virtual void Resize(int w, int h) { Resize({w, h}); };

        /// Changes the size of the widget.
        virtual void Resize(Geometry::Size size) = 0;

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

        /// Returns if this widget has a parent
        bool HasParent() const { return parent != nullptr; }

        /// Returns the parent of this widget, throws if it does not have
        /// a parent.
        WidgetContainer &GetParent() const;

        /// This function should be called whenever a key is pressed or released.
        virtual bool KeyEvent(Input::Key, float) { return false; }

        /// This function should be called whenever a character is received from
        /// operating system.
        virtual bool CharacterEvent(Char) { return false; }

        /// This event will be fired when the widget receives or looses focus.
        Event<WidgetBase> FocusEvent		 = Event<WidgetBase>{*this};

        /// This event will be fired when the area that the widget occupies on
        /// its container is changed. It will be fired when the widget is hidden
        /// or shown or its parent is changed. Movement, resize and parent change
        /// will not trigger this event if the widget is not visible. Similarly,
        /// if the object does not have a parent movement and resize will not
        /// trigger this event. Organizers use this event to rearrange widgets, 
        /// thus it is not advisable to remove all handlers from this event.
        Event<WidgetBase> BoundsChangedEvent = Event<WidgetBase>{*this};

    protected:
        /// Called when it is about to be added to the given container
        virtual bool addingto(WidgetContainer &) { return true; }
        
        /// When called, widget should locate itself on to this layer.
        virtual void addto(Layer &layer) = 0;

        /// Called when this widget added to the given container
        virtual void addedto(WidgetContainer &container) {
            if(parent == &container)
                return;
            
            parent = &container;
            
            if(IsVisible())
                boundschanged();
        }

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
        virtual void focused() {
            focus = true;
            FocusEvent();
        }
        
        /// Should return true if the widget can loose the focus right now.
        /// Even if you return false, you still might be forced to loose
        /// focus, even without this function getting called.
        virtual bool canloosefocus() const { return true; }
        
        /// This is called after the focus is lost. This is called even if
        /// focus removal is forced.
        virtual void focuslost() {
            focus = false;
            FocusEvent();
        }
        
        /// Call this function when the widget bounds is changed
        virtual void boundschanged();
        
    private:
        bool visible = true;
        bool enabled = true;
        bool focus   = false;
        
        /// Never call this function
        virtual void hide() = 0;
        
        /// Never call this function
        virtual void show() = 0;
        
        WidgetContainer *parent = nullptr;
    };
    
    
} }
