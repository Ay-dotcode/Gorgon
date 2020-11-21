#include "Window.h"
#include "../Widgets/Registry.h"


namespace Gorgon { namespace UI {

    
    int Window::GetSpacing() const {
        if(issizesset) {
            return spacing;
        }
        else {
            return Widgets::Registry::Active().GetSpacing();
        }
    }

    int Window::GetUnitWidth() const {
        if(issizesset) {
            return unitwidth;
        }
        else {
            return Widgets::Registry::Active().GetUnitWidth();
        }
    }
    
} }
