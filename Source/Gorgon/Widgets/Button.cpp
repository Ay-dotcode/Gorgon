#include "Button.h"
#include "../UI/WidgetContainer.h"
#include "../Graphics/Bitmap.h"

namespace Gorgon { namespace Widgets {

    
    
    Button::Button(const UI::Template &temp, std::string text) :
        ComponentStackWidget(temp),
        Text(this), Icon(this), text(text) 
    {
        stack.SetData(UI::ComponentTemplate::Text, text);
        stack.HandleMouse(Input::Mouse::Button::Left);

        stack.SetClickEvent([this](auto, auto, auto btn) {
            if(btn == Input::Mouse::Button::Left)
                ClickEvent();
        });

        stack.SetMouseDownEvent([this](auto, auto, auto btn) {
            if(btn == Input::Mouse::Button::Left) {
                if(allowfocus()) {
                    Focus();
                }
                
                PressEvent();
                
                if(repeaten == repeatstandby) {
                    repeaten = repeatondelay;
                    repeatleft = repeatdelay;
                }
                
                mousedown = true;
            }
        });

        stack.SetMouseUpEvent([this](auto, auto, auto btn) {
            if(btn == Input::Mouse::Button::Left) {
                if(spacedown)
                    stack.AddCondition(UI::ComponentCondition::Down);
                else
                    ReleaseEvent();
                
                if(repeaten != repeatdisabled) {
                    repeaten = repeatstandby;
                    repeatleft = -1;
                }
                
                mousedown = false;
            }
        });
    }

    Button::~Button() {
        if(ownicon)
            delete icon;
        
        delete iconprov;
    }

    void Button::SetText(const std::string &value) {
        text = value;
        stack.SetData(UI::ComponentTemplate::Text, text);
    }


    void Button::SetIcon(const Graphics::Animation &value) {
        if(ownicon) {
            delete icon;
        }
        delete iconprov;
        
        icon = &value;
        stack.SetData(UI::ComponentTemplate::Icon, *icon);

        ownicon = false;
    }
    
    void Button::SetIconProvider(const Graphics::AnimationProvider &value) {
        auto &anim = value.CreateAnimation(true);
        
        OwnIcon(anim);
    }
    
    void Button::SetIconProvider(Graphics::AnimationProvider &&provider) {
        iconprov = &(provider.MoveOutProvider());
        auto &anim = iconprov->CreateAnimation(true);
        
        OwnIcon(anim);
    }
    
    void Button::RemoveIcon() {
        if(ownicon) {
            delete icon;
        }
        delete iconprov;
        
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
    
    void Button::OwnIcon(Graphics::Bitmap &&value) {
        OwnIcon(*new Graphics::Bitmap(std::move(value)));
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
            PressEvent();
            ClickEvent();
            ReleaseEvent();
            
            return true;
        }
        else if(key == Keycodes::Space) {
            if(state == 1) {
                spacedown = true;
                stack.AddCondition(UI::ComponentCondition::Down);
                
                PressEvent();
                
                if(repeaten == repeatstandby) {
                    repeaten = repeatondelay;
                    repeatleft = repeatdelay;
                }
                
                return true;
            }
            else if(spacedown) {
                spacedown  =false;
                if(!mousedown) {
                    stack.RemoveCondition(UI::ComponentCondition::Down);
                    
                    ClickEvent();
                    
                    ReleaseEvent();
                    
                    if(repeaten != repeatdisabled) {
                        repeaten = repeatstandby;
                        repeatleft = -1;
                    }
                }
                
                return true;
            }
        }
        
        return false;
    }
    

    void Button::ActivateClickRepeat(int delay, int repeat, int accelerationtime, int minrepeat) { 
        repeatdelay = delay;
        repeatinit  = repeat;
        repeatacc   = accelerationtime;
        repeatfin   = minrepeat;
        repeatdiff  = minrepeat - repeat;
        repeaten    = repeatstandby;
        
        stack.SetFrameEvent(std::bind(&Button::repeattick, this));
    }
    
    void Button::DeactivateClickRepeat() {
        stack.RemoveFrameEvent();
        repeaten   = repeatdisabled;
        repeatleft = -1;
    }
    
    
    void Button::repeattick() {
        if(repeaten == repeatdisabled || repeaten == repeatstandby || repeatleft == -1)
            return;
        
        auto time = Time::DeltaTime();
        
        repeatcur += (float)repeatdiff * time / repeatacc;
        
        if(repeatleft < time) {
            ClickEvent();
            
            if(repeaten == repeatondelay) {
                repeaten = repeating;
                repeatcur = repeatinit;
                repeatleft = repeatcur;
            }
            else {
                if(repeatdiff > 0 ? repeatcur >= repeatfin : repeatcur <= repeatfin)
                    repeatcur = repeatfin;
                
                repeatleft = repeatcur;
            }
        }
        else {
            repeatleft -=time;
        }
    }
    

void Button::SetIcon(const Graphics::Bitmap& value){
           SetIcon(dynamic_cast<const Graphics::Animation&>(value));
       }
} }
