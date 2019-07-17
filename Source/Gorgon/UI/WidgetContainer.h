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
        ~WidgetContainer() { }

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
        /// widget does not exits, nothing will happen.
        void Remove(WidgetBase &widget);

        /// Changes the focus order of the given widget. If the order
        /// is out of bounds, the widget will be moved to the end.
        /// You may use the functions in the widget to manage focus order.
        void ChangeFocusOrder(WidgetBase &widget, int order);

        /// Changes the z-order of the widget. If the order is out of
        /// bounds, the widget will be drawn on top. You may use the
        /// functions in the widget to manage z-order.
        void ChangeZorder(WidgetBase &widget, int order);
        
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
        
        /// Toggles the state of tab switcing. Tab switch allows user to 
        /// change focus to the next widget using tab key
        void ToggleTabSwitchEnabledState() { tabswitch=!tabswitch; }
        
        /// Sets the state of tab switcing. Tab switch allows user to 
        /// change focus to the next widget using tab key
        virtual void SetTabSwitchEnabledState(bool state) { tabswitch = state; }
        
        //? Use some other type for default? ActivatableWidget?
        /// Returns the default element of the container. Default elements are
        /// generally buttons, however, any widget that can be activated can be
        /// default objects.
        virtual WidgetBase &GetDefault() const {
            if(!def) 
                throw std::runtime_error("Container does not have a default");
            
            return *def; 
        }
        
        virtual bool HasDefault() const { return def!=NULL; }
        
        virtual void SetDefault(WidgetBase &widget) { def=&widget; }
        
        virtual void RemoveDefault() { def=NULL; }
        
        virtual WidgetBase &GetCancel() const { 
            if(!def) 
                throw std::runtime_error("Container does not have a default");
            
            return *cancel; 
        }
        
        virtual bool HasCancel() const { return cancel!=NULL; }
        
        virtual void SetCancel(WidgetBase &widget) { cancel=&widget; }
        
        virtual void RemoveCancel() { cancel=NULL; }
        
    protected:
        /// This container is sorted by the focus order
        Containers::Collection<WidgetBase> widgets;

        /// This function can return false to prevent the given
        /// widget from getting added to the container.
        virtual bool addingwidget(WidgetBase &widget) { return true; }

        /// This function is called after a widget is added.
        virtual void widgetadded(WidgetBase &widget) { }
        
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
        bool isenabled = true;
        bool tabswitch = true;
        WidgetBase *def;
        WidgetBase *cancel;
    };
    
} }
