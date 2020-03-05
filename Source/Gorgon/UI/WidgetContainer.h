#pragma once

#include <stdexcept>

#include "WidgetBase.h"
#include "../Layer.h"
#include "../Input/Keyboard.h"
#include "Organizers/Base.h"

namespace Gorgon { namespace UI {
    /**
    * This class is the base class for all widget containers.
    * All widgets require a layer to be placed on, to allow
    * widget containers that are also widgets, this class is
    * left abstract. You may derive from this class and WidgetBase
    * at the same time.
    */
    class WidgetContainer {
        friend class WidgetBase;
    public:
        /// Defines focus strategy for the container. Default is Inherit
        enum FocusStrategy {
            /// Inherit from the parent. If this container is top level
            /// then it will be AllowAll
            Inherit,

            /// All widgets that can be focused are allowed to receive focus, 
            /// including buttons but not labels.
            AllowAll,

            /// Widgets that require input to work or benefit from input
            /// greatly are allowed to receive focus. Button, listbox, and select
            /// will loose the ability to receive focus. However, sliders and
            /// input boxes will receive the focus
            Selective,

            /// Only the widget that will not work without input will receive
            /// focus. This includes input boxes. This will also disable tab
            /// switching.
            Strict,

            /// No widget is allowed focus and this container will not handle
            /// any keys.
            Deny
        };

        WidgetContainer() = default;
        
        WidgetContainer(WidgetContainer &&) = default;
        
        /// Virtual destructor
        virtual ~WidgetContainer();
        
        WidgetContainer &operator=(WidgetContainer&&) = default;

        /// Adds the given widget to this container. Widget will 
        /// be placed to the top of the z-order, to the end of the
        /// focus order. If the given widget cannot be added, this
        /// function will return false.
        bool Add(WidgetBase &widget);

        /// Add the given widget to this container. Widget will
        /// be placed to the top of the z-order, and to the specified
        /// focus order. If the given widget cannot be added, this
        /// function will return false. If the index is out of bounds
        /// the widget will be added at the end.
        bool Insert(WidgetBase &widget, int index);

        /// Removes the given widget from this container. If the
        /// widget does not exits, this function will return true without
        /// taking any additional action. If the widget cannot be removed
        /// from the container, this function will return false.
        bool Remove(WidgetBase &widget);

        /// Forcefully removes the given widget from this container.
        void ForceRemove(WidgetBase &widget);

        /// Changes the focus order of the given widget. If the order
        /// is out of bounds, the widget will be moved to the end.
        /// You may use the functions in the widget to manage focus order.
        void ChangeFocusOrder(WidgetBase &widget, int order);

        int GetFocusOrder(const WidgetBase &widget) const;

        /// Changes the z-order of the widget. If the order is out of
        /// bounds, the widget will be drawn on top. You may use the
        /// functions in the widget to manage z-order.
        void ChangeZorder(WidgetBase &widget, int order);
        
        /// Focuses the first widget that accepts focus. If none of the
        /// widgets accept focus or if the currently focused widget blocks
        /// focus transfer then this function will return false.
        bool FocusFirst();

        /// Focuses the next widget that accepts focus. If no widget 
        /// other than the currently focused widget accept focus then 
        /// this function will return false. Additionally, if the currently
        /// focused widget blocks focus transfer, this function will
        /// return false.
        bool FocusNext();

        /// Focuses the next widget that accepts focus starting from the given
        /// focus index. If no widget other than the currently focused widget 
        /// accept focus then this function will return false. Additionally, 
        /// if the currently focused widget blocks focus transfer, this function 
        /// will return false.
        bool FocusNext(int after);

        /// Focuses the next widget that accepts focus starting from the given
        /// focus index. If no widget other than the currently focused widget 
        /// accept focus then this function will return false. Additionally, 
        /// if the currently focused widget blocks focus transfer, this function 
        /// will return false.
        bool FocusNext(const WidgetBase &widget) { return FocusNext(GetFocusOrder(widget)); }

        /// Focuses the last widget in the container. If none of the
        /// widgets accept focus or if the currently focused widget blocks
        /// focus transfer then this function will return false.
        bool FocusLast() { return FocusPrevious(widgets.GetSize()); }

        /// Focuses the previous widget that accepts focus. If no widget 
        /// other than the currently focused widget accept focus then 
        /// this function will return false. Additionally, if the currently
        /// focused widget blocks focus transfer, this function will
        /// return false.
        bool FocusPrevious() { return FocusPrevious(focusindex); }

        /// Focuses the previous widget that accepts focus. If no widget 
        /// other than the currently focused widget accept focus then 
        /// this function will return false. Additionally, if the currently
        /// focused widget blocks focus transfer, this function will
        /// return false.
        bool FocusPrevious(const WidgetBase& widget) { return FocusNext(GetFocusOrder(widget)); }

        /// Focuses the previous widget that accepts focus. If no widget 
        /// other than the currently focused widget accept focus then 
        /// this function will return false. Additionally, if the currently
        /// focused widget blocks focus transfer, this function will
        /// return false.
        bool FocusPrevious(int before);

        /// Sets the focus to the given widget
        bool SetFocusTo(WidgetBase &widget);
        
        /// Removes the focus from the focused widget
        bool RemoveFocus();
        
        /// Forcefully removes the focus from the focused widget
        void ForceRemoveFocus();

        /// Returns if this container has a focused widget
        bool HasFocusedWidget() const { return focusindex != -1; }

        /// Returns the focused widget. If no widget is focused, this function
        /// will throw.
        WidgetBase &GetFocus() const;
        
        /// Should return whether the container is visible. Due to
        /// different container designs and capabilities, setting
        /// visibility depends on the particular container
        virtual bool IsVisible() const = 0;
        
        /// Returns whether container is enabled.
        virtual bool IsEnabled() const { return isenabled; }
        
        /// Enables the container, allowing interaction with the widgets
        /// in it.
        void Enable() {
            SetEnabled(true);
        }
        
        /// Disables the container, disallowing interactions of all widgets
        /// in it.
        void Disable() {
            SetEnabled(false);
        }
        
        /// Toggles the enabled state of this container. If the state is
        /// toggled after the call, this function will return true.
        void ToggleEnabled() { 
            SetEnabled(!IsEnabled());
        }
        
        /// Sets the enabled state of this container. This function will 
        /// return true if the container is enabled at the end of the call.
        virtual void SetEnabled(bool value) {
            if(value != isenabled) {
                isenabled = value;
                distributeparentenabled(value);
            }
        }
        
        /// Should return the interior (usable) size of the container.
        virtual Geometry::Size GetInteriorSize() const = 0;
        
        /// Should resize the interior (usable) size of the container.
        /// If resize operation cannot set the size exactly to the
        /// requested size, this function returns false.
        virtual bool ResizeInterior(Geometry::Size size) = 0;
        
        /// Check if tab switch is enabled. Tab switch allows user to change
        /// focus to the next widget using tab key. Default is enabled.
        virtual bool IsTabSwitchEnabled() const { return tabswitch; }
        
        /// Enable tab switching. Tab switch allows user to change
        /// focus to the next widget using tab key
        void EnableTabSwitch() { SetTabSwitchEnabledState(true); }
        
        /// Disable tab switching. Tab switch allows user to change
        /// focus to the next widget using tab key
        void DisableTabSwitch() { SetTabSwitchEnabledState(false); }
        
        /// Toggles the state of tab switching. Tab switch allows user to 
        /// change focus to the next widget using tab key
        void ToggleTabSwitchEnabledState() { tabswitch=!tabswitch; }
        
        /// Sets the state of tab switching. Tab switch allows user to 
        /// change focus to the next widget using tab key
        virtual void SetTabSwitchEnabledState(bool state) { tabswitch = state; }
        
        /// Returns the begin iterator for the contained widgets
        auto begin() {
            return widgets.begin();
        }
        
        /// Returns the begin iterator for the contained widgets
        auto begin() const {
            return widgets.begin();
        }
        
        /// Returns the end iterator for the contained widgets
        auto end() {
            return widgets.end();
        }
        
        /// Returns the end iterator for the contained widgets
        auto end() const {
            return widgets.end();
        }
        
        /// Returns the number of widgets in this container
        int GetCount() const {
            return (int)widgets.GetCount();
        }
        
        /// Returns the widget at the given index
        const WidgetBase &operator [](int ind) const {
            return widgets[ind];
        }
        
        /// Returns the widget at the given index
        WidgetBase &operator [](int ind) {
            return widgets[ind];
        }
        
        /// Returns the default element of the container. Default widget is
        /// activated when the user presses enter and the focused widget 
        /// does not consume the key or if the user presses ctrl+enter. 
        /// Default elements are generally buttons, however, any widget 
        /// can be designated as default widget. If there is no default object, 
        /// this function will throw. Use HasDefault to check if this container 
        /// has a default widget.
        virtual WidgetBase &GetDefault() const {
            if(!def) 
                throw std::runtime_error("Container does not have a default");
            
            return *def; 
        }
        
        /// Returns if this container has a default object.
        virtual bool HasDefault() const { return def!=nullptr; }
        
        /// Sets the default object of the container. Ideally this should be
        /// a button or a similar widget.
        virtual void SetDefault(WidgetBase &widget) { def=&widget; }
        
        /// Removes the default widget of this container.
        virtual void RemoveDefault() { def=nullptr; }
        
        /// Returns the cancel element of the container which is called when the
        /// use presses escape key. It also might be activated programmatically. 
        /// Cancel elements are generally buttons, however, any widget can 
        /// be cancel widget. If there is no cancel object set, this function 
        /// will throw. Use HasCancel to check if this container has a cancel widget.
        virtual WidgetBase &GetCancel() const { 
            if(!def) 
                throw std::runtime_error("Container does not have a default");
            
            return *cancel; 
        }
        
        /// Returns if this container has a cancel widget.
        virtual bool HasCancel() const { return cancel!=nullptr; }
        
        /// Sets the cancel widget of the container. Ideally this should be
        /// a button or a similar widget.
        virtual void SetCancel(WidgetBase &widget) { cancel=&widget; }
        
        /// Removes the cancel widget of this container.
        virtual void RemoveCancel() { cancel=nullptr; }

        /// Sets the focus strategy, see FocusStrategy
        void SetFocusStrategy(FocusStrategy value) {
            focusmode = value;
        }

        /// Returns the focus strategy set to this container. Do not use this to determine
        /// current strategy, use CurrentFocusStrategy instead.
        FocusStrategy GetFocusStrategy() const {
            return focusmode;
        }

        /// Returns the active focus strategy. This function will not return Inherit.
        FocusStrategy CurrentFocusStrategy() const {
            if(focusmode == Inherit)
                return getparentfocusstrategy();
            else
                return focusmode;
        }
        
        /// Creates a new organizer that lives with this container
        template <class O_, class ...Args_>
        O_ &CreateOrganizer(Args_ &&... args) {
            auto &org = *new O_(std::forward<Args_>(args)...);
            AttachOrganizer(org);
            
            ownorganizer = true;
            
            return org;
        }
        
        /// Attaches an organizer to this container. Ownership is not
        /// transferred
        void AttachOrganizer(Organizers::Base &organizer);
        
        /// Removes the organizer from this container
        void RemoveOrganizer();
        
        /// Returns if this container has an organizer
        bool HasOrganizer() const {
            return organizer != nullptr;
        }
        
        /// Returns the organizer controlling this container. If this container does not have an organizer
        /// this function will throw
        Organizers::Base &GetOrganizer() const {
            if(organizer == nullptr) {
                throw std::runtime_error("The container does not have an organizer");
            }
            
            return *organizer;
        }

        /// This function should be called whenever a key is pressed or released.
        virtual bool KeyEvent(Input::Key key, float state) { return distributekeyevent(key, state, true); }
        
        /// This function should be called whenever a character is received from
        /// operating system.
        virtual bool CharacterEvent(Char c) { return distributecharevent(c); }
        

    protected:
        /// This container is sorted by the focus order
        Containers::Collection<WidgetBase> widgets;

        /// This function can return false to prevent the given
        /// widget from getting added to the container.
        virtual bool addingwidget(WidgetBase &) { return true; }

        /// This function is called after a widget is added.
        virtual void widgetadded(WidgetBase &) { }

        /// This function is called before removing a widget. Return false
        /// to prevent that widget from getting removed. This widget is
        /// guaranteed to be in this container
        virtual bool removingwidget(WidgetBase &) { return true; }

        /// This function is called after a widget is removed
        virtual void widgetremoved(WidgetBase &) { }

        /// If this widget is not top level, return the current strategy used by the
        /// parent. Never return Inherit from this function.
        virtual FocusStrategy getparentfocusstrategy() const {
            return AllowAll;
        }

        /// Returns the layer that will be used to place the contained widgets.
        virtual Layer &getlayer() = 0;

        /// This function is called when the focus is changed
        virtual void focuschanged() { }
        
        /// Performs the standard operations (tab/enter/escape)
        bool handlestandardkey(Input::Key key);

        /// Distributes the pressed key to the focused widget. If action not handled and
        /// and handlestandard is true, this function will also perform standard action.
        bool distributekeyevent(Input::Key key, float state, bool handlestandard);

        /// Distributes a pressed character to the focused widget.
        bool distributecharevent(Char c);

        /// Distributes a enabled state to children
        void distributeparentenabled(bool state);

        /// The boundary of any of the children is changed. Source could be nullptr
        virtual void childboundschanged(WidgetBase *source);

    private:
        bool isenabled              = true;
        bool tabswitch              = true;
        WidgetBase *def             = nullptr;
        WidgetBase *cancel          = nullptr;
        WidgetBase *focused         = nullptr;
        int focusindex	            = -1;
        Organizers::Base *organizer = nullptr;
        bool ownorganizer           = false;

        FocusStrategy focusmode = Inherit;
    };
    
} }
