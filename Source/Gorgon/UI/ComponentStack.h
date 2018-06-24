#pragma once

#include <set>
#include <vector>

#include "Template.h"
#include "Component.h"
#include "../Containers/Hashmap.h"
#include "../Input/Layer.h"
#include "../Geometry/Point3D.h"

namespace Gorgon { namespace UI {

	class ComponentStack : public Layer {
	public:
		/// should handle instantiation as well
		explicit ComponentStack(const Template &temp, Geometry::Size size);
		
		~ComponentStack() {
			for(auto &p : storage) {
				delete p.second;
			}

			substacks.Destroy();
		}

        /// Adds the given component to the top of the stack
		void AddToStack(const ComponentTemplate &temp);
        
        /// Adds a condition and its associated components to the stack
		void AddCondition(ComponentCondition condition, bool transition = true) { ReplaceCondition(ComponentCondition::Always, condition, transition); }
        
        /// Replaces a condition with another one
		void ReplaceCondition(ComponentCondition from, ComponentCondition to, bool transition = true);
        
        /// Removes a condition and its associated components
        void RemoveCondition(ComponentCondition condition, bool transition = true)  { ReplaceCondition(condition, ComponentCondition::Always, transition); }

        /// Sets the data for a specific data effect. This value will be cached by
        /// the stack for condition changes. This variant supports string based data.
        void SetData(ComponentTemplate::DataEffect effect, const std::string &text);
        
        /// Sets the data for a specific data effect. This value will be cached by
        /// the stack for condition changes. This variant supports image based data.
        /// Ownership of the image stays with the caller.
		void SetData(ComponentTemplate::DataEffect effect, const Graphics::Drawable &image);

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

        /// Removes all repeat points from the given mode
		void RemoveRepeats(ComponentTemplate::RepeatMode mode) {
            repeats.erase(mode);
			checkrepeatupdate(mode);
		}

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
		void Update();

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
		/// states, unless disabled state is active.
		void HandleMouse(Input::Mouse::Button accepted = Input::Mouse::Button::All);
        
        /// Returns whether the component marked with the tag has a substack. If multiple components
        /// are marked to have substack, only the first one is considered.
        bool TagHasSubStack(ComponentTemplate::Tag tag) const {
            auto comp = gettag(tag);
            
            if(!comp)
                return false;
            else
                return substacks.Exists(&comp->GetTemplate());
        }
        
        /// Translates the given coordinates back to values using value scaling and channel mapping.
        /// Only works if the value affects the component location or size.
        std::array<float, 4> CoordinateToValue(ComponentTemplate::Tag tag, Geometry::Point location);
        
        /// Translates the given coordinates to component space from 0 to 1.
        Geometry::Pointf TranslateCoordinates(ComponentTemplate::Tag tag, Geometry::Point location);
        
        /// Translates the given coordinates to component space from 0 to 1.
        Geometry::Pointf TranslateCoordinates(int ind, Geometry::Point location);
        
        /// Returns the boundaries of the component marked with the given tag. This function may cause
        /// update thus may take time to execute.
        Geometry::Bounds TagBounds(ComponentTemplate::Tag tag);
        
        /// Returns the boundaries of the component with the given index.
        Geometry::Bounds BoundsOf(int ind);
        
        /// Returns the index of the component at the given location.
        int ComponentAt(Geometry::Point location);
        
        /// Returns the input layer for this component stack.
        Input::Layer &GetInputLayer() { return mouse; }
        
        /// Returns the input layer for the component marked with the given tag. If the component
        /// has a substack then the returned layer will be for the substack. Otherwise, this function
        /// will return the input layer of this stack.
        Input::Layer &GetInputLayer(ComponentTemplate::Tag tag) { 
            if(TagHasSubStack(tag)) {
                auto comp = gettag(tag);
                
                return substacks[&comp->GetTemplate()].GetInputLayer();
            }
            else
                return mouse;
        }
        
	private:
		Component &get(int ind, int stack = -1) const {
			if(stack == -1) {
				stack = stacksizes[ind]-1;
			}

			return data[ind + stack * indices];
		}

		void update();

		void update(Component &parent);

		void render(Component &component, Graphics::Layer &parentlayer, Geometry::Point offset, Graphics::RGBAf color = 1.f);

        void grow();
        
		int getemsize(const Component &comp);

		float calculatevalue(int channel, const Component &comp) const { return calculatevalue(value, channel, comp); }

		float calculatevalue(const std::array<float, 4> &data, int channel, const Component &comp) const;

		void checkrepeatupdate(ComponentTemplate::RepeatMode mode);
        
        //to should contain the final condition even if there is no transition
        bool addcondition(ComponentCondition from, ComponentCondition to, ComponentCondition hint = ComponentCondition::None);

        //to should contain the final condition to be removed
        bool removecondition(ComponentCondition from, ComponentCondition to);
        
        Component *gettag(ComponentTemplate::Tag tag) const {
            for(int i=0; i<indices; i++) {
                if(stacksizes[i] > 0) {
                    auto &comp = get(i);
                    
                    if(comp.GetTemplate().GetTag() == tag)
                        return &comp;
                }
            }
            
            return nullptr;
        }

        int emsize = 10;
        
		Component *data = nullptr;
        std::vector<int> stacksizes;
        
        std::set<ComponentCondition> disabled;
        std::set<ComponentCondition> conditions;
        std::map<ComponentCondition, ComponentCondition> future_transitions;
        
		std::map<ComponentTemplate::DataEffect, std::string> stringdata;
		Containers::Hashmap<ComponentTemplate::DataEffect, const Graphics::Drawable> imagedata;
        std::map<ComponentTemplate::RepeatMode, std::vector<std::array<float, 4>>> repeats;
		std::array<float, 4> value;
        
        std::map<std::pair<ComponentCondition, ComponentCondition>, unsigned long> transitions;
        
        int stackcapacity = 2;
        
        int indices = 0;

		bool updaterequired = false;
        
        bool handlingmouse = false;

		Geometry::Size size;
        
        const Template &temp;

		Containers::Hashmap<const ComponentTemplate *, ComponentStack> substacks;

		std::map<const ComponentTemplate*, ComponentStorage*> storage;
		std::map<const ComponentTemplate*, std::vector<Component>> repeated;

		Animation::Timer controller;
        
        Graphics::Layer base;
        Input::Layer mouse;
        Input::Mouse::Button mousebuttonaccepted;
	};

}}
