#pragma once

#include <set>
#include <vector>

#include "Template.h"
#include "Component.h"

namespace Gorgon { namespace UI {

	class ComponentStack : public Layer {
	public:
		/// should handle instantiation as well
		explicit ComponentStack(const Template &temp) : temp(temp);
		
        /// Adds the given component to the top of the stack
		void AddToStack(const ComponentTemplate &temp);
        
        /// Adds a condition and its associated components to the stack
		void AddCondition(ComponentCondition condition);
        
        /// Removes a condition and its associated components
        void RemoveCondition(ComponentCondition condition);

	private:
        void grow();
        
		Component *data = nullptr;
        std::vector<int> stacksizes;
        
        std::set<ComponentCondition, bool> conditions;
        
        int stackcapacity = 2;
        
        int indices = 0;
        
        const Template &temp;
	};

}}
