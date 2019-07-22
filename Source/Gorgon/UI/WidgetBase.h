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

		virtual Geometry::Point GetLocation() const = 0;

		/// Changes the size of the widget.
		virtual void Resize(int w, int h) { Resize({w, h}); };

		/// Changes the size of the widget.
		virtual void Resize(Geometry::Size size) = 0;
        
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

		/// Returns if this widget is focused.
		bool IsFocused() const { return focus; }

		/// Returns if this widget has a parent
		bool HasParent() const { return parent != nullptr; }

		/// Returns the parent of this widget, throws if it does not have
		/// a parent.
		WidgetContainer &GetParent() const;

		/// Should return the layer that is used for this widget
		virtual Layer &GetLayer() = 0;

		/// This function should be called whenever a key is pressed or released.
		virtual bool KeyEvent(Input::Key, float) { return false; }

		/// This function should be called whenever a character is received from
		/// operating system.
		virtual bool CharEvent(Char) { return false; }

    protected:
		/// Called when it is about to be added to the given container
		virtual bool addingto(WidgetContainer &) { return true; }

		/// Called when this widget added to the given container
		virtual void addedto(WidgetContainer &container) { parent = &container; }

		/// Called before this widget is removed from its parent.
		virtual bool removingfrom() { return true; }

		/// Called after this widget is removed from its parent.
		virtual void removed() { parent = nullptr; }

		/// Should return true if the widget can be focused
        virtual bool allowfocus() const { return true; }
        
        /// This is called after the focus is transferred to this widget.
        virtual void focused() { focus = true; }
        
        /// Should return true if the widget can loose the focus right now.
        virtual bool canloosefocus() const { return true; }
        
        /// This is called after the focus is lost.
        virtual void focuslost() { focus = false; }
        
    private:
        bool visible = true;
        bool enabled = true;
        bool focus   = false;

		WidgetContainer *parent = nullptr;
    };
    
    
} }
