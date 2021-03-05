#include "Flow.h"

#include "../WidgetContainer.h"
#include "../../Widgets/Registry.h"
#include "../../Widgets/Button.h"

#include <iostream>

namespace Gorgon { namespace UI { namespace Organizers {
    
    void Flow::reorganize() {
        auto &att = GetAttached();
        int x = 0;
        int width = att.GetInteriorSize().Width;
        int s = GetSpacing();
        
        if(tight) {
            Utils::NotImplemented("Tight organization is not implemented yet.");
        }
        else {
            int uw = GetAttached().GetUnitWidth();
            int y = 0;
            int maxy = 0;
            int rowc = 0;
            int ind = -1;
            int breaks = BreakCount(-1);
            auto align = defaultalign;
            auto nextalign = defaultalign;
            
            //to valign
            Containers::Collection<Widget> row;
            
            auto dorow = [&]{
                if(maxy == 0)
                    maxy = uw;
                
                y += maxy + s;
                
                if(breaks > 0)
                    y += (breaks-1) * (uw + s);
                
                int w = 0;
                if(row.GetCount())
                    w = row.Last()->GetBounds().Right;
                
                int off = 0;
                
                if(align == Graphics::TextAlignment::Center) {
                    off = (width - w) / 2;
                }
                else if(align == Graphics::TextAlignment::Right) {
                    off = width - w;
                }
                
                for(auto &cell : row) {
                    cell.Move(
                        cell.GetLocation() + 
                        Geometry::Point{off, (maxy - cell.GetHeight())/2}
                    );
                }
                
                x = 0;
                rowc = 0;
                maxy = 0;
                row.Clear();
            };
            
            for(auto &widget : att)  {
                ind++;
                
                if(!widget.IsVisible() || widget.IsFloating()) {
                    continue;
                }
                
                int w = widget.GetWidth();
                
                if((x + w > width && rowc > 0) && breaks == 0)
                    breaks = 1;
                
                if(breaks) {
                    dorow();
                }
                            
                //skip previous row
                align = nextalign;

                int h = widget.GetHeight();
                if(h > maxy) {
                    maxy = h;
                }
                
                widget.Move(x, y);
                x += w + s;
                rowc++;
                breaks = BreakCount(ind);
                
                if(aligns.count(ind)) {
                    nextalign = aligns[ind];
                }
                
                row.Push(widget);
            }
            dorow();
        }
    }
    
    int Flow::GetSpacing() const {
        if(usedefaultspacing) {
            if(IsAttached()) {
                return GetAttached().GetSpacing();
            }
            else {
                return Widgets::Registry::Active().GetSpacing();
            }
        }
        else {
            return spacing;
        }
    }
    
    void Flow::SetAlignment(Graphics::TextAlignment value) {
        defaultalign = value;
        
        Reorganize();
    }
    
    void Flow::InsertBreak() {
        if(!IsAttached()) {
            throw std::runtime_error("Organizer is not attached.");
        }
        
        int order = GetAttached().GetCount() - 1;
        
        InsertBreak(order);
    }
    
    void Flow::InsertBreak(const Widget &widget) {
        if(!IsAttached()) {
            throw std::runtime_error("Organizer is not attached.");
        }
        
        int order = GetAttached().GetFocusOrder(widget);
        
        if(order != -1)
            InsertBreak(order);
    }
    
    Flow &Flow::Add(Widget &widget) {
        if(nextsize != -1) {
            widget.SetWidthInUnits(nextsize);
            nextsize = -1;
        }
        
        Base::Add(widget);
        
        return *this;
    }
    
    void Flow::flow(std::ostream &(*fn)(std::ostream &)) {
        if(fn == &std::endl<char, std::char_traits<char>>) {
            InsertBreak();
        }
        else {
            throw std::runtime_error("Unsupported manipulator, only std::endl is supported");
        }
    }
    
    void Flow::flow(Graphics::TextAlignment alignment) {
        aligns[GetAttached().GetCount() - 1] = alignment;
        
        Reorganize();
    }
    
    void Flow::flow(const std::pair<std::string, std::function<void()>> &action) {
        if(!IsAttached()) {
            throw std::runtime_error("This organizer is not attached to a container");
        }
        
        auto &b = *new Widgets::Button(action.first, action.second);
        
        Add(b);
        GetAttached().Own(b);
    }
    
    Flow::BreakTag Flow::Break;
    
    Graphics::TextAlignment Flow::Left   = Graphics::TextAlignment::Left;
    Graphics::TextAlignment Flow::Center = Graphics::TextAlignment::Center;
    Graphics::TextAlignment Flow::Right  = Graphics::TextAlignment::Right;
    

} } }
