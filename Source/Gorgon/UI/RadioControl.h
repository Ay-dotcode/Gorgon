#pragma once

#include "TwoStateControl.h"
#include "../Containers/Hashmap.h"

namespace Gorgon { namespace UI {
    
    /**
     * This class is designed to turn any group of two state widgets to
     * a radio button group, only allowing one of them to be checked at
     * the same time. Additionally, it allows setting and retrieving the
     * selected option through as single value. Below is an example that
     * shows how to use RadioControl with Checkbox
     * 
     * @code
     * 
     * auto radiotemplate = generator.RadioButton();
     * 
     * RadioControl<> selected({
     *   {0, new Gorgon::Widgets::Checkbox(radiotemplate, "First")},
     *   {1, new Gorgon::Widgets::Checkbox(radiotemplate, "Second")},
     * }, 0);
     * 
     * selected.ChangedEvent.Register([](auto val) {
     *   std::cout<<val<<std::endl;
     * });
     * 
     * selected.PlaceIn(mywind, {4, 4}, 4);
     * 
     * @endcode
     */
    template<class T_ = int>
    class RadioControl {
    public:
        /// Default constructor. Use filling constructor if possible.
        RadioControl() : ChangedEvent(this) { }
        
        /// No copying.
        RadioControl(const RadioControl &) = delete;
        
        /// Filling constructor that prepares RadioControl from the start. This
        /// variant will not own its children.
        explicit RadioControl(std::initializer_list<std::pair<const T_, TwoStateControl&>> elm, T_ current = T_()) : 
        ChangedEvent(this),
        elements(elm), 
        current(current) 
        {
            for(auto p : elements) {
                p.second.SetState(false, true);
                reverse.insert({&p.second, p.first});
            }
            
            if(elements.Exists(current)) {
                elements[current].SetState(true, true);
            }
            
            for(auto p : elements) {
                p.second.StateChangingEvent.Register(*this, &RadioControl::changing);
            }
        }
        
        /// Filling constructor that prepares RadioControl from the start. This
        /// variant will own its children.
        explicit RadioControl(std::initializer_list<std::pair<const T_, TwoStateControl*>> elm, T_ current = T_()) : 
        ChangedEvent(this),
        elements(elm), 
        current(current) 
        {
            for(auto p : elements) {
                p.second.SetState(false, true);
                reverse.insert({&p.second, p.first});
            }
            
            if(elements.Exists(current)) {
                elements[current].SetState(true, true);
            }
            
            for(auto p : elements) {
                p.second.StateChangingEvent.Register(*this, &RadioControl::changing);
            }
            
            own = true;
        }
        
        ~RadioControl() {
            if(own)
                elements.DeleteAll();
        }
        
        /// Assigns a new value to the radio control. If the specified value exists
        /// in the, it will be selected, if not, nothing will be selected.
        RadioControl &operator =(const T_ value) {
            Set(value);
        }
        
        /// Returns the current value
        operator T_() const {
            return current;
        }
        
        /// Assigns a new value to the radio control. If the specified value exists
        /// in the, it will be selected, if not, nothing will be selected and this
        /// function will return false.
        bool Set(const T_ value) {
            if(value == current)
                return true;
            
            clearall();
            
            current = value;
            
            if(elements.Exists(current)) {
                bool state = elements[current].SetState(true, true);
                
                if(state)
                    ChangedEvent(current);
                
                return state;
            }
            
            return false;
        }
        
        /// Returns the current value
        T_ Get() const {
            return current;
        }
        
        /// Returns if the given element exists
        bool Exists(const T_ value) const {
            return elements.Exists(value);
        }
        
        /// Adds the given element to this controller
        void Add(const T_ value, TwoStateControl &control) {
            elements.Push(value, control);
            reverse.insert({&control, value});
        }
        
        /// This function will add all widgets in this controller
        /// to the given container. If any member is not a widget,
        /// it will be ignored.
        template<class C_>
        void PlaceIn(C_ &container, Geometry::Point start, int spacing) {
            auto loc = start;
            
            for(auto p : elements) {
                auto w = dynamic_cast<WidgetBase *>(&p.second);
                
                if(!w)
                    continue;
                
                container.Add(*w);
                
                w->Move(loc);
                
                loc.Y += w->GetSize().Height + spacing;
            }
        }
        
        auto begin() {
            return elements.begin();
        }
        
        auto end() {
            return elements.begin();
        }
        
        auto begin() const {
            return elements.begin();
        }
        
        auto end() const {
            return elements.begin();
        }
        
        Event<RadioControl, T_> ChangedEvent;
        
    private:
        void changing(TwoStateControl &control, bool state, bool &allow) {
            if(!state) {
                allow = false;
            }
            else {
                if(reverse[&control] == current)
                    return;
                
                clearall();
                current = reverse[&control];
                
                ChangedEvent(current);
            }
        }
        
        void clearall() {
            for(auto p : elements) {
                p.second.SetState(false, true);
            }
        }
        
        Containers::Hashmap<T_, TwoStateControl> elements;
        std::map<TwoStateControl *, T_> reverse;
        
        bool own = false;
        
        T_ current;
    };
} }
