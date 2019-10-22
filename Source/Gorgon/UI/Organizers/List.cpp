#include "List.h"
#include "../WidgetContainer.h"

namespace Gorgon { namespace UI { namespace Organizers {
    
    void List::reorganize() {
        int y = 0;
        int w = GetAttached().GetInteriorSize().Width;
        
        for(auto &widget : GetAttached()) {
            widget.Move(0, y);
            widget.SetWidth(w);
            
            y += widget.GetHeight() + spacing;
        }
    }
    
} } }
