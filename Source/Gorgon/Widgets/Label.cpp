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
            if(btn == Input::Mouse::Button::Left && HasParent()) {
                GetParent().FocusNext(*this);
            }
        });
    }

    Label::~Label() {
        RemoveIcon();
    }

    void Label::SetText(const std::string &value) {
        text = value;
        stack.SetData(UI::ComponentTemplate::Text, text);
    }

    
    void Label::SetIcon(const Graphics::Drawable &value) {
        RemoveIcon();
        
        icon = &value;
        iconprov = nullptr;
        stack.SetData(UI::ComponentTemplate::Icon, *icon);
        
        ownicon = false;
    }
    
    
    void Label::SetIcon(const Graphics::Bitmap& value){
        SetIcon(dynamic_cast<const Graphics::Animation&>(value));
    }
    
    
    void Label::SetIconProvider(const Graphics::AnimationProvider &value) {
        auto &anim = value.CreateAnimation(true);
        
        OwnIcon(anim);
    }
    
    void Label::SetIconProvider(Graphics::AnimationProvider &&provider) {
        iconprov = &(provider.MoveOutProvider());
        auto &anim = iconprov->CreateAnimation(true);
        
        OwnIcon(anim);
    }
    
    void Label::RemoveIcon() {
        if(ownicon) {
            if(dynamic_cast<const Graphics::Animation*>(icon))
                dynamic_cast<const Graphics::Animation*>(icon)->DeleteAnimation();
            else
                delete icon;
        }
        delete iconprov;
        
        icon = nullptr;
        
        stack.RemoveData(UI::ComponentTemplate::Icon);
    }
    
    
    void Label::OwnIcon() {
        ownicon = true;
    }
    
    
    void Label::OwnIcon(const Graphics::Drawable &value) {
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
