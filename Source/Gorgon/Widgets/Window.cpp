#include "Window.h"

#include "../Graphics/Bitmap.h"

namespace Gorgon { namespace Widgets {
   
    Window::Window(const UI::Template &temp, const std::string &title) : 
        Panel(temp),
        Title(this),
        Icon(this),
        title(title)
    {
        stack.SetData(UI::ComponentTemplate::Title, title);
        
        stack.SetMouseDownEvent([this](auto tag, auto location, auto button) { mouse_down(tag, location, button); });
        stack.SetMouseUpEvent([this](auto tag, auto location, auto button) { mouse_up(tag, location, button); });
        stack.SetMouseMoveEvent([this](auto tag, auto location) { mouse_move(tag, location); });
        stack.SetClickEvent([this](auto tag, auto location, auto button) { mouse_click(tag, location, button); });
        
        updatescrollvisibility();
    }
    
    void Window::SetTitle(const std::string &value) {
        title = value;
        stack.SetData(UI::ComponentTemplate::Title, title);
    }
    
	void Window::SetIcon(const Graphics::Animation &value) {
        if(ownicon) {
            icon->DeleteAnimation();;
        }
        delete iconprov;
        
        icon = &value;
        iconprov = nullptr;
        stack.SetData(UI::ComponentTemplate::Icon, *icon);
        
        ownicon = false;
    }
    
    
    void Window::SetIcon(const Graphics::Bitmap& value){
        SetIcon(dynamic_cast<const Graphics::Animation&>(value));
    }
    
    
    void Window::SetIconProvider(const Graphics::AnimationProvider &value) {
        auto &anim = value.CreateAnimation(true);
        
        OwnIcon(anim);
    }
    
    void Window::SetIconProvider(Graphics::AnimationProvider &&provider) {
        iconprov = &(provider.MoveOutProvider());
        auto &anim = iconprov->CreateAnimation(true);
        
        OwnIcon(anim);
    }
    
    void Window::RemoveIcon() {
        if(ownicon) {
            icon->DeleteAnimation();
        }
        delete iconprov;
        
        icon = nullptr;
        
        stack.RemoveData(UI::ComponentTemplate::Icon);
    }
    
    
    void Window::OwnIcon() {
        ownicon = true;
    }
    
    
    void Window::OwnIcon(const Graphics::Animation &value) {
        SetIcon(value);
        
        ownicon = true;
    }
    
    void Window::OwnIcon(Graphics::Bitmap &&value) {
        OwnIcon(*new Graphics::Bitmap(std::move(value)));
    }
    
    void Window::EnableScroll(bool vertical, bool horizontal) {
        Panel::EnableScroll(vertical, horizontal);
        
        updatescrollvisibility();
    }
    
    void Window::updatescroll() {
        Panel::updatescroll();
        updatescrollvisibility();
    }
    
    void Window::updatecontent() {
        Panel::updatecontent();
        updatescrollvisibility();
    }
    
    void Window::updatescrollvisibility() {
        auto val = stack.GetTargetValue();
        
        if(val[2] == 1 || val[2] == 0 || !hscroll) {
            stack.RemoveCondition(UI::ComponentCondition::HScroll);
        }
        else {
            stack.AddCondition(UI::ComponentCondition::HScroll);
        }
        if(val[3] == 1 || val[3] == 0 || !vscroll) {
            stack.RemoveCondition(UI::ComponentCondition::VScroll);
        }
        else {
            stack.AddCondition(UI::ComponentCondition::VScroll);
        }
    }
    
    void Window::mouse_down(UI::ComponentTemplate::Tag tag, Geometry::Point location, Input::Mouse::Button button) {
        if(tag == UI::ComponentTemplate::NoTag) {
            if(stack.IndexOfTag(UI::ComponentTemplate::DragTag) == -1)
                tag = UI::ComponentTemplate::DragTag;
            else {
                int ind = stack.ComponentAt(location);
                
                if(ind != -1)
                    tag = stack.GetTemplate(ind).GetTag();
            }
        }
        
        if(button == Input::Mouse::Button::Left && allowmove && tag == UI::ComponentTemplate::DragTag) {
            moving = true;
            dragoffset = location;
        }
        
        Focus();
    }
    
    void Window::mouse_up(UI::ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button button) {
        if(button == Input::Mouse::Button::Left) {
            moving = false;
        }
    }
    
    void Window::mouse_move(UI::ComponentTemplate::Tag, Geometry::Point location) {
        if(moving && location != dragoffset) {
            Move(GetLocation() + location-dragoffset);
        }
    }
    
    void Window::mouse_click(UI::ComponentTemplate::Tag tag, Geometry::Point location, Input::Mouse::Button button) {
        if(tag == UI::ComponentTemplate::NoTag) {
            if(stack.IndexOfTag(UI::ComponentTemplate::DragTag) == -1)
                tag = UI::ComponentTemplate::DragTag;
            else {
                int ind = stack.ComponentAt(location);
                
                if(ind != -1)
                    tag = stack.GetTemplate(ind).GetTag();
            }
        }
        
        if(button == Input::Mouse::Button::Left && tag == UI::ComponentTemplate::CloseTag) {
            Close();
        }
    }
    
    void Window::AllowMovement(bool allow) {
        if(allowmove == allow) 
            return;
        
        allowmove = allow;
        if(moving)
            mouse_up(UI::ComponentTemplate::NoTag, {0, 0}, Input::Mouse::Button::Left);
    }
    
    void Window::SetCloseButtonVisibility(bool value) {
        if(closebutton == value)
            return;
        
        closebutton = value;
        
        if(stack.TagHasSubstack(UI::ComponentTemplate::CloseTag)) {
            if(closebutton) {
                if(enableclose)
                    stack.GetTagSubstack(UI::ComponentTemplate::CloseTag).RemoveCondition(UI::ComponentCondition::Disabled);
                else
                    stack.GetTagSubstack(UI::ComponentTemplate::CloseTag).AddCondition(UI::ComponentCondition::Disabled);
                
                stack.GetTagSubstack(UI::ComponentTemplate::CloseTag).RemoveCondition(UI::ComponentCondition::Hidden);
            }
            else {
                stack.GetTagSubstack(UI::ComponentTemplate::CloseTag).AddCondition(UI::ComponentCondition::Disabled);
                stack.GetTagSubstack(UI::ComponentTemplate::CloseTag).AddCondition(UI::ComponentCondition::Hidden);
            }
        }
    }

    void Window::SetCloseButtonEnabled(bool value) {
        if(enableclose == value)
            return;
        
        enableclose = value;
        
        if(stack.TagHasSubstack(UI::ComponentTemplate::CloseTag)) {
            if(closebutton) {
                if(enableclose)
                    stack.GetTagSubstack(UI::ComponentTemplate::CloseTag).RemoveCondition(UI::ComponentCondition::Disabled);
                else
                    stack.GetTagSubstack(UI::ComponentTemplate::CloseTag).AddCondition(UI::ComponentCondition::Disabled);
            }
        }
    }


} }
