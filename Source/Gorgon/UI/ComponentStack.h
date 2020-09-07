#pragma once

#include <set>
#include <vector>

#include "Template.h"
#include "Component.h"
#include "../Containers/Hashmap.h"
#include "../Input/Layer.h"
#include "../Geometry/Point3D.h"

namespace Gorgon { namespace UI {

    /**
     * Component stack is the backbone of a widget. It manages the components inside the 
     * widget by showing/hiding components depending on the current state. Additionally,
     * component stack arranges the components by adjusting their locations and sizes.
     * 
     * WARNING: This class might be changed into a tiered class, allowing consumers to use
     * the tier with the minimum necessary functionality.
     */
    class ComponentStack : public Layer, public Updatable {
    public:
        
        /// Initializes a component stack with the given size
        ComponentStack(const Template &temp, Geometry::Size size);
        
        /// Initiates a component stack with default size
        explicit ComponentStack(const Template &temp) : ComponentStack(temp, temp.GetSize()) 
        { }
        
        /// Destructor
        virtual ~ComponentStack();

        /// Adds the given component to the top of the stack. This function will be called
        /// to add all components in the given template
        void AddToStack(const ComponentTemplate &temp, bool reversed);
        
        /// Replaces a condition with another one
        void ReplaceCondition(ComponentCondition from, ComponentCondition to, bool transition = true);
        
        /// Adds a condition and its associated components to the stack
        void AddCondition(ComponentCondition condition, bool transition = true) { 
            //redirect to replace condition
            ReplaceCondition(ComponentCondition::Always, condition, transition); 
        }
        
        /// Removes a condition and its associated components
        void RemoveCondition(ComponentCondition condition, bool transition = true)  { 
            //redirect to replace condition
            ReplaceCondition(condition, ComponentCondition::Always, transition); 
        }

        /// Finalizes on-going transitions immediately
        void FinalizeTransitions();

        /// Sets the data for a specific data effect. This value will be cached by
        /// the stack for condition changes. This variant supports string based data.
        void SetData(ComponentTemplate::DataEffect effect, const std::string &text);
        
        /// Sets the data for a specific data effect. This value will be cached by
        /// the stack for condition changes. This variant supports image based data.
        /// Ownership of the image stays with the caller.
        void SetData(ComponentTemplate::DataEffect effect, const Graphics::Drawable &image);
        
        /// Returns the text data. If data is not set, this will return empty string.
        std::string GetTextData(ComponentTemplate::DataEffect effect) {
            return stringdata.count(effect) ? stringdata[effect] : "";
        }
        
        /// Returns the image data. If data is not set, this will return nullptr.
        const Graphics::Drawable *GetImageData(ComponentTemplate::DataEffect effect) {
            return imagedata.Exists(effect) ? &imagedata[effect] : nullptr;
        }

        /// Removes the data associated with data effect. This will remove all data
        /// variants together.
        void RemoveData(ComponentTemplate::DataEffect effect);

        /// Sets the value for this stack. Value of the stack can affect various
        /// properties of components. This will set the individual channels separately.
        /// Values should be between 0 and 1.
        void SetValue(float first) { SetValue(first, value[1], value[2], value[3]); }

        /// Sets the value for this stack. Value of the stack can affect various
        /// properties of components. This will set the individual channels separately.
        /// Values should be between 0 and 1.
        void SetValue(float first, float second) { SetValue(first, second, value[2], value[3]); }

        /// Sets the value for this stack. Value of the stack can affect various
        /// properties of components. This will set the individual channels separately.
        /// Values should be between 0 and 1.
        void SetValue(float first, float second, float third) { SetValue(first, second, third, value[3]); }

        /// Sets the value for this stack. Value of the stack can affect various
        /// properties of components. This will set the individual channels separately.
        /// Values should be between 0 and 1.
        void SetValue(float first, float second, float third, float fourth);

        /// Sets the value for the stack using a point in coordinate system
        void SetValue(Geometry::Pointf pos) { SetValue(pos.X, pos.Y); }

        /// Sets the value for the stack using a point in coordinate system
        void SetValue(Geometry::Point3D pos) { SetValue(pos.X, pos.Y, pos.Z); }

        /// Sets the value for the stack using a color
        void SetValue(Graphics::RGBAf color) { SetValue(color.R, color.G, color.B, color.A); }

        /// Sets the value for the stack using a color
        void SetValue(Graphics::RGBA color) { SetValue((Graphics::RGBAf)color); }
        
        /// Returns the value of the stack
        std::array<float, 4> GetValue() const { return returntarget ? targetvalue : value; }

        /// Changes the value transition speed. A speed of 0 will disable smooth transition.
        /// The unit is values per second
        void SetValueTransitionSpeed(std::array<float, 4> val) {
            valuespeed = val;
        }

        /// Whether GetValue returns the current transitional value, this will also enable value event
        /// to be called every time transitional value is updated
        void ReturnTransitionalValue() {
            returntarget = false;
        }

        /// Whether GetValue returns the target value. This is the default mode.
        void ReturnTargetValue() {
            returntarget = true;
        }

        /// Sets the function that will be called whenever the value is changed
        void SetValueEvent(std::function<void()> handler) {
            value_fn = handler;
        }


        /// Sets the function that will be used to convert a value to a string. The handler will receive the value channel, data effect
        /// that is causing the translation and the value that needs to be transformed.
        void SetValueToText(std::function<std::string(int, ComponentTemplate::DataEffect, const std::array<float, 4> &)> handler) {
            valuetotext = handler;
        }

        /**
        * @name Repeating components
        * It is possible to repeat components automatically. For this, template should
        * have components marked with the respective RepeatMode and you need to set the
        * repeat points. Repeat points uses the same system as value. Repeating components
        * should have at least Always condition to work properly. It is possible to set
        * the condition of each repeat individually. This system uses indexes of repeat
        * points instead of values to speed up rendering.
        * 
        * @{
        */

        /// Sets the repeat with the given mode to the given vector. Use std::move(data) for
        /// efficient transfer
        void SetRepeat(ComponentTemplate::RepeatMode mode, std::vector<std::array<float, 4>> data) {
            repeats[mode] = std::move(data);
            checkrepeatupdate(mode);
        }
        
        /// Adds a new repeating point to the given mode. Empty values will be set as 0.
        void AddRepeat(ComponentTemplate::RepeatMode mode, float first) {
            AddRepeat(mode, first, 0, 0, 0);
        }
        
        /// Adds a new repeating point to the given mode. Empty values will be set as 0.
        void AddRepeat(ComponentTemplate::RepeatMode mode, float first, float second) {
            AddRepeat(mode, first, second, 0, 0);
        }
        
        /// Adds a new repeating point to the given mode. Empty values will be set as 0.
        void AddRepeat(ComponentTemplate::RepeatMode mode, float first, float second, float third) {
            AddRepeat(mode, first, second, third, 0);
        }
        
        /// Adds a new repeating point to the given mode.
        void AddRepeat(ComponentTemplate::RepeatMode mode, float first, float second, float third, float fourth) {
            repeats[mode].push_back({{first, second, third, fourth}});
            checkrepeatupdate(mode);
        }
        
        /// Adds a new repeating point to the given mode. Empty values will be set as 0.
        void AddRepeat(ComponentTemplate::RepeatMode mode, Geometry::Pointf pos) {
            AddRepeat(mode, pos.X, pos.Y, 0, 0);
        }
        
        /// Adds a new repeating point to the given mode. Empty values will be set as 0.
        void AddRepeat(ComponentTemplate::RepeatMode mode, Geometry::Point3D pos) {
            AddRepeat(mode, pos.X, pos.Y, pos.Z, 0);
        }
        
        /// Adds a new repeating point to the given mode.
        void AddRepeat(ComponentTemplate::RepeatMode mode, Graphics::RGBAf color) {
            AddRepeat(mode, color.R, color.G, color.B, color.A);
        }
        
        /// Adds a new repeating point to the given mode.
        void AddRepeat(ComponentTemplate::RepeatMode mode, Graphics::RGBA color) {
            AddRepeat(mode, (Graphics::RGBAf)color);
        }

        /// Removes all repeat points from the given mode. Call RemoveAllConditions along with
        /// this function if you are using conditions for repeats.
        void RemoveRepeats(ComponentTemplate::RepeatMode mode) {
            repeats.erase(mode);
            checkrepeatupdate(mode);
        }

        /// Sets the condition of a specific repeat index. Nothing will happen if index is
        /// out of bounds or condition does not exist. Setting condition to always will effectively
        /// remove the condition.
        void SetConditionOf(ComponentTemplate::RepeatMode mode, int index, ComponentCondition condition) {
            if(repeatconditions[mode][index] != condition) {
                repeatconditions[mode][index] = condition;
                checkrepeatupdate(mode);
            }
        }

        /// Removes all conditions for a repeat mode.
        void RemoveAllConditionsOf(ComponentTemplate::RepeatMode mode) {
            repeatconditions[mode].clear();
            checkrepeatupdate(mode);
        }

        /// @}

        using Layer::Resize;
        
        /// Notifies the stack about a size change
        virtual void Resize(const Geometry::Size &value) override {
            Layer::Resize(value);
            mouse.Resize(value);
            
            if(size != value) {
                size = value;
                Update();
            }
        }
        
        /// Returns the template used by this stack
        const Template &GetTemplate() const {
            return temp;
        }

        /// Updates the layout of the component stack
        virtual void Update() override { Update(false); }
        
        /// Updates the layout of the component stack
        void Update(bool immediate);

        void Render() override;

        void ResetAnimation() {
            controller.Reset();
        }
        
        /// Returns if this component stack is disabled. Both disabling and enabling animations are counted as disabled.
        bool IsDisabled() const {
            return conditions.count(ComponentCondition::Disabled) || 
                transitions.count({ComponentCondition::Always, ComponentCondition::Disabled}) || 
                transitions.count({ComponentCondition::Disabled, ComponentCondition::Always});
        }
        
        /// Changes the default emsize of 10. This value can be overridden.
        void SetEMSize(int value) {
            emsize = value;
        }
        
        /// This function instructs stack to handle mouse to automatically change hover/down
        /// states, unless disabled state is active. Propagates to substacks.
        void HandleMouse(Input::Mouse::Button accepted = Input::Mouse::Button::All);
        
        /// Returns whether the component marked with the tag has a substack. If multiple components
        /// are marked to have substack, only the first one is considered. If the tag does not exist
        /// this function will return false.
        bool TagHasSubStack(ComponentTemplate::Tag tag) const;
        
        /// Translates the given coordinates back to values using value scaling and channel mapping.
        /// Only works if the value affects the component location or size. If component with the
        /// specified tag does not exist, this function will simply return {0, 0, 0, 0}.
        std::array<float, 4> CoordinateToValue(ComponentTemplate::Tag tag, Geometry::Point location);
        
        /// Translates the given coordinates to component space in pixels.
        Geometry::Point TranslateCoordinates(ComponentTemplate::Tag tag, Geometry::Point location);
        
        /// Translates the given coordinates to component space in pixels.
        Geometry::Point TranslateCoordinates(int ind, Geometry::Point location);
        
        /// Translates the given coordinates to component space from 0 to 1.
        Geometry::Pointf TransformCoordinates(ComponentTemplate::Tag tag, Geometry::Point location);
        
        /// Translates the given coordinates to component space from 0 to 1.
        Geometry::Pointf TransformCoordinates(int ind, Geometry::Point location);
        
        /// Returns the index of the component with the specified tag. This function may cause
        /// update thus may take time to execute. If tag not found, this will return -1.
        int IndexOfTag(ComponentTemplate::Tag tag);

        /// Returns the boundaries of the component marked with the given tag. This function may cause
        /// update thus may take time to execute. The bounds are within the parent.
        Geometry::Bounds TagBounds(ComponentTemplate::Tag tag);

        /// Returns if the component at the given index has a layer.
        bool HasLayer(int ind) const;

        /// Returns the layer of the given component index. If the item does not have a layer, this function
        /// will create a new one for it. In the worst case, this function will return the object itself as
        /// a layer. Layer is probably a graphics layer, you may use RTTI to query layer type.
        Layer &GetLayerOf(int ind);
        
        /// Returns the boundaries of the component with the given index. The bounds are from the top level.
        Geometry::Bounds BoundsOf(int ind);

        /// Returns the index of the component at the given location.
        int ComponentAt(Geometry::Point location) {
            Geometry::Bounds b;

            return ComponentAt(location, b);
        }

        /// Returns the index of the component at the given location while returning the bounds
        /// of the component. 
        int ComponentAt(Geometry::Point location, Geometry::Bounds &bounds);

        /// Returns if a component at ind exists. If ind is negative or out of range, this function simply
        /// returns false.
        bool ComponentExists(int ind) const {
            return Between(ind, 0, indices) && stacksizes[ind];
        }

        /// Returns the template at the given index. If the index does not exists, this function may crash.
        /// Use ComponentExists function to check if it is safe to use the index.
        const ComponentTemplate &GetTemplate(int ind) const {
            return get(ind).GetTemplate();
        }

        /// Set a fixed location for a tagged component
        void SetTagLocation(ComponentTemplate::Tag tag, Geometry::Point location) {
            taglocations[tag] = location;
            Update();
        }
        
        Geometry::Point GetTagLocation(ComponentTemplate::Tag tag) const {
            auto f = taglocations.find(tag);
            
            if(f == taglocations.end())
                return {0, 0};
            else
                return f->second;
        }

        /// Removes the fixed location for a set tagged component
        void RemoveTagLocation(ComponentTemplate::Tag tag) {
            taglocations.erase(tag);
        }

        /// Set a fixed size for a tagged component
        void SetTagSize(ComponentTemplate::Tag tag, Geometry::Size size) {
            tagsizes[tag] = size;
            Update();
        }

        Geometry::Size GetTagSize(ComponentTemplate::Tag tag) const {
            auto f = tagsizes.find(tag);
            
            if(f == tagsizes.end())
                return {0, 0};
            else
                return f->second;
        }

        /// Removes the fixed size for a set tagged component
        void RemoveTagSize(ComponentTemplate::Tag tag) {
            tagsizes.erase(tag);
        }
        
        /// Enables text wrapping on a specific tag, default is enabled.
        void EnableTagWrap(ComponentTemplate::Tag tag) {
            tagnowrap.erase(tag);
        }
        
        /// Disables text wrapping on a specific tag, default is enabled.
        void DisableTagWrap(ComponentTemplate::Tag tag) {
            tagnowrap.insert(tag);
        }


        /// Sets a function to be called before update check
        void SetFrameEvent(std::function<void()> handler) {
            frame_fn = handler;
        }
        
        /// Sets a function to be called before every update
        void SetBeforeUpdateEvent(std::function<void()> handler) {
            beforeupdate_fn = handler;
        }
        
        /// Sets a function to be called after every update before rendering
        void SetUpdateEvent(std::function<void()> handler) {
            update_fn = handler;
        }
        
        /// Sets a function to be called after every render
        void SetRenderEvent(std::function<void()> handler) {
            render_fn = handler;
        }
        
        /// Removes the function that will be called before update check
        void RemoveFrameEvent() {
            frame_fn = std::function<void()>();
        }
        
        /// Removes the function that will be called before every update
        void RemoveBeforeUpdateEvent() {
            beforeupdate_fn = std::function<void()>();
        }
        
        /// Removes the function that will be called after every update before rendering
        void RemoveUpdateEvent() {
            update_fn = std::function<void()>();
        }
        
        /// Removes the function that will be called after every render
        void RemoveRenderEvent() {
            render_fn = std::function<void()>();
        }
        
        /** @name Mouse Events
        * These function will allow handling mouse events. If the mouse event is originating from a
        * substack, it will have a tag other than NoTag. If the tag for the substack is set NoTag,
        * handler will receive UnknownTag. It is possible to obtain the tag and index under the
        * mouse using ComponentAt functions. Along with TranslateCoordinates or 
        * TransformCoordinates functions, it is possible to get the location of a mouse event on a
        * specific component.
        * 
        * @{
        */
        
        /// Sets the mouse down event. If HandleMouse function is called, this function will first
        /// perform mouse event transition, then it will call this handler.
        void SetMouseDownEvent(std::function<void(ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button)> handler) {
            down_fn = handler;
            
            for(auto stack : substacks) {
                stack.second.SetMouseDownEvent([stack, this](ComponentTemplate::Tag tag, Geometry::Point point, Input::Mouse::Button btn) {
                    down_fn(stack.first->GetTag() == ComponentTemplate::NoTag ? ComponentTemplate::UnknownTag : stack.first->GetTag(), point, btn);
                });
            }
        }
        
        /// Sets the mouse up event. If HandleMouse function is called, this function will first
        /// perform mouse event transition, then it will call this handler. This event will be
        /// called even if mouse down is not handled.
        void SetMouseUpEvent(std::function<void(ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button)> handler) {
            up_fn = handler;
            
            for(auto stack : substacks) {
                stack.second.SetMouseUpEvent([stack, this](ComponentTemplate::Tag tag, Geometry::Point point, Input::Mouse::Button btn) {
                    up_fn(stack.first->GetTag() == ComponentTemplate::NoTag ? ComponentTemplate::UnknownTag : stack.first->GetTag(), point, btn);
                });
            }
        }
        
        /// Sets the mouse down event. If HandleMouse function is called, this function will first
        /// perform mouse event transition, then it will call this handler.
        void SetClickEvent(std::function<void(ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button)> handler) {
            click_fn = handler;
            
            for(auto stack : substacks) {
                stack.second.SetClickEvent([stack, this](ComponentTemplate::Tag tag, Geometry::Point point, Input::Mouse::Button btn) {
                    click_fn(stack.first->GetTag() == ComponentTemplate::NoTag ? ComponentTemplate::UnknownTag : stack.first->GetTag(), point, btn);
                });
            }
        }
        
        /// Sets the mouse mvoe event. If HandleMouse function is called, this function will first
        /// perform mouse event transition, then it will call this handler. If this event is not handled
        /// mouse move event of the layer will not be handled too.
        void SetMouseMoveEvent(std::function<void(ComponentTemplate::Tag, Geometry::Point)> handler) {
            move_fn = handler;
            
            mouse.SetMove([this] (Geometry::Point location){
                if(move_fn)
                    move_fn(ComponentTemplate::NoTag, location);
            });
            
            for(auto stack : substacks) {
                stack.second.SetMouseMoveEvent([stack, this](ComponentTemplate::Tag tag, Geometry::Point point) {
                    move_fn(stack.first->GetTag() == ComponentTemplate::NoTag ? ComponentTemplate::UnknownTag : stack.first->GetTag(), point);
                });
            }
        }
        
        /// Sets the mouse over event that is fired when the mouse moves over the component stack or a substack.
        /// it will not be fired for mouse moving over a specific component. If HandleMouse function is called, 
        /// this function will first perform mouse event transition, then it will call this handler.
        void SetMouseOverEvent(std::function<void(ComponentTemplate::Tag)> handler) {
            over_fn = handler;
            
            for(auto stack : substacks) {
                stack.second.SetMouseOverEvent([stack, this](ComponentTemplate::Tag tag) {
                    over_fn(stack.first->GetTag() == ComponentTemplate::NoTag ? ComponentTemplate::UnknownTag : stack.first->GetTag());
                });
            }
        }
        
        /// Sets the mouse out event that is fired when the mouse moves over the component stack or a substack.
        /// it will not be fired for mouse moving over a specific component. If HandleMouse function is called, 
        /// this function will first perform mouse event transition, then it will call this handler. This event 
        /// will be called even if mouse over is not handled.
        void SetMouseOutEvent(std::function<void(ComponentTemplate::Tag)> handler) {
            out_fn = handler;
            
            for(auto stack : substacks) {
                stack.second.SetMouseOutEvent([stack, this](ComponentTemplate::Tag tag) {
                    out_fn(stack.first->GetTag() == ComponentTemplate::NoTag ? ComponentTemplate::UnknownTag : stack.first->GetTag());
                });
            }
        }
        
        /// Sets the handler for scroll (HScroll or VScroll), zoom and rotate events. All these events depend on 
        /// specific hardware and may not be available.
        void SetOtherMouseEvent(std::function<bool(ComponentTemplate::Tag, Input::Mouse::EventType, Geometry::Point, float)> handler);

        
        /// @}

        Event<ComponentStack> ConditionChanged;
        
        
    private:
        ///returns the element at the stack with the given stack offset.
        ///If stack is -1, this function will return top of the stack.
        ///0 is at the bottom of the stack
        Component &get(int ind, int stack = -1) const {
            ASSERT(stacksizes[ind], String::Concat("Stack for index ", ind, " is empty"));

            if(stack == -1) {
                stack = stacksizes[ind]-1;
            }

            return data[ind + stack * indices];
        }

        ///Return the component at the given index with the requested condition. Returns top of stack if condition does not exist.
        Component &get(int ind, ComponentCondition condition) const;
        
        ///Calculates the position results from the anchoring the given component to the area determined by the
        ///given size and margin. Offset is used to move away from the anchor and may result reversing of direction.
        void anchortoparent(Component &parent, Component &comp, const ComponentTemplate &temp, 
                                Geometry::Point offset, Geometry::Margin margin, Geometry::Size maxsize);
        
        
        ///Calculates the position results from the anchoring the given component to another component
        void anchortoother(Component &comp, const ComponentTemplate &temp, 
                        Geometry::Point offset, Geometry::Margin margin, Component &other, Graphics::Orientation orientation);
        
        ///starts the update chain
        void update();

        ///updates a specific container component
        void update(Component &parent, const std::array<float, 4> &value, int ind, int textwidth = -1);

        ///renders the given component, rendering will use parent layer if the component does not have its own layer. Index is for
        ///repeated components, it is the index of the repeat to be rendered. Unlike Layer::Render function, this function does not
        ///run every frame.
        void render(Component &component, Graphics::Layer &parentlayer, Geometry::Point offset, const std::array<float, 4> &value, Graphics::RGBAf color = 1.f, int ind = -1);

        ///grows the size of the stack
        void grow();
        
        ///returns the size of the emdash
        int getemsize(const Component &comp);
        
        ///returns the baseline point of the component
        int getbaseline(const Component &comp);
        
        ///returns the height of the component
        int gettextheight(const Component &comp);

        ///Calculates the value of the given channel for the given component. Uses stored value
        float calculatevalue(int channel, const Component &comp) const { return calculatevalue(value, channel, comp); }

        ///Calculates the value of the given channel for the given component. Uses the supplied value.
        float calculatevalue(const std::array<float, 4> &data, int channel, const Component &comp) const;

        ///Checks if a specific type of repeating component requires update
        void checkrepeatupdate(ComponentTemplate::RepeatMode mode);
        
        ///Adds the given condition. To should contain the final condition even if there is no transition. If there is no
        ///transition, from should be None and hint should contain previous state if it exists.
        bool addcondition(ComponentCondition from, ComponentCondition to, ComponentCondition hint = ComponentCondition::None);

        ///Erases the given condition. To should contain the final condition to be removed.
        ///Caller should erase transition
        bool removecondition(ComponentCondition from, ComponentCondition to);
        
        ///Returns the component marked with the given tag. Returns nullptr if tag is not found
        Component *gettag(ComponentTemplate::Tag tag) const;

        ///ComponentStack wide emsize
        int emsize = 0;
        
        ///ComponentStack wide baseline
        int baseline = 0;
        
        ///ComponentStack wide baseline
        int textheight = 0;
        
        ///This vector contains data for components. This piece of memory will be managed by the stack
        Component *data = nullptr;
        
        ///Stores the sizes of each stack
        std::vector<int> stacksizes;
        
        ///List of conditions that are not applied as the stack is disabled
        std::set<ComponentCondition> disabled;
        
        ///List of conditions applies to the stack. Always is never stored in here
        std::set<ComponentCondition> conditions;
        
        ///List of future transitions that will be performed once the current transition is
        ///finished. The pair stores final condition to the next condition
        std::map<ComponentCondition, ComponentCondition> future_transitions;
        
        ///List of currently running transitions.
        std::map<std::pair<ComponentCondition, ComponentCondition>, unsigned long> transitions;
        
        ///Stores string data for given effect
        std::map<ComponentTemplate::DataEffect, std::string> stringdata;
        
        ///Stores image data for a given effect
        Containers::Hashmap<ComponentTemplate::DataEffect, const Graphics::Drawable> imagedata;
        
        ///Repeated values for each repeat mode
        std::map<ComponentTemplate::RepeatMode, std::vector<std::array<float, 4>>> repeats;
        
        ///Conditions that apply to individual repeat modes.
        std::map<ComponentTemplate::RepeatMode, std::map<int, ComponentCondition>> repeatconditions; 
        
        ///Current value stored in the stack
        std::array<float, 4> value ={{0.f, 0.f, 0.f, 0.f}};

        ///For value animation
        std::array<float, 4> targetvalue = {{0.f, 0.f, 0.f, 0.f}};
        
        ///Dictates the speed which values reach to the target value. valuespeed = 0 disables animation
        std::array<float, 4> valuespeed = {{0.f, 0.f, 0.f, 0.f}};
        
        ///??
        bool returntarget = false;

        ///Number of elements in each stack
        int stackcapacity = 3;
        
        ///Number of indices used in the stack
        int indices = 0;

        ///When set to true an update will be triggered during next render
        bool updaterequired = false;
        
        ///Whether component stack will be handling the mouse events
        bool handlingmouse = false;

        ///Size of the component stack
        Geometry::Size size;
        
        ///The template that is being used. This might be changed to a pointer to be able to update it.
        const Template &temp;

        ///Contains substacks of this component stack
        Containers::Hashmap<const ComponentTemplate *, ComponentStack> substacks;

        std::map<const ComponentTemplate*, ComponentStorage*> storage;
        std::map<const ComponentTemplate*, std::vector<Component>> repeated;
        std::map<ComponentTemplate::Tag, Geometry::Size> tagsizes;
        std::map<ComponentTemplate::Tag, Geometry::Point> taglocations;
        std::set<ComponentTemplate::Tag> tagnowrap;

        Animation::Timer controller;
        
        Graphics::Layer base;
        Input::Layer mouse;
        Input::Mouse::Button mousebuttonaccepted;
        Geometry::Point downlocation;
        
        std::function<void(ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button)> down_fn;
        std::function<void(ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button)> click_fn;
        std::function<void(ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button)> up_fn;
        std::function<void(ComponentTemplate::Tag, Geometry::Point)> move_fn;
        std::function<void(ComponentTemplate::Tag)> over_fn;
        std::function<void(ComponentTemplate::Tag)> out_fn;
        std::function<bool(ComponentTemplate::Tag, Input::Mouse::EventType, Geometry::Point, float)> other_fn; //scroll, zoom, rotate
        std::function<void()> value_fn;
        std::function<void()> frame_fn;
        std::function<void()> beforeupdate_fn;
        std::function<void()> update_fn;
        std::function<void()> render_fn;
        
        std::function<std::string(int ind, ComponentTemplate::DataEffect, const std::array<float, 4> &value)> valuetotext;
    };

}}
