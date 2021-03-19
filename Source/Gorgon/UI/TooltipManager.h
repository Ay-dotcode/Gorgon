#pragma once

#include "../Geometry/Point.h"

#include <string>
#include <functional>
#include "../Event.h"


namespace Gorgon { namespace UI {
    
    class Widget;
    class WidgetContainer;
    
    /**
     * This class manages tooltips in a UI system. Only one tooltip manager is enough for entire
     * UI system. UI::Window provides TooltipManager, therefore, it is only necessary to create a
     * TooltipManager object if you are using LayerAdapter to create UI chain manually.
     *
     * Tooltip manager has two modes: static and dynamic. Dynamic is the default mode. In dynamic 
     * mode, manager resizes, moves, shows, and hides tooltip widget automatically. You may still 
     * access this widget. In static mode manager only updates the displayed text. If the tooltip 
     * will be displayed in a fixed place, ideally you should also remove initial delay.
     *
     * By default, tooltip manager creates a label to display and manage tooltips. This can be 
     * changed. In static mode manager does not require a widget, only a SetText callback. For 
     * dynamic mode to work, a widget must be supplied to the manager.
     */
    class TooltipManager {
    public:
        /// Tooltip manager requires a widget container to work.
        TooltipManager(WidgetContainer &container);
        
        /// Destructor
        ~TooltipManager();
        
        /// Sets the delay in milliseconds before the tooltip is shown. Default value is 1000.
        void SetDelay(int value) {
            delay = value;
        }
        
        /// Returns the delay in milliseconds before the tooltip is shown.
        int GetDelay() const {
            return delay;
        }
        
        /// Sets the duration in milliseconds tooltip will stay on after the mouse leaves a widget. 
        /// If the mouse enters a widget with tooltip before this duration is expended, tooltip will
        /// be updated immediately without waiting for delay or linger. Default is 2000.
        void SetLinger(int value) {
            linger = value;
        }
        
        /// Returns the delay in milliseconds the tooltip will be displayed after the mouse leaves
        /// a widget.
        int GetLinger() const {
            return linger;
        }
        
        /// Removes all the delays in the tooltip including linger.
        void RemoveDelay() {
            delay  = 0;
            linger = 0;
        }
        
        
        /// Sets the mouse movement tolerance. If the mouse is moved more than this amount, tooltip
        /// delay is reset. Default value is 5.
        void SetTolerence(int value) {
            tolerance = value;
        }
        
        /// Returns the mouse movement tolerance before the tooltip delay is reset.
        int GetTolerence() const {
            return tolerance;
        }
        
        /// Enables tooltips. Manager starts enabled.
        void Enable();
        
        /// Disables tooltips. Manager starts enabled.
        void Disable();
        
        /// Hides currently displayed tooltip. The tooltip will only shown if mouse moves to
        /// another widget or tooltip text is changed.
        void Hide();
        
        /// Shows the given text until hovered widget is changed or tooltip of the currently hovered
        /// widget is changed. Tooltip will not be displayed if the text is empty.
        void Show(const std::string &text);
        
        /// Sets the enabled state of the tooltip manager.
        void SetEnabled(bool value) {
            if(value) Enable();
            else      Disable();
        }
        
        /// Returns if the manager is enabled
        bool IsEnabled() const {
            return token != 0;
        }
        
        /// Returns if a tooltip is displayed
        bool IsDisplayed() const {
            return displayed;
        }
        
        /// If a tooltip is displayed returns it, if not this function returns empty string.
        std::string GetTooltip() const {
            if(displayed) return tooltip;
            else          return "";
        }
        
        /// Returns the widget that will be managed by this manager. Throws if no target is set. You
        /// may use CreateTarget or SetTarget functions to create a target. If a widget registry
        /// available, constructor will create a target. If not, it will wait until a registry is
        /// created and then it creates the target automatically if it is not set already.
        Widget &GetTarget() const {
            if(!target)
                throw std::runtime_error("The tooltip target is not set");
            
            return *target;
        }
        
        /// Sets the target that will be used in dynamic mode. If own is true, ownership is 
        /// transferred to this manager
        void SetTarget(Widget &target, bool own);
        
        /// Changes the set text function that will be used to set the tooltip text. CreateTarget
        /// overwrites this value. If there is a tooltip displayed, this function will be called
        /// immediately.
        void SetSetText(std::function<void(const std::string &)> value);
        
        /// Creates a target automatically if there is an active widget registry. Returns true if
        /// successful. Replaces both target and SetText function. This function creates a label
        /// with Tooltip template.
        bool CreateTarget() {
            return false;
        }
        
        /// This function is called automatically to detect current mouse location, adjust and 
        /// display tooltip
        void Tick();
        
    protected:
        
        void destroyed();
        
        void changed();
        
    private:
        WidgetContainer                          *container     ;
        Widget                                   *current       = nullptr;
        
        std::function<void(const std::string &)>  settext       ;
        Widget                                   *target        = nullptr;
        bool                                      owntarget     = false;
        
        EventToken                                token         = 0;
        int                                       delay         = 1000;
        int                                       linger        = 2000;
        int                                       tolerance     = 5;
        
        int                                       delayleft     = -1;
        int                                       lingerleft    = -1;
        Geometry::Point                           lastlocation  = {0, 0};
        std::string                               tooltip       ;
        bool                                      displayed     = false;
        
        EventToken                                changetoken   = 0;
        EventToken                                destroytoken  = 0;

    };
    
} }
