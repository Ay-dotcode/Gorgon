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

        SetSmoothScrollSpeed(scrollspeed);
    }

    
    bool Panel::Activate() {
        if(!Focus())
            return false;

        FocusFirst();

        return true;
    }

    
    bool Panel::KeyEvent(Input::Key key, float state) {
        return UI::WidgetContainer::KeyEvent(key, state);
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
        auto tb = stack.TagBounds(UI::ComponentTemplate::ContentsTag).GetSize();
        Resize(size + stack.GetSize() - tb);

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
        
        stack.SetValue(-float(x) / b.Width(), -float(y) / b.Height());
        scrollclipped = clip;
    }
    
    
    Geometry::Point Panel::ScrollOffset() const {
        auto b = stack.TagBounds(UI::ComponentTemplate::ContentsTag);
        auto val = stack.GetValue();

        return {(int)(-val[0]*b.Width()), (int)(-val[1]*b.Height())};
    }
    
    
    void Panel::childboundschanged(WidgetBase *source) {
        WidgetContainer::childboundschanged(source);
        
        if(updaterequired)
            return;
        
        updaterequired = true;
        Gorgon::RegisterOnce(std::bind(&Panel::updatecontent, this));
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
        
        //clip scroll
        if(scrollclipped)
            ScrollTo(ScrollOffset());
        
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
        auto s = stack.TagBounds(UI::ComponentTemplate::ContentsTag).GetSize();
        
        scrollspeed = value;
        
        if(s.Area() == 0) 
            stack.SetValueTransitionSpeed({0, 0, 0, 0});
        else
            stack.SetValueTransitionSpeed({(float)value / s.Width, (float)value / s.Height, 0, 0});
    }
} }
