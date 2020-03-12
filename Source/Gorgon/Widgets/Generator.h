#pragma once

#include "../UI/Template.h"
#include "../Graphics/Color.h"
#include "../Graphics/Font.h"
#include "../Graphics/Rectangle.h"

namespace Gorgon { namespace Widgets {

    /**
    * Generators create templates for widgets. First setup a generator, then
    * call specific generation functions.
    */
    class Generator {
    public:
        virtual ~Generator() { }
        
        /// Generates a button template
        virtual UI::Template Button() = 0;
        
        /// Generates a button template with the given default size.
        virtual UI::Template IconButton(Geometry::Size iconsize) = 0;
        
        
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
        
        /// Initializes the generator
        explicit SimpleGenerator(int fontsize = 14, std::string fontname = "");

        
        virtual ~SimpleGenerator();
        
        virtual UI::Template Button() override;
        
        virtual UI::Template IconButton(Geometry::Size iconsize = {16, 16}) override;
        
        
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
        
        Graphics::BitmapRectangleProvider &NormalBorder();
        Graphics::BitmapRectangleProvider &HoverBorder();
        Graphics::BitmapRectangleProvider &DownBorder();
        Graphics::BitmapRectangleProvider &DisabledBorder();
        
        Graphics::BitmapRectangleProvider &PanelBorder(int missingedge = 0);
        
        Graphics::BitmapRectangleProvider &NormalEditBorder();
        Graphics::BitmapRectangleProvider &HoverEditBorder();
        Graphics::BitmapRectangleProvider &ReadonlyBorder();
        
        Graphics::BitmapRectangleProvider &NormalEmptyBorder();
        
        Graphics::BitmapRectangleProvider &NormalBG();
        Graphics::BitmapRectangleProvider &HoverBG();
        Graphics::BitmapRectangleProvider &DownBG();
        Graphics::BitmapRectangleProvider &DisabledBG();
        
        Graphics::RectangleProvider &FocusBorder();
        
        int Spacing      = 4;
        int ObjectHeight = 15;
        int ObjectBorder = 2;
        
        /// This function will update default widget dimensions. Call this function after
        /// setting up or changing borders, font size
        void UpdateDimensions();
        
        /// This function will update default widget borders depending on the font size
        void UpdateBorders(bool smooth = true);

        Graphics::StyledRenderer RegularFont;

        struct FocusInfo {
            Graphics::RGBA  Color = {Graphics::Color::Charcoal, 0.7};
            int             Width = 1;
        } Focus;
        
        struct BorderInfo {
            int Width               = 2;
            int Radius              = 0;
            int Divisions           = 1;
            Graphics::RGBA Color    = Graphics::Color::Charcoal;
            Graphics::RGBA Disabled = {Graphics::Color::Grey, 0.5};
        } Border;
        
        struct BackgroundInfo {
            Graphics::RGBA Regular = {Graphics::Color::Ivory, 0.8};
            Graphics::RGBA Hover   = {Graphics::Color::LightTan, 0.5};
            Graphics::RGBA Down    = {Graphics::Color::Crimson, 0.2};
            Graphics::RGBA Disabled=  Graphics::Color::LightGrey;

            
            Graphics::RGBA Edit    = {Graphics::Color::White};
            Graphics::RGBA Panel   = {Graphics::Color::Grey, Graphics::Color::Ivory, 0.5};
            
            Graphics::RGBA Selected= {Graphics::Color::Charcoal, 0.4};
        } Background;
        
        struct ForecolorInfo {
            Graphics::RGBA Regular = Graphics::Color::Charcoal;
            Graphics::RGBA Disabled= {Graphics::Color::Grey, 0.5};
            Graphics::RGBA Hover   = Graphics::Color::Black;
            Graphics::RGBA Down    = Graphics::Color::Black;
            Graphics::RGBA Error   = Graphics::Color::Red;
        } Forecolor;
        
        
        /// This is the width of a one cell widget
        int WidgetWidth = 64;
        
        /// This is the height of a bordered widget
        int BorderedWidgetHeight = 32;
        
        /// This is the height of a non-bordered widget
        int WidgetHeight = 24;

    private:
        Graphics::BitmapRectangleProvider *makeborder(Graphics::RGBA border, Graphics::RGBA bg, int missingedge = 0);
        Graphics::RectangleProvider *makefocusborder();
        UI::Template makepanel(int missingedge);
        
        Graphics::GlyphRenderer *regularrenderer = nullptr;
        Containers::Collection<Graphics::Drawable> drawables;
        Containers::Collection<Graphics::AnimationProvider> providers;
        
        Graphics::BitmapRectangleProvider *normalborder = nullptr;
        Graphics::BitmapRectangleProvider *hoverborder = nullptr;
        Graphics::BitmapRectangleProvider *downborder = nullptr;
        Graphics::BitmapRectangleProvider *disabledborder = nullptr;
        
        Graphics::BitmapRectangleProvider *panelborders[5] = {};
        
        Graphics::BitmapRectangleProvider *normaleditborder = nullptr;
        Graphics::BitmapRectangleProvider *hovereditborder = nullptr;
        Graphics::BitmapRectangleProvider *readonlyborder = nullptr;
        
        Graphics::BitmapRectangleProvider *normalemptyborder = nullptr;
        
        Graphics::BitmapRectangleProvider *normalbg = nullptr;
        Graphics::BitmapRectangleProvider *hoverbg = nullptr;
        Graphics::BitmapRectangleProvider *downbg = nullptr;
        Graphics::BitmapRectangleProvider *disabledbg = nullptr;
        
        Graphics::RectangleProvider *focusborder = nullptr;

        std::pair<int, int> lettervsize, asciivsize;
    };

}}
