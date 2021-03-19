#include "TooltipManager.h"

#include "Widget.h"
#include "WidgetContainer.h"
#include "../Main.h"
#include "../Time.h"
#include "../Window.h"

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
        
        if(!toplevel) {
            toplevel = dynamic_cast<Gorgon::Window*>(&container->TopLevelLayer());
        }
        
        auto wgt = gettooltipwidget();
        
        if(wgt != current) {
            if(current && changetoken) {
                current->TooltipChangedEvent.Unregister(changetoken);
                current->DestroyedEvent.Unregister(destroytoken);
                changetoken  = 0;
                destroytoken = 0;
            }
                
            if(!wgt) {
                if(displayed) {
                    lingerleft = linger + Time::DeltaTime();
                }
                
                delayleft = -1;
            }
            else {
                changetoken = wgt->TooltipChangedEvent.Register(*this, &TooltipManager::changed);
                destroytoken = wgt->DestroyedEvent.Register(*this, &TooltipManager::destroyed);
                
                lingerleft = -1;
                delayleft = displayed ? 0 : delay + Time::DeltaTime();
                toleranceleft = tolerance;
                
                if(toplevel) {
                    lastlocation  = toplevel->GetMouseLocation();
                }
            }
            
            current = wgt;
        }
        
        if(delayleft != -1) {
            if(toplevel) {
                int movement = (toplevel->GetMouseLocation()-lastlocation).ManhattanDistance();
                if(toleranceleft < movement) {
                    lastlocation = toplevel->GetMouseLocation();
                    delayleft = delay + Time::DeltaTime();
                }
                else {
                    toleranceleft -= movement;
                }
            }
            
            if(delayleft <= Time::DeltaTime()) {
                if(current)
                    Show(current->GetTooltip());
                
                delayleft = -1;
                toleranceleft = -1;
            }
            else
                delayleft -= Time::DeltaTime();
        }
        
        if(lingerleft != -1) {
            if(lingerleft <= Time::DeltaTime()) {
                Hide();
                
                lingerleft = -1;
            }
            else
                lingerleft -= Time::DeltaTime();
        }
    }
    
    Widget *TooltipManager::gettooltipwidget() {
        Widget *wgt = container->GetHoveredWidget();
        
        while(wgt) {
            if(!wgt->GetTooltip().empty())
                return wgt;
            else {
                WidgetContainer *cont = wgt->HasParent() ? &wgt->GetParent() : nullptr;
                
                if(cont && cont->IsWidget())
                    wgt = &cont->AsWidget();
                else
                    return nullptr;
            }
        }
        
        return nullptr;
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
