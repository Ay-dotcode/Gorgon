#include "ColorPicker.h"


namespace Gorgon { namespace Widgets {
    
    ColorPicker::ColorPicker(const UI::Template &temp) :
        Inputbox<Graphics::RGBAf>(temp, Graphics::Color::Black),
        plane(
             this->stack.GetTemplate(UI::ComponentTemplate::ListTag)  ? 
            *this->stack.GetTemplate(UI::ComponentTemplate::ListTag) :
             Registry::Active()[Registry::ColorPlane_Regular]
        )
    {
        stack.AddGenerator(UI::ComponentTemplate::ListTag, {}); 
        
        defaultsize  = plane.GetSize();
        
        updatevaluedisplay(false);
        
        stack.SetMouseUpEvent([this](auto tag, auto location, auto btn) {
            if(tag == UI::ComponentTemplate::NoTag) {
                int ind = stack.ComponentAt(location);
                
                if(ind != -1)
                    tag = stack.GetTemplate(ind).GetTag();
            }
            
            if(tag == UI::ComponentTemplate::ButtonTag) {
                Toggle();
                
                return;
            }
            
            mouseup(tag, location, btn);
        });
        
        stack.SetMouseDownEvent([this](auto tag, auto location, auto btn) {
            if(tag == UI::ComponentTemplate::NoTag) {
                int ind = stack.ComponentAt(location);
                
                if(ind != -1)
                    tag = stack.GetTemplate(ind).GetTag();
            }
            
            if(tag == UI::ComponentTemplate::ButtonTag) {
                if(AllowFocus())
                    Focus();
                
                return;
            }
            
            mousedown(tag, location, btn);
        });
        
        plane.ClickedEvent.Register([this](bool alphaclick) {
            bool eq = (*this == plane);
            
            *this = plane;
            ChangedEvent(*this);
            
            if(!alpha || eq || alphaclick)
                Close();
            
            if(autoselectall)
                SelectAll();
        });
    }
    
    void ColorPicker::SetDisplay(const DisplayType &value) {
        if(value == validator.Display)
            return;
        
        validator.Display = value;
        updatevaluedisplay(true);
    }
    
    void ColorPicker::Toggle() {
        if(opened)
            Close();
        else
            Open();
    }

    void ColorPicker::Open() {
        if(!HasParent())
            return;
        if(!GetParent().IsInPartialView(*this))
            return;
        if(opened)
            return;
        
        auto res = GetParent().RequestExtender(stack);
        
        if(!res.Extender)
            return;
        
        opened = true;
        
        int below = res.TotalSize.Height-res.CoordinatesInExtender.Y-GetHeight();
        int above = res.CoordinatesInExtender.Y;
        reversed  = false;
        
        plane.Alpha      = alpha;
        plane.HueDensity = huedensity;
        plane.LCDensity  = lcdensity;
        
        if(below < defaultsize.Height && above > below) {
            plane.SetHeight(std::min(defaultsize.Height, above));
                
            reversed = true;
        }
        else {
            plane.SetHeight(std::min(defaultsize.Height, below));
        }
        
        int targetx = res.CoordinatesInExtender.X - (defaultsize.Width - GetWidth());
        
        if(targetx < 0) {
            plane.SetWidth(std::min(defaultsize.Width, res.TotalSize.Width - res.CoordinatesInExtender.X));
            
            targetx = 0;
        }
        else {
            plane.SetWidth(defaultsize.Width);
        }
        
        
        if(reversed) {
            plane.Move(targetx, res.CoordinatesInExtender.Y - plane.GetHeight());
        }
        else {
            plane.Move(targetx, res.CoordinatesInExtender.Y + GetHeight());
        }
        
        //plane.SetWidth(GetWidth());
        plane.SetFloating(true);
        plane = *this;
        res.Extender->Add(plane);
        
        stack.AddCondition(UI::ComponentCondition::Opened);
        stack.SetFrameEvent(std::bind(&ColorPicker::checkfocus, this));
    }
    
    void ColorPicker::Close() {
        if(!opened)
            return;
        
        opened = false;
        stack.RemoveCondition(UI::ComponentCondition::Opened);
        stack.RemoveFrameEvent();
        plane.Remove();
    }
    
    void ColorPicker::checkfocus() {
        if(!this->IsFocused())
            Close();
    }
    
    constexpr ColorPicker::Density ColorPicker::Low;
    constexpr ColorPicker::Density ColorPicker::Medium;
    constexpr ColorPicker::Density ColorPicker::High;
    constexpr ColorPicker::Density ColorPicker::VeryHigh;
            
    constexpr ColorPicker::DisplayType ColorPicker::Hex;
    constexpr ColorPicker::DisplayType ColorPicker::HTML;
    constexpr ColorPicker::DisplayType ColorPicker::RGBAf;
    
} }
