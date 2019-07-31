#include "Checkbox.h"
#include "../UI/WidgetContainer.h"

namespace Gorgon { namespace Widgets {

    
    
    Checkbox::Checkbox(const UI::Template &temp, std::string text, bool state) :
        ComponentStackWidget(temp),
        Text(this), ChangedEvent(this), 
        text(text), state(state)
    {
        stack.SetData(UI::ComponentTemplate::Text, text);
        stack.HandleMouse(Input::Mouse::Button::Left);

        stack.SetClickEvent([this](auto, auto, auto btn) {
            if(btn == Input::Mouse::Button::Left) {
                Toggle();
            }
        });

        stack.SetMouseDownEvent([this](auto, auto, auto btn) {
            if(allowfocus() && btn == Input::Mouse::Button::Left)
                Focus();
        });

        stack.SetMouseUpEvent([this](auto, auto, auto btn) {
            if(btn == Input::Mouse::Button::Left && spacedown)
                stack.AddCondition(UI::ComponentCondition::Down);
        });
        
        if(state)
            stack.AddCondition(UI::ComponentCondition::State2);
    }

    Checkbox::Checkbox(const UI::Template &temp, std::string text, bool state, std::function<void()> changed) : Checkbox(temp, text, state) {
        ChangedEvent.Register(changed);
    }


    Checkbox::~Checkbox() {
    }

    void Checkbox::SetText(const std::string &value) {
        text = value;
        stack.SetData(UI::ComponentTemplate::Text, text);
    }

    bool Checkbox::Activate() {
        Toggle();

        return true;
    }

    bool Checkbox::allowfocus() const {
        return !HasParent() || GetParent().CurrentFocusStrategy() == UI::WidgetContainer::AllowAll;
    }
    
    
    bool Checkbox::KeyEvent(Input::Key key, float state) {
        if(Input::Keyboard::CurrentModifier.IsModified())
            return false;
        
        namespace Keycodes = Input::Keyboard::Keycodes;
        
        if(key == Keycodes::Space) {
            if(state == 1) {
                spacedown = true;
                stack.AddCondition(UI::ComponentCondition::Down);
                
                return true;
            }
            else if(spacedown) {
                spacedown  =false;
                stack.RemoveCondition(UI::ComponentCondition::Down);
                Toggle();
                
                return true;
            }
        }
        else if(state == 1) {
            if(key == Keycodes::Numpad_Plus) {
                Check();
                if(HasParent())
                    GetParent().FocusNext();
                
                return true;
            }
            else if(key == Keycodes::Numpad_Minus) {
                Clear();

                if(HasParent())
                    GetParent().FocusNext();
                
                return true;
            }
        }
        
        return false;
    }
    
    bool Checkbox::SetState(bool value, bool force) { 
        if(value != state) {
            if(!force) {
                bool allow = true;
                StateChangingEvent(value, allow);
                
                if(!allow)
                    return false;
            }
            
            state = value;
            
            if(value) {
                stack.AddCondition(UI::ComponentCondition::State2);
            }
            else {
                stack.RemoveCondition(UI::ComponentCondition::State2);
            }
            
            ChangedEvent();
            
            return true;
        }
        else
            return true;
    }
    
} }
