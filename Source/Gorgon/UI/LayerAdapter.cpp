#include "LayerAdapter.h"
#include "Window.h"


namespace Gorgon { namespace UI {

    ExtenderRequestResponse LayerAdapter::RequestExtender(const Layer &self) {
        auto toplevel = dynamic_cast<Window *>(&self.GetTopLevel());

        if(toplevel)
            return toplevel->RequestExtender(self);
        else
            return {false, this, self.GetLocation()};
    }

} }
