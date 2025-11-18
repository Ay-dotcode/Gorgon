#include "List.h"
#include "../WidgetContainer.h"
#include "../../Widgets/Registry.h"

namespace Gorgon { namespace UI { namespace Organizers {
    
    void List::reorganize() {
        int y = 0;
        int w = GetAttached().GetInteriorSize().Width;
        int s = GetSpacing();
        
        for(auto &widget : GetAttached()) {
            if(widget.IsVisible() && !widget.IsFloating()) {
                widget.Move(Pixels(0, y));
                widget.SetWidth(Pixels(w));
                
                y += widget.GetCurrentHeight() + s;
            }
        }
    }
    
    int List::GetSpacing() const {
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
    

} } }
