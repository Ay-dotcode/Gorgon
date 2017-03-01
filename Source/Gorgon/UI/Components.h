#pragma once
#include "../../Geometry/Margin.h"
#include "../../Geometry/Size.h"

namespace Gorgon { namespace UI {

	/**
	* @page "Box Model"
	* Object defines any component that can be placed on a widget. This includes
	* non-visual elements, fixed and user defined visual elements, and containers.
	* Container object defines an area that can contain more objects and optionally 
	* has a background and an overlay. This object also has margin to control how it
	* will be placed in relation to other objects at the same level. Shadow extension
	* control the drawing of background, while overlay extension controls the drawing
	* of the overlay image. Positive extension will enlarge the image in that
	* direction. Negative extensions also work. Extensions should not cover the border
	* size, border margin should be within the object, extensions can be used to align
	* overlay to background and to ignore shadows, which should stay outside the
	* object. Border size controls the size of the border. (0, 0) of the object
	* starts from this point. Padding controls the distance of the sub-objects from
	* the border. Padding and the margin collapse on edge, the maximum value of
	* margin or the padding is used. However, negative values are always subtracted.
	* Finally, indent offsets the object from edges of its container. This offset is
	* calculated after margin and padding is applied and only to the edges touching to
	* its container.
	*
	* Objects are aligned and placed using anchor points. 
	*
	* This system is quite close to HTML box model, however, there are differences:
	* background and overlay can be extended beyond the area, margin collapses with
	* padding and there is indent to control distance from edges. All objects have
	* margin and indent while only containers have extensions, border size, and
	* padding.
	*/

	/// Defines an object according to the Box Model.
	class Object {
	public:

	private:
		Geometry::Margin margin;
		Geometry::Margin indents;
		bool fixedsize;
		Geometry::Size size;

	};
	
	/// Defines a placeholder according to the Box Model. Placeholder is replaced with
	/// a visual component, it is 
	class Placeholder : public Object {
	public:

	private:
	};
	
	/// Container class that defines an area according to the Box Model.
	/// It can contain other objects.
	class Container : public Object {
	};

} }
