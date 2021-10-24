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
            int uw = GetAttached().GetUnitSize();
            int y = 0;
            int maxy = 0;
            int rowc = 0;
            int ind = 0;
            int indent = 0;
            int breaks = 0;
            int xoff;
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
                    cell.Move(Pixels(
                        cell.GetCurrentLocation() +
                        Geometry::Point{off, (maxy - cell.GetCurrentHeight())/2}
                    ));
                }
                
                x = 0;
                rowc = 0;
                maxy = 0;
                row.Clear();
            };
            
            for(auto &widget : att)  {
                auto p = modifiers.equal_range(ind);
                breaks = 0;
                xoff   = 0;
                indent = 0;
                for(auto it = p.first; it != p.second; it++) {
                    switch(it->second.type) {
                    case Flow::Modifier::Break:
                        breaks++;
                        break;
                    case Flow::Modifier::Align:
                        nextalign = it->second.align;
                        break;
                    case Flow::Modifier::HSpace:
                        xoff = it->second.size * att.GetUnitSize() + GetSpacing();
                        break;
                    case Flow::Modifier::VSpace:
                        y += it->second.size * GetSpacing();
                        break;
                    case Flow::Modifier::Indent:
                        indent += it->second.size;
                        break;
                    case Flow::Modifier::IndentUnits:
                        indent += it->second.size * att.GetUnitSize() + GetSpacing();
                        break;
                    case Flow::Modifier::IndentSpaces:
                        indent += it->second.size * GetSpacing();
                        break;
                    }
                }
                ind++;
                
                if(!widget.IsVisible() || widget.IsFloating()) {
                    continue;
                }
                
                int w = widget.GetCurrentWidth();
                
                if((x + w > width && rowc > 0) && breaks == 0)
                    breaks = 1;
                
                if(breaks) {
                    dorow();
                }
                
                //skip previous row
                if(rowc == 0)
                    x = indent;
                
                x += xoff;
                align = nextalign;

                int h = widget.GetCurrentHeight();
                if(h > maxy) {
                    maxy = h;
                }
                
                widget.Move(Pixels(x, y));
                x += w + s;
                rowc++;
                
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
        
        int order = GetAttached().GetCount();
        
        InsertBreak(order);
    }
    
    void Flow::InsertBreak(const Widget &widget) {
        if(!IsAttached()) {
            throw std::runtime_error("Organizer is not attached.");
        }
        
        int order = GetAttached().GetFocusOrder(widget);
        
        if(order != -1)
            InsertBreak(order + 1);
    }
    
    void Flow::InsertBreak(int index) {
        modifiers.insert({index, Break});
        Reorganize();
    }

    Flow &Flow::Add(Widget &widget) {
        if(nextsize != -1) {
            widget.SetWidth(Units(nextsize));
            nextsize = -1;
        }
        
        Base::Add(widget);
        
        return *this;
    }
    
    void Flow::flow(std::ostream &(*fn)(std::ostream &)) {
        if(fn == &std::endl<char, std::char_traits<char>>) {
            flow(Break);
        }
        else {
            throw std::runtime_error("Unsupported manipulator, only std::endl is supported");
        }
    }
    
    void Flow::flow(Graphics::TextAlignment alignment) {
        flow(Modifier(alignment));
    }
    
    void Flow::flow(Flow::Modifier mod) {
        modifiers.insert({GetAttached().GetCount(), mod});
    }
    
    void Flow::flow(const std::pair<std::string, std::function<void()>> &action) {
        if(!IsAttached()) {
            throw std::runtime_error("This organizer is not attached to a container");
        }
        
        auto &b = *new Widgets::Button(action.first, action.second);
        
        Add(b);
        GetAttached().Own(b);
    }
    
    void Flow::flow(BreakTag) {
        InsertBreak();
        
        if(nextsize != -1) {
            flow(Modifier(Flow::Modifier::VSpace, nextsize));
            nextsize = -1;
        }
    }

    void Flow::flow(const std::string& title) {
        if(title.empty()) {
            flow(Modifier(Flow::Modifier::HSpace, nextsize != -1 ? nextsize : 1));
            nextsize = -1;
        }
        else {
            Add(title);
        }
    }

    Flow::BreakTag Flow::Break;
    
    Graphics::TextAlignment Flow::Left   = Graphics::TextAlignment::Left;
    Graphics::TextAlignment Flow::Center = Graphics::TextAlignment::Center;
    Graphics::TextAlignment Flow::Right  = Graphics::TextAlignment::Right;
    





} } }
