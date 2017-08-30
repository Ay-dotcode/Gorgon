#pragma once

#include <set>
#include <vector>

#include "Template.h"
#include "Component.h"

namespace Gorgon { namespace UI {

	class ComponentStack : public Layer {
	public:
		/// should handle instantiation as well
		explicit ComponentStack(const Template &temp, Geometry::Size size);
		
        /// Adds the given component to the top of the stack
		void AddToStack(const ComponentTemplate &temp);
        
        /// Adds a condition and its associated components to the stack
		void AddCondition(ComponentCondition condition);
        
        /// Removes a condition and its associated components
        void RemoveCondition(ComponentCondition condition);

        /// Sets the data for a specific data component. This value will be cached by
        /// the stack for condition changes. This variant supports string based data.
        void SetData(ComponentTemplate::DataEffect effect, const std::string &text);

		/// Notifies the stack about a size change
		void SetSize(Geometry::Size value) {
			size = value;
			update();
		}

		/// Returns the template used by this stack
		const Template &GetTemplate() const {
			return temp;
		}
        
	private:
		Component &get(int ind, int stack = -1) {
			if(stack == -1) {
				stack = stacksizes[ind]-1;
			}

			return data[ind + stack * indices];
		}

        void grow();

		void update();

		void update(Component &parent);
        
		Component *data = nullptr;
        std::vector<int> stacksizes;
        
        std::set<ComponentCondition> conditions;
		std::map<ComponentTemplate::DataEffect, std::string> stringdata;
        
        int stackcapacity = 2;
        
        int indices = 0;

		Geometry::Size size;
        
        const Template &temp;
	};

}}
