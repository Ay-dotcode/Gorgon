#include "Window.h"

#include "../Window.h"
#include "../UI/Window.h"
#include "../Graphics/Bitmap.h"

namespace Gorgon { namespace Widgets {
   
    Window::Window(const UI::Template &temp, const std::string &title, bool autoplace) : 
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
        if(autoplace) {
            bool done = false;
            for(auto &w : Gorgon::Window::Windows) {
                auto cont = dynamic_cast<UI::Window *>(&w);
                if(cont && w.IsVisible() && !w.IsClosed()) {
                    cont->WindowContainer().Add(*this);
                    done = true;
                    break;
                }
            }

            //UI::Window not found, search for any window that is a container
            if(!done) {
                for(auto &w : Gorgon::Window::Windows) {
                    auto cont = dynamic_cast<UI::WidgetContainer *>(&w);
                    if(cont && w.IsVisible() && !w.IsClosed()) {
                        cont->Add(*this);
                        done = true;
                        break;
                    }
                }
            }
            
            Center();
        }
        
        minsize = GetSize() - GetInteriorSize() + Geometry::Size(stack.GetTemplate().GetUnitWidth()*2, stack.GetTemplate().GetUnitWidth());
    }
    
    Window::Window(const UI::Template &temp, const std::string &title, const Geometry::Size size, bool autoplace) : 
        Window(temp, title, autoplace)
    {
        Resize(size);
        updatescrollvisibility();
        
        if(autoplace)
            Center();
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

    void Window::focused() {
        if(Panel::allowfocus())
            Panel::focused();
        else
            ComponentStackWidget::focused();
        
        if(HasParent())
            GetParent().ChangeZorder(*this, -1);
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
        
        if(button == Input::Mouse::Button::Left) {
            if(allowmove && tag == UI::ComponentTemplate::DragTag) {
                moving = true;
                dragoffset = location;
            }
            else if(allowresize && tag == UI::ComponentTemplate::ResizeTag) {
                auto size = GetSize();
                int maxdist = stack.GetTemplate().GetResizeHandleSize();
                
                int leftdist = location.X, rightdist  = size.Width  - location.X;
                int topdist  = location.Y, bottomdist = size.Height - location.Y;
                
                resizing = none; //just to make sure
                
                if(leftdist < rightdist) {
                    if(leftdist <= maxdist) {
                        resizing = resizedir(resizing | left);
                    }
                }
                else {
                    if(rightdist <= maxdist) {
                        resizing = resizedir(resizing | right);
                    }
                }
                
                if(topdist < bottomdist) {
                    if(topdist <= maxdist) {
                        resizing = resizedir(resizing | top);
                    }
                }
                else {
                    if(bottomdist <= maxdist) {
                        resizing = resizedir(resizing | bottom);
                    }
                }
                
                dragoffset = location;
            }
        }
        
        Focus();
    }
    
    void Window::mouse_up(UI::ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button button) {
        if(button == Input::Mouse::Button::Left) {
            moving = false;
            resizing = none;
        }
    }
    
    void Window::mouse_move(UI::ComponentTemplate::Tag, Geometry::Point location) {
        if(location == dragoffset) 
            return;
        
        if(moving) {
            Move(GetLocation() + location-dragoffset);
        }
        
        switch(resizing) {
        case bottomleft:
        case bottomright:
        case bottom: {
            int ch = GetHeight();
            int h = ch + location.Y - dragoffset.Y;
            if(h < minsize.Height)
                h = minsize.Height;
            
            SetHeight(h);
            dragoffset.Y = location.Y;
            break;
        }    
        case topleft:
        case topright:
        case top: {
            int ch = GetHeight();
            int h = ch - location.Y + dragoffset.Y;
            
            if(h < minsize.Height)
                h = minsize.Height;
            
            SetHeight(h);
            
            Move(GetLocation() + Geometry::Point(0, ch-h));
            
            break;
        }
        default:
            //nothing
            break;
        }
        
        switch(resizing) {
        case topright:
        case bottomright:
        case right: {
            int cw = GetWidth();
            int w = cw + location.X - dragoffset.X;
            if(w < minsize.Width)
                w = minsize.Width;
            
            SetWidth(w);
            dragoffset.X = location.X;
            break;
        }
        case topleft:
        case bottomleft:
        case left: {
            int cw = GetWidth();
            int w = cw - location.X + dragoffset.X;
            
            if(w < minsize.Width)
                w = minsize.Width;
            
            SetWidth(w);
            
            Move(GetLocation() + Geometry::Point(cw-w, 0));
            
            break;
        }
        default:
            //nothing
            break;
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
    
    void Window::AllowResize(bool allow) {
        if(allowresize == allow) 
            return;
        
        allowresize = allow;
        if(resizing != none)
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

    void Window::Center() {
        if(HasParent())
            Move(Geometry::Point((GetParent().GetInteriorSize() - GetSize())/2));
    }
    
    bool Window::allowfocus() const {
        if(CurrentFocusStrategy() == Deny)
            return false;
        
        return true;
    }

} }
