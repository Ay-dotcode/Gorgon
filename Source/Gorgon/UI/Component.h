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
            
            swap(left.temp     , right.temp     );
            swap(left.location , right.location );
            swap(left.size     , right.size     );
            swap(left.innersize, right.innersize);
        }
        

		Geometry::Point location;
        
		Geometry::Size size;
        
        //for containers
		Geometry::Size innersize;
        
        //for horizontal this means right, for vertical this means bottom.
        bool anchorotherside = false;
        
    private:
        const ComponentTemplate *temp;
	};

	/// This class stores component related data. It will be instantiated whenever a new 
	/// template is instantiated and will be preserved even after the component is destroyed.
	/// This prevents constant construction and destruction of objects.
	class ComponentStorage {
	public:
		/// Primary drawable is either background for container or the graphics for the graphic
		/// template
		const Graphics::Drawable *primary   = nullptr;

		/// Secondary is for container overlay.
		const Graphics::Drawable *secondary = nullptr;
                
        /// If necessary a layer will be assigned to this component
        Graphics::Layer *layer = nullptr;

	};

}}
