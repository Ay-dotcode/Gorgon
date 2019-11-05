#pragma once

#include <functional>
#include <string>

#include "../Event.h"
#include "../UI/Validators.h"
#include "../UI/ComponentStackWidget.h"

namespace Gorgon { namespace Widgets {
    
    /**
     * This class allows users to enter any value to an inputbox. This
     * class is specialized as Textbox and Numberbox. It is possible to
     * supply a validator to a specific inputbox. Inputbox is designed
     * for value objects that can be copied.
     */
    template<class T_, class V_ = UI::EmptyValidator<T_>>
    class Inputbox : public UI::ComponentStackWidget {
    public:
        
        /// Initializes the inputbox
        explicit Inputbox(const UI::Template &temp, T_ value = T_()) : 
            ComponentStackWidget(temp), value(value), display(validator.ToString(value))
        {
            stack.SetData(UI::ComponentTemplate::Text);
        }
        
        /// Initializes the inputbox
        explicit Inputbox(const UI::Template &temp, std::function<void()> changedevent) : Inputbox(temp) {
            ChangedEvent.Register(changedevent);
        }
        
        /// Initializes the inputbox
        explicit Inputbox(const UI::Template &temp, T_ value, std::function<void()> changedevent) : Inputbox(temp, value)
        {
            ChangedEvent.Register(changedevent);
        }
        
        
        /// Returns the value in the box.
        operator T_() const {
            return value;
        }
        
        /// Fired after the value of the inputbox is changed. Parameter is the previous 
        /// value before the change. If the user is typing, this event will be fired
        /// after typing stops for a set amount of time. ChangedEvent will be fired
        /// immediately when a value is pasted to the box.
        Event<Inputbox, const T_ &> ChangedEvent = Event<Inputbox>{this};
        
    private:
        V_ validator;
        T_ value;
        std::string display;
        
        int selstart = 0;
        int sellen   = 0;
    };
    
} }
