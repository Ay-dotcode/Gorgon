#include "Panel.h"
#include "../Main.h"

#include <math.h>

namespace Gorgon { namespace Widgets {
    Panel::Panel(const UI::Template &temp) : 
    UI::ComponentStackWidget(temp) 
    {
        stack.HandleMouse();
        stack.SetOtherMouseEvent([this](UI::ComponentTemplate::Tag, Input::Mouse::EventType type, Geometry::Point location, float amount) {
            return MouseScroll(type, location, amount);
        });
        
        stack.AddGenerator(UI::ComponentTemplate::VScrollTag, 
                           std::bind(&Panel::createvscroll, this, std::placeholders::_1));
        
        stack.AddGenerator(UI::ComponentTemplate::HScrollTag, 
                           std::bind(&Panel::createhscroll, this, std::placeholders::_1));
        
        
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
        
        stack.AddCondition(UI::ComponentCondition::VScroll);

        SetSmoothScrollSpeed(scrollspeed);
        
        scrolldist = {temp.GetWidth()/4, temp.GetHeight()/4};
    }

    bool Panel::Activate() {
        if(!Focus())
            return false;

        FocusFirst();

        return true;
    }
    
    bool Panel::KeyPressed(Input::Key key, float state) {
        if(UI::WidgetContainer::KeyPressed(key, state))
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
    
    bool Panel::CharacterPressed(Char c) {
        return UI::WidgetContainer::CharacterPressed(c);
    }
    
    bool Panel::IsDisplayed() const {
        return stack.IsVisible() && IsVisible() && HasParent() && GetParent().IsDisplayed();
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
    
    void Panel::Resize(const Geometry::Size &size) { 
        ComponentStackWidget::Resize(size);
        
        if(HasOrganizer())
            GetOrganizer().Reorganize();
        
        childboundschanged(nullptr);
        distributeparentboundschanged();
    }
    
    void Panel::Move(const Geometry::Point &location) { 
        ComponentStackWidget::Move(location);
        
        distributeparentboundschanged();
    }
    
    bool Panel::allowfocus() const {
        if(CurrentFocusStrategy() == Deny)
            return false;
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
        ComponentStackWidget::focused();
    }
    
    void Panel::focuslost() {
        RemoveFocus();
        ComponentStackWidget::focuslost();
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
            scrolloffset = {x, y};
        }
        else {
            if(!isscrolling) {
                stack.SetFrameEvent(std::bind(&Panel::updatescroll, this));
                isscrolling = true;
            }
        }
        
        updatebars();        
        scrollclipped = clip;
    }
    
    void Panel::updatescroll() {
        if(!isscrolling)
            Utils::ASSERT_FALSE("This should not happen");
        
        auto b = stack.TagBounds(UI::ComponentTemplate::ContentsTag);
        
        auto cur = -b.TopLeft();
        
        auto curspeed = scrollspeed;
        
        if(1000*target.ManhattanDistance(cur)/scrollspeed > maxscrolltime) {
            //due to integer division, this value would be scrollspeed at some points, which will reset smooth speed
            //if not, when scrolling is finished it will be reset
            curspeed = int(1000*target.ManhattanDistance(cur) / maxscrolltime);
        
            auto vscroller = dynamic_cast<VScroller<int>*>(stack.GetWidget(UI::ComponentTemplate::VScrollTag));
            if(vscroller)
                vscroller->SetSmoothChangeSpeed(curspeed);
            
            auto hscroller = dynamic_cast<HScroller<int>*>(stack.GetWidget(UI::ComponentTemplate::HScrollTag));
            if(hscroller)
                hscroller->SetSmoothChangeSpeed(curspeed);
        }
        
        float d = (float)curspeed/1000 * Time::DeltaTime() + scrollleftover;
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
        
        scrolloffset = cur;
        stack.SetTagLocation(UI::ComponentTemplate::ContentsTag, -cur);
        
        distributeparentboundschanged();
        
        if(done == 2) {
            isscrolling = false;
            stack.RemoveFrameEvent();
                
            auto vscroller = dynamic_cast<VScroller<float>*>(stack.GetWidget(UI::ComponentTemplate::VScrollTag));
            if(vscroller)
                vscroller->SetSmoothChangeSpeed(scrollspeed);
            
            auto hscroller = dynamic_cast<HScroller<float>*>(stack.GetWidget(UI::ComponentTemplate::HScrollTag));
            if(hscroller)
                hscroller->SetSmoothChangeSpeed(scrollspeed);
                    
            scrollleftover = 0;
        }
    }
    
    void Panel::updatebars() {
        auto b = stack.TagBounds(UI::ComponentTemplate::ContentsTag);
        
        auto vscroller = dynamic_cast<VScrollbar*>(stack.GetWidget(UI::ComponentTemplate::VScrollTag));
        
        if(vscroller != nullptr) {
            vscroller->Maximum = b.Height();
            vscroller->Range   = GetInteriorSize().Height;
            *vscroller         = target.Y;
        }
        
        auto hscroller = dynamic_cast<HScrollbar*>(stack.GetWidget(UI::ComponentTemplate::HScrollTag));
        
        if(hscroller != nullptr) {
            hscroller->Maximum = b.Width();
            hscroller->Range   = GetInteriorSize().Width;
            *hscroller         = target.X;
        }
    }
    
    Geometry::Point Panel::ScrollOffset() const {
        return scrolloffset;
    }
    
    void Panel::childboundschanged(Widget *source) {
        WidgetContainer::childboundschanged(source);
        
        if(updaterequired)
            return;
        
        updaterequired = true;
        //TODO: Fix me
        //Gorgon::RegisterOnce([this] { 
            updatecontent(); 
        //});
        updatebars();
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
        
        auto vscroller = dynamic_cast<VScrollbar*>(stack.GetWidget(UI::ComponentTemplate::VScrollTag));
        
        if(vscroller != nullptr) {
            vscroller->SetSmoothChangeSpeed(scrollspeed);
        }
        
        auto hscroller = dynamic_cast<HScrollbar*>(stack.GetWidget(UI::ComponentTemplate::HScrollTag));
        
        if(hscroller != nullptr) {
            hscroller->SetSmoothChangeSpeed(scrollspeed);
        }
        
        if(s.Area() == 0) 
            stack.SetValueTransitionSpeed({0, 0, 0, 0});
        else
            stack.SetValueTransitionSpeed({(float)value / s.Width, (float)value / s.Height, 0, 0});
    }
    
    bool Panel::EnsureVisible(const UI::Widget &widget) {
        if(widgets.Find(widget) == widgets.end())
            return false;
        
        if(!widget.IsVisible())
            return false;
        
        auto wb = widget.GetBounds();
        
        Geometry::Bounds cb = {target, GetInteriorSize()};
        
        bool doscroll = false;
        auto scrollto = ScrollOffset();
        
        //TODO minimal scrolling
        if(hscroll && (cb.Left > wb.Left || cb.Right < wb.Right)) {
            scrollto.X = wb.Left;
            doscroll = true;
        }
        
        if(vscroll && (cb.Top > wb.Top || cb.Bottom < wb.Bottom)) {
            scrollto.Y = wb.Top;
            doscroll = true;
        }
        
        if(doscroll)
            ScrollTo(scrollto);
        
        return true;
    }
    
    void Panel::EnableScroll(bool vertical, bool horizontal) {
        if(vertical && !vscroll) {
            stack.AddCondition(UI::ComponentCondition::VScroll);
        }
        if(!vertical && vscroll) {
            stack.RemoveCondition(UI::ComponentCondition::VScroll);
        }
        if(horizontal && !hscroll) {
            stack.AddCondition(UI::ComponentCondition::HScroll);
        }
        if(!horizontal && hscroll) {
            stack.RemoveCondition(UI::ComponentCondition::HScroll);
        }
        if((!horizontal || !vertical) && hscroll && vscroll) {
            stack.RemoveCondition(UI::ComponentCondition::HVScroll);
        }
        if(horizontal && vertical && (!vscroll || !hscroll)) {
            stack.AddCondition(UI::ComponentCondition::HVScroll);
        }
        
        vscroll = vertical;
        hscroll = horizontal;
    }
    
    UI::Widget* Panel::createvscroll(const UI::Template& temp) {
        auto vscroller = new VScrollbar(temp);
        vscroller->Maximum = stack.TagBounds(UI::ComponentTemplate::ContentsTag).Height();
        vscroller->Range   = GetInteriorSize().Height;
        *vscroller         = target.Y;
        vscroller->SetSmoothChangeSpeed(scrollspeed);
        vscroller->ValueChanged.Register(*this, &Panel::scrolltoy);
        
        return vscroller;
    }
    
    UI::Widget* Panel::createhscroll(const UI::Template& temp) {
        auto hscroller = new HScrollbar(temp);
        hscroller->Maximum = stack.TagBounds(UI::ComponentTemplate::ContentsTag).Width();
        hscroller->Range   = GetInteriorSize().Width;
        *hscroller         = target.X;
        hscroller->SetSmoothChangeSpeed(scrollspeed);
        hscroller->ValueChanged.Register(*this, &Panel::scrolltox);
        
        return hscroller;
    }


    UI::ExtenderRequestResponse Panel::RequestExtender(const Layer &self) {
        if(HasParent()) {
            auto ans = GetParent().RequestExtender(self);

            if(ans.Extender) {
                if(!ans.Transformed) {
                    Geometry::Point offset = stack.TagBounds(UI::ComponentTemplate::ContentsTag).TopLeft();
                    
                    ans.CoordinatesInExtender += GetLocation() + offset;
                }

                return ans;
            }
        }

        auto size = GetInteriorSize();
        return {false, this, self.GetLocation(), {hscroll ? -1 : size.Width, vscroll ? -1 : size.Height}};
    }
    
    int Panel::GetSpacing() const {
        if(issizesset) {
            return spacing;
        }
        else {
            return stack.GetTemplate().GetSpacing();
        }
    }

    int Panel::GetUnitWidth() const {
        if(issizesset) {
            return unitwidth;
        }
        else {
            return stack.GetTemplate().GetUnitWidth();
        }
    }

    bool Panel::MouseScroll(Input::Mouse::EventType type, Geometry::Point, float amount) {
        //if only horizontal scroll is enabled, use regular scroll to scroll that direction
        if(!vscroll && hscroll && type == Input::Mouse::EventType::Scroll_Vert) {
            type = Input::Mouse::EventType::Scroll_Hor;
        }
        else if(hscroll && type == Input::Mouse::EventType::Scroll_Vert && (Input::Keyboard::CurrentModifier & Input::Keyboard::Modifier::Shift)) {
            type = Input::Mouse::EventType::Scroll_Hor;
        }
        
        if(type == Input::Mouse::EventType::Scroll_Vert && vscroll) {
            if(amount<0 && ScrollOffset().Y >= MaxScrollOffset().Y)
                return false;
            
            if(amount>0 && ScrollOffset().Y <= 0)
                return false;
            
            ScrollBy(-(int)amount*scrolldist.Y);
            return true;
        }
        
        if(type == Input::Mouse::EventType::Scroll_Hor && hscroll) {
            if(amount<0 && ScrollOffset().X >= MaxScrollOffset().X)
                return false;
            
            if(amount>0 && ScrollOffset().X <= 0)
                return false;
            
            ScrollBy(-(int)amount*scrolldist.X, 0);
            return true;
        }
        
        return false;
    }

} }

