#include "Base.h"
#include "../WidgetContainer.h"
#include "../../Widgets/Label.h"

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
    
    void Base::PauseReorganize() { 
        paused = true;
    }

    void Base::StartReorganize() { 
        if(!paused)
            return;
        
        paused = false;
        Reorganize();
    }

    void Base::Reorganize(){
        //TODO queue organizing
        if(organizing || paused)
            return;
        
        if(IsAttached()) {
            organizing = true;
            reorganize();
            organizing = false;
        }
    }

    Base &Base::Add (Widget &widget) {
        GetAttached().Add(widget);

        return *this;
    }

    Base &Base::Add (const std::string &title) {
        if(!IsAttached()) {
            throw std::runtime_error("This organizer is not attached to a container");
        }
        
        auto &l = *new Widgets::Label(title);
        
        Add(l);
        GetAttached().Own(l);

        return *this;
    }
    
    
} } }
