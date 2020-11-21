#include "Flow.h"
#include "../WidgetContainer.h"
#include "../../Widgets/Registry.h"

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
            
            for(auto &widget : att)  {
                ind++;
                
                if(!widget.IsVisible() || widget.IsFloating()) {
                    breaks = 0;
                    continue;
                }
                
                int w = widget.GetWidth();
                
                breaks += (x + w > width && rowc > 0) ? 1 : 0;
                
                if(breaks) {
                    if(maxy == 0)
                        maxy = uw;
                    
                    y += maxy + s;
                    
                    if(breaks > 0)
                        y += (breaks-1) * (uw + s);
                    
                    x = 0;
                    rowc = 0;
                    maxy = 0;
                }
                
                int h = widget.GetHeight();
                if(h > maxy) {
                    maxy = h;
                }
                
                widget.Move(x, y);
                x += w + s;
                rowc++;
                breaks = BreakCount(ind);
            }
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

} } }
