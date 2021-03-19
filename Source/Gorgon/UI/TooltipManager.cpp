#include "TooltipManager.h"

#include "Widget.h"
#include "WidgetContainer.h"
#include "../Main.h"

namespace Gorgon { namespace UI {
    
    TooltipManager::TooltipManager(WidgetContainer &container) :
        container(&container)
    {
        Enable();
    }
    
    TooltipManager::~TooltipManager() {
        Disable();
        
        if(owntarget)
            delete target;
    }
        
    void TooltipManager::Enable() {
        if(!token)
            token = BeforeFrameEvent.Register(*this, &TooltipManager::Tick);
    }
    
    void TooltipManager::Disable() {
        if(token) {
            BeforeFrameEvent.Unregister(token);
            token = 0;
        }
    }
    
    void TooltipManager::Hide() {
        if(!displayed)
            return;
        
        displayed = false;
        tooltip   = "";
        settext("");
    }

    void TooltipManager::Show(const std::string &text) {
        if(!text.empty()) {
            tooltip   = text;
            settext(tooltip);
            displayed = true;
        }
        else {
            Hide();
        }
    }

    void TooltipManager::SetTarget(Widget &target, bool own) {
        if(owntarget)
            delete this->target;
        
        this->target = &target;
        owntarget = own;
    }
    
    void TooltipManager::SetSetText(std::function<void(const std::string &)> value) {
        settext = value;
    }
    
    void TooltipManager::Tick() {
        if(!settext)
            return;
        
        
        
        auto wgt = container->GetHoveredWidget();
        
        if(wgt != current) {
            if(current && changetoken) {
                current->TooltipChangedEvent.Unregister(changetoken);
                current->DestroyedEvent.Unregister(destroytoken);
                changetoken  = 0;
                destroytoken = 0;
            }
                
            if(!wgt) {
                //TODO start linger timeout
                
                Hide();
            }
            else {
                changetoken = wgt->TooltipChangedEvent.Register(*this, &TooltipManager::changed);
                destroytoken = wgt->DestroyedEvent.Register(*this, &TooltipManager::destroyed);
                
                //TODO start delay timeout
                
                Show(wgt->GetTooltip());
            }
            
            current = wgt;
        }
    }
    
    void TooltipManager::changed() {
        Show(current->GetTooltip());
    }
    
    void TooltipManager::destroyed() {
        if(current && changetoken) {
            current->TooltipChangedEvent.Unregister(changetoken);
            current->DestroyedEvent.Unregister(destroytoken);
        }
        
        Hide();
        current = nullptr;
    }
    
} }
