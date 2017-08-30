#pragma once

#include "Template.h"
#include "../Graphics/Layer.h"

namespace Gorgon { namespace UI {

    /**
     * This class is an instance of a component. It simply links the component
     * to its template, stores calculated location, size, instances of
     * graphics and possibly the text that will be rendered. Components are
     * internal objects that are managed by ComponentStack.
     */
	class Component {
    public:
        Component(const ComponentTemplate &temp) : temp(&temp) { }
        
        /// Returns the template, component should have a template at all times
        const ComponentTemplate &GetTemplate() const {
            return *temp;
        }
        
        static void Swap(Component &left, Component &right) {
            using std::swap;
            
            swap(left.layer , right.layer );
            swap(left.temp  , right.temp  );
        }
        
        /// If necessary a layer will be assigned to this component
        Graphics::Layer *layer = nullptr;
        
        bool needupdate = false;
        
    private:
        const ComponentTemplate *temp;
	};

}}
