#pragma once

#include "../Geometry/Point.h"

namespace Gorgon { namespace UI {
   
    /**
     * This class is the base for all widgets. 
     */
    class WidgetBase {
    public:
        
        virtual ~WidgetBase() { }
        
        virtual void Move(int x, int y) = 0;
        
        void Move(Geometry::Point target) { Move(target.X, target.Y); }
        
        virtual void Resize(int w, int h) = 0;
        
        /// Activates the widget. This might perform the action if the
        /// widget is a button, forward the focus if it is a label or
        /// focus if it is an input field.
        virtual bool Activate() = 0;
        
        /// Transfers the focus to this widget. Returns true if the focus
        /// is actually transferred to this widget
        bool Focus();
        
        /// Removes the focus from this widget if this widget is focused.
        /// This function will not transfer the focus to another widget.
        bool Defocus();
        
    protected:
        
        /// Should return true if the widget can be focussed
        virtual bool allowfocus() const { return true; }
        
        /// This is called after the focus is transferred to this widget.
        virtual void focused() { }
        
        /// Should return true if the widget can loose the focus right now.
        virtual bool canloosefocus() const { return true; }
        
        /// This is called after the focus is lost.
        virtual void focuslost() { }
        
    private:
        bool visible = true;
        bool enabled = true;
        bool focus   = false;
    };
    
    
} }
