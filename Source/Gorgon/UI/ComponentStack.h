#pragma once

#include <set>
#include <vector>

#include "Template.h"
#include "Component.h"
#include "../Containers/Hashmap.h"
#include "../Input/Layer.h"

namespace Gorgon { namespace UI {

	class ComponentStack : public Layer {
	public:
		/// should handle instantiation as well
		explicit ComponentStack(const Template &temp, Geometry::Size size);
		
		~ComponentStack() {
			for(auto &p : storage) {
				delete p.second;
			}
		}

        /// Adds the given component to the top of the stack
		void AddToStack(const ComponentTemplate &temp);
        
        /// Adds a condition and its associated components to the stack
		void AddCondition(ComponentCondition condition);
        
        /// Removes a condition and its associated components
        void RemoveCondition(ComponentCondition condition, bool check = true);

        /// Sets the data for a specific data effect. This value will be cached by
        /// the stack for condition changes. This variant supports string based data.
        void SetData(ComponentTemplate::DataEffect effect, const std::string &text);
        
        /// Sets the data for a specific data effect. This value will be cached by
        /// the stack for condition changes. This variant supports image based data.
        /// Ownership of the image stays with the caller.
        void SetData(ComponentTemplate::DataEffect effect, const Graphics::Drawable &image);
        
        /// Sets the value for this stack. Value of the stack can affect various
        /// properties of components.
        void SetValue(float val);

        using Layer::Resize;
        
		/// Notifies the stack about a size change
		virtual void Resize(const Geometry::Size &value) override {
            Layer::Resize(value);
            mouse.Resize(value);
            
			size = value;
			Update();
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
            return conditions.count(ComponentCondition::Disabled) || conditions.count(ComponentCondition::Normal__Disabled) || conditions.count(ComponentCondition::Disabled__Normal);
        }
		
		/// Changes the default emsize of 10. This value can be overridden.
		void SetEMSize(int value) {
            emsize = value;
        }
		
		/// This function instructs stack to handle mouse to automatically change hover/down
		/// states, unless disabled state is active.
		void HandleMouse(Input::Mouse::Button accepted = Input::Mouse::Button::All);
        
	private:
		Component &get(int ind, int stack = -1) {
			if(stack == -1) {
				stack = stacksizes[ind]-1;
			}

			return data[ind + stack * indices];
		}

		void update();

		void update(Component &parent);

		void render(Component &component, Graphics::Layer &parentlayer, Geometry::Point offset);

        void grow();
        
        int getemsize(const Component &comp);
        
        int emsize = 10;
        
		Component *data = nullptr;
        std::vector<int> stacksizes;
        
        std::set<ComponentCondition> disabled;
        std::set<ComponentCondition> conditions;
        
		std::map<ComponentTemplate::DataEffect, std::string> stringdata;
		Containers::Hashmap<ComponentTemplate::DataEffect, const Graphics::Drawable> imagedata;
		float value = 0;
        
        unsigned long conditionstart[(int)ComponentCondition::Max];
        
        int stackcapacity = 2;
        
        int indices = 0;

		bool updaterequired = false;

		Geometry::Size size;
        
        const Template &temp;

		std::map<const ComponentTemplate*, ComponentStorage*> storage;

		Animation::Timer controller;
        
        Graphics::Layer base;
        Input::Layer mouse;
        Input::Mouse::Button mousebuttonaccepted;
	};

}}
