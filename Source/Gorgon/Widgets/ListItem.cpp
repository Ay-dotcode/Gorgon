#include "ListItem.h"


namespace Gorgon { namespace Widgets {
        
    void ListItem::SetText(const std::string& value) {
        text = value;
        stack.SetData(UI::ComponentTemplate::Text, value);
    }


    void ListItem::SetIndex(int value) {
        index = value;
    }

    
    void ListItem::SetIcon(const Graphics::Animation &value) {
        if(ownicon) {
            icon->DeleteAnimation();;
        }
        delete iconprov;
        
        icon = &value;
        iconprov = nullptr;
        stack.SetData(UI::ComponentTemplate::Icon, *icon);
        
        ownicon = false;
    }
    
    
    void ListItem::SetIcon(const Graphics::Bitmap& value){
        SetIcon(dynamic_cast<const Graphics::Animation&>(value));
    }
    
    
    void ListItem::SetIconProvider(const Graphics::AnimationProvider &value) {
        auto &anim = value.CreateAnimation(true);
        
        OwnIcon(anim);
    }
    
    void ListItem::SetIconProvider(Graphics::AnimationProvider &&provider) {
        iconprov = &(provider.MoveOutProvider());
        auto &anim = iconprov->CreateAnimation(true);
        
        OwnIcon(anim);
    }
    
    void ListItem::RemoveIcon() {
        if(ownicon) {
            icon->DeleteAnimation();
        }
        delete iconprov;
        
        icon = nullptr;
        
        stack.RemoveData(UI::ComponentTemplate::Icon);
    }
    
    
    void ListItem::OwnIcon() {
        ownicon = true;
    }
    
    
    void ListItem::OwnIcon(const Graphics::Animation &value) {
        SetIcon(value);
        
        ownicon = true;
    }
    
    
    void ListItem::OwnIcon(Graphics::Bitmap &&value) {
        OwnIcon(*new Graphics::Bitmap(std::move(value)));
    }
    
    
    bool ListItem::Activate() {
        ToggleEvent();
        
        return true;
    }
    

    void ListItem::SetFocusState(bool value) {
        if(value == focus)
            return;
        
        focus = value;
        
        if(value) {
            stack.AddCondition(UI::ComponentCondition::Focused);
        }
        else {
            stack.RemoveCondition(UI::ComponentCondition::Focused);
        }
    }


    void ListItem::SetSelected(bool value) {
        if(value == selected)
            return;
        
        selected = value;
        
        if(value) {
            stack.AddCondition(UI::ComponentCondition::State2);
        }
        else {
            stack.RemoveCondition(UI::ComponentCondition::State2);
        }
    }


    void ListItem::SetParity(Parity value) {
        if(value == parity)
            return;
        
        if(parity == Parity::Odd) {
            stack.RemoveCondition(UI::ComponentCondition::Odd);
        }
        else if(parity == Parity::Even) {
            stack.RemoveCondition(UI::ComponentCondition::Even);
        }
        
        parity = value;
        
        if(value == Parity::Odd) {
            stack.AddCondition(UI::ComponentCondition::Odd);
        }
        else if(value == Parity::Even) {
            stack.AddCondition(UI::ComponentCondition::Even);
        }
    }

    
    void ListItem::SetOpened(YesNoUnset value) {
        if(value == opened)
            return;
        
        if(opened == YesNoUnset::Yes) {
            stack.RemoveCondition(UI::ComponentCondition::Opened);
        }
        else if(opened == YesNoUnset::No) {
            stack.RemoveCondition(UI::ComponentCondition::Closed);
        }
        
        opened = value;
        
        if(value == YesNoUnset::Yes) {
            stack.AddCondition(UI::ComponentCondition::Opened);
        }
        else if(value == YesNoUnset::No) {
            stack.AddCondition(UI::ComponentCondition::Closed);
        }
        
    }


    void ListItem::SetPosition(ItemPosition value) {
        if(value == position)
            return;
        
        switch(position) {
        case ItemPosition::Alone:
            stack.RemoveCondition(UI::ComponentCondition::Alone);
            break;
        case ItemPosition::First:
            stack.RemoveCondition(UI::ComponentCondition::First);
            break;
        case ItemPosition::Middle:
            stack.RemoveCondition(UI::ComponentCondition::Middle);
            break;
        case ItemPosition::Last:
            stack.RemoveCondition(UI::ComponentCondition::Last);
            break;
        default: 
            ; //nothing else needs to be done
        }
        
        position = value;
        
        switch(value) {
        case ItemPosition::Alone:
            stack.AddCondition(UI::ComponentCondition::Alone);
            break;
        case ItemPosition::First:
            stack.AddCondition(UI::ComponentCondition::First);
            break;
        case ItemPosition::Middle:
            stack.AddCondition(UI::ComponentCondition::Middle);
            break;
        case ItemPosition::Last:
            stack.AddCondition(UI::ComponentCondition::Last);
            break;
        default: 
            ; //nothing else needs to be done
        }
    }

    
    bool ListItem::allowfocus() const {
        return false;
    }
    

} }
