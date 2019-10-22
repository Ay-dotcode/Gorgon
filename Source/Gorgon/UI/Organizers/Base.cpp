#include "Base.h"
#include "../WidgetContainer.h"

namespace Gorgon { namespace UI { namespace Organizers {
    
    void Base::AttachTo(WidgetContainer& container) {
        if(attachedto == &container)
            return;
        
        attachedto = &container;
        
        container.AttachOrganizer(*this);
        
        attachmentchanged();
    }

    void Base::RemoveFrom() {
        if(attachedto == nullptr)
            return;
        
        auto org = attachedto;
        
        attachedto = nullptr;
        
        org->RemoveOrganizer();
        
        attachmentchanged();
    }
    

    void Base::Reorganize(){
        if(organizing)
            return;
        
        if(IsAttached()) {
            organizing = true;
            reorganize();
            organizing = false;
        }
    }
} } }
