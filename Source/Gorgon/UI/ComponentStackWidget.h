#pragma once

#include "WidgetBase.h"
#include "ComponentStack.h"

namespace Gorgon { namespace UI {
    
   
    /**
     * This class acts as a widget base that uses component stack to handle
     * rendering, resizing and other operations.
     */
    class ComponentStackWidget : public WidgetBase {
    public:
        ComponentStackWidget(const Template &temp) : stack(temp, temp.GetSize()) { }
        
        ComponentStackWidget(ComponentStackWidget &&) = default;
        
        ComponentStackWidget &operator =(ComponentStackWidget &&) = default;
        
        using WidgetBase::Move;
        
        virtual void Move(Geometry::Point location) override {
			stack.Move(location);

			if(IsVisible() && HasParent())
                boundschanged();
		}
        
        using WidgetBase::Resize;
        
        virtual void Resize(Geometry::Size size) override {
            stack.Resize(size);

			if(IsVisible() && HasParent())
                boundschanged();
        }

		virtual Geometry::Point GetLocation() const override {
			return stack.GetLocation();
		}

		virtual Geometry::Size GetSize() const override {
			return stack.GetSize();
		}

	protected:
        mutable ComponentStack stack;

		virtual void focused() override {
			stack.AddCondition(ComponentCondition::Focused);
			FocusEvent();
		}

		virtual void focuslost() override {
			stack.RemoveCondition(ComponentCondition::Focused);
			FocusEvent();
		}
        
		virtual void removed() override {
			WidgetBase::removed();

			stack.FinalizeTransitions();
		}

		virtual void addto(Layer &layer) override {
			layer.Add(stack);
		}


		virtual void removefrom(Layer &layer) override {
			layer.Remove(stack);
		}


		virtual void setlayerorder(Layer &, int order) override {
			stack.PlaceBefore(order);
		}

    private:
        virtual void show() override {
            stack.Show();

			if(HasParent())
                boundschanged();
		}
        
        virtual void hide() override {
            stack.Hide();

			if(HasParent())
                boundschanged();
		}
	};
    
} }
