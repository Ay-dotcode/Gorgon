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
        
        virtual void Move(Geometry::Point location) override {
			stack.Move(location);
		}
        
        virtual void Resize(Geometry::Size size) override {
            stack.Resize(size);
        }

		virtual Layer &GetLayer() override {
			return stack;
		}
        
		virtual Geometry::Point GetLocation() const override {
			return stack.GetLocation();
		}

		virtual Geometry::Size GetSize() const override {
			return stack.GetSize();
		}

	protected:
        ComponentStack stack;

		virtual void focused() override {
			stack.AddCondition(ComponentCondition::Focused);
		}

		virtual void focuslost() override {
			stack.RemoveCondition(ComponentCondition::Focused);
		}
        
		virtual void removed() override {
			WidgetBase::removed();

			stack.FinalizeTransitions();
		}
    };
     
    
} }
