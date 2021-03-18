#include "LayerAdapter.h"
#include "Window.h"
#include "../Widgets/Registry.h"


namespace Gorgon { namespace UI {

    ExtenderRequestResponse LayerAdapter::RequestExtender(const Layer &self) {
        auto toplevel = dynamic_cast<Window *>(&self.GetTopLevel());

        if(toplevel)
            return toplevel->RequestExtender(self);
        else
            return {false, this, self.GetLocation(), GetInteriorSize()};
    }
    
    int LayerAdapter::GetSpacing() const {
        if(issizesset) {
            return spacing;
        }
        else {
            return Widgets::Registry::Active().GetSpacing();
        }
    }

    int LayerAdapter::GetUnitWidth() const {
        if(issizesset) {
            return unitwidth;
        }
        else {
            return Widgets::Registry::Active().GetUnitWidth();
        }
    }

    void LayerAdapter::SetHoveredWidget(Widget *widget) {
        hovered = widget;
        
        if(base) {
            auto toplevel = dynamic_cast<Window *>(&base->GetTopLevel());

            if(toplevel)
                toplevel->SetHoveredWidget(widget);
        }
    }
    
} }
