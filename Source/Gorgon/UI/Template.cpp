
#include "Template.h"

namespace Gorgon { namespace UI {

    PlaceholderTemplate& Template::AddPlaceholder(int index, ComponentCondition condition){ 
        auto obj = new PlaceholderTemplate();
        components.Add(obj);
        
        obj->SetIndex(index);
        obj->SetCondition(condition);
        
        tokens.push_back(
            obj->ChangedEvent.Register(ChangedEvent, &Event<Template>::operator ())
        );
        
        ChangedEvent();
        
        return *obj;
    }

    TextholderTemplate& Template::AddTextholder(int index, ComponentCondition condition){ 
        auto obj = new TextholderTemplate();
        components.Add(obj);
        
        obj->SetIndex(index);
        obj->SetCondition(condition);
        
        tokens.push_back(
            obj->ChangedEvent.Register(ChangedEvent, &Event<Template>::operator ())
        );
        
        ChangedEvent();

        return *obj;
    }

	GraphicsTemplate& Template::AddGraphics(int index, ComponentCondition condition){
        auto obj = new GraphicsTemplate();
        components.Add(obj);
        
        obj->SetIndex(index);
        obj->SetCondition(condition);
        
        tokens.push_back(
            obj->ChangedEvent.Register(ChangedEvent, &Event<Template>::operator ())
        );
        
        ChangedEvent();

        return *obj;
    }

    ComponentTemplate& Template::Release(int index) { 
        auto &c = components[index];
        
        c.ChangedEvent.Unregister(tokens[index]);
        
        tokens.erase(tokens.begin() + index);
        
        components.Remove(index);
        
        ChangedEvent();
        
        return c;
    }

    void Template::Assume(ComponentTemplate& component) { 
        components.Add(component);
        
        tokens.push_back(
            component.ChangedEvent.Register(ChangedEvent, &Event<Template>::operator ())
        );
        
        ChangedEvent();
    }
    
} }
