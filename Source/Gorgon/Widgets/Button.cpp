#include "Button.h"
#include "../UI/WidgetContainer.h"

namespace Gorgon { namespace Widgets {
   
    
    
	Button::Button(const UI::Template &temp, std::string text) :
		ComponentStackWidget(temp),
		Text(this), Icon(this),
		ClickEvent(this), text(text) 
	{
		stack.SetData(UI::ComponentTemplate::Text, text);
		stack.HandleMouse(Input::Mouse::Button::Left);

		stack.SetClickEvent([this](auto, auto, auto btn) {
			if(btn == Input::Mouse::Button::Left)
				ClickEvent();
		});

		stack.SetMouseDownEvent([this](auto, auto, auto btn) {
			if(allowfocus() && btn == Input::Mouse::Button::Left)
				Focus();
		});

		stack.SetMouseUpEvent([this](auto, auto, auto btn) {
			if(btn == Input::Mouse::Button::Left && spacedown)
				stack.AddCondition(UI::ComponentCondition::Down);
		});
	}

	Button::Button(const UI::Template & temp, std::function<void()> clickfn) : Button(temp) {
		ClickEvent.Register(clickfn);
	}



	Button::Button(const UI::Template &temp, std::string text, std::function<void()> clickfn) : Button(temp, text) {
		ClickEvent.Register(clickfn);
	}


	Button::~Button() {
		if(ownicon)
			delete icon;
	}

	void Button::SetText(const std::string &value) {
		text = value;
		stack.SetData(UI::ComponentTemplate::Text, text);
	}


	void Button::SetIcon(const Graphics::Animation &value) {
		if(ownicon)
			delete icon;

		icon = &value;
		stack.SetData(UI::ComponentTemplate::Icon, *icon);

		ownicon = false;
	}

	void Button::RemoveIcon() {
		if(ownicon)
			delete icon;

		icon = nullptr;

		stack.RemoveData(UI::ComponentTemplate::Icon);
	}


	void Button::OwnIcon() {
		ownicon = true;
	}


	void Button::OwnIcon(const Graphics::Animation &value) {
		SetIcon(value);

		ownicon = true;
	}

	bool Button::Activate() {
		ClickEvent();

		return true;
	}

	bool Button::allowfocus() const {
		return !HasParent() || GetParent().CurrentFocusStrategy() == UI::WidgetContainer::AllowAll;
	}
	
	
    bool Button::KeyEvent(Input::Key key, float state) {
        if(Input::Keyboard::CurrentModifier.IsModified())
            return false;
        
        namespace Keycodes = Input::Keyboard::Keycodes;
        
        if(key == Keycodes::Enter && state == 1) {
            ClickEvent();
            
            return true;
        }
        else if(key == Keycodes::Space) {
            if(state == 1) {
                spacedown = true;
                stack.AddCondition(UI::ComponentCondition::Down);
                
                return true;
            }
            else if(spacedown) {
                spacedown  =false;
                stack.RemoveCondition(UI::ComponentCondition::Down);
                ClickEvent();
                
                return true;
            }
        }
        
        return false;
    }
    
}
}
