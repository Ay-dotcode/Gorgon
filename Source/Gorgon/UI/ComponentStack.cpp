#include "ComponentStack.h"


namespace Gorgon { namespace UI {
    
    

    ComponentStack::ComponentStack(const Template& temp, Geometry::Size size) : temp(temp), size(size) {
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
        
        Component &comp = *new (&data + (ind + si*indices)) Component(temp);
        
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
        
        bool updatereq = false;
        
        for(int i=0; i<temp.GetCount(); i++) {
            if(temp[i].GetCondition() == condition) {
                updatereq = true;
                AddToStack(temp[i]);
            }
        }
        
        if(updatereq)
            update();
    }
    
    void ComponentStack::RemoveCondition(ComponentCondition condition){ 
        if(!conditions.count(condition)) return;
        
        conditions.erase(condition);
        
        bool updatereq = false;
        
        for(int i=0; i<indices; i++) {
            for(int j=stacksizes[i]-1; j>=0; j--) {
                if(data[i + j*indices].GetTemplate().GetCondition() == condition) {
                    if(j == stacksizes[i]-1) {
                        //optimize: check if update really needed
                        
                        updatereq = true;
                        stacksizes[i]--;
                        get(i, j).~Component();
                    }
                    else {
                        //bubble the item to be deleted to the top of stack.
                        for(int k=j; k<stacksizes[i]-1; k++) {
                            using std::swap;
                            swap(get(i, k), get(i, k+1));
                        }
                        
                        stacksizes[i]--;
                        get(i, stacksizes[i]).~Component();
                    }
                }
            }
        }
        
        if(updatereq)
            update();
    }
    
	void ComponentStack::SetData(ComponentTemplate::DataEffect effect, const std::string &text) {
		stringdata[effect] = text;

		bool updatereq = false;

		for(int i=0; i<indices; i++) {
			if(stacksizes[i] > 0) {
				const ComponentTemplate &temp = get(i).GetTemplate();

				if(temp.GetDataEffect() == effect) {
					//do text related stuff

					updatereq = true;
				}
			}
		}

		if(updatereq)
			update();
	}

	void ComponentStack::update() {
		if(!stacksizes[0]) return;

		update(get(0));
	}

	void ComponentStack::update(Component &parent) {
		const ComponentTemplate &temp = parent.GetTemplate();

		if(temp.GetType() != ComponentType::Container) return;

		const ContainerTemplate &cont = dynamic_cast<const ContainerTemplate&>(temp);
		
		for(int i=0; i<cont.GetCount(); i++) {
			int ci = cont[i];

			if(!stacksizes[ci]) continue;


		}
	}
    
    
} }
