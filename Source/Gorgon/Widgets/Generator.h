#pragma once

#include "../UI/Template.h"
#include "../Graphics/Color.h"
#include "../Graphics/Font.h"
#include "../Graphics/Rectangle.h"
#include "../Graphics/MaskedObject.h"
#include "Registry.h"

namespace Gorgon { namespace Widgets {

    /**
    * Generators create templates for widgets. First setup a generator, then
    * call specific generation functions.
    */
    class Generator : public Registry {
    public:
        virtual ~Generator() { }
        
        Generator(bool activate = true) : Registry(activate) { }
        
        /// Generates a button template
        virtual UI::Template Button() = 0;
        
        /// Generates a button template with the given default size.
        virtual UI::Template IconButton(Geometry::Size iconsize = {-1, -1}) = 0;
        
        
        virtual UI::Template Checkbox() = 0;
        
        virtual UI::Template CheckboxButton() = 0;
        
        virtual UI::Template RadioButton() = 0;
        
        
        virtual UI::Template Label() = 0;
        
        virtual UI::Template ErrorLabel() = 0;
        
        
        virtual UI::Template Panel() = 0;
        
        virtual UI::Template TopPanel() = 0;
        
        virtual UI::Template LeftPanel() = 0;
        
        virtual UI::Template BottomPanel() = 0;
        
        virtual UI::Template RightPanel() = 0;
        
        virtual UI::Template BlankPanel() = 0;
        
        
        virtual UI::Template Inputbox() = 0;
        
        
        virtual UI::Template Progressbar() = 0;
        
        virtual UI::Template HScrollbar() = 0;
        
        virtual UI::Template VScrollbar() = 0;
        
        
        virtual UI::Template Layerbox() = 0;
        
        virtual UI::Template BlankLayerbox() = 0;
        
        
        virtual UI::Template Listbox() = 0;
        
        virtual UI::Template Dropdown() = 0;
        
    protected:
        
        virtual UI::Template &generate(Gorgon::Widgets::Registry::TemplateType type) override {
            switch(type) {
            case Button_Regular:
                return *new UI::Template(Button());
            case Button_Icon:
                return *new UI::Template(IconButton());
            case Label_Regular:
                return *new UI::Template(Label());
            case Label_Error:
                return *new UI::Template(ErrorLabel());
            case Checkbox_Regular:
                return *new UI::Template(Checkbox());
            case Checkbox_Button:
                return *new UI::Template(CheckboxButton());
            case Radio_Regular:
                return *new UI::Template(RadioButton());
            case Inputbox_Regular:
                return *new UI::Template(Inputbox());
            case Panel_Regular:
                return *new UI::Template(Panel());
            case Panel_Blank:
                return *new UI::Template(BlankPanel());
            case Panel_Top:
                return *new UI::Template(TopPanel());
            case Panel_Left:
                return *new UI::Template(LeftPanel());
            case Panel_Bottom:
                return *new UI::Template(BottomPanel());
            case Panel_Right:
                return *new UI::Template(RightPanel());
            case Progress_Regular:
                return *new UI::Template(Progressbar());
            case Layerbox_Regular:
                return *new UI::Template(Layerbox());
            case Layerbox_Blank:
                return *new UI::Template(BlankLayerbox());
            case Scrollbar_Vertical:
                return *new UI::Template(VScrollbar());
            case Scrollbar_Horizontal:
                return *new UI::Template(HScrollbar());
            case Listbox_Regular:
                return *new UI::Template(Listbox());
            case Dropdown_Regular:
                return *new UI::Template(Dropdown());
            default:
                return *new UI::Template();
            }
        }
    };
    
    /**
    * This class generates very simple templates. Hover and down states are marked
    * with simple fore and background color changes. For background, hover and down
    * state colors are blended with the regular color. Font is shared, thus any
    * changes to it will effect existing templates too. Most graphics are generated
    * immediately upon creating and will not be modified if they are already created
    */
    class SimpleGenerator : public Generator {
    public:
        
        /// Initializes the generator. Density controls the spacing between elements
        explicit SimpleGenerator(int fontsize, std::string fontname = "", bool activate = true, float density = 7.5);
        
        /// Creates a non-working simple generator. Calls to any function other than Init
        /// is undefined behaviour.
        SimpleGenerator() : Generator(false) {
        }
        
        /// Initializes the generator
        void Init(int fontsize = 14, std::string fontname = "");
        
        virtual ~SimpleGenerator();
        
        virtual UI::Template Button() override;
        
        virtual UI::Template IconButton(Geometry::Size iconsize = {-1, -1}) override;
        
        
        virtual UI::Template Checkbox() override;
        
        virtual UI::Template CheckboxButton() override;
        
        virtual UI::Template RadioButton() override;
        
        
        virtual UI::Template Label() override;

        virtual UI::Template ErrorLabel() override;
        
        
        virtual UI::Template BlankPanel() override;
        
        virtual UI::Template Panel() override;
        
        virtual UI::Template TopPanel() override;
        
        virtual UI::Template LeftPanel() override;
        
        virtual UI::Template RightPanel() override;
        
        virtual UI::Template BottomPanel() override;
        
        
        virtual UI::Template Inputbox() override;
        
        
        virtual UI::Template Progressbar() override;
        
        virtual UI::Template HScrollbar() override;
        
        virtual UI::Template VScrollbar() override;
        
        
        
        virtual UI::Template BlankLayerbox() override;
        
        virtual UI::Template Layerbox() override;
        
        virtual UI::Template Listbox() override;
        
        virtual UI::Template Dropdown() override;
        

        virtual int GetSpacing() const override {
            return Spacing;
        }

        virtual int GetEmSize() const override {
            return lettervsize.first + lettervsize.second;
        }
        
        virtual int GetUnitWidth() const override {
            return BorderedWidgetHeight; //UnitWidth = Bordered height
        }

        Graphics::BitmapRectangleProvider &NormalBorder();
        Graphics::BitmapRectangleProvider &HoverBorder();
        Graphics::BitmapRectangleProvider &DownBorder();
        Graphics::BitmapRectangleProvider &DisabledBorder();
        
        Graphics::BitmapRectangleProvider &PanelBorder(int missingedge = 0);
        
        Graphics::BitmapRectangleProvider &GrooveBorder();
        
        Graphics::BitmapRectangleProvider &NormalEditBorder();
        Graphics::BitmapRectangleProvider &HoverEditBorder();
        Graphics::BitmapRectangleProvider &ReadonlyBorder();
        
        Graphics::BitmapRectangleProvider &NormalEmptyBorder();
        
        Graphics::BitmapRectangleProvider &NormalBG();
        Graphics::BitmapRectangleProvider &HoverBG();
        Graphics::BitmapRectangleProvider &DownBG();
        Graphics::BitmapRectangleProvider &DisabledBG();
        Graphics::BitmapRectangleProvider &NormalStraightBG();
        Graphics::BitmapRectangleProvider &AltStraightBG();
        Graphics::BitmapRectangleProvider &HoverStraightBG();
        Graphics::BitmapRectangleProvider &DownStraightBG();
        Graphics::BitmapRectangleProvider &DisabledStraightBG();
        Graphics::BitmapRectangleProvider &NormalRBG();
        Graphics::BitmapRectangleProvider &HoverRBG();
        Graphics::BitmapRectangleProvider &DownRBG();
        Graphics::BitmapRectangleProvider &DisabledRBG();
        Graphics::BitmapRectangleProvider &ObjectShape();
        Graphics::MaskedObjectProvider &InnerObjectShape();
        
        Graphics::BitmapRectangleProvider &GrooveBG();
        
        Graphics::RectangleProvider &FocusBorder();
        
        int Spacing       = 4;
        int ObjectHeight  = 15;
        int ObjectBorder  = 2;
        float ShapeBorder = 2;
        
        /// This function will update default widget dimensions. Call this function after
        /// setting up or changing borders, font size
        void UpdateDimensions();
        
        /// This function will update default widget borders depending on the font size
        void UpdateBorders(bool smooth = true);

        Graphics::StyledRenderer RegularFont;
        Graphics::StyledRenderer CenteredFont;

        struct FocusInfo {
            Graphics::RGBA  Color   = {Graphics::Color::Charcoal, 0.7};
            int             Width   = 1;
            //focus to content spacing
            int             Spacing = 1;
        } Focus;
        
        struct BorderInfo {
            int Width               = 2;
            int Radius              = 0;
            int Divisions           = 1;
            Graphics::RGBA Color    = Graphics::Color::Charcoal;
            Graphics::RGBA Disabled = {Graphics::Color::Charcoal, 0.5};
        } Border;
        
        struct BackgroundInfo {
            Graphics::RGBA Regular  = {Graphics::Color::Ivory, 0.8};
            Graphics::RGBA Alternate= {Graphics::Color::DarkGrey, 0.2};
            Graphics::RGBA Hover    = {Graphics::Color::Tan, 0.5};
            Graphics::RGBA Down     = {Graphics::Color::Crimson, 0.2};
            Graphics::RGBA Disabled =  Graphics::Color::LightGrey;
            
            Graphics::RGBA Edit    = {Graphics::Color::White};
            Graphics::RGBA Panel   = {Graphics::Color::Grey, Graphics::Color::Ivory, 0.5};
            
            Graphics::RGBA Groove  = {Graphics::Color::Charcoal, 0.5};
            
            Graphics::RGBA Selected= {Graphics::Color::Charcoal, 0.4};
        } Background;
        
        struct ForecolorInfo {
            Graphics::RGBA Regular = Graphics::Color::Charcoal;
            Graphics::RGBA Disabled= {Graphics::Color::Grey, 0.5};
            Graphics::RGBA Hover   = Graphics::Color::Black;
            Graphics::RGBA Down    = Graphics::Color::Black;
            Graphics::RGBA Error   = Graphics::Color::Red;
            Graphics::RGBA Inverted= {Graphics::Color::White, 0.8};
            Graphics::RGBA InvertedActive  = Graphics::Color::White;
        } Forecolor;
        
        
        /// This is the width of a one cell widget
        int WidgetWidth = 64;
        
        /// This is the height of a bordered widget
        int BorderedWidgetHeight = 32;
        
        /// This is the height of a non-bordered widget
        int WidgetHeight = 24;
        
        /// This controls the automatic spacing. After chaning this member
        /// you need to call UpdateDimensions to get the desired effect.
        float Density = 7.5;

    private:
        Graphics::BitmapRectangleProvider *makeborder(Graphics::RGBA border, Graphics::RGBA bg, int missingedge = 0, int borderwidth = -1, int borderradius = -1);
        Graphics::RectangleProvider *makefocusborder();
        UI::Template makepanel(int missingedge, bool scrollers);
        
        UI::Template maketemplate();
        
        void setupfocus(UI::GraphicsTemplate &focus);
        
        Graphics::GlyphRenderer *regularrenderer = nullptr;
        Containers::Collection<Graphics::Drawable> drawables;
        Containers::Collection<Graphics::AnimationProvider> providers;
        
        Graphics::BitmapRectangleProvider *normalborder = nullptr;
        Graphics::BitmapRectangleProvider *hoverborder = nullptr;
        Graphics::BitmapRectangleProvider *downborder = nullptr;
        Graphics::BitmapRectangleProvider *disabledborder = nullptr;
        
        Graphics::BitmapRectangleProvider *panelborders[5] = {};
        
        Graphics::BitmapRectangleProvider *grooveborder = nullptr;
        
        Graphics::BitmapRectangleProvider *normaleditborder = nullptr;
        Graphics::BitmapRectangleProvider *hovereditborder = nullptr;
        Graphics::BitmapRectangleProvider *readonlyborder = nullptr;
        
        Graphics::BitmapRectangleProvider *normalemptyborder = nullptr;
        
        Graphics::BitmapRectangleProvider *normalrbg = nullptr;
        Graphics::BitmapRectangleProvider *hoverrbg = nullptr;
        Graphics::BitmapRectangleProvider *downrbg = nullptr;
        Graphics::BitmapRectangleProvider *disabledrbg = nullptr;
        
        Graphics::BitmapRectangleProvider *normalbg = nullptr;
        Graphics::BitmapRectangleProvider *hoverbg = nullptr;
        Graphics::BitmapRectangleProvider *downbg = nullptr;
        Graphics::BitmapRectangleProvider *disabledbg = nullptr;
        Graphics::BitmapRectangleProvider *normalstraight = nullptr;
        Graphics::BitmapRectangleProvider *altstraight = nullptr;
        Graphics::BitmapRectangleProvider *hoverstraight = nullptr;
        Graphics::BitmapRectangleProvider *downstraight = nullptr;
        Graphics::BitmapRectangleProvider *disabledstraight = nullptr;
        Graphics::BitmapRectangleProvider *groovebg = nullptr;
        Graphics::BitmapRectangleProvider *objectshape = nullptr;
        Graphics::MaskedObjectProvider *innerobjectshape = nullptr;
        
        Graphics::RectangleProvider *focusborder = nullptr;
        
        UI::Template listbox_listitem;

        std::pair<int, int> lettervsize, asciivsize;
    };

}}
