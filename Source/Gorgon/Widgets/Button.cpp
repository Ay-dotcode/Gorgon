#include "Button.h"

namespace Gorgon { namespace Widgets {
   
    
    
	Button::Button(const UI::Template &temp, std::string text) :
		ComponentStackWidget(temp),
		Text(this), Icon(this),
		ClickEvent(this), text(text) 
	{
		stack.SetData(UI::ComponentTemplate::Text, text);
		stack.HandleMouse(Input::Mouse::Button::Left);

		stack.SetClickEvent([this](auto, auto, auto) {
			ClickEvent();
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

}
}
