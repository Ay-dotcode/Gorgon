/// @file UI/Template.h This file contains UI component template system.

#pragma once

#include "../Types.h"
#include "../Geometry/Margin.h"
#include "../Geometry/Size.h"
#include "../Event.h"
#include "../Containers/Collection.h"
#include "../Graphics/Drawables.h"
#include "../Graphics/Animations.h"
#include "Dimension.h"

//TODO: Add baseline for all components

namespace Gorgon { 
    namespace Graphics {
        class TextRenderer;
    }
    
    /// This namespace contains User interface related functionality. This namespace
    /// does not contain the actual widgets that can be used. For that purpose, use
    /// Gorgon::Widgets namespace.
    namespace UI {

    // TextPlaceholder
    // TextPlaceholder should have its own layer (same for others?)
    // Obtained size, position
    // 

    /// Anchor position
    ENUMCLASS Anchor {
        /// This anchor position should is only used to denote object will
        /// not be anchored to a previous object, only to its parent. If
        /// used as parent anchor it will be ignored and default anchor
        /// will be used instead. This should be paired with absolute 
        /// positioning.
        None = 0,
        
        /// Top left
        TopLeft = 1,
        /// Top center
        TopCenter,
        /// Top right
        TopRight,

        /// Middle left
        MiddleLeft = 4,
        /// Middle center, using this position ensures that the
        /// components will be inside each other.
        MiddleCenter,
        /// Middle right
        MiddleRight,
        
        /// Bottom left
        BottomLeft = 7,
        /// Bottom center
        BottomCenter,
        /// Bottom right
        BottomRight,
        
        /// Baseline left, for text, this aligns to the baseline of
        /// the first line
        FirstBaselineLeft,
        /// Baseline right, for text, this aligns to the baseline of
        /// the first line
        FirstBaselineRight,
        
        /// Baseline left, for text, this aligns to the baseline of
        /// the last line. This mode only works properly if
        /// Sizing is set to automatic.
        LastBaselineLeft,
        
        /// Baseline right, for text, this aligns to the baseline of
        /// the last line. This mode only works properly if
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
            case Anchor::FirstBaselineLeft:
            case Anchor::FirstBaselineRight:
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
        Ignored,
    };

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

        /// Component is visible when the widget is readonly.
        Readonly,

        /// Widget has the focus
        Focused,

        /// Mouse is over the widget, or over a particular repeat
        Hover,
        
        /// This is activated when the mouse is pressed on the component stack. However,
        /// it can be activated in cases when activation key is pressed.
        Down,
        

        /// Second state of the widget, first state is Always
        State2,

        /// Third state of the widget, first state is Always
        State3,

        /// Fourth state of the widget, first state is Always
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
        
        /// This is for widgets that can be activated, like a count down timer
        Active,
        
        /// There is space horizontally to be scrolled
        HScroll,
        
        /// There is space vertically to be scrolled.
        VScroll,
        
        /// There is space both horizontally and vertically to be scrolled.
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
        
        /// This widget is the default widget of its container
        Default,
        
        
        /// This widget is the cancel widget of its container
        Cancel,
        
        /// Do not use this value
        DataEffectStart = Cancel,
        
        /// Data effect of the component is set
        TextIsSet,
        
        /// Data effect of the component is set
        TitleIsSet,
        
        /// Data effect of the component is set
        LabelIsSet,
        
        /// Data effect of the component is set
        ValueText1IsSet,
        
        /// Data effect of the component is set
        ValueText2IsSet,
        
        /// Data effect of the component is set
        ValueText3IsSet,
        
        /// Data effect of the component is set
        ValueText4IsSet,
        
        /// Data effect of the component is set
        State1TextIsSet,
        
        /// Data effect of the component is set
        State2TextIsSet,
        
        /// Data effect of the component is set
        State3TextIsSet,
        
        /// Data effect of the component is set
        State4TextIsSet,
        
        /// Data effect of the component is set
        Icon1IsSet,
        
        /// Data effect of the component is set
        Icon2IsSet,
        
        /// Data effect of the component is set
        StateIcon1IsSet,
        
        /// Data effect of the component is set
        StateIcon2IsSet,
        
        /// Data effect of the component is set
        StateIcon3IsSet,
        
        /// Data effect of the component is set
        StateIcon4IsSet,
        
        /// Do not use this condition, this is to size the arrays.
        Max,
    };
    
    /// Returns if the given condition is related to mouse events
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
    class IgnoredTemplate;

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

        Template(Template &&) = default;
        
        Template &operator =(Template &&) = default;

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
        
        /// This will create a new ignored template. Ignored templates are used to erase
        /// templates that are set to Always
        IgnoredTemplate &AddIgnored(int index, ComponentCondition cond) { return AddIgnored(index, cond, ComponentCondition::None); }

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
        
        /// This will create a new ignored template. Ignored templates are used to erase
        /// templates that are set to Always
        IgnoredTemplate &AddIgnored(int index, ComponentCondition from, ComponentCondition to);
        
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
        
        /// Sets the spacing required for this template. Organizer widgets
        /// uses this spacing in order to layout the widgets.
        void SetSpacing(int value) {
            spacing = value;
            ChangedEvent();
        }
        
        /// Returns the spacing required for this template. Organizer widgets
        /// uses this spacing in order to layout the widgets.
        int GetSpacing() const {
            return spacing;
        }


        /// This event is fired whenever template or its components are changed.
        Event<Template> ChangedEvent = Event<Template>{*this};
        
        std::string Name;
        
    private:
        Containers::Collection<ComponentTemplate> components;
        std::vector<Event<ComponentTemplate>::Token> tokens;
        std::map<std::pair<ComponentCondition, ComponentCondition>, int> durations;

        SizeMode xsizing = Free, ysizing = Free;
        Geometry::Size size;
        Geometry::Size additional = {0, 0};
        int spacing = 4;
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

            /// Absolute positioning, coordinates will start from the container,
            /// percent based movement will move to stay within the container.
            /// If the component is filling the container, it cannot be moved.
            AbsoluteSliding,
            
            /// Absolute positioning, coordinates will start from the container.
            /// Percent based movement is relative to the size of the component.
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
            
            Title,
            
            Label,

            //Value texts can automatically be calculated depending on the widget
            ValueText1,

            //Value texts can automatically be calculated depending on the widget
            ValueText2,

            //Value texts can automatically be calculated depending on the widget
            ValueText3,

            //Value texts can automatically be calculated depending on the widget
            ValueText4,
            
            State1Text,
            State2Text,
            State3Text,
            State4Text,
            
            AutoStart = ValueText1,
            AutoEnd   = State4Text,

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

            /// In text and graphics templates, this setting will blend the color to target color.
            /// If there is only one value source, all channels will be blended using the given factor.
            /// Two value sources will have separate blending factor RGB and A. Three sources will
            /// cause R, G, and B to have separate blender factors. A of the original color will be
            /// used. Four channels will have four separate blending factors.
            BlendColor,
            
            /// Data affects the rotation of the component. Rotation angle will start from 0 degrees
            /// for 0, 360 degrees for 1. You may use min and max to modify this. For instance, if max
            /// is set to 0.5, the degrees will go up to 180. For now, this effect is not in use.
            ModifyRotation,

            /// Data will affect the progress of the animation. Will only work for Objects
            /// with animations.
            ModifyAnimation,

            /// Size of this component will be affected. Data will be given as a percent
            /// and will modify Size property. Useful for sliders and progress bars. The direction
            /// of the container is used to determine which axis will get affected.
            ModifySize,

            /// Width of this component will be affected. Data will be given as a percent
            /// and will modify Size property. Useful for sliders and progress bars. The direction
            /// of the container is used to determine which axis will get affected. 
            ModifyWidth,

            /// Height of this component will be affected. Data will be given as a percent
            /// and will modify Size property. Useful for sliders and progress bars. The direction
            /// of the container is used to determine which axis will get affected.
            ModifyHeight,
            
            
            /// This is a combined modification of position and size. Single channel will control
            /// the position along the orientation. Two channels will control position and size along
            /// the orientation. Three channels will control xy position and size along the 
            /// orientation. Finally four channels will control every aspect of the component. 
            ModifyPositionAndSize,
            
            /// Modifies X position and width
            ModifyXAndWidth,
            
            /// Modifies X position and height
            ModifyXAndHeight,
            
            /// Modifies Y position and width
            ModifyYAndWidth,
            
            /// Modifies Y position and height
            ModifyYAndHeight,
            
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
            SecondDragTag,
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
            ContentsTag,
            ViewPortTag,
            SelectionTag,
            CaretTag,
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

            if(sizingw == Automatic && size.Width.GetValue() != 0)
                sizingw = Fixed;

            if(sizingh == Automatic && size.Height.GetValue() != 0)
                sizingh = Fixed;

            ChangedEvent(); 
        }

        /// Changes the sizing mode of the component.
        void SetSizing(SizingMode value) { sizingw = sizingh = value; ChangedEvent(); }

        /// Changes the sizing mode of the component.
        void SetSizing(SizingMode hor, SizingMode vert) { sizingw = hor; sizingh = vert; ChangedEvent(); }

        /// Returns the size of the component. This value is *not* absolute final size as
        /// it cannot be determined before the component is rendered in a widget.
        Size GetSize() const { return size; }

        /// Returns the horizontal sizing mode of the component.
        SizingMode GetHorizontalSizing() const { return sizingw; }

        /// Returns the horizontal sizing mode of the component.
        SizingMode GetVerticalSizing() const { return sizingh; }


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
        void SetIndent(int value, Dimension::Unit unit = Dimension::Pixel) { indent = {{value, unit}}; ChangedEvent(); }

        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
        void SetIndent(int hor, int ver, Dimension::Unit unit = Dimension::Pixel) { indent = {{hor, unit}, {ver, unit}}; ChangedEvent(); }

        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
        void SetIndent(int left, int top, int right, int bottom, Dimension::Unit unit = Dimension::Pixel) { 
            indent = {{left, unit}, {top, unit}, {right, unit}, {bottom, unit}}; 
            ChangedEvent(); 
        }

        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
        void SetIndent(Geometry::Margin value, Dimension::Unit unit = Dimension::Pixel) { 
            indent = {{value.Left, unit}, {value.Top, unit}, {value.Right, unit}, {value.Bottom, unit}}; 
            ChangedEvent(); 
        }

        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
        void SetIndent(Dimension value) { indent = {value}; ChangedEvent(); }

        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
        void SetIndent(Dimension hor, Dimension ver) { indent = {hor, ver}; ChangedEvent(); }

        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
        void SetIndent(Dimension left, Dimension top, Dimension right, Dimension bottom) { indent = {left, top, right, bottom}; ChangedEvent(); }

        /// Changes the indent of the component. Indent is added to the margin if the component is at the
        /// edge of the container.
        void SetIndent(Margin value) { indent = value; ChangedEvent(); }

        /// Returns the current indent.
        Margin GetIndent() const { return indent; }



        /// Sets the data effect for this component. Default is None.
        void SetDataEffect(DataEffect effect) {
            dataeffect = effect;
            
            ChangedEvent(); 
        }

        /// Returns how the data will affect this component
        DataEffect GetDataEffect() const { return dataeffect; }
        
        /// Changes the ordering of the values. This allows swaps like X-Y. You should specify which channel will receive which value. 
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



        /// Changes the baseline of the current component. If set to 0, it will be determined automatically.
        void SetBaseline(int value) { baseline = value; ChangedEvent(); } 

        /// Returns the baseline. If set to 0, it will be detected automatically
        int GetBaseline() const { return baseline; }



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
        SizingMode sizingw = Fixed, sizingh = Fixed;

        /// Size of the object.
        Size size = {{100, Dimension::Percent}, {100, Dimension::Percent}};

        /// Margin around the object, will be collapsed with other object margins
        /// and padding
        Margin margin = {0};

        /// Indent is added to the margin and padding on the edge of the container
        Margin indent = {0};
        
        /// Center point for rotation
        Point center = {{50, Dimension::Percent}, {50, Dimension::Percent}};

        /// Anchor point of the previous component that this component will be attached
        /// to. If the component positioning is absolute or this is the first component, 
        /// it will be anchored to the container and parent anchor point will be used.
        Anchor previous = Anchor::TopRight;
        
        /// Anchor point of the container that this component will be attached to, if it
        /// is attaching to its parent.
        Anchor container = Anchor::TopLeft;

        /// Anchor point of the current component. This point will be matched to the
        /// previous component's anchor point
        Anchor my = Anchor::TopLeft;
        
        /// Manually set baseline for this component. When 0, it will not be effective.
        int baseline = 0;

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
            sizingw = Automatic;
            sizingh = Automatic;
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
    
    /// This component type will be ignored by ComponentStack, effectively erasing
    /// Always when necessary.
    class IgnoredTemplate : public ComponentTemplate {
    public:
        
        /// Returns the type of the component.
        virtual ComponentType GetType() const noexcept override {
            return ComponentType::Ignored;
        }
        
    };

    /**
     * Textholder is designed to hold text data. This data is set by the widget through
     * DataEffect. Alignment for this component uses baseline and it is automatically sized.
     */
    class TextholderTemplate : public ComponentTemplate {
    public:
        TextholderTemplate() {
            my = Anchor::FirstBaselineLeft;
            previous = Anchor::FirstBaselineRight;
            container = Anchor::FirstBaselineLeft;
            SetSizing(Automatic);
        }

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

        /// Changes the color that will override default color of the text drawn.
        void SetColor(Graphics::RGBAf value) {
            color = value;

            ChangedEvent();
        }

        /// Returns current color.
        Graphics::RGBAf GetColor() const {
            return color;
        }


        /// Sets the target color for BlendColor value modification.
        void SetTargetColor(Graphics::RGBAf value) {
            targetcolor = value;

            ChangedEvent();
        }

        /// Returns the target color that will be used for BlendColor value modification.
        Graphics::RGBAf GetTargetColor() const {
            return targetcolor;
        }
        
        /// Sets the default text for the textholder. This text can be overriden
        /// by DataEffect
        void SetText(const std::string &value) {
            text = value;
            
            ChangedEvent();
        }
        
        /// Returns the default text for the textholder.
        std::string GetText() const {
            return text;
        }
    
    private:
        const Graphics::TextRenderer *renderer = nullptr;
        Graphics::RGBAf color = 1.0f;
        Graphics::RGBAf targetcolor = 0.f;
        std::string text;
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
            sizingw = Automatic;
            sizingh = Automatic;
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
        
        /// TODO: add size controller
        
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

        /// Changes the color that will set tint of the drawn graphics.
        void SetColor(Graphics::RGBAf value) {
            color = value;

            ChangedEvent();
        }

        /// Returns current tint color.
        Graphics::RGBAf GetColor() const {
            return color;
        }

        /// Sets the target tint color for BlendColor value modification.
        void SetTargetColor(Graphics::RGBAf value) {
            targetcolor = value;

            ChangedEvent();
        }

        /// Returns the target tint color that will be used for BlendColor value modification.
        Graphics::RGBAf GetTargetColor() const {
            return targetcolor;
        }

        /// Graphical representation of the template
        VisualProvider Content = {ChangedEvent};

    private:
        bool fill = true;
        Geometry::Margin padding = {0, 0, 0, 0};
        Graphics::RGBAf color = 1.0f;
        Graphics::RGBAf targetcolor = 0.f;
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
        void SetOverlayExtent(int value) { overlayextent ={value}; ChangedEvent(); }

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
        ContainerTemplate &AddIndex(int componentindex) {
            indices.push_back(componentindex);
            ChangedEvent();
            
            return *this;
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
