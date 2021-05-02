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
        
        /// Generates a button template
        virtual UI::Template DialogButton() = 0;
        
        
        virtual UI::Template Checkbox() = 0;
        
        virtual UI::Template CheckboxButton() = 0;
        
        virtual UI::Template RadioButton() = 0;
        
        
        virtual UI::Template Label() = 0;
        
        virtual UI::Template ErrorLabel() = 0;
        
        virtual UI::Template BoldLabel() = 0;
        
        virtual UI::Template TitleLabel() = 0;
        
        virtual UI::Template SubtitleLabel() = 0;
        
        virtual UI::Template LeadingLabel() = 0;
        
        virtual UI::Template InfoLabel() = 0;
        
        
        
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
        
        
        virtual UI::Template Window() = 0;
        
        virtual UI::Template DialogWindow() = 0;
        
        
        virtual UI::Template ColorPlane() = 0;
        
        virtual UI::Template ColorPicker() = 0;
        
    protected:
        
        virtual UI::Template &generate(Gorgon::Widgets::Registry::TemplateType type) override {
            switch(type) {
            case Button_Regular:
                return *new UI::Template(Button());
            case Button_Icon:
                return *new UI::Template(IconButton());
            case Button_Dialog:
                return *new UI::Template(DialogButton());
            case Label_Regular:
                return *new UI::Template(Label());
            case Label_Error:
                return *new UI::Template(ErrorLabel());
            case Label_Bold:
                return *new UI::Template(BoldLabel());
            case Label_Title:
                return *new UI::Template(TitleLabel());
            case Label_Subtitle:
                return *new UI::Template(SubtitleLabel());
            case Label_Leading:
                return *new UI::Template(LeadingLabel());
            case Label_Info:
                return *new UI::Template(InfoLabel());
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
            case Window_Regular:
                return *new UI::Template(Window());
            case Window_Dialog:
                return *new UI::Template(DialogWindow());
            case ColorPlane_Regular:
                return *new UI::Template(ColorPlane());
            case ColorPicker_Regular:
                return *new UI::Template(ColorPicker());
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
        
        /// Identifies and helps with the creation of assets used in a generator
        struct AssetID {
            enum AssetType {
                Rectangle,
                Background,
                Frame,
                UpArrow,
                DownArrow,
                Box,
                Tick, //sized to fit into the box
                EmptyCircle,
                CircleFill, //sized to fit into the empty circle
                Cross,
                White,
                Checkered,
                Focus,
                Edit, //rectangle with background set to edit
                FgFilled,
                BorderFilled
            };
            
            enum BorderSide {
                None,
                Left,
                Top,
                Right,
                Bottom,
                AllExceptLeft,
                AllExceptTop,
                AllExceptRight,
                AllExceptBottom,
                Horizontal,
                Vertical,
                All
            };
            
            static int HBorders(BorderSide b) {
                switch(b) {
                case None:
                case Top:
                case Bottom:
                case Vertical:
                    return 0;
                case Left:
                case Right:
                case AllExceptLeft:
                case AllExceptRight:
                    return 1;
                default:
                    return 2;
                }
            }
            
            static int VBorders(BorderSide b) {
                switch(b) {
                case None:
                case Left:
                case Right:
                case Horizontal:
                    return 0;
                case Top:
                case Bottom:
                case AllExceptTop:
                case AllExceptBottom:
                    return 1;
                default:
                    return 2;
                }
            }
            
            static int TotalBorders(BorderSide b) {
                switch(b) {
                case None:
                    return 0;
                case Left:
                case Right:
                case Top:
                case Bottom:
                    return 1;
                case Horizontal:
                case Vertical:
                    return 2;
                case AllExceptTop:
                case AllExceptBottom:
                case AllExceptLeft:
                case AllExceptRight:
                    return 3;
                default:
                    return 4;
                }
            }
            
            static bool HasLeft(BorderSide b) {
                switch(b) {
                case Left:
                case AllExceptTop:
                case AllExceptRight:
                case AllExceptBottom:
                case Vertical:
                case All:
                    return true;
                default:
                    return false;
                }
            }
            
            static bool HasTop(BorderSide b) {
                switch(b) {
                case Top:
                case AllExceptLeft:
                case AllExceptRight:
                case AllExceptBottom:
                case Vertical:
                case All:
                    return true;
                default:
                    return false;
                }
            }
            
            static bool HasRight(BorderSide b) {
                switch(b) {
                case Right:
                case AllExceptLeft:
                case AllExceptTop:
                case AllExceptBottom:
                case Vertical:
                case All:
                    return true;
                default:
                    return false;
                }
            }
            
            static bool HasBottom(BorderSide b) {
                switch(b) {
                case Bottom:
                case AllExceptLeft:
                case AllExceptTop:
                case AllExceptRight:
                case Vertical:
                case All:
                    return true;
                default:
                    return false;
                }
            }
            
            AssetID(AssetType type, Graphics::Color::Designation color = Graphics::Color::Regular, BorderSide border = All, float radius = std::numeric_limits<float>::max(), float thickness = std::numeric_limits<float>::max()) :
                Type(type),
                Color(color),
                Border(border),
                BorderRadius(radius),
                BorderWidth(thickness)
            { }
            
            AssetType Type;
            Graphics::Color::Designation Color;
            BorderSide Border;
            float BorderRadius = std::numeric_limits<float>::max();
            float BorderWidth = std::numeric_limits<float>::max();
        };
        
        /// Initializes the generator. Density controls the spacing between elements
        explicit SimpleGenerator(int fontsize, std::string fontname = "", std::string boldfontname = "", bool activate = true, float density = 7.5);
        
        /// Creates a non-working simple generator. Calls to any function other than Init
        /// is undefined behaviour.
        SimpleGenerator() : Generator(false) {
        }
        
        /// Initializes the generator
        void Init(int fontsize = 14, std::string fontname = "", std::string boldfontname = "");
        
        virtual ~SimpleGenerator();
        
                UI::Template Button(bool border);
        
        virtual UI::Template Button() override { return Button(true); }
        
        virtual UI::Template IconButton(Geometry::Size iconsize = {-1, -1}) override;
        
        virtual UI::Template DialogButton() override;
        
        
        virtual UI::Template Checkbox() override;
        
        virtual UI::Template CheckboxButton() override;
        
        virtual UI::Template RadioButton() override;
        
        
        virtual UI::Template Label() override;

        virtual UI::Template ErrorLabel() override;
        
        virtual UI::Template BoldLabel() override;
        
        virtual UI::Template TitleLabel() override;
        
        virtual UI::Template SubtitleLabel() override;
        
        virtual UI::Template LeadingLabel() override;
        
        virtual UI::Template InfoLabel() override;
        
        
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
        
        
        virtual UI::Template Window() override;
        
        virtual UI::Template DialogWindow() override;
        
        
        virtual UI::Template ColorPlane() override;
        
        
        virtual UI::Template ColorPicker() override;
        

        virtual int GetSpacing() const override {
            return Spacing;
        }

        virtual int GetEmSize() const override {
            return lettervsize.first + lettervsize.second;
        }
        
        virtual int GetUnitSize() const override {
            return BorderedWidgetHeight; //UnitWidth = Bordered height
        }
        
        /// Returns the foreground color for the requested designation.
        virtual Graphics::RGBA Forecolor(Graphics::Color::Designation designation) const override {
            return Colors[designation].Forecolor;
        }
        
        /// Returns the background for the requested designation.
        virtual Graphics::RGBA Backcolor(Graphics::Color::Designation designation) const override {
            return Colors[designation].Backcolor;
        }
        
        /// Returns an printer instance that will render the requested text style
        virtual const Graphics::StyledPrinter &Printer(const Graphics::NamedFont &type) const override {
            Utils::NotImplemented();
        }
        
        /// Returns an advanced printer instance that will be able to render any text style
        virtual const Graphics::AdvancedPrinter &Printer() const override {
            Utils::NotImplemented();
        }
        
        Graphics::AnimationProvider &GetAsset(const AssetID &id);
        
        
        int Spacing       = 4;
        int ObjectHeight  = 15;
        int ObjectBorder  = 2;
        float ShapeBorder = 2;
        
        /// This function will update default widget dimensions. Call this function after
        /// setting up or changing borders, font size
        void UpdateDimensions();
        
        /// This function will update default widget borders depending on the font size
        void UpdateBorders(bool smooth = true);

        Graphics::StyledPrinter RegularFont;
        Graphics::StyledPrinter CenteredFont;
        Graphics::StyledPrinter BoldFont;
        Graphics::StyledPrinter TitleFont;
        Graphics::StyledPrinter SubtitleFont;
        Graphics::StyledPrinter SmallFont;
        Graphics::StyledPrinter InfoFont;

        struct FocusInfo {
            int             Width   = 1;
            //focus to content spacing
            int             Spacing = 1;
        } Focus;
        
        struct BorderInfo {
            int Width                    = 2;
            int Radius                   = 0;
            int Divisions                = 1;
        } Border;
        
        Graphics::Color::TripletPack Colors = {
            {Graphics::Color::Regular, {Graphics::Color::Charcoal, {Graphics::Color::Ivory, 0.8}}},
            {Graphics::Color::Hover, {Graphics::Color::Charcoal, {Graphics::Color::Tan, Graphics::Color::Ivory, 0.5}}},
            {Graphics::Color::Down, {Graphics::Color::Charcoal, {Graphics::Color::Crimson, Graphics::Color::Ivory, 0.8}}},
            {Graphics::Color::Disabled, {{Graphics::Color::Grey, 0.8}, Graphics::Color::LightGrey}},
            {Graphics::Color::Edit, {Graphics::Color::Charcoal, Graphics::Color::White}},
            {Graphics::Color::Container, {Graphics::Color::Charcoal, {Graphics::Color::Grey, Graphics::Color::Ivory, 0.5}}},
            {Graphics::Color::PassiveContiner, {{Graphics::Color::White, 0.8}, {Graphics::Color::SemiDarkGrey, 0.9}, Graphics::Color::SemiDarkGrey}},
            {Graphics::Color::ActiveContainer, {{Graphics::Color::White, 0.9}, {Graphics::Color::Charcoal, 0.9}, Graphics::Color::Charcoal}},
            {Graphics::Color::Selection, {Graphics::Color::Charcoal, {Graphics::Color::Charcoal, 0.4}, Graphics::Color::Transparent}},
            {Graphics::Color::Groove, {{Graphics::Color::White, 0.9}, {Graphics::Color::Charcoal, 0.5}, Graphics::Color::Transparent}},
            {Graphics::Color::Info, {{Graphics::Color::DarkBlue, 0.9}, {Graphics::Color::BabyBlue, 0.8}, Graphics::Color::Charcoal}},
            {Graphics::Color::Odd, {Graphics::Color::Charcoal, {Graphics::Color::Ivory, 0.2}}},
            {Graphics::Color::Even, {{Graphics::Color::DarkBlue, 0.9}, {Graphics::Color::DarkGrey, 0.2}}},
            {Graphics::Color::Active, {Graphics::Color::Ivory, {Graphics::Color::Charcoal, 0.8}, Graphics::Color::Transparent}},
            {Graphics::Color::Error, {Graphics::Color::DarkRed, {Graphics::Color::White, 0.2}}},
            {Graphics::Color::Title, {Graphics::Color::DarkGreen, Graphics::Color::Transparent}},
        };
        
        
        /// This is the width of a three cell widget
        int WidgetWidth = 64;
        
        /// This is the height of a bordered widget
        int BorderedWidgetHeight = 32;
        
        /// This is the height of a non-bordered widget
        int WidgetHeight = 24;
        
        /// This controls the automatic spacing. After changing this member
        /// you need to call UpdateDimensions to get the desired effect.
        float Density = 7.5;

    private:
        Graphics::BitmapRectangleProvider *makeborder(Graphics::RGBA border, Graphics::RGBA bg, AssetID::BorderSide borders, int borderwidth = -1, int borderradius = -1);
        Graphics::BitmapRectangleProvider *makecheckeredbg();
        Graphics::RectangleProvider *makefocusborder();
        UI::Template makepanel(SimpleGenerator::AssetID::BorderSide edge, bool scrollers, bool spacing = true);
        //rotation 0 is up
        Graphics::Bitmap *arrow(Graphics::RGBA color, Geometry::Size size, float rotation);
        Graphics::Bitmap *cross(Graphics::RGBA color, Geometry::Size size);
        Graphics::Bitmap *box(Graphics::RGBA color, Geometry::Size size);
        //will fit into the box of the same size
        Graphics::Bitmap *tick(Graphics::RGBA color, Geometry::Size size);
        Graphics::Bitmap *emptycircle(Graphics::RGBA color, Geometry::Size size);
        //will fit into the box of the same size
        Graphics::Bitmap *circlefill(Graphics::RGBA color, Geometry::Size size);
        
        float expandedradius(float pixels) {
            if(Border.Radius)
                return Border.Radius+pixels;
            else
                return (float)Border.Radius;
        }
        
        UI::Template maketemplate();
        
        void setupfocus(UI::GraphicsTemplate &focus);
        
        Graphics::GlyphRenderer *regularrenderer = nullptr;
        Graphics::GlyphRenderer *boldrenderer = nullptr;
        Graphics::GlyphRenderer *titlerenderer = nullptr;
        Graphics::GlyphRenderer *subtitlerenderer = nullptr;
        Graphics::GlyphRenderer *smallrenderer = nullptr;
        
        Containers::Collection<Graphics::Drawable> drawables;
        Containers::Collection<Graphics::AnimationProvider> providers;
        Containers::Hashmap<AssetID, Graphics::AnimationProvider> assets;
        
        UI::Template listbox_listitem;
        UI::Template empty;

        std::pair<int, int> lettervsize, asciivsize;
    };
    
}}
