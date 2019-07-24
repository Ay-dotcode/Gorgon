#pragma once

#include "../UI/Template.h"
#include "../CGI/Line.h"
#include "../CGI/Polygon.h"
#include "../Graphics/Color.h"
#include "../Graphics/Font.h"


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
		explicit SimpleGenerator(int fontsize = 15, std::string fontname = "");

        
		virtual ~SimpleGenerator();

		virtual UI::Template Button(
			Geometry::Size size = {100, 40}
		) override;

		int Spacing = 5;

		Graphics::StyledRenderer RegularFont;
        
        struct BorderInfo {
            int Width             = 2;
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
        } Forecolor;
        
	private:
		Graphics::GlyphRenderer *regularrenderer = nullptr;
		Containers::Collection<Graphics::Drawable> drawables;
		Containers::Collection<Graphics::AnimationProvider> providers;

	};

}}
