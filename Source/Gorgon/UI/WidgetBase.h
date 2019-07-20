#pragma once

#include "../Layer.h"
#include "../Geometry/Point.h"

namespace Gorgon { namespace UI {

	class WidgetContainer;
   
    /**
     * This class is the base for all widgets. 
     */
    class WidgetBase {
	friend class WidgetContainer;
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

		WidgetContainer &GetParent() const;

		virtual Layer &GetLayer() const = 0;
        
    protected:
		/// Called when it is about to be added to the given container
		virtual bool addingto(WidgetContainer &) { return true; }

		/// Called when this widget added to the given container
		virtual void addedto(WidgetContainer &) { }

		/// Called before this widget is removed from its parent.
		virtual bool removingfrom() { return true; }

		/// Called after this widget is removed from its parent.
		virtual void removed() { }

		/// Should return true if the widget can be focused
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
