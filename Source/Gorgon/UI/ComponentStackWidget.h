#pragma once

#include "WidgetBase.h"
#include "ComponentStack.h"
#include "WidgetContainer.h"

namespace Gorgon { namespace UI {
    
   
    /**
     * This class acts as a widget base that uses component stack to handle
     * rendering, resizing and other operations.
     */
    class ComponentStackWidget : public WidgetBase {
    public:
        ComponentStackWidget(const Template &temp) : stack(*new ComponentStack(temp, temp.GetSize())) { }
        
        ComponentStackWidget(ComponentStackWidget &&) = default;
        
        ComponentStackWidget &operator =(ComponentStackWidget &&) = default;

        virtual ~ComponentStackWidget() {
            delete &stack;
        }
        
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

        virtual void SetEnabled(bool value) override {
            if(enabled == value)
                return;

            enabled = value;

            if(!value) {
                stack.AddCondition(ComponentCondition::Disabled);
                if(HasParent() && IsFocused()) {
                    GetParent().FocusNext();
                    if(IsFocused())
                        GetParent().ForceRemoveFocus();
                }
            }
            else {
                if(!HasParent() || GetParent().IsEnabled()) {
                    stack.RemoveCondition(ComponentCondition::Disabled);
                }
            }
        }
        
        virtual bool IsEnabled() const override {
            return enabled;
        }
        
	protected:
        ComponentStack &stack; //allocate on heap due to its size.
        
        bool enabled = true;

        /// This function is called when the parent's enabled state changes.
        virtual void parentenabledchanged(bool state) {
            if(enabled && !state) {
                stack.AddCondition(ComponentCondition::Disabled);
            }
            else if(enabled && state) {
                stack.RemoveCondition(ComponentCondition::Disabled);
            }
        }

		virtual void focused() override {
            WidgetBase::focused();
			stack.AddCondition(ComponentCondition::Focused);
			FocusEvent();
		}

		virtual void focuslost() override {
            WidgetBase::focuslost();
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
