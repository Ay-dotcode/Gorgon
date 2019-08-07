#include "Label.h"
#include "../UI/WidgetContainer.h"
#include "../Graphics/Bitmap.h"

namespace Gorgon { namespace Widgets {
   
    
    
	Label::Label(const UI::Template &temp, std::string text) :
		ComponentStackWidget(temp),
		Text(this), Icon(this), text(text) 
	{
		stack.SetData(UI::ComponentTemplate::Text, text);
		stack.HandleMouse(Input::Mouse::Button::Left);

		stack.SetClickEvent([this](auto, auto, auto btn) {
            if(btn == Input::Mouse::Button::Left && HasParent())
				GetParent().FocusNext();
		});
	}

	Label::~Label() {
		if(ownicon)
			delete icon;
	}

	void Label::SetText(const std::string &value) {
		text = value;
		stack.SetData(UI::ComponentTemplate::Text, text);
	}


	void Label::SetIcon(const Graphics::Animation &value) {
		if(ownicon)
			delete icon;

		icon = &value;
		stack.SetData(UI::ComponentTemplate::Icon, *icon);

		ownicon = false;
	}

	void Label::RemoveIcon() {
		if(ownicon)
			delete icon;

		icon = nullptr;

		stack.RemoveData(UI::ComponentTemplate::Icon);
	}


	void Label::OwnIcon() {
		ownicon = true;
	}

	
	void Label::OwnIcon(const Graphics::Animation &value) {
        SetIcon(value);
        
        ownicon = true;
    }
    
    void Label::OwnIcon(Graphics::Bitmap &&value) {
        OwnIcon(*new Graphics::Bitmap(std::move(value)));
    }
    
	bool Label::Activate() {
		if(HasParent())
            GetParent().FocusNext();

		return true;
	}

	bool Label::allowfocus() const {
		return false;
	}
	
}
}
