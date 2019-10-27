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
        
        /// Generates a button template with the given default size.
        virtual UI::Template Button(Geometry::Size size) = 0;
        
        /// Generates a button template with the given default size.
        virtual UI::Template IconButton(Geometry::Size size) = 0;
        
        virtual UI::Template Checkbox(Geometry::Size size) = 0;
        
        virtual UI::Template RadioButton(Geometry::Size size) = 0;
        
        virtual UI::Template Label(Geometry::Size size) = 0;
		
		virtual UI::Template ErrorLabel(Geometry::Size size) = 0;
		        
        virtual UI::Template BlankPanel(Geometry::Size size) = 0;
    };
    
    /**
     * This class generates very simple templates. Hover and down states are marked
     * with simple fore and background color changes. For background, hover and down
     * state colors are blended with the regular color. Font is shared, thus any
     * changes to it will effect existing templates too.
     */
	class SimpleGenerator : public Generator {
	public:
		/// Initializes the generator
		explicit SimpleGenerator(int fontsize = 14, std::string fontname = "");

        
        virtual ~SimpleGenerator();
        
        virtual UI::Template Button(
            Geometry::Size size = {70, 32}
        ) override;
        
        
        virtual UI::Template IconButton(
            Geometry::Size size = {24, 24}
        ) override;
        

        virtual UI::Template Checkbox(
            Geometry::Size size = {166, 24}
        ) override;
        
        virtual UI::Template RadioButton(
            Geometry::Size size = {155, 24}
        ) override;
        
        virtual UI::Template Label(
            Geometry::Size size = {155, 24}
        ) override;


		virtual UI::Template ErrorLabel(
			Geometry::Size size = { 155, 24 }
		) override;
		
	
		
        virtual UI::Template BlankPanel(
            Geometry::Size size = {155, 300}
        ) override;
        
        Graphics::BitmapRectangleProvider &NormalBorder();
        Graphics::BitmapRectangleProvider &HoverBorder();
        Graphics::BitmapRectangleProvider &DownBorder();
        
		int Spacing      = 4;
		int ObjectHeight = 15;
		int ObjectBorder = 2;

		Graphics::StyledRenderer RegularFont;

		struct FocusInfo {
			Graphics::RGBA	Color = {Graphics::Color::Charcoal, 0.7};
			int				Width = 1;
		} Focus;
        
        struct BorderInfo {
            int Width             = 2;
            int Radius            = 0;
            Graphics::RGBA Color  = Graphics::Color::Charcoal;
        } Border;
        
        struct BackgroundInfo {
            Graphics::RGBA Regular = {Graphics::Color::Ivory, 0.8};
            Graphics::RGBA Hover   = {Graphics::Color::White, 0.2};
            Graphics::RGBA Down    = {Graphics::Color::Crimson, 0.2};
            
        } Background;
        
        struct ForecolorInfo {
            Graphics::RGBA Regular = Graphics::Color::Charcoal;
            Graphics::RGBA Hover   = Graphics::Color::Black;
            Graphics::RGBA Down    = Graphics::Color::Black;
			Graphics::RGBA Error = Graphics::Color::Red;
        } Forecolor;
		
		
    private:
        Graphics::BitmapRectangleProvider makeborder(Graphics::RGBA border, Graphics::RGBA bg);
        
		Graphics::GlyphRenderer *regularrenderer = nullptr;
		Containers::Collection<Graphics::Drawable> drawables;
		Containers::Collection<Graphics::AnimationProvider> providers;

	};

}}
