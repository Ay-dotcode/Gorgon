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
        
        virtual void Move(int x, int y) override;
        
        virtual void Resize(int w, int h) override {
            stack.Resize(w, h);
        }
        
    protected:
        ComponentStack stack;
        
    };
     
    
} }
