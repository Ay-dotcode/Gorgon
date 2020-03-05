#include "ComponentStack.h"

#include "../Graphics/Font.h"
#include "../Time.h"
#include "../Config.h"

#include "math.h"

namespace Gorgon { namespace UI {

    ComponentStack::ComponentStack(const Template& temp, Geometry::Size size) : temp(temp), size(size), ConditionChanged(this) {
        int maxindex = 0;
        
        for(int i=0; i<temp.GetCount(); i++) {
            if(maxindex < temp[i].GetIndex())
                maxindex = temp[i].GetIndex();
        }
    
        indices = maxindex + 1;
        
        data = (Component*)malloc(sizeof(Component) * indices * stackcapacity);
        stacksizes.resize(indices);
        
        Add(base);
        Add(mouse);
        
        addcondition(ComponentCondition::None, ComponentCondition::Always);
        
        mouse.SetOver([this]{
            if(handlingmouse) {
                if(IsDisabled()) {
                    disabled.insert(ComponentCondition::Hover);
                }
                else {
                    AddCondition(ComponentCondition::Hover);
                }
            }
                
            if(!IsDisabled() && over_fn)
                over_fn(ComponentTemplate::NoTag);
        });
        
        mouse.SetOut([this] {
            if(handlingmouse) {
                RemoveCondition(ComponentCondition::Hover, !IsDisabled());
                disabled.erase(ComponentCondition::Hover);
            }

            if(!IsDisabled() && out_fn)
                out_fn(ComponentTemplate::NoTag);
        });
        
        mouse.SetDown([this](Geometry::Point location, Input::Mouse::Button btn) {
            if(handlingmouse) {
                if(btn && mousebuttonaccepted) {
                    if(IsDisabled()) {
                        disabled.insert(ComponentCondition::Down);
                    }
                    else {
                        AddCondition(ComponentCondition::Down);
                    }
                }
            }

            if(!IsDisabled() && down_fn)
                down_fn(ComponentTemplate::NoTag, location, btn);
            
            downlocation = location;
        });
        
        mouse.SetUp([this](Geometry::Point location, Input::Mouse::Button btn) {
            if(handlingmouse) {
                if(btn && mousebuttonaccepted) {
                    RemoveCondition(ComponentCondition::Down, !IsDisabled());
                    disabled.erase(ComponentCondition::Down);
                }
            }

            if(!IsDisabled()) {
                if(click_fn && downlocation.Distance(location) < WindowManager::ClickThreshold)
                    click_fn(ComponentTemplate::NoTag, location, btn);
            }
            
            if(!IsDisabled() && up_fn)
                up_fn(ComponentTemplate::NoTag, location, btn);
            
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

        //this is here to simplify the update process, we are making sure that always is
        //at the top of the stack
        for(int i=0; i<temp.GetCount(); i++) {
            const auto &t = temp[i];

            if(t.GetRepeatMode() != ComponentTemplate::NoRepeat && t.GetCondition() != ComponentCondition::Always) {
                AddToStack(temp[i], false);
            }
        }

        for(int i=0; i<temp.GetCount(); i++) {
            const auto &t = temp[i];

            if(t.GetRepeatMode() != ComponentTemplate::NoRepeat && t.GetCondition() == ComponentCondition::Always) {
                AddToStack(temp[i], false);
            }
        }
    }

    void ComponentStack::AddToStack(const ComponentTemplate& temp, bool reversed) {
        int ind = temp.GetIndex();
        
        if(stacksizes[ind] == 0 || &get(ind).GetTemplate() != &temp) {
            int si = stacksizes[ind];
        
            if(si == stackcapacity) {
                grow();
            }
        
            new (data + (ind + si*indices)) Component(temp);
            data[ind + si*indices].reversed = reversed;
            stacksizes[ind]++;
        }
        else {
            get(ind).reversed = reversed;
        }

        if(!storage.count(&temp)) {
            auto storage = new ComponentStorage;
            this->storage[&temp] = storage;
            
            if(temp.GetClip()) {
                storage->layer = new Graphics::Layer;
                storage->layer->EnableClipping();
                storage->layer->Hide();
            }

            if(temp.GetType() == ComponentType::Container) {
                const auto &ctemp = dynamic_cast<const ContainerTemplate&>(temp);

                Animation::ControllerBase *cnt = &controller;
                
                if(ctemp.GetValueModification() == ctemp.ModifyAnimation) {
                    storage->timer = new Animation::ControlledTimer();
                    cnt = storage->timer;
                }
                
                if(ctemp.Background.HasContent()) {
                    storage->primary = &ctemp.Background.Instantiate(*cnt);
                }

                if(ctemp.Overlay.HasContent()) {
                    storage->secondary = &ctemp.Overlay.Instantiate(*cnt);
                }
            }
            else if(temp.GetType() == ComponentType::Placeholder) {
                const auto &ptemp = dynamic_cast<const PlaceholderTemplate&>(temp);

                if(ptemp.HasTemplate()) {
                    auto s = new ComponentStack(ptemp.GetTemplate(), {0, 0});
                    substacks.Add(&temp, s);
                    if(handlingmouse)
                        s->HandleMouse(mousebuttonaccepted);
                }
            }
            else if(temp.GetType() == ComponentType::Graphics) {
                const auto &gtemp = dynamic_cast<const GraphicsTemplate&>(temp);

                Animation::ControllerBase *cnt = &controller;
                
                if(gtemp.GetValueModification() == gtemp.ModifyAnimation) {
                    storage->timer = new Animation::ControlledTimer();
                    cnt = storage->timer;
                }
                
                if(gtemp.Content.HasContent()) {
                    storage->primary = &gtemp.Content.Instantiate(*cnt);
                }
            }
        }

        if(temp.GetType() == ComponentType::Placeholder) {
            const auto &ptemp = dynamic_cast<const PlaceholderTemplate&>(temp);

            if(ptemp.HasTemplate()) {
                base.Add(substacks[&temp]);
            }
        }
        
        controller.Reset();
        
        //handle repeat storage
        if(temp.GetRepeatMode() != temp.NoRepeat && !repeated.count(&temp) && temp.GetCondition() == ComponentCondition::Always) {
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
    
    bool ComponentStack::addcondition(ComponentCondition from, ComponentCondition to, ComponentCondition hint) {        
        if(IsMouseRelated(to) && IsDisabled()) {
            disabled.insert(to);
            return false;
        }
        
        if(from != ComponentCondition::None && temp.GetConditionDuration(from, to) == 0) {
            hint = from;
            from = ComponentCondition::None;
        }
        
        if(from != ComponentCondition::None) {
            if(transitions.count({from, to}))
                return false;
            
            transitions[{from, to}] = Time::FrameStart();
        }
        else {
            if(conditions.count(to))
                return false;
            
            conditions.insert(to);
        }
        
        if(to == ComponentCondition::Disabled && handlingmouse) {
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
        
        //no transition
        if(from == ComponentCondition::None) {
            //direct search: perfect fit
            std::set<int> indicesdone;
            for(int i=0; i<temp.GetCount(); i++) {
                if(temp[i].GetRepeatMode() != ComponentTemplate::NoRepeat)
                    continue;

                if(temp[i].GetCondition() == to && !temp[i].IsTransition()) {
                    updatereq = true;
                    AddToStack(temp[i], false);
                    indicesdone.insert(temp[i].GetIndex());
                }
            }
            
            //if an index does not contain condition in from field and an empty to field
            //it might still have it as a transition from the previous condition to our condition. 
            //In this case we should take this non-perfect fit.
            if(!updatereq && hint != ComponentCondition::None) {
                for(int i=0; i<temp.GetCount(); i++) {
                    if(temp[i].GetRepeatMode() != ComponentTemplate::NoRepeat)
                        continue;

                    if(temp[i].GetCondition() == hint && temp[i].GetTargetCondition() == to) {
                        updatereq = true;
                        AddToStack(temp[i], false);
                    }
                }
            }
        }
        else {
            for(int i=0; i<temp.GetCount(); i++) {
                if(temp[i].GetRepeatMode() != ComponentTemplate::NoRepeat)
                    continue;

                if(temp[i].GetCondition() == from && temp[i].GetTargetCondition() == to) {
                    updatereq = true;
                    AddToStack(temp[i], false);
                    break;
                }
            }
            
            //search for reversed
            for(int i=0; i<temp.GetCount(); i++) {
                if(temp[i].GetRepeatMode() != ComponentTemplate::NoRepeat)
                    continue;

                if(temp[i].IsReversible() && temp[i].GetCondition() == to && temp[i].GetTargetCondition() == from) {
                    updatereq = true;
                    AddToStack(temp[i], true);
                    break;
                }
            }
        }
        
        if(updatereq)
            Update();

        ConditionChanged();
        
        return updatereq;
    }
    
    bool ComponentStack::removecondition(ComponentCondition from, ComponentCondition to) {
        bool updatereq = false;
        bool erased = false;
        
        if(transitions.count({from, to})) {
            //caller should erase transition
            //transitions.erase({from, to});
            erased = true;
        }
        
        if(conditions.count(to) && to != ComponentCondition::Always) {
            conditions.erase(to);
            erased = true;
        }
        
        if(erased) {
            for(int i=0; i<indices; i++) {
                for(int j=stacksizes[i]-1; j>=0; j--) {
                    auto &comp = data[i + j*indices];
                    const auto &temp = comp.GetTemplate();
                    bool remove = false;

                    //do not remove repeating components
                    if(temp.GetRepeatMode() != ComponentTemplate::NoRepeat)
                        continue;
                    
                    if(from == ComponentCondition::Never)
                        remove = temp.GetCondition() == to || temp.GetTargetCondition() == to;
                    else {
                        if(comp.reversed)
                            remove = temp.GetCondition() == to && temp.GetTargetCondition() == from;
                        else
                            remove = temp.GetCondition() == from && temp.GetTargetCondition() == to;
                    }
                    
                    if(remove) {                            
                        if(j == stacksizes[i]-1) {
                            updatereq = true;

                            if(temp.GetType() == ComponentType::Placeholder) {
                                const auto &ptemp = dynamic_cast<const PlaceholderTemplate&>(temp);

                                if(ptemp.HasTemplate() && substacks[&temp].HasParent()) {
                                    substacks[&temp].GetParent().Remove(substacks[&temp]);
                                }
                            }

                            get(i, j).~Component();
                            stacksizes[i]--;
                        }
                        else {
                            
                            //bubble the item to be deleted to the top of stack.
                            for(int k=j; k<stacksizes[i]-1; k++) {
                                using std::swap;
                                swap(get(i, k), get(i, k+1));
                            }
                            
                            get(i, stacksizes[i]).~Component();
                            stacksizes[i]--;
                            
                            //no need to update
                        }
                    }
                }
            }
        }
    
        if(to == ComponentCondition::Disabled && handlingmouse) {
            for(auto d : disabled)
                ReplaceCondition(ComponentCondition::Disabled, d);
            
            disabled.clear();
        }

        if(from == ComponentCondition::None) {
            //also remove any transitions going to to
            
            for(const auto &t : transitions) {
                if(t.first.second == to) {
                    if(removecondition(t.first.first, to))
                        updatereq = true;
                }
            }
        }
        
        if(updatereq)
            Update();
        
        return erased || updatereq;
    }
    
    void ComponentStack::ReplaceCondition(ComponentCondition from, ComponentCondition to, bool transition) {
        //the transition exists, nothing to do
        if(transitions.count({from, to}) || (future_transitions.count(from) && future_transitions[from] == to))
            return;

        //the condition already exists
        if(!transitions.count({to, from}) && (conditions.count(to) || to == ComponentCondition::Always)) {
            //source condition does not exist, thus there is nothing to do
            if(!conditions.count(from))
                return;
            //else try animating from to to. this should not happen unless to is Always
        }
        
        //if a reverse animation exists, reverse the direction, reverse the time
        if(transitions.count({to, from})) {
            if(transition && temp.GetConditionDuration(from, to)) {
                auto elapsed = Time::FrameStart() - transitions[{to, from}];
                auto completion = 1.0 - (double)elapsed / temp.GetConditionDuration(to, from);
                
                removecondition(to, from);
                transitions.erase({to, from});
                
                addcondition(from, to);
                transitions[{from, to}] = Time::FrameStart() - (unsigned long)(completion * temp.GetConditionDuration(from, to));
            }
            else {
                removecondition(to, from);
                transitions.erase({to, from});

                addcondition(ComponentCondition::None, to);
            }
        }
        else if(!conditions.count(from) && from != ComponentCondition::Always) {
            for(const auto &t : transitions) {
                if(t.first.second == from) {
                    if(transition && temp.GetConditionDuration(t.first.first, to)) {
                        addcondition(t.first.first, to);
                    }
                    else if(transition && temp.GetConditionDuration(ComponentCondition::Always, to)) {
                        addcondition(ComponentCondition::Always, to);
                    }
                    else if(transition && temp.GetConditionDuration(from, to)) {
                        future_transitions.insert({from, to});
                    }
                    else {
                        addcondition(ComponentCondition::None, to);
                    }
                }
            }
        }        
        else if(transition && temp.GetConditionDuration(from, to)) {
            removecondition(ComponentCondition::None, from);
            addcondition(from, to);
        }
        else {
            if(from != ComponentCondition::Always) {
                removecondition(ComponentCondition::None, from);
                removecondition(ComponentCondition::Always, from);
            }
            
            if(to != ComponentCondition::Always) {
                addcondition(ComponentCondition::Always, to);
            }
        }
    }
    
    void ComponentStack::FinalizeTransitions() { //untested
        for(auto iter = transitions.begin(); iter != transitions.end();) {
            auto c = *iter;

            if(future_transitions.count(c.first.second)) {
                removecondition(c.first.first, c.first.second);
                addcondition(ComponentCondition::None, future_transitions[c.first.second], c.first.first);
            }
            else {
                removecondition(c.first.first, c.first.second);
                addcondition(ComponentCondition::None, c.first.second, c.first.first);
            }
            iter = transitions.erase(iter);
        }
    }

    void ComponentStack::SetData(ComponentTemplate::DataEffect effect, const Graphics::Drawable &image) {
        bool wasset = imagedata.Exists(effect);
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

        if(!wasset) {
            AddCondition((ComponentCondition)((int)ComponentCondition::DataEffectStart + (int)effect));
        }
        if(updatereq) {
            Update();
        }
    }
    
    void ComponentStack::SetData(ComponentTemplate::DataEffect effect, const std::string &text) {
        bool wasset = stringdata.count(effect);
        stringdata[effect] = text;

        bool updatereq = false;

        for(int i=0; i<indices; i++) {
            if(stacksizes[i] > 0) {
                const ComponentTemplate &temp = get(i).GetTemplate();

                if(temp.GetDataEffect() == effect) {
                    updatereq = true;
                    break;
                }
            }
        }

        if(!wasset) {
            AddCondition((ComponentCondition)((int)ComponentCondition::DataEffectStart + (int)effect));
        }
        if(updatereq) {
            Update();
        }
    }
    
    void ComponentStack::RemoveData(ComponentTemplate::DataEffect effect) {
        bool update = stringdata.count(effect) || imagedata.Exists(effect);

        if(!update)
            return;
        
        RemoveCondition((ComponentCondition)((int)ComponentCondition::DataEffectStart + (int)effect));
        
        stringdata.erase(effect);
        imagedata.Remove(effect);

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
        if(targetvalue[0] == first && targetvalue[1] == second && targetvalue[2]== third && targetvalue[3] == fourth)
            return;

        targetvalue = {{first, second, third, fourth}};

        bool changed = false;
        auto tm = Time::DeltaTime();
        for(int i=0; i<4; i++) {
            if(targetvalue[i] != value[i]) {
                if(valuespeed[i] == 0 || fabs(value[i] - targetvalue[i]) < valuespeed[i] * tm / 1000) {
                    value[i] = targetvalue[i];
                }
                else {
                    value[i] += Sign(targetvalue[i] - value[i]) * valuespeed[i] * tm / 1000;
                }

                changed = true;
            }
        }

        if(!changed)
            return;
        
        if(value_fn)
            value_fn();

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
        else
            value = targetvalue;
    }

    void ComponentStack::Update() {
        updaterequired = true;
    }

    Component &ComponentStack::get(int ind, ComponentCondition condition) const {
        ASSERT(stacksizes[ind], String::Concat("Stack for index ", ind, " is empty"));

        for(int i=stacksizes[ind]-1; i>=0; i--) {
            if(data[ind + i * indices].GetTemplate().GetCondition() == condition)
                return data[ind + i * indices];
        }

        return data[ind + (stacksizes[ind]-1) * indices];
    }

    void ComponentStack::update() {
        if(!stacksizes[0]) return;
        
        if(beforeupdate_fn)
            beforeupdate_fn();

        get(0).size = size;
        get(0).location = {0,0};
        get(0).parent = -1;

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
        for(auto &s : storage) 
            if(s.second->layer)
                s.second->layer->Hide();
            
        if(update_fn)
            update_fn();
            
        //draw everything
        render(get(0), base, {0,0});
        
        if(render_fn)
            render_fn();
    }

    void ComponentStack::render(Component &comp, Graphics::Layer &parent, Geometry::Point offset, Graphics::RGBAf color, int ind) {
        const ComponentTemplate &tempp = comp.GetTemplate();
        auto tempptr = &tempp;
        std::array<float, 4> val = value;

        if(tempp.GetRepeatMode() != ComponentTemplate::NoRepeat) {
            ComponentCondition rc = ComponentCondition::Always;
            if(repeatconditions[tempp.GetRepeatMode()].count(ind))
                rc = repeatconditions[tempp.GetRepeatMode()][ind];

            tempptr = &get(tempp.GetIndex(), rc).GetTemplate();
            
            val = repeats[tempp.GetRepeatMode()][ind];
            
        }

        const auto &temp = *tempptr;

        Graphics::Layer *target = nullptr;
        auto &st = *storage[&temp];
        
        if(st.layer) {
            target = st.layer;
            parent.Add(*target);
            target->Resize(comp.size);
            target->Move(comp.location);
            if(st.layer) {
                st.layer->Show();
                st.layer->Clear();
            }

            offset -= comp.location;
        }
        else {
            target = &parent;
        }

        if(temp.GetValueModification() == ComponentTemplate::ModifyColor) {
            if(NumberOfSetBits(temp.GetValueSource()) == 1) {
                color *= Graphics::RGBAf(calculatevalue(val, 0, comp));
            }
            else if(NumberOfSetBits(temp.GetValueSource()) == 2) {
                color *= Graphics::RGBAf(calculatevalue(val, 0, comp), calculatevalue(val, 1, comp));
            }
            else if(NumberOfSetBits(temp.GetValueSource()) == 3) {
                color *= Graphics::RGBAf(calculatevalue(val, 0, comp), calculatevalue(val, 1, comp), calculatevalue(val, 2, comp), 1.f);
            }
            else
                color *= Graphics::RGBAf(calculatevalue(val, 0, comp), calculatevalue(val, 1, comp), calculatevalue(val, 2, comp), calculatevalue(val, 3, comp));
        }
        else if(temp.GetValueModification() == ComponentTemplate::ModifyAlpha)
            color *= Graphics::RGBAf(1.f, calculatevalue(val, 0, comp));

        if(temp.GetType() == ComponentType::Container) {
            const auto &cont = dynamic_cast<const ContainerTemplate&>(temp);
            
            offset += cont.GetBorderSize().TopLeft();
            offset += comp.location;

            if(st.primary && target) {
                auto rectangular = dynamic_cast<const Graphics::RectangularDrawable*>(st.primary);
                if(rectangular)
                    rectangular->DrawIn(*target, offset-cont.GetBorderSize().TopLeft(), comp.size, color);
                else
                    st.primary->Draw(*target, offset-cont.GetBorderSize().TopLeft(), color);
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
                        int index = 0;
                        for(auto &r : repeated[&temp])
                            render(r, target ? *target : parent, offset, color, index++);
                    }
                }
            }
            
            if(st.secondary && target) {
                auto rectangular = dynamic_cast<const Graphics::RectangularDrawable*>(st.secondary);
                if(rectangular)
                    rectangular->DrawIn(*target, offset-cont.GetBorderSize().TopLeft()+cont.GetOverlayExtent().TopLeft(), comp.size, color);
                else
                    st.secondary->Draw(*target, offset-cont.GetBorderSize().TopLeft()+cont.GetOverlayExtent().TopLeft(), color);
            }
            
            offset -= cont.GetBorderSize().TopLeft();
        }
        else if(temp.GetType() == ComponentType::Graphics) {
            if(st.primary && target) {
                auto rectangular = dynamic_cast<const Graphics::RectangularDrawable*>(st.primary);
                const auto &gt = dynamic_cast<const GraphicsTemplate&>(temp);

                auto c = gt.GetColor();

                if(temp.GetValueModification() == temp.BlendColor)  {
                    auto c2 = gt.GetTargetColor();
                    switch(NumberOfSetBits(temp.GetValueSource())) {
                        case 1:
                            c.Blend(c2, calculatevalue(val, 0, comp));
                            break;

                        case 2:
                            c.Blend(c2, calculatevalue(val, 0, comp), calculatevalue(val, 1, comp));
                            break;

                        case 3:
                            c.Blend(c2, {calculatevalue(val, 0, comp), calculatevalue(val, 1, comp), calculatevalue(val, 2, comp), 0});
                            break;

                        case 4:
                            c.Blend(c2, {calculatevalue(val, 0, comp), calculatevalue(val, 1, comp), calculatevalue(val, 2, comp), calculatevalue(val, 3, comp)});
                            break;

                        default:
                            break;
                    }
                }
                
                if(rectangular && gt.GetFillArea())
                    rectangular->DrawIn(*target, comp.location+offset+gt.GetPadding().TopLeft(), comp.size, color * c);
                else
                    st.primary->Draw(*target, comp.location+offset+gt.GetPadding().TopLeft(), color * c);
            }
        }
        else if(temp.GetType() == ComponentType::Textholder) {
            const auto &th = dynamic_cast<const TextholderTemplate&>(temp);

            auto c = th.GetColor();

            if(temp.GetValueModification() == temp.BlendColor) {
                auto c2 = th.GetTargetColor();
                switch(NumberOfSetBits(temp.GetValueSource())) {
                    case 1:
                        c.Blend(c2, calculatevalue(val, 0, comp));
                        break;

                    case 2:
                        c.Blend(c2, calculatevalue(val, 0, comp), calculatevalue(val, 1, comp));
                        break;

                    case 3:
                        c.Blend(c2, {calculatevalue(val, 0, comp), calculatevalue(val, 1, comp), calculatevalue(val, 2, comp), 0});
                        break;

                    case 4:
                        c.Blend(c2, {calculatevalue(val, 0, comp), calculatevalue(val, 1, comp), calculatevalue(val, 2, comp), calculatevalue(val, 3, comp)});
                        break;

                    default:
                        break;
                }
            }

            target->SetColor(color * c);
            if(th.IsReady()) {
                if(valuetotext && (ind != -1 || !stringdata.count(temp.GetDataEffect())) ) {
                    if(tagnowrap.count(temp.GetTag()))
                        th.GetRenderer().PrintNoWrap(*target, valuetotext(temp.GetTag(), temp.GetDataEffect(), val), comp.location+offset, comp.size.Width);
                    else
                        th.GetRenderer().Print(*target, valuetotext(temp.GetTag(), temp.GetDataEffect(), val), comp.location+offset, comp.size.Width);
                }
                else if(stringdata.count(temp.GetDataEffect())) {
                    if(tagnowrap.count(temp.GetTag()))
                        th.GetRenderer().PrintNoWrap(*target, stringdata[temp.GetDataEffect()], comp.location+offset, comp.size.Width);
                    else
                        th.GetRenderer().Print(*target, stringdata[temp.GetDataEffect()], comp.location+offset, comp.size.Width);
                }
            }
            target->SetColor(1.f);
        }
        else if(temp.GetType() == ComponentType::Placeholder && comp.size.Area() > 0) {
            const auto &ph = dynamic_cast<const PlaceholderTemplate&>(temp);
            
            if(ph.HasTemplate()) {
                auto &stack = substacks[&temp];
                target->Add(stack);
                stack.Move(comp.location + offset);
                stack.Resize(comp.size);
            }

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

    Geometry::Bounds ComponentStack::TagBounds(ComponentTemplate::Tag tag) {
        Component *comp  = gettag(tag);
        
        if(!comp)
            return {0, 0, 0, 0};
        
        //update needed?
        if(updaterequired)
            update();
        
        return {comp->location, comp->size};
    }

    bool ComponentStack::HasLayer(int ind) const {
        if(stacksizes[ind] == 0)
            return false;

        try {
            return storage.at(&get(ind).GetTemplate())->layer != nullptr;
        }
        catch(...) {
            return false;
        }
    }

    Layer &ComponentStack::GetLayerOf(int ind) {
        if(stacksizes[ind] == 0)
            return *this;

        if(storage[&get(ind).GetTemplate()]->layer == nullptr) {
            storage[&get(ind).GetTemplate()]->layer = new Graphics::Layer;
            update();
        }

        return *storage[&get(ind).GetTemplate()]->layer;
    }

    Geometry::Bounds ComponentStack::BoundsOf(int ind) {
        if(stacksizes[ind] == 0)
            return {0, 0, 0, 0};
        
        //update needed?
        if(updaterequired)
            update();

        
        auto &comp = get(ind);

        Component *t = &comp;

        Geometry::Point off = {0, 0};

        while(t->parent != -1) {
            if(stacksizes[t->parent] == 0)
                break;

            t = &get(t->parent);

            off += t->location;
        }

        return {comp.location + off, comp.size};
    }

    Geometry::Point ComponentStack::TranslateCoordinates(int ind, Geometry::Point location) {
        Geometry::Bounds bounds = BoundsOf(ind);
        
        if(substacks.Exists(&temp.Get(ind)))
            bounds.Move(0, 0);
        
        location -= bounds.TopLeft();
        
        return location;
    }
    
    Geometry::Point ComponentStack::TranslateCoordinates(ComponentTemplate::Tag tag, Geometry::Point location) {
        Component *comp  = gettag(tag);
        
        if(!comp)
            return {0, 0};
        
        int ind  = comp->GetTemplate().GetIndex();
        
        return TranslateCoordinates(ind, location);
    }

    Geometry::Pointf ComponentStack::TransformCoordinates(int ind, Geometry::Point location) {
        Geometry::Bounds bounds = BoundsOf(ind);
        
        if(substacks.Exists(&temp.Get(ind)))
            bounds.Move(0, 0);
        
        location -= bounds.TopLeft();
        
        return {float(location.X) / bounds.Width(), float(location.Y) / bounds.Height()};
    }
    
    Geometry::Pointf ComponentStack::TransformCoordinates(ComponentTemplate::Tag tag, Geometry::Point location) {
        Component *comp  = gettag(tag);
        
        if(!comp)
            return {0, 0};
        
        int ind  = comp->GetTemplate().GetIndex();
        
        return TransformCoordinates(ind, location);
    }

    int ComponentStack::IndexOfTag(ComponentTemplate::Tag tag) {
        Component *comp  = gettag(tag);

        if(!comp)
            return -1;

        return comp->GetTemplate().GetIndex();
    }

    std::array<float, 4> ComponentStack::CoordinateToValue(ComponentTemplate::Tag tag, Geometry::Point location) {
        Component *comp  = gettag(tag);
        
        if(!comp)
            return {{0.f, 0.f, 0.f, 0.f}};
        
        const auto &ct = comp->GetTemplate();
        int ind  = ct.GetIndex();
        int pind = -1;
        
        for(int i=0; i<indices; i++) {
            if(stacksizes[i] > 0 && get(i).GetTemplate().GetType() == ComponentType::Container) {
                const auto &cont = dynamic_cast<const ContainerTemplate &>(get(i).GetTemplate());
                for(int j=0; j<cont.GetCount(); j++)
                    if(cont[j] == ind) {
                        pind = cont.GetIndex();
                        break;
                    }
            }
        }
        
        std::array<float, 4> val = {{0, 0, 0, 0}}, ret = {{0, 0, 0, 0}};
        
        auto pnt    = TranslateCoordinates(pind, location);
        auto bounds = BoundsOf(pind);
        
        switch(ct.GetValueModification()) {
            //default is position modification, if the mode is not valid
            //for mouse coordinates, this will be used. We will assume
            //absolute positioning.
            default: {
                bounds.Right -= comp->size.Width;
                bounds.Bottom -= comp->size.Height;
                
                int emsize = getemsize(*comp);
                pnt -=  Convert(ct.GetCenter(), comp->size, emsize);
                
                val[0] = float(pnt.X) / bounds.Width();
                val[1] = float(pnt.Y) / bounds.Height();
                
                if(ct.GetValueModification() == ContainerTemplate::ModifyX)
                    val[1] = 0;
                else if(ct.GetValueModification() == ContainerTemplate::ModifyY) {
                    val[0] = val[1];
                    val[1] = 0;
                }
                break;
            }
            
            case ComponentTemplate::ModifyRotation:
                //todo
                break;

            case ComponentTemplate::ModifySize:
            case ComponentTemplate::ModifyWidth:
            case ComponentTemplate::ModifyHeight:
                if(IsLeft(ct.GetContainerAnchor())) {
                    val[0] = float(pnt.X);
                }
                else if(IsCenter(ct.GetContainerAnchor())) {
                    val[0] = float(abs(pnt.X - bounds.Width()) * 2);
                }
                else if(IsRight(ct.GetContainerAnchor())) {
                    val[0] = float(bounds.Width() - pnt.X);
                }

                if(IsTop(ct.GetContainerAnchor())) {
                    val[1] = float(pnt.Y);
                }
                else if(IsMiddle(ct.GetContainerAnchor())) {
                    val[1] = float(abs(pnt.Y - bounds.Height()) * 2);
                }
                else if(IsBottom(ct.GetContainerAnchor())) {
                    val[1] = float(bounds.Height() - pnt.Y);
                }
                
                val[0] = float(val[0]) / bounds.Width();
                val[1] = float(val[1]) / bounds.Height();

                if(ct.GetValueModification() == ContainerTemplate::ModifyWidth)
                    val[1] = 0;
                else if(ct.GetValueModification() == ContainerTemplate::ModifyHeight) {
                    val[0] = val[1];
                    val[1] = 0;
                }

                break;
        }
        
        if((ct.GetValueModification() == ComponentTemplate::ModifyPosition || ct.GetValueModification() == ComponentTemplate::ModifySize) && NumberOfSetBits(ct.GetValueSource()) == 1) {
            if(stacksizes[pind] && dynamic_cast<const ContainerTemplate&>(get(pind).GetTemplate()).GetOrientation() == Graphics::Orientation::Vertical) {
                val[0] = val[1];
                val[1] = 0;
            }
        }
        
        //do channel mapping
        switch(ct.GetValueSource()) {
            case ComponentTemplate::UseFirst:
                val[1] = 0;
                break;
                
            case ComponentTemplate::UseSecond:
                val[1] = val[0];
                break;
                
            case ComponentTemplate::UseThird:
                val[2] = val[0];
                val[1] = 0;
                break;
                
            case ComponentTemplate::UseFourth:
                val[3] = val[0];
                val[1] = 0;
                break;
                
            case ComponentTemplate::UseXY:
            case ComponentTemplate::UseXYZ:
            case ComponentTemplate::UseRGA:
            case ComponentTemplate::UseRGBA:
                //do nothing
                break;
                
            case ComponentTemplate::UseYZ:
            case ComponentTemplate::UseGBA:
                val[2] = val[1];
                val[1] = val[0];
                val[0] = 0;
                break;
                
            case ComponentTemplate::UseXZ:
            case ComponentTemplate::UseRBA:
                val[2] = val[1];
                val[1] = 0;
                
                break;
                
            case ComponentTemplate::UseRA:
                val[3] = val[1];
                val[1] = 0;
                break;
                
            case ComponentTemplate::UseBA:
                val[3] = val[1];
                val[2] = val[0];
                val[1] = 0;
                val[0] = 0;                
                break;
                
            case ComponentTemplate::UseGA:
                val[3] = val[1];
                val[1] = val[0];
                val[0] = 0;                
                break;
                
            case ComponentTemplate::UseGray:
                val = {{val[0], val[0], val[0], val[0]}};
                break;
                
            case ComponentTemplate::UseGrayAlpha:
                val = {{val[0], val[0], val[0], val[1]}};
                break;
                
            case ComponentTemplate::UseL:
            case ComponentTemplate::UseC:
            case ComponentTemplate::UseH:
            case ComponentTemplate::UseLC:
            case ComponentTemplate::UseCH:
            case ComponentTemplate::UseLH:
            case ComponentTemplate::UseLCH:
            case ComponentTemplate::UseLCHA:
                //todo
                break;
        }
        
        //do ordering transformation
        const auto valueordering = ct.GetValueOrdering();
        ret[valueordering[0]] = val[0];
        ret[valueordering[1]] = val[1];
        ret[valueordering[2]] = val[2];
        ret[valueordering[3]] = val[3];
        
        return ret;
    }

    int ComponentStack::ComponentAt(Geometry::Point location, Geometry::Bounds &bounds) {
        if(!stacksizes[0]) return -1;

        std::vector<std::pair<int, bool>> todo;
        todo.push_back({0, false});

        Geometry::Point off = {0, 0};

        while(todo.size()) {
            if(stacksizes[todo.back().first] == 0) {
                todo.pop_back();
                continue;
            }

            auto &comp = get(todo.back().first);
            const auto &temp = comp.GetTemplate();

            if(dynamic_cast<const ContainerTemplate *>(&temp) && !todo.back().second) {
                auto &cont = dynamic_cast<const ContainerTemplate &>(temp);
                todo.back().second = true;

                for(int i=0; i<cont.GetCount(); i++) {
                    if(cont[i] >= indices) continue;
                    if(stacksizes[cont[i]]) {
                        todo.push_back({cont[i], false});
                    }
                }

                off += comp.location;
            }
            else {
                if(dynamic_cast<const ContainerTemplate *>(&temp)) {
                    off -= comp.location;
                }

                Geometry::Bounds b = {comp.location + off, comp.size};
                if(IsInside(b, location)) {
                    bounds = b;

                    return temp.GetIndex();
                }

                todo.pop_back();
            }
        }

        return -1;
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

        float v = 0;
        
        if(vs == ComponentTemplate::UseTransition) {
            auto dur = this->temp.GetConditionDuration(temp.GetCondition(), temp.GetTargetCondition());
            auto cur = dur;
            if(!comp.reversed) {
                if(transitions.count({temp.GetCondition(), temp.GetTargetCondition()})) {
                    cur = Time::FrameStart() - transitions.at({temp.GetCondition(), temp.GetTargetCondition()});
                }
            }
            else {
                if(transitions.count({temp.GetTargetCondition(), temp.GetCondition()})) {
                    cur = Time::FrameStart() - transitions.at({temp.GetTargetCondition(), temp.GetCondition()});
                }
                else {
                    cur = 0;
                }
                dur = this->temp.GetConditionDuration(temp.GetTargetCondition(), temp.GetCondition());
            }
            
            const auto valueordering = temp.GetValueOrdering();
            
            if(valueordering[channel] == 0)
                v = float(cur) / dur;
            
            if(comp.reversed)
                v = 1 - v;
            
        }
        else {
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
            
            const auto valueordering = temp.GetValueOrdering();

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
        
        if(cont.GetHorizontalSizing() == cont.Fixed && parent.innersize.Width <= 0) return;
        if(cont.GetVerticalSizing() == cont.Fixed && parent.innersize.Height <= 0) return;

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

            const auto &tempparent = compparent.GetTemplate();
            
            
            for(int j = 0; tempparent.GetRepeatMode() == tempparent.NoRepeat ? j == 0 : repeats.count(tempparent.GetRepeatMode()) && j < repeats[tempparent.GetRepeatMode()].size(); j++) {
                Component *compptr;
                const ComponentTemplate *tempptr;

                const std::array<float, 4> *val;

                if(tempparent.GetRepeatMode() == tempparent.NoRepeat) {
                    compptr = &compparent;
                    tempptr = &tempparent;
                    val     = &value;
                }
                else {
                    compptr = &repeated[&tempparent][j];
                    ComponentCondition rc = ComponentCondition::Always;

                    if(repeatconditions[tempparent.GetRepeatMode()].count(j))
                        rc = repeatconditions[tempparent.GetRepeatMode()][j];

                    tempptr = &get(ci, rc).GetTemplate();
                    val     = &repeats[tempparent.GetRepeatMode()][j];
                }

                auto &comp = *compptr;
                const auto &temp = *tempptr;

                comp.parent = cont.GetIndex();

                //check if textholder and if so use emsize from the font
                int emsize = getemsize(comp);
        
                auto parentmargin = Convert(
                        temp.GetMargin(), parent.innersize, emsize
                    ).CombinePadding(
                        Convert(cont.GetPadding(), parent.size, emsize)
                    ) + 
                    Convert(temp.GetIndent(), parent.innersize, emsize);
            
                auto maxsize = parent.innersize - parentmargin;
            
                if(temp.GetPositioning() != temp.Absolute && temp.GetPositioning() != temp.PolarAbsolute) {
                    if(cont.GetOrientation() == Graphics::Orientation::Horizontal) {
                        maxsize.Width = spaceleft - parentmargin.TotalX();
                        if(maxsize.Width < 0) 
                            maxsize.Width = 0;
                    }
                    else {
                        maxsize.Height = spaceleft - parentmargin.TotalY();
                        if(maxsize.Height < 0) 
                            maxsize.Height = 0;
                    }
                }
            
                auto size = temp.GetSize();
            
            
                if(temp.GetValueModification() == temp.ModifySize) {
                    if(NumberOfSetBits(temp.GetValueSource()) == 1) {
                        if(cont.GetOrientation() == Graphics::Orientation::Horizontal) {
                            size = {
                                {int(calculatevalue(*val, 0, comp)*10000), Dimension::BasisPoint}, 
                                size.Height
                            };
                        }
                        else {
                            size = {
                                size.Width, 
                                {int(calculatevalue(*val, 0, comp)*10000), Dimension::BasisPoint}
                            };
                        }
                    }
                    else {
                        size ={
                            {int(calculatevalue(*val, 0, comp)*10000), Dimension::BasisPoint}, 
                            {int(calculatevalue(*val, 1, comp)*10000), Dimension::BasisPoint}
                        };
                    }
                }
                else if(temp.GetValueModification() == temp.ModifyWidth) {
                    size ={{int(calculatevalue(*val, 0, comp)*10000), Dimension::BasisPoint}, size.Height};
                }
                else if(temp.GetValueModification() == temp.ModifyHeight) {
                    size ={size.Width, {int(calculatevalue(*val, 0, comp)*10000), Dimension::BasisPoint}};
                }
                
                if(temp.GetValueModification() == temp.ModifySize && NumberOfSetBits(temp.GetValueSource()) > 1) {
                    auto minimum = Convert(temp.GetSize(), maxsize, emsize);
                    
                    comp.size = Convert(size, maxsize - minimum, emsize) + minimum;
                }
                else if(temp.GetValueModification() == temp.ModifyWidth || (temp.GetValueModification() == temp.ModifySize && cont.GetOrientation() == Graphics::Orientation::Horizontal)) {
                    auto minimum = Convert(temp.GetSize(), maxsize, emsize);
                                        
                    comp.size = Convert(size, {maxsize.Width - minimum.Width, maxsize.Height}, emsize) + Geometry::Size(minimum.Width, 0);
                }
                else if(temp.GetValueModification() == temp.ModifyHeight || (temp.GetValueModification() == temp.ModifySize && cont.GetOrientation() == Graphics::Orientation::Vertical)) {
                    auto minimum = Convert(temp.GetSize(), maxsize, emsize);
                                        
                    comp.size = Convert(size, {maxsize.Width, maxsize.Height - minimum.Height}, emsize) + Geometry::Size(0, minimum.Height);
                }
                else if(tagsizes.count(temp.GetTag())) {
                    comp.size = tagsizes[temp.GetTag()];
                }
                else {
                    comp.size = Convert(size, maxsize, emsize);
                }
                
                if(temp.GetPositioning() == temp.Relative && (
                    (cont.GetOrientation() == Graphics::Orientation::Horizontal && 
                        (size.Width.GetUnit() == Dimension::Percent || size.Width.GetUnit() == Dimension::BasisPoint)) ||
                    (cont.GetOrientation() == Graphics::Orientation::Vertical && 
                        (size.Height.GetUnit() == Dimension::Percent || size.Height.GetUnit() == Dimension::BasisPoint))
                ))
                    requiresrepass = true;
                
                if(!(tagsizes.count(temp.GetTag())) && 
                    (temp.GetHorizontalSizing() != temp.Fixed || temp.GetVerticalSizing() != temp.Fixed) &&
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
                                comp.size = rectangular->GetSize() + dynamic_cast<const GraphicsTemplate&>(temp).GetPadding();
                        }
                        else {
                            comp.size = dynamic_cast<const GraphicsTemplate&>(temp).GetPadding().Total();
                        }
                    }
                    else if(temp.GetType() == ComponentType::Textholder) {
                        const auto &th = dynamic_cast<const TextholderTemplate&>(temp);

                        if(th.IsReady() && stringdata[temp.GetDataEffect()] != "") {
                            auto s = size.Width(maxsize.Width, emsize);
                            
                            if(tagnowrap.count(temp.GetTag()))
                                s = 0;

                            if(s > 0)
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

                    if(temp.GetHorizontalSizing() == ComponentTemplate::GrowOnly) {
                        if(comp.size.Width < orgsize.Width)
                            comp.size.Width = orgsize.Width;
                    }
                    else if(temp.GetHorizontalSizing() == ComponentTemplate::ShrinkOnly) {
                        if(comp.size.Width > orgsize.Width)
                            comp.size.Width = orgsize.Width;
                    }

                    if(temp.GetHorizontalSizing() == ComponentTemplate::GrowOnly) {
                        if(comp.size.Height < orgsize.Height)
                            comp.size.Height = orgsize.Height;
                    }
                    else if(temp.GetHorizontalSizing() == ComponentTemplate::ShrinkOnly) {
                        if(comp.size.Height > orgsize.Height)
                            comp.size.Height = orgsize.Height;
                    }

                    if(temp.GetValueModification() == temp.ModifySize) {
                        if(cont.GetOrientation() == Graphics::Orientation::Horizontal)
                            comp.size.Width = orgsize.Width;
                        else
                            comp.size.Height = orgsize.Height;
                    }
                    else if(temp.GetValueModification() == temp.ModifyWidth) {
                        comp.size.Width = orgsize.Width;
                    }
                    else if(temp.GetValueModification() == temp.ModifyHeight) {
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
                else if(taglocations.count(temp.GetTag())) {
                    pos = {taglocations[temp.GetTag()]};
                }
                
                if(temp.GetPositioning() == temp.PolarAbsolute) {
                    auto pcenter = Geometry::Pointf(cont.GetCenter().X.CalculateFloat((float)maxsize.Width, (float)emsize), cont.GetCenter().Y.CalculateFloat((float)maxsize.Height, (float)emsize));
                    auto center  = Geometry::Pointf(temp.GetCenter().X.CalculateFloat((float)comp.size.Width, (float)emsize), temp.GetCenter().Y.CalculateFloat((float)comp.size.Height, (float)emsize));

                    pcenter += parentmargin.TopLeft();

                    auto r = pos.X.CalculateFloat(Geometry::Point(maxsize).Distance()/(float)sqrt(2), (float)emsize);

                    auto a = pos.Y.CalculateFloat(360, PI);

                    a *= PI / 180.0f;

                    comp.location = {int(std::round(r * cos(a) + pcenter.X - center.X)), int(std::round(r * sin(a) + pcenter.Y - center.Y))};
                }
                else {
                    auto offset = Convert(pos, (temp.GetPositioning() == temp.AbsoluteSliding ?  maxsize - comp.size : comp.size), emsize);
            
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

                        if(temp.GetPositioning() != temp.Absolute && temp.GetPositioning() != temp.PolarAbsolute) {
                            if(comp.anchorotherside) {
                                rightused = parent.innersize.Width - comp.location.X;
                            }
                            else if(comp.size.Width > 0) {
                                leftused = (   
                                    comp.location.X + comp.size.Width + 
                                    std::max(temp.GetMargin().Right(parent.innersize.Width, emsize), cont.GetPadding().Right(parent.size.Width, emsize))
                                
                                );
                            }
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

                    if(temp.GetPositioning() != temp.Absolute && temp.GetPositioning() != temp.PolarAbsolute) {
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
        if(frame_fn)
            frame_fn();
        
        std::vector<std::pair<ComponentCondition, ComponentCondition>> tobereplaced;
        for(auto iter = transitions.begin(); iter != transitions.end();) {
            auto c = *iter;
            auto delta = Time::FrameStart() - c.second;
            auto dur = (unsigned)temp.GetConditionDuration(c.first.first, c.first.second);
            if(dur < delta) {
                if(future_transitions.count(c.first.second)) {
                    tobereplaced.push_back({c.first.second, future_transitions[c.first.second]});
                    future_transitions.erase(c.first.second);
                }
                else {
                    removecondition(c.first.first, c.first.second);
                    addcondition(ComponentCondition::None, c.first.second, c.first.first);
                }
                iter = transitions.erase(iter);
            }
            else {
                for(int i=0; i<indices; i++)  {
                    if(!stacksizes[i]) continue;
                    
                    if(get(i).GetTemplate().GetValueSource() == ComponentTemplate::UseTransition) {
                        if(get(i).GetTemplate().GetValueModification() != ComponentTemplate::ModifyAnimation && get(i).GetTemplate().GetValueModification() != ComponentTemplate::NoModification)
                            updaterequired = true;
                    }
                }
                        
                ++iter;
            }
        }

        bool changed = false;
        auto tm = Time::DeltaTime();
        for(int i=0; i<4; i++) {
            if(targetvalue[i] != value[i]) {
                if(valuespeed[i] == 0 || fabs(value[i] - targetvalue[i]) < valuespeed[i] * tm / 1000) {
                    value[i] = targetvalue[i];
                }
                else {
                    value[i] += Sign(targetvalue[i] - value[i]) * valuespeed[i] * tm / 1000;
                }

                changed = true;
            }
        }

        if(changed) {
            updaterequired = true;

            if(!returntarget && value_fn)
                value_fn();
        }

        
        for(auto it : tobereplaced)
            ReplaceCondition(it.first, it.second);
        
        if(updaterequired)
            update();
        
        for(int i=0; i<indices; i++)  {
            if(!stacksizes[i]) continue;
            
            if(storage[&get(i).GetTemplate()]->timer) {
                storage[&get(i).GetTemplate()]->timer->SetProgress(nextafter(1.0f, 0.0f) * calculatevalue(0, get(i))); 
            }
        }

        Gorgon::Layer::Render();
    }
    
    void ComponentStack::HandleMouse(Input::Mouse::Button accepted) {
        mousebuttonaccepted=accepted;

        for(auto s : substacks)
            s.second.HandleMouse(mousebuttonaccepted);

        handlingmouse = true;
    }

    void ComponentStack::SetOtherMouseEvent(std::function<bool(ComponentTemplate::Tag, Input::Mouse::EventType, Geometry::Point, float)> handler) {
        other_fn = handler;

        mouse.SetScroll([this](Geometry::Point location, float amount) {
            if(other_fn)
                return other_fn(ComponentTemplate::NoTag, Input::Mouse::EventType::Scroll_Vert, location, amount);
            
            return false;
        });

        mouse.SetHScroll([this](Geometry::Point location, float amount) {
            if(other_fn)
                return other_fn(ComponentTemplate::NoTag, Input::Mouse::EventType::Scroll_Hor, location, amount);
            return false;
        });

        mouse.SetRotate([this](Geometry::Point location, float amount) {
            if(other_fn)
                return other_fn(ComponentTemplate::NoTag, Input::Mouse::EventType::Rotate, location, amount);
            return false;
        });

        mouse.SetZoom([this](Geometry::Point location, float amount) {
            if(other_fn)
                return other_fn(ComponentTemplate::NoTag, Input::Mouse::EventType::Zoom, location, amount);
            return false;
        });

        for(auto stack : substacks) {
            stack.second.SetOtherMouseEvent([stack, this](ComponentTemplate::Tag tag, Input::Mouse::EventType type, Geometry::Point point, float amount) {
                return other_fn(stack.first->GetTag() == ComponentTemplate::NoTag ? ComponentTemplate::UnknownTag : stack.first->GetTag(), type, point, amount);
            });
        }
    }
} }
