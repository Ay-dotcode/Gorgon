/// @file UI/Template.h This file contains UI component template system.

#pragma once

#include "../Types.h"
#include "../Geometry/Margin.h"
#include "../Geometry/Size.h"
#include "../Event.h"
#include "../Containers/Collection.h"
#include "../Graphics/Drawables.h"
#include "../Graphics/Animations.h"

namespace Gorgon { 
    namespace Graphics {
        class TextRenderer;
    }
    
    /// This namespace contains User interface related functionality.
    namespace UI {


	/**
	 * @page components Components
	 * 
	 * Components are the building blocks of widgets. They are the visual elements
	 * that makes up most of the visual aspect of a widget. Components have templates,
	 * which they use as basis for their placement and drawing. ComponentTemplate is 
	 * the base class for all component templates. Components have component index
	 * and component condition (see ComponentCondition) to control which components
	 * will be visible at a given instance. Components have the same component indices
	 * but only one will be shown at a time. The components that have more specific
	 * condition will have precedence. For instance, if two components have index of 2
	 * and one has condition of ComponentCondition::Always and the other has condition
	 * of ComponentCondition::Focused, first component will be displayed unless the
	 * widget has received focus, in that case the second one will be shown.
	 * 
	 * 
	 * Every widget requires a container component at index 0 as the root. If this
	 * component has a non-0 size, the widget size will be fixed [this might change
	 * in the future].
	 * 
	 * 
	 * Components can be modified by the widget data. This is controlled through data
	 * effect. Various data effects exist to suit different type of widgets. See the
	 * widget documentation for specific data exported by them.
	 * 
	 * See @ref boxmodel to learn how the components are spaced out.
	 * 
	 * 
	 * ### How to create a simple button using components
	 * Create the following templates:
	 * 
	 * @code
	 * ContainerTemplate, index = 0, background = bg image, set size, children = 1, 2
	 * TextholderTemplate, index = 2, set font, data effect = Text
	 * @endcode
	 * 
	 * These two templates will create a very simple button that has the background and
	 * it will display the text that is set by the programmer. If you wish to change the
	 * color of the text when the user moves mouse into the button, add the following
	 * template as well:
	 * 
	 * @code
	 * TextholderTemplate index = 2, set font, data effect = Text, condition = Hover
	 * @endcode
	 * 
	 * This component will override the previous if the mouse is over the button. In order
	 * to change the background when the user clicks the button, add the following:
	 * 
	 * @code
	 * ContainerTemplate, index = 0, background = pressed, set size, condition = Down, 
	 * children = 1, 2
	 * @endcode
	 * 
	 * This will change the background when the user presses the mouse button. Both hover and
	 * down conditions can occur at the same time. If user uses mouse to press the button
	 * they will both be active at the same time, however, if the user uses space bar to
	 * press the button when it is focused, only Down condition will be satisfied. In addition
	 * to these, it is helpful to provide a visual clue when the widget is focused. This
	 * is generally done by adding a focus rectangle like the following:
	 * 
	 * @code
	 * VisualTemplate, index = 1, Drawable = focus rectangle, Positioning = Absolute, 
	 * Size = 100, 100, Unit::Percent, set margin, condition = Focused
	 * @endcode
	 * 
	 * This last piece will only be shown when the button is focused. There will be no errors
	 * when it is invisible even when the container lists this template as its child.
	 * 
	 * 
	 * ###See also:
	 * 
	 * @subpage boxmodel
	 */


	/**
	* @page boxmodel Box Model
	* 
	* Component template defines any component that can be placed on a widget. This includes
	* non-visual elements, fixed and user defined visual elements, and containers.
	* Container components defines an area that can contain more objects and optionally 
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
	* padding of container and there is indent to control distance from edges. 
	* All objects have margin and indent while only containers have extensions, 
	* border size, and padding.
	* 
	* [Might need rewording.]
	*/

	// TextPlaceholder
	// TextPlaceholder should have its own layer (same for others?)
	// Obtained size, position, think about animation frame
	// 

	/// Anchor position
	ENUMCLASS Anchor {
        /// This anchor position should is only used to denote object will
        /// not be anchored to a previous object, only to its parent. If
        /// used as parent anchor it will be ignored and default anchor
        /// will be used instead.
        None = 0,
        
        /// Top left
		TopLeft,
        /// Top center
		TopCenter,
        /// Top right
		TopRight,

        /// Middle left
		MiddleLeft,
        /// Middle center, using this position ensures that the
        /// components will be inside each other.
		MiddleCenter,
        /// Middle right
		MiddleRight,
        
        /// Bottom left
		BottomLeft,
        /// Bottom center
		BottomCenter,
        /// Bottom right
		BottomRight,
        
        /// Baseline left, for text, this aligns to the baseline of
        /// the last line; if there is no text related data, this will
        /// align to the middle left.
		FirstBaselineLeft,
        /// Baseline right, for text, this aligns to the baseline of
        /// the last line; if there is no text related data, this will
        /// align to the middle right.
		FirstBaselineRight,
		
        /// Baseline left, for text, this aligns to the baseline of
        /// the last line; if there is no text related data, this will
        /// align to the bottom left. This mode only works properly if
        /// Sizing is set to automatic.
		LastBaselineLeft,
        
        /// Baseline right, for text, this aligns to the baseline of
        /// the last line; if there is no text related data, this will
        /// align to the bottom right. This mode only works properly if
        /// Sizing is set to automatic.
		LastBaselineRight,
	};

    /// Returns if an anchor is on the left
	inline bool IsLeft(Anchor a) {
		switch(a) {
			case Anchor::TopLeft:
			case Anchor::BottomLeft:
			case Anchor::MiddleLeft:
			case Anchor::FirstBaselineLeft:
			case Anchor::LastBaselineLeft:
				return true;
			default:
				return false;
		}
	}

    /// Returns if an anchor is on the right
	inline bool IsRight(Anchor a) {
		switch(a) {
			case Anchor::TopRight:
			case Anchor::BottomRight:
			case Anchor::MiddleRight:
			case Anchor::FirstBaselineRight:
			case Anchor::LastBaselineRight:
				return true;
			default:
				return false;
		}
	}

    /// Returns if an anchor is centered horizontally
	inline bool IsCenter(Anchor a) {
		switch(a) {
			case Anchor::TopCenter:
			case Anchor::BottomCenter:
			case Anchor::MiddleCenter:
				return true;
			default:
				return false;
		}
	}

	/// Returns if the given anchor is at top
	inline bool IsTop(Anchor a) {
		switch(a) {
			case Anchor::TopRight:
			case Anchor::TopLeft:
			case Anchor::TopCenter:
				return true;
			default:
				return false;
		}
	}

	/// Returns if the given anchor is at bottom
	inline bool IsBottom(Anchor a) {
		switch(a) {
			case Anchor::BottomLeft:
			case Anchor::BottomRight:
			case Anchor::BottomCenter:
			case Anchor::LastBaselineLeft:
			case Anchor::LastBaselineRight:
				return true;
			default:
				return false;
		}
	}

	/// Returns if the given anchor is at middle vertically
	inline bool IsMiddle(Anchor a) {
		switch(a) {
			case Anchor::MiddleLeft:
			case Anchor::MiddleRight:
			case Anchor::MiddleCenter:
			case Anchor::FirstBaselineLeft:
			case Anchor::FirstBaselineRight:
				return true;
			default:
				return false;
		}
	}
	
	/// Types of components, see respective classes for details.
	ENUMCLASS ComponentType{
		Placeholder,
		Textholder,
		Graphics,
		Container,
	};

    /// Dimension data for components. Allows relative position and sizing.
	class Dimension {
	public:

		/// Unit for dimensions. Dimensions in UI system does not allow floating
		/// point numbers as floating point numbers are not precise and may cause
		/// issues. Additionally, final values always rounded, so that the symbols
		/// are always on full pixels.
		enum Unit {
			/// Fixed pixel based dimensions
			Pixel,

			/// Dimension will be relative to the parent and given in percent.
			/// If higher resolution is necessary use BasisPoint.
			Percent,

			/// 1/1000th of a pixel, there are only few places that this will be used.
			/// Currently only rotation center use non-integer pixels
			MilliPixel,

			/// Dimension will be relative to the parent and given in 1/10000.
			BasisPoint,

			/// Dimension will be relative to the text size, given value is the
			/// percent of the width of an EM dash. If no font information is 
			/// found, 10px will be used for EM dash. Thus, 1 unit will be 0.1
			/// pixels.
			EM,
            
            //todo add line height
		};

		/// Constructs a new dimension or type casts integer to dimension		 
		Dimension(int value = 0, Unit unit = Pixel) : value(value), unit(unit) {/* implicit */
		}

		/// Returns the calculated dimension in pixels
		int operator ()(int parentwidth, int emwidth = 10) const {
			return Calculate(parentwidth, emwidth);
		}

		/// Returns the calculated dimension in pixels
		int Calculate(int parentwidth, int emwidth = 10) const {
			switch(unit) {
				case Percent:
					return int(std::round((double)value * parentwidth / 100));
				case MilliPixel:
					return int(std::round((double)value / 1000));
				case BasisPoint:
					return int(std::round((double)value * parentwidth / 10000));
				case EM:
					return int(std::round(value * emwidth / 100));
				case Pixel:
				default:
					return value;
			}
		}

		/// Returns the calculated dimension in pixels
		float CalculateFloat(float parentwidth, float emwidth = 10) const {
			switch(unit) {
				case Percent:
					return (float)value * parentwidth / 100.f;
				case BasisPoint:
					return (float)value * parentwidth / 10000.f;
				case MilliPixel:
					return (float)value / 1000;
				case EM:
					return (float)value * emwidth / 100.f;
				case Pixel:
				default:
					return (float)value;
			}
		}

		/// Returns the value of the dimension, should not be considered as
		/// pixels
		int GetValue() const {
			return value;
		}

		/// Returns the unit of the dimension
		Unit GetUnit() const {
			return unit;
		}

		/// Changes the value of the dimension without modifying the units
		void Set(int value) {
			this->value = value;
		}

		/// Changes the value and unit of the dimension.
		void Set(int value, Unit unit) {
			this->value = value;
			this->unit = unit;
		}

	private:
		int value;
		Unit unit;
	};

	/// This class stores the location information for a box object
	using Point = Geometry::basic_Point<Dimension>;

	/// This class stores the size information for a box object
	using Size = Geometry::basic_Size<Dimension>;

	/// This class stores the margin information for a box object
	using Margin = Geometry::basic_Margin<Dimension>;
    
    /// Converts a dimension based point to pixel based point
    inline Geometry::Point Convert(const Point &p, const Geometry::Size &parent, int emwidth = 10) {
        return {p.X(parent.Width, emwidth), p.Y(parent.Height, emwidth)};
    }
    
    /// Converts a dimension based size to pixel based size
    inline Geometry::Size Convert(const Size &s, const Geometry::Size &parent, int emwidth = 10) {
        return {s.Width(parent.Width, emwidth), s.Height(parent.Height, emwidth)};
    }
    
    /// Converts a dimension based margin to pixel based margin
    inline Geometry::Margin Convert(const Margin &m, const Geometry::Size &parent, int emwidth = 10) {
        return {m.Left(parent.Width, emwidth), m.Top(parent.Height, emwidth), m.Right(parent.Width, emwidth), m.Bottom(parent.Height, emwidth), };
    }

	/// Controls the condition when the components are visible.
    /// Components with the same ID will replace a previous one. If there is a
    /// condition that is satisfied, the component at the specific index will 
    /// be replaced with that one, otherwise, if a component with condition
    /// always exists, it will be used.
	ENUMCLASS ComponentCondition {
        /// Component is always active
		Always,
        
        /// Component is always disabled
		Never, 
        
        None = Never,

        /// Component is visible when the widget is disabled.
		Disabled,

        
		Focused,

        
		Hover,
        
		Down,
		

		State2,

		State3,

        State4,

        /// This condition is triggered when the widget is opened like a combobox
        /// showing its list part. For widgets which are "opened" by default like 
		/// window, this condition will never be satisfied. Instead of this condition, 
		/// use Always for the base state.
        Opened,

		/// This condition is triggered when the widget is closed like a 
		/// tree view item that is folded, or a window that is rolled.
		/// For widgets which are "closed" by default, like combobox, this 
		/// condition will never be satisfied. Instead of this condition, use Always 
		/// for the base state.
		Closed,
        
        Active,
        
        HScroll,
        
        VScroll,
        
        HVScroll,

		/// Channel 1 value is 0, the value will be
		/// rounded to 4 decimal points before comparison
		Ch1V0,

		/// Channel 1 value is 0.5, the value will be
		/// rounded to 4 decimal points before comparison
		Ch1V05,

		/// Channel 1 value is 1, the value will be
		/// rounded to 4 decimal points before comparison
		Ch1V1,

		/// Channel 2 value is 0, the value will be
		/// rounded to 4 decimal points before comparison
		Ch2V0,

		/// Channel 2 value is 0.5, the value will be
		/// rounded to 4 decimal points before comparison
		Ch2V05,

		/// Channel 2 value is 1, the value will be
		/// rounded to 4 decimal points before comparison
		Ch2V1,

		/// Channel 3 value is 0, the value will be
		/// rounded to 4 decimal points before comparison
		Ch3V0,

		/// Channel 3 value is 0.5, the value will be
		/// rounded to 4 decimal points before comparison
		Ch3V05,

		/// Channel 3 value is 1, the value will be
		/// rounded to 4 decimal points before comparison
		Ch3V1,

		/// Channel 4 value is 0, the value will be
		/// rounded to 4 decimal points before comparison
		Ch4V0,

		/// Channel 4 value is 0.5, the value will be
		/// rounded to 4 decimal points before comparison
		Ch4V05,

		/// Channel 4 value is 1, the value will be
		/// rounded to 4 decimal points before comparison
		Ch4V1,


        /// Do not use this condition, this is to size the arrays.
        Max,
	};
    
    inline bool IsMouseRelated(ComponentCondition condition) {
        switch(condition) {
            case ComponentCondition::Hover:
            case ComponentCondition::Down:
				return true;
            default:
                return false;
        }
    }
    
	
	class ComponentTemplate;
	class PlaceholderTemplate;
    class TextholderTemplate;
    class GraphicsTemplate;
    class ContainerTemplate;

    /**
     * This class stores visual information about a widget template.
     */
	class Template {
	public:

		/// Size mode of the template, will be applied separately to X and Y dimensions
		enum SizeMode {
			/// This template can be freely resized
			Free,

			/// Size of the template is fixed and should not be modified
			Fixed,

			/// Size of the template should be incremented at the multiples of the size
			/// and addition size should be added to it.
			Multiples
		};
        
        Template() {
        }

        /// Destructor
        ~Template() {
            components.Destroy();
        }

        /// This will create a new placeholder and return it. The ownership will stay
        /// with the template. Index is the component index not the order in the template.
        PlaceholderTemplate &AddPlaceholder(int index, ComponentCondition cond) { return AddPlaceholder(index, cond, ComponentCondition::None); }

        /// This will create a new placeholder and return it. The ownership will stay
        /// with the template. Index is the component index not the order in the template.
        PlaceholderTemplate &AddPlaceholder(int index, ComponentCondition from, ComponentCondition to);

        /// This will create a new textholder and return it. The ownership will stay
        /// with the template. Index is the component index not the order in the template.
        TextholderTemplate &AddTextholder(int index, ComponentCondition cond) { return AddTextholder(index, cond, ComponentCondition::None); }

        /// This will create a new textholder and return it. The ownership will stay
        /// with the template. Index is the component index not the order in the template.
        TextholderTemplate &AddTextholder(int index, ComponentCondition from, ComponentCondition to);

        /// This will create a new drawable and return it. The ownership will stay
        /// with the template. Index is the component index not the order in the template.
		GraphicsTemplate &AddGraphics(int index, ComponentCondition cond) { return AddGraphics(index, cond, ComponentCondition::None); }

        /// This will create a new drawable and return it. The ownership will stay
        /// with the template. Index is the component index not the order in the template.
		GraphicsTemplate &AddGraphics(int index, ComponentCondition from, ComponentCondition to);

        /// This will create a new drawable and return it. The ownership will stay
        /// with the template. Index is the component index not the order in the template.
		ContainerTemplate &AddContainer(int index, ComponentCondition cond) { return AddContainer(index, cond, ComponentCondition::None); }

        /// This will create a new drawable and return it. The ownership will stay
        /// with the template. Index is the component index not the order in the template.
		ContainerTemplate &AddContainer(int index, ComponentCondition from, ComponentCondition to);
        
        /// Removes the component at the given index.
        void Remove(int index) {
            components.Delete(index);
            ChangedEvent();
        }
        
        /// Releases the component and its ownership
        ComponentTemplate &Release(int index);
        
        /// Adds a previously created component to the template, the ownership of
        /// the object is transferred to the template object.
        void Assume(ComponentTemplate &component);
        
        /// Returns the number of components in the template
        int GetCount() const {
            return components.GetCount();
        }
        
        /// Returns the component at the given index. This index is not component
        /// index, but rather the location of the component in the template.
        ComponentTemplate &Get(int index) const {
            return components[index];
        }
        
        /// Returns the component at the given index. This index is not component
        /// index, but rather the location of the component in the template.
        ComponentTemplate &operator[](int index) const {
            return components[index];
        }

		/// Changes the size mode of the template
		void SetSizing(SizeMode x, SizeMode y) {
			xsizing = x;
			ysizing = y;
			ChangedEvent();
		}

		/// Returns the sizing mode
		SizeMode GetXSizing() const {
			return xsizing;
		}

		/// Returns the sizing mode
		SizeMode GetYSizing() const {
			return ysizing;
		}

		/// Changes the size of the template
		void SetSize(int w, int h) {
			size = {w, h};
			ChangedEvent();
		}

		/// Changes the size of the template
		void SetSize(Geometry::Size value) {
			size = value;
			ChangedEvent();
		}

		/// Returns the size of the template
		Geometry::Size GetSize() const {
			return size;
		}

		/// Returns the size of the template
		int GetWidth() const {
			return size.Width;
		}

		/// Returns the size of the template
		int GetHeight() const {
			return size.Height;
		}

		/// Changes the additional size of the template. This value should only be set and
		/// used if the size mode is multiple
		void SetAdditionalSize(int w, int h) {
			additional = {w, h};
			ChangedEvent();
		}

		/// Changes the additional size of the template. This value should only be set and
		/// used if the size mode is multiple
		void SetAdditionalSize(Geometry::Size value) {
			additional = value;
			ChangedEvent();
		}

		/// Returns the additional size. This value should only be set and
		/// used if the size mode is multiple
		Geometry::Size GetAdditionalSize() const {
			return additional;
		}
		
		/// Changes the duration of a component condition. Durations on final conditions are 
		/// ignored, only transition condition durations are used. Duration is in milliseconds
		void SetConditionDuration(ComponentCondition from, ComponentCondition to, int duration) {
            durations[{from, to}] = duration;
            ChangedEvent();
        }
        
        /// Returns the duration of the component condition
        int GetConditionDuration(ComponentCondition from, ComponentCondition to) const {
            if(durations.count({from, to}))
                return durations.at({from, to});
            else
                return 0;
        }


        /// This event is fired whenever template or its components are changed.
        Event<Template> ChangedEvent = Event<Template>{*this};
        
	private:
		Containers::Collection<ComponentTemplate> components;
        std::vector<Event<ComponentTemplate>::Token> tokens;
        std::map<std::pair<ComponentCondition, ComponentCondition>, int> durations;

		SizeMode xsizing, ysizing;
		Geometry::Size size;
		Geometry::Size additional;
	};

	/// Defines an object according to the Box Model.
	class ComponentTemplate {
	public:
        
        /// Destructor
        virtual ~ComponentTemplate() { }

		/// Controls where the component will be placed.
		enum PositionType {

			/// Component will be placed relative to the previous component
			Relative,

			/// Absolute positioning, coordinates will start from the container
			Absolute,
            
            /// The given coordinates are polar coordinates, The radius is given 
            /// in pixels and angle is specified in degrees. Parent's center point
            /// is used as pole. X component is used as radius and Y component is
            /// used as angle.
            PolarAbsolute,
		};

		/// Controls how the size is affected from the contents of the object
		enum SizingMode {
			/// The given size is absolute, it is not affected by the contents
			Fixed,

			/// Given size is not used, object is sized to its contents
			Automatic,

			/// Given size is the minimum, if the contents are bigger, the object
			/// be resized to fit
			GrowOnly,

			/// Given size is the maximum, if the contents are smaller, the object
			/// be resized
			ShrinkOnly
		};

		/// Which property will the data of the widget affect. 
		enum DataEffect {
			/// Nothing will be affected
			None,

			/// Works only for TextholderTemplate, data will affect the text that is displayed.
			Text,

			ValueText1,

			ValueText2,

			ValueText3,

			ValueText4,

			Title,

			Label,
			
			State1Text,
			State2Text,
			State3Text,
			State4Text,

			/// Data will effect the displayed graphics
            Icon,
			Icon1 = Icon,
			Icon2, 
			State1Icon,
			State2Icon,
			State3Icon,
			State4Icon,
		};
        
        /// Which property will the value of the widget affect. It will be scaled between
		/// valuemin and valuemax.
        enum ValueModification {
            /// Nothing will be modified
            NoModification,
            
			/// Position of this component will be affected by the data. Data will be
			/// given as a percent and will modify Position property. Useful for sliders,
			/// scrollbars and progress bars. The direction of the container is used to 
            /// determine which axis will get affected if only one channel is used.
			ModifyPosition,
            
            /// Modifies the X coordinate of the component regardless of the container
            /// direction.
            ModifyX,
            
            /// Modifies the Y coordinate of the component regardless of the container
            /// direction.
            ModifyY,
            
            /// Value modifies the opacity of the component. If used on a container, it will effect
            /// all components in that container
            ModifyAlpha,
            
            /// Value modifies the color of the component. If used on a container, it will tint
            /// all components in that container
            ModifyColor, 
            
			/// Data affects the rotation of the component. Rotation angle will start from 0 degrees
			/// for 0, 360 degrees for 1. You may use min and max to modify this. For instance, if max
			/// is set to 0.5, the degrees will go up to 180. For now, this effect is not in use.
			ModifyRotation,

			/// Data will affect the frame of the animation. Will only work for Objects
			/// with animations. For now, this effect is disabled.
			Frame,

			/// Size of this component will be affected. Data will be given as a percent
			/// and will modify Size property. Useful for sliders and progress bars. The direction
            /// of the container is used to determine which axis will get affected.
			ModifySize,
        };
        
        /// Which data channels should be used as the value, common combinations are listed, however, all
        /// combinations are valid except when they are used for mouse mapping. Only the values listed here
        /// that use two channels will work fully with mouse mapping. LCh is for circular La*b* color system.
        /// Color can also be mapped to coordinate system. Particularly, CH can be mapped to polar coordinates 
        /// to create a color map. LCh color system is not yet working
        enum ValueSource {
            UseFirst = 1,
            UseX = UseFirst,
            UseWidth = UseFirst,
            /// Red or radius for polar coordinates
            UseR = UseFirst,
            
            UseSecond = 2,
            UseY = UseSecond,
            UseHeight = UseSecond,
            UseG = UseSecond,
            /// Theta for polar coordinates
            UseT = UseSecond,
            
            UseThird = 4,
            UseZ = UseThird,
            UseB = UseThird,
            
            UseFourth = 8,
            UseA = UseFourth,
			UseW = UseFourth,

			/// Grayscale value of color
			UseGray = 16,

			/// Lightness
			UseL = 32,
            
            /// Hue
            UseH = 64, 
            
            /// Chromacity
            UseC = 128,
            
            /// This channel will give the progress of a transition. It will be set
            /// to 1 for non-transitional components. Use this channel alone, do not
            /// combine it with others.
            UseTransition = 0x10000,

			/// Maximum power of two
			ValueSourceMaxPower = 7,
            
            UseXY   = UseFirst | UseSecond,
            UseSize = UseFirst | UseSecond,
            UseRG   = UseFirst | UseSecond,
            
            UseYZ = UseSecond | UseThird,
            UseGB = UseSecond | UseThird,
            
            UseXZ = UseFirst | UseThird,
            UseRB = UseFirst | UseThird,
            
            UseRA = UseFirst  | UseFourth,
            UseBA = UseSecond | UseFourth,
            UseGA = UseThird  | UseFourth,
            
            UseXYZ = UseFirst | UseSecond | UseThird,
            UseRGB = UseFirst | UseSecond | UseThird,

            UseRGA = UseFirst  | UseSecond | UseFourth,
            UseRBA = UseFirst  | UseThird  | UseFourth,
            UseGBA = UseSecond | UseThird  | UseFourth,
            
            UseLH = UseL | UseH,
			UseLC = UseL | UseC,
			UseCH = UseC | UseH,

			UseGrayAlpha = UseGray | UseA,

            UseLCH  = UseL | UseH | UseC,
            UseLCHA = UseL | UseH | UseC | UseA,
            
            UseRGBA = UseFirst | UseSecond | UseThird | UseFourth,
        };
        
        /// Tags mark a component to be modified in a way meaningful to specific widgets. Components can be
        /// queried for their size and positions from the component stack using their tag.
        enum Tag {
            NoTag,
            
            /// Do not use this tag for regular components, it is used to identify substacks without a tag
            UnknownTag,
            TickTag,
            HScrollTag,
            VScrollTag,
            DragTag,
            DragBarTag,
            XDragTag,
            YDragTag,
            MinimapTag,
            IncrementTag,
            DecrementTag,
            LeftTag,
            RightTag,
            TopTag,
            BottomTag,
            ExpandTag,
            ToggleTag,
        };
        
        /// Some components are repeated along some axis, this property controls how they will be
        /// repeated. Use X direction if there is no direction. Y is the angular direction on
        /// polar systems. Repeated components will have their values set to the values specified
        /// by their position.
        enum RepeatMode {
            NoRepeat       = 0,

            Minor           = 1,
            Major           = 2,

            XGrid           = 4,
            YGrid           = 8,

            XTick           = 16,
            YTick           = 20,
            
            XMinorGrid      = 5,
            YMinorGrid      = 9,
            XMajorGrid      = 6,
            YMajorGrid      = 10,
            
            XMinorTick      = 17,
            YMinorTick      = 21,
            XMajorTick      = 18,
            YMajorTick      = 22,
        };

		/// Returns the type of the component.
		virtual ComponentType GetType() const noexcept = 0;
        
        /// Changes the coordinates of the component to the given position. 
		void SetPosition(int x, int y, Dimension::Unit unit = Dimension::Pixel) { position = {{x, unit}, {y, unit}}; ChangedEvent(); }

        /// Changes the coordinates of the component to the given position. 
		void SetPosition(Geometry::Point pos, Dimension::Unit unit = Dimension::Pixel) { position = {{pos.X, unit}, {pos.Y, unit}}; ChangedEvent(); }

        /// Changes the coordinates of the component to the given position. 
		void SetPosition(Dimension x, Dimension y) { position = {x, y}; ChangedEvent(); }

        /// Changes the coordinates of the component to the given position. 
		void SetPosition(Point value) { position = value; ChangedEvent(); }

        /// Changes the positioning method of the component.
		void SetPositioning(PositionType value) { positioning = value; ChangedEvent(); }

		/// Returns the current position of the component. This value is *not* absolute final position as
		/// it cannot be determined before the component is rendered in a widget.
		Point GetPosition() const { return position; }

		/// Returns the positioning method of the component
		PositionType GetPositioning() const { return positioning; }
		
		/// Changes the center coordinate that will be used in rotation
		void SetCenter(int x, int y, Dimension::Unit unit = Dimension::Pixel) { center = {{x, unit}, {y, unit}}; ChangedEvent(); }

		/// Changes the center coordinate that will be used in rotation
		void SetCenter(Geometry::Point pos, Dimension::Unit unit = Dimension::Pixel) { center = {{pos.X, unit}, {pos.Y, unit}}; ChangedEvent(); }

		/// Changes the center coordinate that will be used in rotation
		void SetCenter(Dimension x, Dimension y) { center = {x, y}; ChangedEvent(); }

		/// Changes the center coordinate that will be used in rotation
		void SetCenter(Point value) { center = value; ChangedEvent(); }
		
		/// Returns the center point that would be used for rotation
		Point GetCenter() const { return center; }



        /// Changes the size of the component. The given values are ignored if the sizing mode is Automatic.
		void SetSize(int w, int h, Dimension::Unit unit = Dimension::Pixel) { SetSize({{w, unit}, {h, unit}}); }

        /// Changes the size of the component. The given values are ignored if the sizing mode is Automatic.
		void SetSize(Geometry::Size size, Dimension::Unit unit = Dimension::Pixel) { SetSize({{size.Width, unit}, {size.Height, unit}}); }

        /// Changes the size of the component. If sizing mode is automatic, it will be set to fixed.
		void SetSize(Dimension w, Dimension h) { SetSize({w, h}); }

        /// Changes the size of the component. If sizing mode is automatic, it will be set to fixed.
		void SetSize(Size size) { 
            this->size = size; 
            if(sizing == Automatic)
                sizing = Fixed;
            ChangedEvent(); 
        }

		/// Changes the sizing mode of the component.
		void SetSizing(SizingMode value) { sizing = value; ChangedEvent(); }

		/// Returns the size of the component. This value is *not* absolute final size as
		/// it cannot be determined before the component is rendered in a widget.
		Size GetSize() const { return size; }

		/// Returns the sizing mode of the component.
		SizingMode GetSizing() const { return sizing; }


        /// Changes the margin of the component. Margin is the minimum spacing around the component. Negative 
        /// margin is possible and will always be subtracted from the other component's margin.
		void SetMargin(int value, Dimension::Unit unit = Dimension::Pixel) { margin = {{value, unit}}; ChangedEvent(); }

        /// Changes the margin of the component. Margin is the minimum spacing around the component. Negative 
        /// margin is possible and will always be subtracted from the other component's margin.
		void SetMargin(int hor, int ver, Dimension::Unit unit = Dimension::Pixel) { margin = {{hor, unit}, {ver, unit}}; ChangedEvent(); }

        /// Changes the margin of the component. Margin is the minimum spacing around the component. Negative 
        /// margin is possible and will always be subtracted from the other component's margin.
		void SetMargin(int left, int top, int right, int bottom, Dimension::Unit unit = Dimension::Pixel) { 
            margin = {{left, unit}, {top, unit}, {right, unit}, {bottom, unit}}; 
            ChangedEvent(); 
        }

        /// Changes the margin of the component. Margin is the minimum spacing around the component. Negative 
        /// margin is possible and will always be subtracted from the other component's margin.
		void SetMargin(Geometry::Margin value, Dimension::Unit unit = Dimension::Pixel) { 
            margin = {{value.Left, unit}, {value.Top, unit}, {value.Right, unit}, {value.Bottom, unit}}; 
            ChangedEvent(); 
        }

        /// Changes the margin of the component. Margin is the minimum spacing around the component. Negative 
        /// margin is possible and will always be subtracted from the other component's margin.
		void SetMargin(Dimension value) { margin = {value}; ChangedEvent(); }

        /// Changes the margin of the component. Margin is the minimum spacing around the component. Negative 
        /// margin is possible and will always be subtracted from the other component's margin.
		void SetMargin(Dimension hor, Dimension ver) { margin = {hor, ver}; ChangedEvent(); }

        /// Changes the margin of the component. Margin is the minimum spacing around the component. Negative 
        /// margin is possible and will always be subtracted from the other component's margin.
		void SetMargin(Dimension left, Dimension top, Dimension right, Dimension bottom) { margin = {left, top, right, bottom}; ChangedEvent(); }

        /// Changes the margin of the component. Margin is the minimum spacing around the component. Negative 
        /// margin is possible and will always be subtracted from the other component's margin.
		void SetMargin(Margin value) { margin = value; ChangedEvent(); }

		/// Returns the margin.
		Margin GetMargin() const { return margin; }


        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
		void SetIndent(int value, Dimension::Unit unit = Dimension::Pixel) { margin = {{value, unit}}; ChangedEvent(); }

        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
		void SetIndent(int hor, int ver, Dimension::Unit unit = Dimension::Pixel) { margin = {{hor, unit}, {ver, unit}}; ChangedEvent(); }

        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
		void SetIndent(int left, int top, int right, int bottom, Dimension::Unit unit = Dimension::Pixel) { 
            margin = {{left, unit}, {top, unit}, {right, unit}, {bottom, unit}}; 
            ChangedEvent(); 
        }

        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
		void SetIndent(Geometry::Margin value, Dimension::Unit unit = Dimension::Pixel) { 
            margin = {{value.Left, unit}, {value.Top, unit}, {value.Right, unit}, {value.Bottom, unit}}; 
            ChangedEvent(); 
        }

        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
		void SetIndent(Dimension value) { margin = {value}; ChangedEvent(); }

        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
		void SetIndent(Dimension hor, Dimension ver) { margin = {hor, ver}; ChangedEvent(); }

        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
		void SetIndent(Dimension left, Dimension top, Dimension right, Dimension bottom) { margin = {left, top, right, bottom}; ChangedEvent(); }

        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
		void SetIndent(Margin value) { margin = value; ChangedEvent(); }

        /// Returns the current indent.
		Margin GetIndent() const { return indent; }



		/// Sets the data effect for this component. Default is None.
		void SetDataEffect(DataEffect effect) {
            dataeffect = effect;
            
            ChangedEvent(); 
		}

		/// Returns how the data will affect this component
		DataEffect GetDataEffect() const { return dataeffect; }
        
		/// Changes the ordering of the values. This allows swaps like X-Y. 
		void SetValueOrdering(int first, int second, int third, int fourth) {
            valueordering = {{first, second, third, fourth}};
            
            ChangedEvent();
        }
        
        /// Returns the ordering of value channels.
        std::array<int, 4> GetValueOrdering() const {
            return valueordering;
        }
		
		/// Sets the property that will be affected by the value of the widget. Default is NoModification. 
		/// If min and max is specified incoming value will be scaled accordingly. 
		void SetValueModification(ValueModification mod, ValueSource source = UseFirst, std::array<float, 4> min = {{0, 0, 0, 0}}, std::array<float, 4> max = {{1, 1, 1, 1}}) {
            valuemod = mod;
            valuemin = min;
            valuemax = max;
			this->source = source;
            
            ChangedEvent(); 
        }

		/// Changes the data range, which scales the data effect on the component. Not all effects
		/// are affected by the range.
		void SetValueRange(std::array<float, 4> min, std::array<float, 4> max) {
			valuemin = min;
			valuemax = max;

			ChangedEvent();
		}

		/// Changes the data range, which scales the data effect on the component. Not all effects
		/// are affected by the range.
		void SetValueRange(int channel, float min, float max) {
			ASSERT(channel>=0 && channel<4, "Channel index out of bounds");

			valuemin[channel] = min;
			valuemax[channel] = max;

			ChangedEvent();
		}

        /// Returns which property of this component will be modified by the value
		ValueModification GetValueModification() const { return valuemod; }
		
		/// Returns the value source that will be used
		void SetValueSource(ValueSource value) {
            source = value;
            
            ChangedEvent(); 
        }
        
        /// Returns the value source that will be used
        ValueSource GetValueSource() const {
            return source;
        }
        
        /// Changes the tag of this component
        void SetTag(Tag value) {
            tag = value;
            
            ChangedEvent(); 
        }
        
        /// Returns the tag of the component
        Tag GetTag() const {
            return tag;
        }
        
        /// Changes the repeat mode of this component. If the consumer of the component
        /// does not know about this repeat mode, this component will be ignored
        void SetRepeatMode(RepeatMode value) {
            repeat = value;
            
            ChangedEvent();
        }
        
        /// Returns the repeat mode of this component.
        RepeatMode GetRepeatMode() const {
            return repeat;
        }
        

		/// Returns the value scale minimum.
		std::array<float, 4> GetValueMin() const { return valuemin; }

		/// Returns the range of the value scale.
		std::array<float, 4> GetValueRange() const { return {{valuemax[0]-valuemin[0], valuemax[1]-valuemin[1], valuemax[2]-valuemin[2], valuemax[3]-valuemin[3]}}; }

		/// Returns the value scale maximum.
		std::array<float, 4> GetValueMax() const { return valuemax; }

		/// Returns the value scale minimum.
		float GetValueMin(int channel) const {
			ASSERT(channel>=0 && channel<4, "Channel index out of bounds");

			return valuemin[channel];
		}
		
		/// Returns the range of the value scale.
		float GetValueRange(int channel) const {
			ASSERT(channel>=0 && channel<4, "Channel index out of bounds");

			return valuemax[channel]-valuemin[channel];
		}

		/// Returns the value scale maximum.
		float GetValueMax(int channel) const {
			ASSERT(channel>=0 && channel<4, "Channel index out of bounds");

			return valuemax[channel];
		}


        /// Changes the anchor of the component to the given values.
		void SetAnchor(Anchor previous, Anchor container, Anchor my) { 
            this->previous = previous; 
            this->container = container;
            this->my = my; 
            
            ChangedEvent(); 
        }

        /// Returns the anchor point of the previous component that this component will attach to. This value
        /// will be used if this component attaches to another of its siblings.
		Anchor GetPreviousAnchor() const { return previous; }
		
        /// Returns the anchor point of the container that this component will attach to. This value will be
        /// used if this component attaches to its container.
		Anchor GetContainerAnchor() const { return container; }

        /// Returns the anchor point of this component.
		Anchor GetMyAnchor() const { return my; }



		/// Changes the index of the current component. Only one component can be rendered at an index which is
		/// determined by the component condition. See component indexing for more information.
		void SetIndex(int value) { index = value; ChangedEvent(); } 

		/// Returns the component index.
		int GetIndex() const { return index; }



		/// Sets the condition when this component will be visible. The visibility also depens on whether there
		/// are other visible components at the same component index. If that is the case, most specific condition
		/// will be rendered.
		void SetCondition(ComponentCondition value) { condition = value; condition_to = ComponentCondition::None; ChangedEvent(); }

		/// Sets the condition when this component will be visible. This variant will create a transition condition.
		void SetCondition(ComponentCondition from, ComponentCondition to) { condition = from; condition_to = to; ChangedEvent(); }

		/// Returns the current component condition
		ComponentCondition GetCondition() const { return condition; }
		
		/// Returns the target component condition
		ComponentCondition GetTargetCondition() const { return condition_to; }
		
		/// Returns whether this component is a transition component
		bool IsTransition() const { return condition_to != ComponentCondition::None; }
		
		/// Sets whether the component transition can be reversed. Can be used to simplify from - to - from animations.
		void SetReversible(bool value) { reversible = value; ChangedEvent(); }
		
		/// Returns whether this component transition can be reversed.
		bool IsReversible() const { return reversible; }
		
		
		/// Whether to clip the contents of this container, default value is false. Due to shadows, it is advicable
		/// not to set clipping on the outer most container. Activating clipping creates a new layer for the 
		/// component, requiring additional memory. If clipping is on, a component cannot be drawn more than once
		/// in different containers.
		void SetClip(bool value) {
            clip = value;
        }
        
        /// Returns whether currently clipping the contents
        bool GetClip() const {
            return clip;
        }


		/// This event will be fired whenever any property is changed. Can be used to update widget
		/// automatically.
		Event<ComponentTemplate> ChangedEvent = Event<ComponentTemplate>{*this};

	protected:
        
        /// If set to true, will clip the contents of the component to the bounds.
        bool clip = false;
        
        bool reversible = false;
        
        
        /// Condition when this component will be visible
		ComponentCondition condition = ComponentCondition::Always;
        
        /// Condition when this component will be visible, setting condition_to will
        /// create a transition component.
		ComponentCondition condition_to = ComponentCondition::None;

        /// The effect that the data will have on this component
		DataEffect dataeffect = None;
        
        /// The property of the component that will be affected by the value
		ValueModification valuemod = NoModification;
        
        /// The value that will be used for this component
        ValueSource source = UseFirst;
        
        /// Changes the ordering of the value source
        std::array<int, 4>   valueordering{0,1,2,3};

        /// If required, can be used to scale incoming data
		std::array<float, 4> valuemin = {{0, 0, 0, 0}}, valuemax = {{1, 1, 1, 1}};
        
        /// Tag identifies a component for various modifications depending on the
        /// widget.
        Tag tag = NoTag;
        
        /// Whether the component will be repeated along an axis. If an item will
        /// 
        RepeatMode repeat = NoRepeat;
        
        /// Positioning mode
		PositionType positioning = Relative;

        /// Position of the component
		Point position = {0, 0};

        /// Sizing mode
		SizingMode sizing = Fixed;

		/// Size of the object.
		Size size = {0, 0};

		/// Margin around the object, will be collapsed with other object margins
		/// and padding
		Margin margin = {0};

		/// Indent is added to the margin and padding on the edge of the container
		Margin indent = {0};
        
        /// Center point for rotation
        Point center = {0, 0};

        /// Anchor point of the previous component that this component will be attached
        /// to. If the component positioning is absolute or this is the first component, 
        /// it will be anchored to the container and parent anchor point will be used.
		Anchor previous = Anchor::FirstBaselineRight;
        
        /// Anchor point of the container that this component will be attached to, if it
        /// is attaching to its parent.
		Anchor container = Anchor::MiddleLeft;

        /// Anchor point of the current component. This point will be matched to the
        /// previous component's anchor point
		Anchor my = Anchor::FirstBaselineLeft;

        /// Component index. Only one component can exist for a specific index position.
        /// The ordering and visibility of the components will be determined from the condition.
        /// Components will be laid out according to this index. Z-ordering is performed using
        /// the order of components in container.
		int index = 0;
	};

	inline ComponentTemplate::ValueSource operator | (ComponentTemplate::ValueSource l, ComponentTemplate::ValueSource r) {
		return ComponentTemplate::ValueSource((int)l | (int)r);
	}

	/// Defines a placeholder according to the Box Model. Placeholder is replaced with
	/// a visual component. Default sizing mode for a placeholder is Automatic.
	class PlaceholderTemplate : public ComponentTemplate {
	public:
        PlaceholderTemplate() {
            sizing = Automatic;
        }

		/// Returns the type of the component.
		virtual ComponentType GetType() const noexcept override {
			return ComponentType::Placeholder;
		}
		
		/// Sets the sub template for this placeholder.
		void SetTemplate(const Template &value) {
			temp = &value;
			ChangedEvent();
		}
        
		/// Returns if this placeholder has a sub template
        bool HasTemplate() const {
			return temp != nullptr;
		}
        
		/// Returns the sub template of this placeholder. If the template does 
		/// not exists, assertation violation will be fired, thus it is advisable
		/// to check if the template exists using HasTemplate function.
        const Template &GetTemplate() const {
			ASSERT(temp, "Template is empty.");
			return *temp;
		}
		
    private:
        const Template *temp = nullptr;
	};

	class TextholderTemplate : public ComponentTemplate {
	public:
        

		/// Returns the type of the component.
		virtual ComponentType GetType() const noexcept override {
			return ComponentType::Textholder;
		}
		
		/// Returns if this text holder can perform rendering
		bool IsReady() const;
		
		/// Changes the renderer
		void SetRenderer(const Graphics::TextRenderer &value) {
            renderer = &value;
        }
        
        /// Returns the renderer for this textholder
        const Graphics::TextRenderer &GetRenderer() const {
            ASSERT(renderer, "Renderer is not set.");
            
            return *renderer;
        }
    
    private:
        const Graphics::TextRenderer *renderer = nullptr;
	};

	class VisualProvider {
	public:
		VisualProvider(Event<ComponentTemplate> &changed) : changed(&changed) { }

		VisualProvider(Event<ComponentTemplate> &changed, const Graphics::AnimationProvider &content) :
			changed(&changed), provider(&content) 
		{ }

		VisualProvider(Event<ComponentTemplate> &changed, const Graphics::Drawable &content) : 
			changed(&changed), drawable(&content) 
		{ }

		/// If this drawable has any content
		bool HasContent() const {
			return drawable != nullptr || provider != nullptr;
		}

		/// Of this drawable template has animation content
		bool IsAnimation() const {
			return provider != nullptr;
		}

		/// Of this drawable template has drawable content
		bool IsDrawable() const {
			return drawable != nullptr;
		}

		/// Returns the content as animation provider
		const Graphics::AnimationProvider &GetAnimation() const {
			if(provider == nullptr) {
				throw std::runtime_error("Animation not set.");
			}

			return *provider;
		}

		/// Returns the content as a drawable
		const Graphics::Drawable &GetDrawable() const {
			if(drawable == nullptr) {
				throw std::runtime_error("Drawable not set.");
			}

			return *drawable;
		}

		const Graphics::Drawable &Instantiate(Animation::ControllerBase &controller) const {
			if(drawable)
				return *drawable;
			else if(provider) {
				return provider->CreateAnimation(controller);
			}
			else {
				throw std::runtime_error("Visual provider is empty.");
			}
		}

		/// Sets the content from a drawable
		void SetDrawable(const Graphics::Drawable &value) {
			drawable = &value;
			provider = nullptr;

			(*changed)();
		}

		/// Sets the content from an animation provider
		void SetAnimation(const Graphics::AnimationProvider &value) {
			drawable = nullptr;
			provider = &value;

			(*changed)();
		}

	private:
		Event<ComponentTemplate> *changed;
        
		const Graphics::Drawable *drawable = nullptr;
		const Graphics::AnimationProvider *provider = nullptr;
	};

	/// Defines a visual component. RelativeToContents sizing mode is selected if no drawable 
	/// is supplied, the relative size will be taken as 0x0. If the drawable has no size
	/// information, object will be treated as RelativeToContainer and size will be taken
	/// as 100%. This component can either work with animation providers, which are meant to
	/// be instantiated or drawable which will directly be used.
	class GraphicsTemplate : public ComponentTemplate {
	public:

		/// Default constructor.
		GraphicsTemplate() {
            sizing = Automatic;
        }
		
		/// Filling constructor, might cause ambiguous call due to most drawables being
		/// AnimationProviders as well. You might typecast or use Content.SetDrawable function
		GraphicsTemplate(const Graphics::Drawable &content) : Content(ChangedEvent, content) {
		}

		/// Filling constructor, might cause ambiguous call due to most drawables being
		/// AnimationProviders as well. You might typecast or use Content.SetDrawable function
		GraphicsTemplate(const Graphics::AnimationProvider &content) : Content(ChangedEvent, content) {
		}

		/// Returns the type of the component.
		virtual ComponentType GetType() const noexcept override {
			return ComponentType::Graphics;
		}
		
		/// Changes the padding of the component. Padding is the minimum spacing inside the component.
		void SetPadding(int value) { padding = {value}; ChangedEvent(); }

		/// Changes the padding of the component. Padding is the minimum spacing inside the component.
		void SetPadding(int hor, int ver) { padding = {hor, ver}; ChangedEvent(); }

		/// Changes the padding of the component. Padding is the minimum spacing inside the component.
		void SetPadding(int left, int top, int right, int bottom) {
			padding ={left, top, right, bottom};
			ChangedEvent();
		}

		/// Changes the padding of the component. Padding is the minimum spacing inside the component.
		void SetPadding(Geometry::Margin value) { padding = value; ChangedEvent(); }

		/// Returns the padding.
		Geometry::Margin GetPadding() const { return padding; }

		
		/// Set to true if you want to fill the region with the given graphics. This will
		/// only work if the given animation/drawable is rectangular
		void SetFillArea(bool value) {
            fill = value;
            
            ChangedEvent();
        }
        
        /// Returns whether this graphics will fill the component area
        bool GetFillArea() const {
            return fill;
        }

		/// Graphical representation of the template
		VisualProvider Content = {ChangedEvent};

	private:
		bool fill = true;
		Geometry::Margin padding = {0, 0, 0, 0};
	};
	
	/// Container class that defines an area according to the Box Model.
	/// It can contain other objects.
	class ContainerTemplate : public ComponentTemplate {
	public:
		/// Changes the padding of the component. Padding is the minimum spacing inside the component.
		void SetPadding(int value, Dimension::Unit unit = Dimension::Pixel) { padding ={{value, unit}}; ChangedEvent(); }

		/// Changes the padding of the component. Padding is the minimum spacing inside the component.
		void SetPadding(int hor, int ver, Dimension::Unit unit = Dimension::Pixel) { padding ={{hor, unit},{ver, unit}}; ChangedEvent(); }

		/// Changes the padding of the component. Padding is the minimum spacing inside the component.
		void SetPadding(int left, int top, int right, int bottom, Dimension::Unit unit = Dimension::Pixel) {
			padding ={{left, unit},{top, unit},{right, unit},{bottom, unit}};
			ChangedEvent();
		}

		/// Changes the padding of the component. Padding is the minimum spacing inside the component.
		void SetPadding(Geometry::Margin value, Dimension::Unit unit = Dimension::Pixel) {
			padding ={{value.Left, unit},{value.Top, unit},{value.Right, unit},{value.Bottom, unit}};
			ChangedEvent();
		}

		/// Changes the padding of the component. Padding is the minimum spacing inside the component.
		void SetPadding(Dimension value) { padding ={value}; ChangedEvent(); }

		/// Changes the padding of the component. Padding is the minimum spacing inside the component.
		void SetPadding(Dimension hor, Dimension ver) { padding ={hor, ver}; ChangedEvent(); }

		/// Changes the padding of the component. Padding is the minimum spacing inside the component.
		void SetPadding(Dimension left, Dimension top, Dimension right, Dimension bottom) { padding = {left, top, right, bottom}; ChangedEvent(); }

		/// Changes the padding of the component. Padding is the minimum spacing inside the component.
		void SetPadding(Margin value) { padding = value; ChangedEvent(); }

		/// Returns the padding.
		Margin GetPadding() const { return padding; }



		/// Changes the border size of the component. Border size stays within the object area, but excluded
		/// from the interior area.
		void SetBorderSize(int value) { bordersize ={value}; ChangedEvent(); }

		/// Changes the border size of the component. Border size stays within the object area, but excluded
		/// from the interior area.
		void SetBorderSize(int hor, int ver) { bordersize ={hor, ver}; ChangedEvent(); }

		/// Changes the border size of the component. Border size stays within the object area, but excluded
		/// from the interior area.
		void SetBorderSize(int left, int top, int right, int bottom) { bordersize = {left, top, right, bottom}; ChangedEvent(); }

		/// Changes the border size of the component. Border size stays within the object area, but excluded
		/// from the interior area.
		void SetBorderSize(Geometry::Margin value) { bordersize = value; ChangedEvent(); }

		/// Returns the border size
		Geometry::Margin GetBorderSize() const { return bordersize; }


		/// Changes the shadow extent of the component. Shadow extent stays within the object area, but excluded
		/// from the interior area.
		void SetShadowExtent(int value) { padding ={value}; ChangedEvent(); }

		/// Changes the shadow extent of the component. Shadow extent stays within the object area, but excluded
		/// from the interior area.
		void SetShadowExtent(int hor, int ver) { shadowextent ={hor, ver}; ChangedEvent(); }

		/// Changes the shadow extent of the component. Shadow extent stays within the object area, but excluded
		/// from the interior area.
		void SetShadowExtent(int left, int top, int right, int bottom) { shadowextent = {left, top, right, bottom}; ChangedEvent(); }

		/// Changes the shadow extent of the component. Shadow extent stays within the object area, but excluded
		/// from the interior area.
		void SetShadowExtent(Geometry::Margin value) { shadowextent = value; ChangedEvent(); }

		/// Returns the shadow extent
		Geometry::Margin GetShadowExtent() const { return shadowextent; }



		/// Changes the overlay extent of the component. Overlay extent stays within the object area, but excluded
		/// from the interior area.
		void SetOverlayExtent(int value) { padding ={value}; ChangedEvent(); }

		/// Changes the overlay extent of the component. Overlay extent stays within the object area, but excluded
		/// from the interior area.
		void SetOverlayExtent(int hor, int ver) { overlayextent ={hor, ver}; ChangedEvent(); }

		/// Changes the overlay extent of the component. Overlay extent stays within the object area, but excluded
		/// from the interior area.
		void SetOverlayExtent(int left, int top, int right, int bottom) { overlayextent = {left, top, right, bottom}; ChangedEvent(); }

		/// Changes the overlay extent of the component. Overlay extent stays within the object area, but excluded
		/// from the interior area.
		void SetOverlayExtent(Geometry::Margin value) { overlayextent = value; ChangedEvent(); }

		/// Returns the overlay extent
		Geometry::Margin GetOverlayExtent() const { return overlayextent; }
		
		/// Changes the orientation of the template
		void SetOrientation(Graphics::Orientation value) { orientation = value; ChangedEvent(); }
		
		/// Returns the current orientation of the container
		Graphics::Orientation GetOrientation() const { return orientation; }

		/// Adds an index to the container. The components will be drawn in order. Thus the components that are added
		/// later will be drawn on top. Multiple indexes will not cause any crashes, however, same component might be drawn 
		/// multiple times on top of itself.
		void AddIndex(int componentindex) {
			indices.push_back(componentindex);
			ChangedEvent();
		}

		/// Insert an index to the specified location in the container. The components will be drawn in order. Thus the 
		/// components that are added later will be drawn on top. Multiple indexes will not cause any crashes, however, 
		/// same component might be drawn multiple times on top of itself.
		void InsertIndex(int before, int componentindex) {
			indices.insert(indices.begin() + before, componentindex);
			ChangedEvent();
		}

		/// Removes the index at the given position.
		void RemoveIndexAt(int index) {
			indices.erase(indices.begin() + index);
			ChangedEvent();
		}

		/// Returns the number of component indices stored in this container
		int GetCount() const {
			return (int)indices.size();
		}

		/// Returns the component index at the given location
		int operator[] (int index) const {
			return indices[index];
		}

		/// Returns the component index at the given location
		int &operator[] (int index) {
			return indices[index];
		}

		/// Returns the type of the component.
		virtual ComponentType GetType() const noexcept override {
			return ComponentType::Container;
		}
		
		/// Mouse reporting allows component stack to report mouse events over this container to be reported separately.
		/// Setting a tag with this property will help distinguishing between multiple regions.
		void SetReportMouse(bool value);
        
        bool GetReportMouse() const;
		

		/// Background graphics
		VisualProvider Background = {ChangedEvent};

		/// Overlay graphics, overlay will be drawn on top of the contents.
		VisualProvider Overlay    = {ChangedEvent};

    private:
		Margin padding;
		Geometry::Margin bordersize, shadowextent, overlayextent;
		std::vector<int> indices;
        Graphics::Orientation orientation = Graphics::Orientation::Horizontal;
        bool reportmouse = false;
	};

} }
