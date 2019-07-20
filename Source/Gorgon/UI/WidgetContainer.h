#pragma once

#include "WidgetBase.h"
#include "../Layer.h"

namespace Gorgon { namespace UI {


    /**
    * This class is the base class for all widget containers.
    * All widgets require a layer to be placed on, to allow
    * widget containers that are also widgets, this class is
    * left abstract. You may derive from this class and WidgetBase
    * at the same time.
    */
    class WidgetContainer {
    public:
        /// Virtual destructor
        virtual ~WidgetContainer() { }

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
        
        /// Should return whether the container is visible. Due to
        /// different container designs and capabilities, setting
        /// visibility depends on the particular container
        virtual bool IsVisible() const = 0;
        
        /// Returns whether container is enabled.
        virtual bool IsEnabled() const { return isenabled; }
        
        /// Enables the container, allowing interaction with the widgets
        /// in it. This function will return true if the container is
        /// enabled at the end of the call.
        bool Enable() {
            if(!IsEnabled()) {
                isenabled = true;
                return enable();
            }
            
            return true;
        }
        
        /// Disables the container, disallowing interactions of all widgets
        /// in it. This function will return true if the container is
        /// enabled at the end of the call.
        bool Disable() {
            if(IsEnabled()) {
                isenabled = false;
                return disable();
            }
            
            return true;
        }
        
        /// Toggles the enabled state of this container. If the state is
        /// toggled after the call, this function will return true.
        bool ToggleEnabled() { 
            if(!isenabled)
                return Enable(); 
            else
                return Disable();
        }
        
        /// Sets the enabled state of this container. This function will 
        /// return true if the container is enabled at the end of the call.
        bool SetEnabled(bool enabled) {
            if(enabled)
                return Enable();
            else
                return Disable();
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
        
        /// This function is called when the container is to be enabled. This function
        /// will only be called when the container was disabled prior to the call.
        /// Return false if this container cannot be enabled.
        virtual bool enable() { return true; }
        
        /// This function is called when the container is to be disabled. This function
        /// will only be called when the container is enabled prior to the call.
        /// Return false if this container cannot be disabled.
        virtual bool disable() { return true; }

        /// Returns the layer that will be used to place the contained widgets.
        virtual Layer &layer() = 0;
        
    
    private:
        bool isenabled       = true;
        bool tabswitch       = true;
        WidgetBase *def      = nullptr;
        WidgetBase *cancel   = nullptr;
		WidgetBase *focused  = nullptr;
		int focusindex	     = -1;
    };
    
} }
