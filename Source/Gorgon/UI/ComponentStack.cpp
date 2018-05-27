#include "ComponentStack.h"

#include "../Graphics/Font.h"
#include "../Time.h"


namespace Gorgon { namespace UI {

    ComponentStack::ComponentStack(const Template& temp, Geometry::Size size) : temp(temp), size(size) {
        int maxindex = 0;

		value[0] = value[1] = value[2] = value[3] = 0;
        
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
            auto c = ComponentCondition::Hover;
            
            if(this->temp.GetConditionDuration(ComponentCondition::Normal__Hover))
                c = ComponentCondition::Normal__Hover;
            
            if(IsDisabled()) {
                disabled.insert(c);
            }
            else {
                AddCondition(c);
            }
        });
        
        mouse.SetOut([this]{
            RemoveCondition(ComponentCondition::Hover);
            disabled.erase(ComponentCondition::Hover);
            
            RemoveCondition(ComponentCondition::Normal__Hover);
            disabled.erase(ComponentCondition::Normal__Hover);
            
            if(!IsDisabled() && this->temp.GetConditionDuration(ComponentCondition::Hover__Normal)) {
                AddCondition(ComponentCondition::Hover__Normal);
            }
        });
        
        mouse.SetDown([this](Input::Mouse::Button btn) {
            if(btn && mousebuttonaccepted) {
                auto c = ComponentCondition::Down;
                
                if(this->temp.GetConditionDuration(ComponentCondition::Normal__Down))
                    c = ComponentCondition::Normal__Down;
            
                if(IsDisabled()) {
                    disabled.insert(c);
                }
                else {
                    AddCondition(c);
                }
            }
        });
        
        mouse.SetUp([this](Input::Mouse::Button btn) {
            if(btn && mousebuttonaccepted) {
                RemoveCondition(ComponentCondition::Down);
                disabled.erase(ComponentCondition::Down);
            }
            
            if(!IsDisabled() && this->temp.GetConditionDuration(ComponentCondition::Down__Normal)) {
                AddCondition(ComponentCondition::Down__Normal);
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
		
		if(&get(ind).GetTemplate() != &temp) {
			int si = stacksizes[ind];
        
			if(si == stackcapacity) {
				grow();
			}
        
			new (data + (ind + si*indices)) Component(temp);
			stacksizes[ind]++;
		}

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
		
		//handle repeat storage
		if(temp.GetRepeatMode() != temp.NoRepeat && !repeated.count(&temp)) {
			repeated.insert({&temp, {}});
		}
		else if(temp.GetRepeatMode() == temp.NoRepeat && repeated.count(&temp)) {
			//this is for future where a component template will have dynamic effect on stack
			repeated.erase(&temp);
		}
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
        
        if(IsTransition(condition)) {
            conditionstart[(int)condition] = Time::FrameStart();
        }
        
        if(UI::IsDisabled(condition) && mouse.HasParent()) {
            for(auto iter = conditions.begin(); iter != conditions.end();) {
                auto c = *iter;
                if(IsMouseRelated(c)) {
                    disabled.insert(c);
                    iter = conditions.erase(iter);
                    RemoveCondition(c, false);
                }
                else
                    ++iter;
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
    
    void ComponentStack::RemoveCondition(ComponentCondition condition, bool check){ 
        if(check && !conditions.count(condition)) return;
        
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
       
        if(UI::IsDisabled(condition) && mouse.HasParent()) {
            for(auto d : disabled)
                AddCondition(d);
            
            disabled.clear();
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
    
	void ComponentStack::SetValue(float first, float second, float third, float fourth) {
		value = {{first, second, third, fourth}};
        
		bool updatereq = false;

		for(int i=0; i<indices; i++) {
			if(stacksizes[i] > 0) {
				const ComponentTemplate &temp = get(i).GetTemplate();

				if(temp.GetValueModification() != temp.NoModification) {
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

		//update repeat counts
		for(auto &r : repeated) {
			if(repeats.count(r.first->GetRepeatMode()))
				r.second.resize(repeats[r.first->GetRepeatMode()].size(), Component(*r.first));
		}
        
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
		render(get(0), base, {0,0});
	}

	void ComponentStack::render(Component &comp, Graphics::Layer &parent, Geometry::Point offset, Graphics::RGBAf color) {
		const ComponentTemplate &temp = comp.GetTemplate();

        Graphics::Layer *target = nullptr;
        auto &st = *storage[&temp];
        
        if(st.layer) {
            target = st.layer;
            parent.Add(*target);
            target->Resize(comp.size);
            target->Move(comp.location);
            offset -= comp.location;
        }
        else {
            target = &parent;
        }

		if(temp.GetValueModification() == ComponentTemplate::ModifyColor) {
			if(NumberOfSetBits(temp.GetValueSource()) == 1) {
				color *= Graphics::RGBAf(calculatevalue(0, comp));
			}
			else if(NumberOfSetBits(temp.GetValueSource()) == 2) {
				color *= Graphics::RGBAf(calculatevalue(0, comp), calculatevalue(1, comp));
			}
			else if(NumberOfSetBits(temp.GetValueSource()) == 3) {
				color *= Graphics::RGBAf(calculatevalue(0, comp), calculatevalue(1, comp), calculatevalue(2, comp), 1.f);
			}
			else
				color *= Graphics::RGBAf(calculatevalue(0, comp), calculatevalue(1, comp), calculatevalue(2, comp), calculatevalue(3, comp));
		}
		else if(temp.GetValueModification() == ComponentTemplate::ModifyAlpha)
			color *= Graphics::RGBAf(1.f, calculatevalue(0, comp));

		if(temp.GetType() == ComponentType::Container) {
            const auto &cont = dynamic_cast<const ContainerTemplate&>(temp);
            
            offset += cont.GetBorderSize().TopLeft();
			offset += comp.location;

            if(st.primary && target) {
                auto rectangular = dynamic_cast<const Graphics::RectangularDrawable*>(st.primary);
                if(rectangular)
                    rectangular->DrawIn(*target, comp.location+offset-cont.GetBorderSize().TopLeft(), comp.size, color);
                else
                    st.primary->Draw(*target, comp.location+offset-cont.GetBorderSize().TopLeft(), color);
            }
            
            for(int i=0; i<cont.GetCount(); i++) {
                if(cont[i] >= indices) continue;
                if(stacksizes[cont[i]]) {
					auto &compparent = get(cont[i]);
					auto &temp       = compparent.GetTemplate();
					if(temp.GetRepeatMode() == ComponentTemplate::NoRepeat) {
						render(compparent, target ? *target : parent, offset, color);
					}
					else if(repeats.count(temp.GetRepeatMode())) {
						for(auto &r : repeated[&temp])
							render(r, target ? *target : parent, offset, color);
					}
				}
            }
            
            if(st.secondary && target) {
                auto rectangular = dynamic_cast<const Graphics::RectangularDrawable*>(st.secondary);
                if(rectangular)
                    rectangular->DrawIn(*target, comp.location+offset-cont.GetOverlayExtent().TopLeft(), comp.size, color);
                else
                    st.secondary->Draw(*target, comp.location+offset-cont.GetOverlayExtent().TopLeft(), color);
            }
            
            offset -= cont.GetBorderSize().TopLeft();
		}
		else if(temp.GetType() == ComponentType::Graphics) {
            if(st.primary && target) {
                auto rectangular = dynamic_cast<const Graphics::RectangularDrawable*>(st.primary);
                if(rectangular)
                    rectangular->DrawIn(*target, comp.location+offset, comp.size, color);
                else
                    st.primary->Draw(*target, comp.location+offset, color);
            }
        }
        else if(temp.GetType() == ComponentType::Textholder) {
            const auto &th = dynamic_cast<const TextholderTemplate&>(temp);
            
			target->SetTintColor(color);
            if(th.IsReady()) {
				if(stringdata.count(temp.GetDataEffect())) {
					th.GetRenderer().Print(*target, stringdata[temp.GetDataEffect()], comp.location+offset, comp.size.Width);
				}
            }
			target->SetTintColor(1.f);
		}
        else if(temp.GetType() == ComponentType::Placeholder && comp.size.Area() > 0) {
            const auto &ph = dynamic_cast<const PlaceholderTemplate&>(temp);
            
            if(imagedata.Exists(ph.GetDataEffect())) {
                auto rectangular = dynamic_cast<const Graphics::RectangularDrawable*>(&imagedata[ph.GetDataEffect()]);
                if(rectangular) {
                    rectangular->DrawIn(*target, comp.location+offset, comp.size, color);
                }
                else {
                    imagedata[ph.GetDataEffect()].Draw(*target, comp.location+offset, color);
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
            (ca == Anchor::FirstBaselineLeft || ca == Anchor::FirstBaselineRight || ca == Anchor::LastBaselineLeft || ca == Anchor::LastBaselineRight)
        ) {
            const auto &th = dynamic_cast<const TextholderTemplate&>(temp);
            
            if(th.IsReady()) {
                
                switch(ca) {
                case Anchor::FirstBaselineLeft:
                    cp = {-offset.X, int(-offset.Y-th.GetRenderer().GetGlyphRenderer().GetBaseLine())};
                    break;
                    
                case Anchor::FirstBaselineRight:
                    cp = {-offset.X + csize.Width, int(-offset.Y-th.GetRenderer().GetGlyphRenderer().GetBaseLine())};
                    break;
                    
                case Anchor::LastBaselineLeft:
                    cp = {-offset.X, int(offset.Y+th.GetRenderer().GetGlyphRenderer().GetBaseLine()-th.GetRenderer().GetGlyphRenderer().GetHeight()-csize.Height)};
                    break;
                    
                case Anchor::LastBaselineRight:
                    cp = {-offset.X + csize.Width, int(offset.Y+th.GetRenderer().GetGlyphRenderer().GetBaseLine()-th.GetRenderer().GetGlyphRenderer().GetHeight()-csize.Height)};
                    break;
                default: ;//to silence warnings
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
            (ca == Anchor::FirstBaselineLeft || ca == Anchor::FirstBaselineRight || ca == Anchor::LastBaselineLeft || ca == Anchor::LastBaselineRight)
        ) {
            const auto &th = dynamic_cast<const TextholderTemplate&>(other.GetTemplate());
            
            if(th.IsReady()) {
                
                switch(ca) {
                case Anchor::FirstBaselineLeft:
                    pp = {-margin.Right, int(-margin.Bottom+th.GetRenderer().GetGlyphRenderer().GetBaseLine())};
                    break;
                    
                case Anchor::FirstBaselineRight:
                    pp = {margin.Left + asize.Width, int(-margin.Bottom+th.GetRenderer().GetGlyphRenderer().GetBaseLine())};
                    break;
                    
                case Anchor::LastBaselineLeft:
                    cp = {-margin.Right, int(-margin.Bottom-th.GetRenderer().GetGlyphRenderer().GetBaseLine()+th.GetRenderer().GetGlyphRenderer().GetHeight()+asize.Height)};
                    break;
                    
                case Anchor::LastBaselineRight:
                    cp = {margin.Left + asize.Width, int(-margin.Bottom-th.GetRenderer().GetGlyphRenderer().GetBaseLine()+th.GetRenderer().GetGlyphRenderer().GetHeight()+asize.Height)};
                    break;
                default: ;//to silence warnings
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
            (ca == Anchor::FirstBaselineLeft || ca == Anchor::FirstBaselineRight || ca == Anchor::LastBaselineLeft || ca == Anchor::LastBaselineRight)
        ) {
            const auto &th = dynamic_cast<const TextholderTemplate&>(temp);
            
            if(th.IsReady()) {
                
                switch(ca) {
                case Anchor::FirstBaselineLeft:
                    cp = {-offset.X, int(-offset.Y+th.GetRenderer().GetGlyphRenderer().GetBaseLine())};
                    break;
                    
                case Anchor::FirstBaselineRight:
                    cp = {-offset.X + csize.Width, int(-offset.Y+th.GetRenderer().GetGlyphRenderer().GetBaseLine())};
                    break;
                    
                case Anchor::LastBaselineLeft:
                    cp = {-offset.X, int(offset.Y+th.GetRenderer().GetGlyphRenderer().GetBaseLine()-th.GetRenderer().GetGlyphRenderer().GetHeight()-csize.Height)};
                    break;
                    
                case Anchor::LastBaselineRight:
                    cp = {-offset.X + csize.Width, int(offset.Y+th.GetRenderer().GetGlyphRenderer().GetBaseLine()-th.GetRenderer().GetGlyphRenderer().GetHeight()-csize.Height)};
                    break;
                default: ;//to silence warnings
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
	
	float ComponentStack::calculatevalue(const std::array<float, 4> &value, int channel, const Component &comp) const {
		const auto &temp = comp.GetTemplate();

		int vs = temp.GetValueSource();
		ComponentTemplate::ValueSource src = (ComponentTemplate::ValueSource)(1<<channel);

		int c = channel;
		int i=0;
		while(c >= 0 && i <= ComponentTemplate::ValueSourceMaxPower) {
			if(vs & (1<<i)) {
				if(!c) {
					src = (ComponentTemplate::ValueSource)(1<<i);
				}

				c--;
			}

			i++;
		}

		float v = 0;
        
        const auto &valueordering = temp.GetValueOrdering();


		switch(src) {
			case ComponentTemplate::UseFirst:
				v = value[valueordering[0]];
				break;

			case ComponentTemplate::UseSecond:
				v = value[valueordering[1]];
				break;

			case ComponentTemplate::UseThird:
				v = value[valueordering[2]];
				break;

			case ComponentTemplate::UseFourth:
				v = value[valueordering[3]];
				break;

			case ComponentTemplate::UseGray:
				v = value[valueordering[0]] * 0.2126f + value[valueordering[1]] * 0.7152f + value[valueordering[2]] * 0.0722f;
				break;
                
            //missing: L C H
            
            default: ;//to silence warnings
		}

		return v * temp.GetValueRange(channel) + temp.GetValueMin(channel);
	}

	void ComponentStack::checkrepeatupdate(ComponentTemplate::RepeatMode mode) {
		bool updatereq = false;

		for(int i=0; i<indices; i++) {
			if(stacksizes[i] > 0) {
				const ComponentTemplate &temp = get(i).GetTemplate();

				if(temp.GetRepeatMode() == mode) {
					updatereq = true;
				}
			}
		}

		if(updatereq)
			Update();
	}

	//location depends on the container location
	void ComponentStack::update(Component &parent) {
		const ComponentTemplate &ctemp = parent.GetTemplate();

		if(ctemp.GetType() != ComponentType::Container) return;

		const ContainerTemplate &cont = dynamic_cast<const ContainerTemplate&>(ctemp);
        
        parent.innersize = parent.size - cont.GetBorderSize();
        
        if(cont.GetSizing() == cont.Fixed) {
            if(parent.innersize.Width <= 0) return;
            if(parent.innersize.Height <= 0) return;
        }

        bool requiresrepass = false;
        bool repassdone = false;
        
        int spaceleft = 0;

realign:
        // first pass for size, second pass will cover the sizes that are percent based.
		for(int i=0; i<cont.GetCount(); i++) {
            
			int ci = cont[i];

            if(ci >= indices) continue;
			if(!stacksizes[ci]) continue;


            auto &compparent = get(cont[i]);
            
            const auto &temp = compparent.GetTemplate();
            
			
			for(int j = 0; temp.GetRepeatMode() == temp.NoRepeat ? j == 0 : repeats.count(temp.GetRepeatMode()) && j < repeats[temp.GetRepeatMode()].size(); j++) {
				Component *compptr;
				const std::array<float, 4> *val;

				if(temp.GetRepeatMode() == temp.NoRepeat) {
					compptr = &compparent;
					val     = &value;
				}
				else {
					compptr = &repeated[&temp][j];
					val     = &repeats[temp.GetRepeatMode()][j];
				}

				auto &comp = *compptr;

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
            
				auto size = temp.GetSize();
            
            
				if(temp.GetValueModification() == temp.ModifySize) {
					if(NumberOfSetBits(temp.GetValueSource()) == 1) {
						if(cont.GetOrientation() == Graphics::Orientation::Horizontal) {
							size = {{int(calculatevalue(*val, 0, comp)*10000), Dimension::BasisPoint}, size.Height};
						}
						else {
							size = {size.Width, {int(calculatevalue(*val, 0, comp)*10000), Dimension::BasisPoint}};
						}
					}
					else {
						size ={{int(calculatevalue(*val, 0, comp)*10000), Dimension::BasisPoint}, {int(calculatevalue(*val, 1, comp)*10000), Dimension::BasisPoint}};
					}
				}
            
				comp.size = Convert(size, maxsize, emsize);
            
				if(
					(cont.GetOrientation() == Graphics::Orientation::Horizontal && 
						(size.Width.GetUnit() == Dimension::Percent || size.Width.GetUnit() == Dimension::BasisPoint)) ||
					(cont.GetOrientation() == Graphics::Orientation::Vertical && 
						(size.Height.GetUnit() == Dimension::Percent || size.Height.GetUnit() == Dimension::BasisPoint))
				)
					requiresrepass = true;
                
				if(temp.GetSizing() != temp.Fixed && 
				   !(temp.GetValueModification() == temp.ModifySize &&  NumberOfSetBits(temp.GetValueSource()) > 1)
				) {
					auto &st = *storage[&temp];

					auto orgsize = comp.size;

					if(temp.GetType() == ComponentType::Container) {
						comp.size = {0, 0};
						update(comp);
					}
					else if(temp.GetType() == ComponentType::Graphics) {
						if(st.primary) {
							auto rectangular = dynamic_cast<const Graphics::RectangularDrawable*>(st.primary);
							if(rectangular)
								comp.size = rectangular->GetSize();
						}
						else {
							comp.size = {0, 0};
						}
					}
					else if(temp.GetType() == ComponentType::Textholder) {
						const auto &th = dynamic_cast<const TextholderTemplate&>(temp);

						if(th.IsReady() && stringdata[temp.GetDataEffect()] != "") {
							auto s = size.Width(maxsize.Width, emsize);

							if(s)
								comp.size = th.GetRenderer().GetSize(stringdata[temp.GetDataEffect()], s);
							else
								comp.size = th.GetRenderer().GetSize(stringdata[temp.GetDataEffect()]);

						}
					}
					else if(temp.GetType() == ComponentType::Placeholder) {
						const auto &ph = dynamic_cast<const PlaceholderTemplate&>(temp);

						if(imagedata.Exists(ph.GetDataEffect())) {
							auto rectangular = dynamic_cast<const Graphics::RectangularDrawable*>(&imagedata[ph.GetDataEffect()]);
							if(rectangular) {
								comp.size = rectangular->GetSize();
							}
						}
						else {
							comp.size = {0, 0};
						}
					}

					if(temp.GetSizing() == ComponentTemplate::GrowOnly) {
						if(comp.size.Width < orgsize.Width)
							comp.size.Width = orgsize.Width;

						if(comp.size.Height < orgsize.Height)
							comp.size.Height = orgsize.Height;
					}
					else if(temp.GetSizing() == ComponentTemplate::ShrinkOnly) {
						if(comp.size.Width > orgsize.Width)
							comp.size.Width = orgsize.Width;

						if(comp.size.Height > orgsize.Height)
							comp.size.Height = orgsize.Height;
					}
				
					if(temp.GetValueModification() == temp.ModifySize) {
						if(cont.GetOrientation() == Graphics::Orientation::Horizontal)
							comp.size.Width = orgsize.Width;
						else
							comp.size.Height = orgsize.Height;
					}

					if(
						(cont.GetOrientation() == Graphics::Orientation::Horizontal &&
						(size.Width.GetUnit() == Dimension::Percent || size.Width.GetUnit() == Dimension::BasisPoint)) ||
						 (cont.GetOrientation() == Graphics::Orientation::Vertical &&
						 (size.Height.GetUnit() == Dimension::Percent || size.Height.GetUnit() == Dimension::BasisPoint))
						)
					{
						if(maxsize.Width == 0)
							comp.size.Width = 0;
						if(maxsize.Height == 0)
							comp.size.Height = 0;
					}

					if(comp.size.Width < 0)
						comp.size.Width = 0;
					if(comp.size.Height < 0)
						comp.size.Height = 0;
				}
			}
        }

		Component *prev = nullptr, *next = nullptr;

        //second pass will align everything
		for(int i=0; i<cont.GetCount(); i++) {
            
			int ci = cont[i];

            if(ci >= indices) continue;
			if(!stacksizes[ci]) continue;


            auto &compparent = get(cont[i]);
            
            const auto &temp = compparent.GetTemplate();

			for(int j = 0; temp.GetRepeatMode() == temp.NoRepeat ? j == 0 : repeats.count(temp.GetRepeatMode()) && j < repeats[temp.GetRepeatMode()].size(); j++) {
				Component *compptr;
				const std::array<float, 4> *val;

				if(temp.GetRepeatMode() == temp.NoRepeat) {
					compptr = &compparent;
					val     = &value;
				}
				else {
					compptr = &repeated[&temp][j];
					val     = &repeats[temp.GetRepeatMode()][j];
				}

				auto &comp = *compptr;

				//check if textholder and if so use emsize from the font
				int emsize = getemsize(comp);
            
				//check anchor object by observing temp.GetPreviousAnchor and direction
				Component *anch = nullptr;
            
            
				//if absolute, nothing to anchor to but to parent
				if(temp.GetPositioning() == temp.Relative && temp.GetPreviousAnchor() != Anchor::None) {
					if(cont.GetOrientation() == Graphics::Orientation::Horizontal) {
						if((IsLeft(temp.GetPreviousAnchor()) && IsRight(temp.GetMyAnchor())) || 
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
						if((IsTop(temp.GetPreviousAnchor()) && IsBottom(temp.GetMyAnchor())) || 
							(temp.GetPreviousAnchor() == Anchor::None && IsBottom(temp.GetContainerAnchor()))) 
						{
							anch = prev;
							comp.anchorotherside = true;
						}
						else {
							anch = next;
						}
					}
				}
            
				auto parentmargin = Convert(temp.GetMargin(), parent.innersize, emsize).CombinePadding(Convert(cont.GetPadding(), parent.size, emsize)) + Convert(temp.GetIndent(), parent.innersize, emsize);
            
				Geometry::Margin margin;
            
				if(anch) {
					margin = Convert(temp.GetMargin(), parent.innersize, emsize).CombineMargins(Convert(anch->GetTemplate().GetMargin(), parent.innersize, emsize));
				}
				else {
					margin = parentmargin;
				}
            
				auto maxsize = parent.innersize - parentmargin;
            
				auto pos = temp.GetPosition();
            
				if(temp.GetValueModification() == temp.ModifyPosition) {
					if(NumberOfSetBits(temp.GetValueSource()) == 1) {
						if(cont.GetOrientation() == Graphics::Orientation::Horizontal) {
							pos = {{int(calculatevalue(*val, 0, comp)*10000), Dimension::BasisPoint}, pos.Y};
						}
						else {
							pos = {pos.X, {int(calculatevalue(*val, 0, comp)*10000), Dimension::BasisPoint}};
						}
					}
					else {
						pos ={{int(calculatevalue(*val, 0, comp)*10000), Dimension::BasisPoint}, {int(calculatevalue(*val, 1, comp)*10000), Dimension::BasisPoint}};
					}
				}
				else if(temp.GetValueModification() == temp.ModifyX) {
					pos = {{int(calculatevalue(*val, 0, comp)*10000), Dimension::BasisPoint}, pos.Y};
				}
				else if(temp.GetValueModification() == temp.ModifyY) {
					pos = {pos.X, {int(calculatevalue(*val, 0, comp)*10000), Dimension::BasisPoint}};
				}
            
				if(temp.GetPositioning() == temp.PolarAbsolute) {
					auto pcenter = Geometry::Pointf(cont.GetCenter().X.CalculateFloat((float)maxsize.Width, (float)emsize), cont.GetCenter().Y.CalculateFloat((float)maxsize.Height, (float)emsize));
					auto center  = Geometry::Pointf(temp.GetCenter().X.CalculateFloat((float)comp.size.Width, (float)emsize), temp.GetCenter().Y.CalculateFloat((float)comp.size.Height, (float)emsize));

					auto r = pos.X.CalculateFloat(Geometry::Point(maxsize).Distance()/(float)sqrt(2), (float)emsize);

					auto a = pos.Y.CalculateFloat(360, PI);

					a *= PI / 180.0f;

					comp.location = {int(std::round(r * cos(a) + pcenter.X - center.X)), int(std::round(r * sin(a) + pcenter.Y - center.Y))};
				}
				else {
					auto offset = Convert(pos, maxsize-comp.size, emsize);
            
					if(anch) {
						anchortoother(comp, temp, offset, margin, *anch, cont.GetOrientation());
					}
					else {
						anchortoparent(comp, temp, offset, margin, parent.innersize);
					}
				}

				//Which anchor side is to be changed
				if(temp.GetPositioning() == temp.Relative) {
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
                        else if(comp.size.Width) {
                            leftused = (   
                                comp.location.X + comp.size.Width + 
                                std::max(temp.GetMargin().Right(parent.innersize.Width, emsize), cont.GetPadding().Right(parent.size.Width, emsize))
                                
                            );
                        }
                    }
                }
                
                spaceleft = parent.innersize.Width - rightused - leftused;
            }
            else {
                int bottomused = 0, topused = 0;
                
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
                            bottomused = parent.innersize.Height - comp.location.X;
                        }
                        else if(comp.size.Height) {
                            topused = (   
                                comp.location.X + comp.size.Height + 
                                std::max(temp.GetMargin().Right(parent.innersize.Height, emsize), cont.GetPadding().Right(parent.size.Height, emsize))
                                
                            );
                        }
                    }
                }
                
                spaceleft = parent.innersize.Height - bottomused - topused;
            }

            repassdone = true;
            goto realign;
        }


		for(int i=0; i<cont.GetCount(); i++) {

			int ci = cont[i];

			if(ci >= indices) continue;
			if(!stacksizes[ci]) continue;


			auto &comp = get(cont[i]);

			const auto &temp = comp.GetTemplate();

			if(temp.GetType() == ComponentType::Container) {
				update(comp);
			}
		}
	}

	void ComponentStack::Render() {        
        for(auto iter = conditions.begin(); iter != conditions.end();) {
            auto c = *iter;
            if(IsTransition(c) && (unsigned)temp.GetConditionDuration(c) < Time::FrameStart()-conditionstart[(int)c]) {
                iter = conditions.erase(iter);
                RemoveCondition(c, false);
                auto nc = TransitionEnd(c);
                if(nc != ComponentCondition::Always)
                    AddCondition(nc);
            }
            else
                ++iter;
        }
        
		if(updaterequired)
			update();


		Gorgon::Layer::Render();
	}
	
	void ComponentStack::HandleMouse(Input::Mouse::Button accepted) {
        Add(mouse);
        mousebuttonaccepted=accepted;
    }
    
} }
