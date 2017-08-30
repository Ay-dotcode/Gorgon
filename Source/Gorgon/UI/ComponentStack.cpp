#include "ComponentStack.h"


namespace Gorgon { namespace UI {
    
    

    ComponentStack::ComponentStack(const Template& temp) : temp(temp) {
        int maxindex = 0;
        
        for(int i=0; i<temp.GetCount(); i++) {
            if(maxindex < temp[i].GetIndex())
                maxindex = temp[i].GetIndex();
        }
       
        indices = maxindex + 1;
        
        data = (Component*)malloc(sizeof(Component) * indices * stackcapacity);
        stacksizes.resize(maxindex);
        
        for(int i=0; i<temp.GetCount(); i++) {
            if(temp[i].GetCondition() == ComponentCondition::Always) {
                AddToStack(temp[i]);
            }
        }
        
        conditions.insert(ComponentCondition::Always);
    }

    void ComponentStack::AddToStack(const ComponentTemplate& temp) {
        int ind = temp.GetIndex();
        int si = stacksizes[ind];
        
        if(si == stackcapacity) {
            grow();
        }
        
        Component &comp = *new (&data[ind + si*indices]) Component(temp);
        
        if(temp.GetClip()) {
            comp.layer = new Graphics::Layer;
        }
        
        stacksizes[ind]++;
    }
    
    void ComponentStack::grow() { 
        stackcapacity += 2;
        
        data = (Component*)realloc(data, sizeof(Component) * indices * stackcapacity);
    }
    
    void ComponentStack::AddCondition(ComponentCondition condition) {
        if(conditions.count(condition)) return;
        
        conditions.insert(condition);
        
        for(int i=0; i<temp.GetCount(); i++) {
            if(temp[i].GetCondition() == ComponentCondition::Always) {
                AddToStack(temp[i]);
            }
        }
    }
    
    void ComponentStack::RemoveCondition(ComponentCondition condition){ 
        if(!conditions.count(condition)) return;
        
        conditions.erase(condition);
        
        for(int i=0; i<indices; i++) {
            for(int j=stacksizes[i]-1; j>=0; j--) {
                if(data[i + j*indices].GetTemplate().GetCondition() == condition) {
                    if(j == stacksizes[i]-1) {
                        //probably will need update
                        
                        stacksizes[i]--;
                        data[i + j*indices].~Component();
                    }
                    else {
                    }
                }
            }
        }
    }
    
    
} }
