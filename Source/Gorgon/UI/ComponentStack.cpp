#include "ComponentStack.h"

#include "../Graphics/Font.h"


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
        
        mouse.SetOver([this]{        
            if(conditions.count(ComponentCondition::Disabled) && mouse.HasParent()) {
                disabled.insert(ComponentCondition::Hover);
            }
            else {
                AddCondition(ComponentCondition::Hover);
            }
        });
        
        mouse.SetOut([this]{
            RemoveCondition(ComponentCondition::Hover);
            disabled.erase(ComponentCondition::Hover);
        });
        
        mouse.SetDown([this](Input::Mouse::Button btn) {
            if(btn && mousebuttonaccepted) {
                if(conditions.count(ComponentCondition::Disabled) && mouse.HasParent()) {
                    disabled.insert(ComponentCondition::Down);
                }
                else {
                    AddCondition(ComponentCondition::Down);
                }
            }
        });
        
        mouse.SetUp([this](Input::Mouse::Button btn) {
            if(btn && mousebuttonaccepted) {
                RemoveCondition(ComponentCondition::Down);
                disabled.erase(ComponentCondition::Down);
            }
        });
        
        Resize(size);
        
        for(int i=0; i<temp.GetCount(); i++) {
            if(temp[i].GetType() == ComponentType::Textholder) {
                const auto &th = dynamic_cast<const TextholderTemplate&>(temp[i]);
                
                if(th.IsReady()) {
                    emsize = th.GetRenderer().GetEMSize();
                    break;
                }
            }
        }
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
			else if(temp.GetType() == ComponentType::Graphics) {
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
        
        auto ndata = (Component*)realloc(data, sizeof(Component) * indices * stackcapacity);
        
        if(ndata)
            data = ndata;
        else {
            free(data);
            
            throw std::bad_alloc();
        }
    }
    
    void ComponentStack::AddCondition(ComponentCondition condition) {
        if(conditions.count(condition)) return;
        
        conditions.insert(condition);
        
        if(condition == ComponentCondition::Disabled && mouse.HasParent()) {
            if(conditions.count(ComponentCondition::Hover)) {
                disabled.insert(ComponentCondition::Hover);
                RemoveCondition(ComponentCondition::Hover);
            }
            
            if(conditions.count(ComponentCondition::Down)) {
                disabled.insert(ComponentCondition::Down);
                RemoveCondition(ComponentCondition::Down);
            }
        }
        
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
       
        if(condition == ComponentCondition::Disabled && mouse.HasParent()) {
            if(disabled.count(ComponentCondition::Hover)) {
                AddCondition(ComponentCondition::Hover);
                disabled.erase(ComponentCondition::Hover);
            }
            
            if(disabled.count(ComponentCondition::Down)) {
                AddCondition(ComponentCondition::Down);
                disabled.erase(ComponentCondition::Down);
            }
        }
        
        if(updatereq)
            Update();
    }
    
	void ComponentStack::SetData(ComponentTemplate::DataEffect effect, const Graphics::Drawable &image) {
        imagedata.Add(effect, image);
        
		bool updatereq = false;

		for(int i=0; i<indices; i++) {
			if(stacksizes[i] > 0) {
				const ComponentTemplate &temp = get(i).GetTemplate();

				if(temp.GetDataEffect() == effect) {
					updatereq = true;
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
        
        //calculate common emsize        
		for(int i=0; i<indices; i++) {
			if(stacksizes[i] > 0) {
                if(get(i).GetTemplate().GetType() == ComponentType::Textholder) {
                    const auto &th = dynamic_cast<const TextholderTemplate&>(get(i).GetTemplate());
                    
                    if(th.IsReady()) {
                        emsize = th.GetRenderer().GetEMSize();
                        break;
                    }
                }
            }
        }
        
		update(get(0));

		updaterequired = false;

        base.Clear();
        
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
                if(cont[i] >= indices) continue;
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
        else if(temp.GetType() == ComponentType::Textholder) {
            const auto &th = dynamic_cast<const TextholderTemplate&>(temp);
            
            if(th.IsReady() && stringdata[temp.GetDataEffect()] != "") {
                th.GetRenderer().Print(*target, stringdata[temp.GetDataEffect()], comp.location, comp.size.Width);
            }
        }
        else if(temp.GetType() == ComponentType::Placeholder) {
            const auto &ph = dynamic_cast<const PlaceholderTemplate&>(temp);
            
            if(imagedata.Exists(ph.GetDataEffect())) {
                auto rectangular = dynamic_cast<const Graphics::RectangularDrawable*>(&imagedata[ph.GetDataEffect()]);
                if(rectangular) {
                    rectangular->DrawIn(*target, comp.location, comp.size);
                }
                else {
                    imagedata[ph.GetDataEffect()].Draw(*target, comp.location);
                }
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
				pp = {margin.Left - margin.Right + maxsize.Width / 2, margin.Top};
				break;

			case Anchor::TopRight:
				pp = { -margin.Right + maxsize.Width, margin.Top};
				break;


			case Anchor::MiddleLeft:
			case Anchor::FirstBaselineLeft:
				pp = {margin.Left, margin.Top - margin.Bottom + maxsize.Height / 2};
				break;

			case Anchor::MiddleCenter:
				pp = {margin.Left - margin.Right + maxsize.Width / 2, margin.Top - margin.Bottom + maxsize.Height / 2};
				break;

			case Anchor::MiddleRight:
			case Anchor::FirstBaselineRight:
				pp = { -margin.Right + maxsize.Width, margin.Top - margin.Bottom + maxsize.Height / 2};
				break;

        
			case Anchor::BottomLeft:
            case Anchor::LastBaselineLeft:
				pp = {margin.Left, -margin.Bottom + maxsize.Height};
				break;

			case Anchor::BottomCenter:
				pp = {margin.Left - margin.Right + maxsize.Width / 2, -margin.Bottom + maxsize.Height};
				break;

			case Anchor::BottomRight:
            case Anchor::LastBaselineRight:
				pp = { -margin.Right + maxsize.Width, -margin.Bottom + maxsize.Height};
				break;

        }
        
        auto csize = comp.size;
        
        if(temp.GetType() == ComponentType::Textholder && 
            (ca == Anchor::FirstBaselineLeft || ca == Anchor::FirstBaselineRight)
        ) {
            const auto &th = dynamic_cast<const TextholderTemplate&>(temp);
            
            if(th.IsReady()) {
                
                switch(ca) {
                case Anchor::FirstBaselineLeft:
                    cp = {-offset.X, -offset.Y-th.GetRenderer().GetGlyphRenderer().GetBaseLine()};
                    break;
                    
                case Anchor::FirstBaselineRight:
                    cp = {-offset.X + csize.Width, -offset.Y-th.GetRenderer().GetGlyphRenderer().GetBaseLine()};
                    break;
                }
                
                ca = Anchor::None;
            }
        }
        
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

    bool IsIn(Anchor left, Anchor right) {
        bool ret = true;
        
        if(IsLeft(left) == IsRight(right) && !IsCenter(left) && !IsCenter(right))
            ret = false;
        
        if(IsTop(left) == IsBottom(right) && !IsMiddle(left) && !IsMiddle(right))
            ret = false;
        
        return ret;
    }
    
	void anchortoother(Component &comp, const ComponentTemplate &temp, 
                        Geometry::Point offset, Geometry::Margin margin, Component &other, Graphics::Orientation orientation) {
        
        Anchor pa = temp.GetPreviousAnchor();
        Anchor ca = temp.GetMyAnchor();
        
        Geometry::Point pp, cp;
        
        auto asize = other.size;
        
        if(IsIn(pa, ca))
            margin = 0;
        
        if(orientation == Graphics::Orientation::Horizontal) 
            margin.Top = margin.Bottom = 0;
        else
            margin.Left = margin.Right = 0;
        
        
        if(other.GetTemplate().GetType() == ComponentType::Textholder && 
            (ca == Anchor::FirstBaselineLeft || ca == Anchor::FirstBaselineRight)
        ) {
            const auto &th = dynamic_cast<const TextholderTemplate&>(other.GetTemplate());
            
            if(th.IsReady()) {
                
                switch(ca) {
                case Anchor::FirstBaselineLeft:
                    pp = {-margin.Right, -margin.Bottom+th.GetRenderer().GetGlyphRenderer().GetBaseLine()};
                    break;
                    
                case Anchor::FirstBaselineRight:
                    pp = {margin.Left + asize.Width, - margin.Bottom+th.GetRenderer().GetGlyphRenderer().GetBaseLine()};
                    break;
                }
                
                pa = Anchor::None;
            }
        }
        
        switch(pa) {
            case Anchor::None: //do nothing
                break;
                
            default:
			case Anchor::TopLeft:
				pp = {-margin.Right,- margin.Bottom};
				break;

			case Anchor::TopCenter:
				pp = {asize.Width / 2, -margin.Bottom};
				break;

			case Anchor::TopRight:
				pp = {margin.Left + asize.Width, -margin.Bottom};
				break;


			case Anchor::MiddleLeft:
			case Anchor::FirstBaselineLeft:
				pp = {-margin.Right, asize.Height / 2};
				break;

			case Anchor::MiddleCenter:
				pp = {asize.Width / 2, asize.Height / 2};
				break;

			case Anchor::MiddleRight:
			case Anchor::FirstBaselineRight:
				pp = {margin.Left + asize.Width, asize.Height / 2};
				break;

        
			case Anchor::BottomLeft:
            case Anchor::LastBaselineLeft:
				pp = {-margin.Right, margin.Top + asize.Height};
				break;

			case Anchor::BottomCenter:
				pp = {asize.Width / 2, margin.Top + asize.Height};
				break;

			case Anchor::BottomRight:
            case Anchor::LastBaselineRight:
				pp = {margin.Left + asize.Width, margin.Top + asize.Height};
				break;

        }
        
        auto csize = comp.size;
       
        if(temp.GetType() == ComponentType::Textholder && 
            (ca == Anchor::FirstBaselineLeft || ca == Anchor::FirstBaselineRight)
        ) {
            const auto &th = dynamic_cast<const TextholderTemplate&>(temp);
            
            if(th.IsReady()) {
                
                switch(ca) {
                case Anchor::FirstBaselineLeft:
                    cp = {-offset.X, -offset.Y+th.GetRenderer().GetGlyphRenderer().GetBaseLine()};
                    break;
                    
                case Anchor::FirstBaselineRight:
                    cp = {-offset.X + csize.Width, -offset.Y+th.GetRenderer().GetGlyphRenderer().GetBaseLine()};
                    break;
                }
                
                ca = Anchor::None;
            }
        }
        
        switch(ca) {
            case Anchor::None: //do nothing
                break;
                
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
        
        comp.location = pp - cp + other.location;
    }
    
    int ComponentStack::getemsize(const Component &comp) {
        if(comp.GetTemplate().GetType() == ComponentType::Textholder) {
            const auto &th = dynamic_cast<const TextholderTemplate&>(comp.GetTemplate());
            
            if(th.IsReady()) {
                return th.GetRenderer().GetEMSize();
            }
        }
        
        return emsize;
    }
	
	//location depends on the container location
	void ComponentStack::update(Component &parent) {
		const ComponentTemplate &ctemp = parent.GetTemplate();

		if(ctemp.GetType() != ComponentType::Container) return;

		const ContainerTemplate &cont = dynamic_cast<const ContainerTemplate&>(ctemp);
        
        parent.innersize = parent.size - cont.GetBorderSize();
        
        if(parent.innersize.Width <= 0) return;
        if(parent.innersize.Height <= 0) return;

        bool requiresrepass = false;
        bool repassdone = false;
        
        int spaceleft = 0;

realign:
        // first pass for size, second pass will cover the sizes that are percent based.
		for(int i=0; i<cont.GetCount(); i++) {
            
			int ci = cont[i];

            if(ci >= indices) continue;
			if(!stacksizes[ci]) continue;


            auto &comp = get(cont[i]);
            
            const auto &temp = comp.GetTemplate();        
            
            //check if textholder and if so use emsize from the font
			int emsize = getemsize(comp);
           
            auto parentmargin = Convert(temp.GetMargin(), parent.innersize, emsize).CombinePadding(Convert(cont.GetPadding(), parent.size, emsize)) + Convert(temp.GetIndent(), parent.innersize, emsize);
            
            auto maxsize = parent.innersize - parentmargin;
            
            if(temp.GetPositioning() != temp.Absolute) {
                if(cont.GetOrientation() == Graphics::Orientation::Horizontal)
                    maxsize.Width = spaceleft;
                else
                    maxsize.Height = spaceleft;
            }
            
            if(temp.GetSizing() == temp.Fixed) {
                comp.size = Convert(temp.GetSize(), maxsize, emsize);
                
                if(
                    (cont.GetOrientation() == Graphics::Orientation::Horizontal && 
                        (temp.GetSize().Width.GetUnit() == Dimension::Percent || temp.GetSize().Width.GetUnit() == Dimension::BasisPoint)) ||
                    (cont.GetOrientation() == Graphics::Orientation::Vertical && 
                        (temp.GetSize().Height.GetUnit() == Dimension::Percent || temp.GetSize().Height.GetUnit() == Dimension::BasisPoint))
                )
                    requiresrepass = true;
                
            }
            else {
                //todo: separate for text, graphics, placeholder and container
            }
        }

		Component *prev = nullptr, *next = nullptr;

        //second pass will align everything
		for(int i=0; i<cont.GetCount(); i++) {
            
			int ci = cont[i];

            if(ci >= indices) continue;
			if(!stacksizes[ci]) continue;


            auto &comp = get(cont[i]);
            
            const auto &temp = comp.GetTemplate();            
            
            //check if textholder and if so use emsize from the font
			int emsize = getemsize(comp);
            
            //check anchor object by observing temp.GetPreviousAnchor and direction
			Component *anch = nullptr;
			if(cont.GetOrientation() == Graphics::Orientation::Horizontal) {
 				if(IsLeft(temp.GetPreviousAnchor()) && IsRight(temp.GetMyAnchor()) || 
                    (temp.GetPreviousAnchor() == Anchor::None && IsRight(temp.GetContainerAnchor()))) 
                {
					anch = prev;
                    comp.anchorotherside = true;
				}
				else {
					anch = next;
				}
			}
			else {
				if(IsTop(temp.GetPreviousAnchor()) && IsBottom(temp.GetMyAnchor()) || 
                    (temp.GetPreviousAnchor() == Anchor::None && IsBottom(temp.GetContainerAnchor()))) 
                {
					anch = prev;
                    comp.anchorotherside = true;
				}
				else {
					anch = next;
				}
			}
            
            //if absolute, nothing to anchor to but to parent
            if(temp.GetPositioning() == temp.Absolute || temp.GetPreviousAnchor() == Anchor::None)
                anch = nullptr;
            
            auto parentmargin = Convert(temp.GetMargin(), parent.innersize, emsize).CombinePadding(Convert(cont.GetPadding(), parent.size, emsize)) + Convert(temp.GetIndent(), parent.innersize, emsize);
            
            Geometry::Margin margin;
            
            if(anch) {
                margin = Convert(temp.GetMargin(), parent.innersize, emsize).CombineMargins(Convert(anch->GetTemplate().GetMargin(), parent.innersize, emsize));
            }
            else {
                margin = parentmargin;
            }
            
            auto maxsize = parent.innersize - parentmargin;
            
            auto offset = Convert(temp.GetPosition(), maxsize, emsize);
            
            if(anch) {
                anchortoother(comp, temp, offset, margin, *anch, cont.GetOrientation());
            }
            else {
                anchortoparent(comp, temp, offset, margin, parent.innersize);
            }

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
            
		}//for indices
		
		if(requiresrepass && !repassdone) {
            if(cont.GetOrientation() == Graphics::Orientation::Horizontal) {
                int rightused = 0, leftused = 0;
                
                for(int i=0; i<cont.GetCount(); i++) {
                    
                    int ci = cont[i];

                    if(ci >= indices) continue;
                    if(!stacksizes[ci]) continue;

                    auto &comp = get(cont[i]);
                    const auto &temp = comp.GetTemplate();
                    
                    //check if textholder and if so use emsize from the font
                    int emsize = getemsize(comp);

                    if(temp.GetPositioning() != temp.Absolute) {
                        if(comp.anchorotherside) {
                            rightused = parent.innersize.Width - comp.location.X;
                        }
                        else {
                            leftused = (   
                                comp.location.X + comp.size.Width + 
                                std::max(temp.GetMargin().Right(parent.innersize.Width, emsize), cont.GetPadding().Right(parent.size.Width, emsize))
                                
                            );
                        }
                    }
                }
                
                spaceleft = parent.innersize.Width - rightused - leftused;
            }

            repassdone = true;
            goto realign;
        }
	}

	void ComponentStack::Render() {
		if(updaterequired)
			update();


		Gorgon::Layer::Render();
	}
	
	void ComponentStack::HandleMouse(Input::Mouse::Button accepted) {
        Add(mouse);
        mousebuttonaccepted=accepted;
    }
    
    
} }
