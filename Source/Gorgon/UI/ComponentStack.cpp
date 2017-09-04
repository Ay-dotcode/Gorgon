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
        stacksizes.resize(indices);
        
        Add(base);
        
        AddCondition(ComponentCondition::Always);
    }

    void ComponentStack::AddToStack(const ComponentTemplate& temp) {
        int ind = temp.GetIndex();
        int si = stacksizes[ind];
        
        if(si == stackcapacity) {
            grow();
        }
        
        new (data + (ind + si*indices)) Component(temp);

		if(!storage.count(&temp)) {
			auto storage = new ComponentStorage;
			this->storage[&temp] = storage;
            
            if(temp.GetClip()) {
                storage->layer = new Graphics::Layer;
            }

			if(temp.GetType() == ComponentType::Container) {
				const auto &ctemp = dynamic_cast<const ContainerTemplate&>(temp);

				if(ctemp.Background.HasContent()) {
					storage->primary = &ctemp.Background.Instantiate(controller);
				}

				if(ctemp.Overlay.HasContent()) {
					storage->secondary = &ctemp.Overlay.Instantiate(controller);
				}
			}
			if(temp.GetType() == ComponentType::Graphics) {
				const auto &gtemp = dynamic_cast<const GraphicsTemplate&>(temp);

				if(gtemp.Content.HasContent()) {
					storage->primary = &gtemp.Content.Instantiate(controller);
				}
			}
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
            Update();
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
            Update();
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
			Update();
	}

	void ComponentStack::Update() {
		updaterequired = true;
	}

	void ComponentStack::update() {
		if(!stacksizes[0]) return;

        get(0).size = size;
        get(0).location = {0,0};
        
		update(get(0));

		updaterequired = false;

		//draw everything
		render(get(0), base);
	}

	void ComponentStack::render(Component &comp, Graphics::Layer &parent) {
		const ComponentTemplate &temp = comp.GetTemplate();

        Graphics::Layer *target = nullptr;
        auto &st = *storage[&temp];
        
        if(st.layer) {
            target = st.layer;
            parent.Add(*target);
            target->Resize(comp.size);
        }
        else {
            target = &parent;
        }
        
		if(temp.GetType() == ComponentType::Container) {
            const auto &cont = dynamic_cast<const ContainerTemplate&>(temp);
            
            if(st.primary && target) {
                auto rectangular = dynamic_cast<const Graphics::RectangularDrawable*>(st.primary);
                if(rectangular)
                    rectangular->DrawIn(*target, comp.location, comp.size);
                else
                    st.primary->Draw(*target, comp.location);
            }
            
            for(int i=0; i<cont.GetCount(); i++) {
                if(stacksizes[cont[i]])
                    render(get(cont[i]), target ? *target : parent);
            }
            
            if(st.secondary && target) {
                auto rectangular = dynamic_cast<const Graphics::RectangularDrawable*>(st.secondary);
                if(rectangular)
                    rectangular->DrawIn(*target, comp.location, comp.size);
                else
                    st.secondary->Draw(*target, comp.location);
            }
		}
		else if(temp.GetType() == ComponentType::Graphics) {
            if(st.primary && target) {
                auto rectangular = dynamic_cast<const Graphics::RectangularDrawable*>(st.primary);
                if(rectangular)
                    rectangular->DrawIn(*target, comp.location, comp.size);
                else
                    st.primary->Draw(*target, comp.location);
            }
        }
	}

	void anchortoparent(Component &comp, const ComponentTemplate &temp, 
                        Geometry::Point offset, Geometry::Margin margin, Geometry::Size maxsize) {
        
        Anchor pa = temp.GetContainerAnchor();
        Anchor ca = temp.GetMyAnchor();
        
        Geometry::Point pp, cp;
        
        switch(pa) {
            default:
			case Anchor::TopLeft:
				pp = margin.TopLeft();
				break;

			case Anchor::TopCenter:
				pp = {margin.Left + maxsize.Width / 2, margin.Top};
				break;

			case Anchor::TopRight:
				pp = {margin.Left + maxsize.Width, margin.Top};
				break;


			case Anchor::MiddleLeft:
			case Anchor::FirstBaselineLeft:
				pp = {margin.Left, margin.Top + maxsize.Height / 2};
				break;

			case Anchor::MiddleCenter:
				pp = {margin.Left + maxsize.Width / 2, margin.Top + maxsize.Height / 2};
				break;

			case Anchor::MiddleRight:
			case Anchor::FirstBaselineRight:
				pp = {margin.Left + maxsize.Width, margin.Top + maxsize.Height / 2};
				break;

        
			case Anchor::BottomLeft:
            case Anchor::LastBaselineLeft:
				pp = {margin.Left, margin.Top + maxsize.Height};
				break;

			case Anchor::BottomCenter:
				pp = {margin.Left + maxsize.Width / 2, margin.Top + maxsize.Height};
				break;

			case Anchor::BottomRight:
            case Anchor::LastBaselineRight:
				pp = {margin.Left + maxsize.Width, margin.Top + maxsize.Height};
				break;

        }
        
        auto csize = comp.size;
        
        switch(ca) {
            default:
			case Anchor::TopLeft:
				cp = {-offset.X, -offset.Y};
				break;

			case Anchor::TopCenter:
				cp = {-offset.X + csize.Width / 2, -offset.Y};
				break;

			case Anchor::TopRight:
				cp = { offset.X + csize.Width, -offset.Y};
				break;


			case Anchor::MiddleLeft:
			case Anchor::FirstBaselineLeft:
				cp = {-offset.X, csize.Height / 2 - offset.Y};
				break;

			case Anchor::MiddleCenter:
				cp = {-offset.X + csize.Width / 2, csize.Height / 2 - offset.Y};
				break;

			case Anchor::MiddleRight:
			case Anchor::FirstBaselineRight:
				cp = { offset.X + csize.Width, csize.Height / 2 - offset.Y};
				break;

        
			case Anchor::BottomLeft:
            case Anchor::LastBaselineLeft:
				cp = {-offset.X, csize.Height + offset.Y};
				break;

			case Anchor::BottomCenter:
				cp = {-offset.X + csize.Width / 2, csize.Height + offset.Y};
				break;

			case Anchor::BottomRight:
            case Anchor::LastBaselineRight:
				cp = { offset.X + csize.Width, csize.Height + offset.Y};
				break;

        }
        
        comp.location = pp - cp;
    }
	
	void ComponentStack::update(Component &parent) {
		const ComponentTemplate &ctemp = parent.GetTemplate();

		if(ctemp.GetType() != ComponentType::Container) return;

		const ContainerTemplate &cont = dynamic_cast<const ContainerTemplate&>(ctemp);
        
        parent.innersize = parent.size - cont.GetBorderSize();

		Component *prev = nullptr, *next = nullptr;

		for(int i=0; i<cont.GetCount(); i++) {
			int ci = cont[i];

			if(!stacksizes[ci]) continue;

            auto &comp = get(cont[i]);
            
            const auto &temp = comp.GetTemplate();            
            
            //check if textholder and if so use emsize from the font
			int emsize = 10;
            
            //check anchor object by observing temp.GetPreviousAnchor and direction
			Component *anch = nullptr;
			if(cont.GetOrientation() == Graphics::Orientation::Horizontal) {
				if(IsLeft(temp.GetPreviousAnchor())) {
					anch = prev;
				}
				else {
					anch = next;
				}
			}
			else {
				if(IsTop(temp.GetPreviousAnchor())) {
					anch = prev;
				}
				else {
					anch = next;
				}
			}
            
            if(temp.GetPositioning() == temp.Absolute || !anch) { //absolute means anchor is to the parent
                auto margin = Convert(temp.GetMargin(), parent.innersize, emsize).CombinePadding(Convert(cont.GetPadding(), parent.size, emsize)) + Convert(temp.GetIndent(), parent.innersize, emsize);
                
                auto maxsize = parent.innersize - margin;
                
                if(temp.GetSizing() == temp.Fixed) {
                    comp.size = Convert(temp.GetSize(), maxsize, emsize);
                }
                else {
                    //todo
                }
                
                auto offset = Convert(temp.GetPosition(), maxsize, emsize);
                
                anchortoparent(comp, temp, offset, margin, maxsize);

				//Which anchor side is to be changed
				if(temp.GetPositioning() != temp.Absolute) {
					if(cont.GetOrientation() == Graphics::Orientation::Horizontal) {
						if(IsRight(temp.GetMyAnchor())) {
							prev = &comp;
						}
						else {
							next = &comp;
						}
					}
					else {
						if(IsBottom(temp.GetMyAnchor())) {
							prev = &comp;
						}
						else {
							next = &comp;
						}
					}
				}
            }
            else {
                //todo
            }
		}

	}

	void ComponentStack::Render() {
		if(updaterequired)
			update();


		Gorgon::Layer::Render();
	}
    
    
} }
