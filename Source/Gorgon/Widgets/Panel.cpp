#include "Panel.h"
#include "../Main.h"

namespace Gorgon { namespace Widgets {
    Panel::Panel(const UI::Template &temp) : 
    UI::ComponentStackWidget(temp) 
    {
        stack.SetOtherMouseEvent([this](UI::ComponentTemplate::Tag tag, Input::Mouse::EventType type, Geometry::Point, float amount) {
            if(type == Input::Mouse::EventType::Scroll_Vert) {
                if(amount<0 && ScrollOffset().Y >= MaxScrollOffset().Y)
                    return false;
                
                if(amount>0 && ScrollOffset().Y <= 0)
                    return false;
                
                ScrollBy(-(int)amount*scrolldist.Y);
                return true;
            }
            
            if(type == Input::Mouse::EventType::Scroll_Hor) {
                if(amount<0 && ScrollOffset().X >= MaxScrollOffset().X)
                    return false;
                
                if(amount>0 && ScrollOffset().X <= 0)
                    return false;
                
                ScrollBy(0, -(int)amount*scrolldist.X);
                return true;
            }
            
            return false;
        });
        
        
        repeater.Register(Input::Keyboard::Keycodes::Up);
        repeater.Register(Input::Keyboard::Keycodes::Down);
        repeater.Register(Input::Keyboard::Keycodes::PageUp);
        repeater.Register(Input::Keyboard::Keycodes::PageDown);

        repeater.SetRepeatOnPress(true);

        repeater.Repeat.Register([this](Input::Key key) {
            namespace Keycodes = Input::Keyboard::Keycodes;
            
            switch(key) {
            case Keycodes::Down:
                ScrollBy(scrolldist.Y);
                break;
                
            case Keycodes::Up:
                ScrollBy(-scrolldist.Y);
                break;
                
            case Keycodes::PageDown:
                ScrollBy(std::max(scrolldist.Y, GetInteriorSize().Height - scrolldist.Y));
                break;
                
            case Keycodes::PageUp:
                ScrollBy(-std::max(scrolldist.Y, GetInteriorSize().Height - scrolldist.Y));
                break;
            }
        });

        SetSmoothScrollSpeed(scrollspeed);
    }

    
    bool Panel::Activate() {
        if(!Focus())
            return false;

        FocusFirst();

        return true;
    }

    
    bool Panel::KeyEvent(Input::Key key, float state) {
        if(UI::WidgetContainer::KeyEvent(key, state))
            return true;
        
        namespace Keycodes = Input::Keyboard::Keycodes;
        
        if(state) {
            switch(key) {
            case Keycodes::Down:
            case Keycodes::PageDown:
                if(ScrollOffset().Y >= MaxScrollOffset().Y)
                    return false;
                break;
                
            case Keycodes::Up:
            case Keycodes::PageUp:
                if(ScrollOffset().Y <= 0)
                    return false;
                
                break;
            }
        }
        
        if(repeater.KeyEvent(key, state))
            return true;
        
        return false;
    }

    
    bool Panel::CharacterEvent(Char c) {
        return UI::WidgetContainer::CharacterEvent(c);
    }

    
    bool Panel::IsVisible() const {
        return stack.IsVisible();
    }

    
    Geometry::Size Panel::GetInteriorSize() const {
        auto size = stack.TagBounds(UI::ComponentTemplate::ViewPortTag).GetSize();
        
        if(size.Width==0 && size.Height==0) {
            size = stack.GetSize();
        }
        
        return size;
    }
    

    bool Panel::ResizeInterior(Geometry::Size size) {
        Geometry::Size border = {0, 0};
        
        auto innersize = stack.TagBounds(UI::ComponentTemplate::ViewPortTag).GetSize();
        
        if(innersize.Area() != 0)
            border = GetSize() - innersize;
        
        Resize(size + border);

        return stack.TagBounds(UI::ComponentTemplate::ContentsTag).GetSize() == size;
    }
    
    void Panel::Resize(Geometry::Size size) { 
        ComponentStackWidget::Resize(size);
        
        if(HasOrganizer())
            GetOrganizer().Reorganize();
        
        childboundschanged(nullptr);
    }
    
    bool Panel::allowfocus() const {
        for(auto &w : widgets) {
            if(w.AllowFocus()) {
                return true;
            }
        }
        
        return false;
    }
    

    void Panel::focused() {
        if(!HasFocusedWidget())
            FocusFirst();
        WidgetBase::focused();
    }
    

    void Panel::focuslost() {
        RemoveFocus();
        WidgetBase::focuslost();
    }

    
    Gorgon::Layer &Panel::getlayer() {
        return stack.GetLayerOf(stack.IndexOfTag(UI::ComponentTemplate::ContentsTag));
    }


    void Panel::focuschanged() { 
        if(HasFocusedWidget() && !IsFocused())
            Focus();
        else if(!HasFocusedWidget() && IsFocused())
            RemoveFocus();
    }
    

    void Panel::ScrollTo(int x, int y, bool clip){
        auto b = stack.TagBounds(UI::ComponentTemplate::ContentsTag);
        
        if(clip) {
            auto max = MaxScrollOffset();
            
            if(x > max.X)
                x = max.X;
            
            if(x < 0)
                x = 0;
            
            if(y > max.Y)
                y = max.Y;
            
            if(y < 0)
                y = 0;
        }
        
        if(x == target.X && y == target.Y) return;
        
        stack.SetValue(-float(x) / b.Width(), -float(y) / b.Height());
        target = {x, y};
        
        if(scrollspeed == 0) {
            stack.SetTagLocation(UI::ComponentTemplate::ContentsTag, {-x, -y});
        }
        else {
            if(!isscrolling) {
                stack.SetFrameEvent(std::bind(&Panel::updatescroll, this));
                isscrolling = true;
            }
        }
        
        scrollclipped = clip;
    }
    
    void Panel::updatescroll() {
        if(!isscrolling)
            Utils::ASSERT_FALSE("This should not happen");
        
        auto b = stack.TagBounds(UI::ComponentTemplate::ContentsTag);
        
        auto cur = -b.TopLeft();
        
        float d = (float)scrollspeed/1000 * Time::DeltaTime() + scrollleftover;
        int dist = (int)std::round(d);
        scrollleftover = d - dist;
        
        int done = 0;
        
        auto doaxis = [&](int &c, int t) {
            if(t < c) {
                if(c - dist <= t) {
                    c = t;
                }
                else {
                    c -= dist;
                }
            }
            else if(t > c) {
                if(c + dist >= t) {
                    c = t;
                }
                else {
                    c += dist;
                }
            }
            else {
                done++;
            }                
        };
        
        doaxis(cur.X, target.X);
        doaxis(cur.Y, target.Y);
        
        stack.SetTagLocation(UI::ComponentTemplate::ContentsTag, -cur);
        
        if(done == 2) {
            isscrolling = false;
            stack.RemoveFrameEvent();
            scrollleftover = 0;
        }
    }
    
    
    Geometry::Point Panel::ScrollOffset() const {
        auto b = stack.TagBounds(UI::ComponentTemplate::ContentsTag);

        return -b.TopLeft();
    }
    
    
    void Panel::childboundschanged(WidgetBase *source) {
        WidgetContainer::childboundschanged(source);
        
        if(updaterequired)
            return;
        
        updaterequired = true;
        //TODO: Fix me
        //Gorgon::RegisterOnce([this] { 
            updatecontent(); 
        //});
    }
    
    void Panel::updatecontent() {
        int maxx = 0, maxy = 0;
        for(auto &c : widgets) {
            if(!c.IsVisible()) 
                continue;
            
            auto b = c.GetBounds();
            
            if(b.Right > maxx)
                maxx = b.Right;
            
            if(b.Bottom > maxy)
                maxy = b.Bottom;
        }
        
        stack.SetTagSize(UI::ComponentTemplate::ContentsTag, {maxx, maxy});
        
        ScrollTo(ScrollOffset(), scrollclipped);

        auto size = GetInteriorSize();
        auto val = stack.GetValue();
        stack.SetValue(val[0], val[1], Clamp((size.Width+1.f) / (maxx+1.f), 0.f, 1.f), Clamp((size.Height+1.f) / (maxy+1.f), 0.f, 1.f));

        SetSmoothScrollSpeed(scrollspeed);
        
        updaterequired = false;
    }
    
    
    void Panel::SetOverscroll(int value) {
        overscroll = value;
        
        if(scrollclipped)
            ScrollTo(ScrollOffset());
    }

    Geometry::Point Panel::MaxScrollOffset() const {
        auto cont = stack.TagBounds(UI::ComponentTemplate::ContentsTag);
        auto size = GetInteriorSize();
        
        int xscroll = cont.Width() - size.Width;
        
        if(xscroll <= 0)
            xscroll = 0;
        else
            xscroll += overscroll;
        
        int yscroll = cont.Height() - size.Height;
        
        if(yscroll <= 0)
            yscroll = 0;
        else
            yscroll += overscroll;
        
        return {xscroll, yscroll};
    }


    void Panel::SetSmoothScrollSpeed(int value){
        auto b = stack.TagBounds(UI::ComponentTemplate::ContentsTag);
        auto s = b.GetSize();
        
        scrollspeed = value;
        
        if(value == 0 && isscrolling) {
            stack.SetTagLocation(UI::ComponentTemplate::ContentsTag, target);
            
            isscrolling = false;
            scrollleftover = 0;
            stack.RemoveFrameEvent();
        }
        
        if(s.Area() == 0) 
            stack.SetValueTransitionSpeed({0, 0, 0, 0});
        else
            stack.SetValueTransitionSpeed({(float)value / s.Width, (float)value / s.Height, 0, 0});
    }
    
    
     bool Panel::EnsureVisible(const UI::WidgetBase &widget) {
        if(widgets.Find(widget) == widgets.end())
            return false;
        
        if(!widget.IsVisible())
            return false;
        
        auto wb = widget.GetBounds();
        
        Geometry::Bounds cb = {target, GetInteriorSize()};
        
        if(!Contains(cb, wb)) {
            ScrollTo(wb.TopLeft());
        }
        
        return true;
    }
    
} }
