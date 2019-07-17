#pragma once

#include "../Geometry/Point.h"
#include "ComponentStack.h"

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
        
        
    private:
        bool visible = true;
        bool enabled = true;
        bool focused = false;
    };
    
    
    //!MOVE TO ANOTHER FILE
    /**
     * This class acts as a widget base that uses component stack to handle
     * rendering, resizing and other operations.
     */
    class WidgetBaseWithStack : public WidgetBase {
    public:
        WidgetBaseWithStack(const Template &temp) : stack(temp, temp.GetSize()) { }
        
        virtual void Move(int x, int y) override;
        
        virtual void Resize(int w, int h) override {
            stack.Resize(w, h);
        }
        
    protected:
        ComponentStack stack;
        
    };
    
} }
