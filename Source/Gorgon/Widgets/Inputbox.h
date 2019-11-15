#pragma once

#include <functional>
#include <string>

#include "../Event.h"
#include "../Property.h"
#include "../UI/Validators.h"
#include "../UI/ComponentStackWidget.h"

namespace Gorgon { namespace Widgets {
    
    /// @cond internal
    namespace internal {
        template<class I_, class T_>
        struct prophelper {
			prophelper(I_ *obj) : obj(obj) {}

            ~prophelper() { 
                
            }
            
            void set_(const T_&v) {
                obj->set(v);
            }
            T_ get_() const {
                return obj->get();
            }

			I_ *obj;
        };
    }
    /// @endcond
    
    /**
     * This class allows users to enter any value to an inputbox. This
     * class is specialized as Textbox and Numberbox. It is possible to
     * supply a validator to a specific inputbox. Inputbox is designed
     * for value objects that can be copied and serialized to string.
     */
    template<class T_, class V_ = UI::EmptyValidator<T_>, template<class C_, class PT_, PT_(C_::*Getter_)() const, void(C_::*Setter_)(const PT_ &)> class P_ = Gorgon::Property>
    class Inputbox : 
        public UI::ComponentStackWidget, 
        public P_<internal::prophelper<Inputbox<T_, V_, P_>, T_>, T_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::get_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::set_> {
	public:
        
        using Type     = T_;
        using PropType = P_<internal::prophelper<Inputbox<T_, V_, P_>, T_>, T_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::get_, &internal::prophelper<Inputbox<T_, V_, P_>, T_>::set_>;

		friend class PropType;
		template<class T_, class V_ = UI::EmptyValidator<T_>, template<class C_, class PT_, PT_(C_::*Getter_)() const, void(C_::*Setter_)(const PT_&)> class P_ = Gorgon::Property>
		friend class Inputbox;
		friend struct internal::prophelper<Inputbox<T_, V_, P_>, T_>;
        
        /// Initializes the inputbox
        explicit Inputbox(const UI::Template &temp, T_ value = T_()) : 
        ComponentStackWidget(temp), PropType(&helper), value(value), display(validator.ToString(value))
        {
            updatevalue();
            updateselection();
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
        
        /// Assignment to the value type
        Inputbox &operator =(const T_ value) {
            set(value);
            
            return *this;
        }
        
        /// Copy assignment will only copy the value
        Inputbox &operator =(const Inputbox &value) {
            set(value.Get());
            
            return *this;
        }
        
        
        /// Returns the value in the box.
        operator T_() const {
            return value;
        }
        
        /// Returns the length of the text in this inputbox
        int Length() const {
            return display.length();
        }
        
        /// Returns the current text in the inputbox. Use Get() to obtain the value of
        /// the inputbox, this function exists for special uses.
        std::string GetText() const {
            return display;
        }
        
        /// @name Selection
        /// @{
        
        /// Selects the entire contents of this inputbox
        void SelectAll() {
            selstart = 0;
            sellen   = -1;
            updateselection();
        }
        
        /// Removes any selection, does not move the start of the
        /// cursor.
        void SelectNone() {
            sellen = 0;
            updateselection();
        }
        
        /// Returns the start of the selection in characters.
        int SelectionStart() const {
            return selstart;
        }
        
        /// Returns the length of the selection in characters. Selection length could be 
        /// negative denoting the selection is backwards.
        int SelectionLength() const {
            return sellen;
        }
        
        /// Returns the location of the caret. It is always at the end of the selection
        int CaretLocation() const {
            return selstart + sellen;
        }
        
        /// @}
        
        bool Activate() override {
            return Focus();
        }
        
        /// 
        bool CharacterEvent(Gorgon::Char c) override  {
            display.push_back(c);
            
            value = validator.From(display);
            updatevalue(false);
            
            return true;
        }
        
        /// Fired after the value of the inputbox is changed. Parameter is the previous 
        /// value before the change. If the user is typing, this event will be fired
        /// after typing stops for a set amount of time. ChangedEvent will be fired
        /// immediately when a value is pasted to the box.
        Event<Inputbox, const T_ &> ChangedEvent = Event<Inputbox, const T_ &>{this};
        
        
    protected:
        
        /// updates the selection display
        void updateselection() {
            
        }
        
        /// updates the value display
        void updatevalue(bool updatedisplay = true) {
            if(updatedisplay)
                display = validator.ToString(value);
            
            stack.SetData(UI::ComponentTemplate::Text, display);
        }
        
        /// Returns the value in the box
        T_ get() const {
            return value;
        }
        
        /// Changes the value in the box
        void set(const T_ &val) {
            if(value == val)
                return;
            
            value = val;
            
            updatevalue();
            updateselection();
        }
        
        
    private:
        V_ validator;
        T_ value;
        std::string display;
        
        int selstart = 0;
        int sellen   = 0;

		struct internal::prophelper<Inputbox<T_, V_, P_>, T_> helper = struct internal::prophelper<Inputbox<T_, V_, P_>, T_>(this);
    };
    
} }
