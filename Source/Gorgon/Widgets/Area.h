#pragma once

namespace Gorgon { namespace Graphics {

	/**
	 * @page "Box Model"
	 * Area object defines an area that can contain more objects and optionally has
	 * a background and an overlay. This object also has margin to control how it
	 * will be placed in relation to other objects at the same level. Shadow extension
	 * control the drawing of background, while overlay extension controls the drawing
	 * of the overlay image. Positive extension will enlarge the image in that
	 * direction. Negative extensions also work. Extensions should not cover the border
	 * size, border margin should be within the object, extensions can be used to align
	 * overlay to background and to ignore shadows, which should stay outside the
	 * object. Border size controls the size of the border. (0, 0) of the object
	 * starts from this point. Padding controls the distance of the sub-objects from
	 * the border. Padding and the margin collapse, on an edge, the maximum value of
	 * margin or the border is used. However, negative values are always subtracted. 
	 * Finally, indent offsets the object from edges of its container. This offset is 
	 * calculated after margin and padding is applied and only to the edges touching to 
	 * its container. 
	 * 
	 * This system is quite close to HTML box model, however, there are differences: 
	 * background and overlay can be extended beyond the area, margin collapses with 
	 * padding and there is indent to control distance from edges. All objects have
	 * margin and indent while only containers have extensions, border size, and 
	 * padding.
	 */

	/// Area class that defines an area according to the Box Model. This class does not
	/// represent an object, just the area properties: extension, border size, and 
	/// padding as well as visual aspect: background.
	class Area {
	};

} }