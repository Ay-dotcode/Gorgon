#pragma once

#include "../UI/Template.h"
#include "../CGI/Line.h"
#include "../CGI/Polygon.h"
#include "../Graphics/Color.h"
#include "../Graphics/Font.h"


namespace Gorgon { namespace Widgets {

	/**
	 * Functions in this namespace generates templates for widgets. To use this system
	 * create a generator first, change any desired properties and call individual
	 * generation functions such as Button. Every function allows override. This object
	 * holds the generated images and fonts, thus, it should not be destroyed before the 
	 * templates are destroyed.
	 */
	class Generator {
	public:
		/// 
		explicit Generator(int fontsize = 15, std::string fontname = "");

		~Generator();

		/// Defines area parameters for the UI that will be generated
		struct AreaParams {
			/// If set and possible, the values will be ignored and automatically
			/// generated.
			bool            Automatic       = true;

			/// Width of the area border
			int			BorderWidth			=  2;

			/// Color of the area border
			Graphics::RGBA	BorderColor		= Graphics::Color::Charcoal;

			/// Background color of the area
			Graphics::RGBA	BackgroundColor	= {Graphics::Color::Ivory, 0.8};

			/// Radius of the rounded corners, 0 to disable
			float			BorderRadius	=  0;

			/// The number of points that will be used for corner smoothing. 0 will
			/// give a bevel edge
			int				CornerSmoothing = 0;

			/// Padding inside the area. Set -1 to automatically use standard spacing
			int				Padding		    = -1;
		};

		/// Defines text parameters for the UI that will be generated
		struct TextParams {
			TextParams() = default;

			explicit TextParams(Graphics::RGBA color) : Automatic(false), Color(color) { }
			/// If set and possible, the values will be ignored and automatically.
			/// generated.
			bool					 Automatic	= true;

			/// Renderer to be used. If left nullptr, a default will be provided.
			Graphics::TextRenderer *Renderer	= nullptr;

			/// The color to be used
			Graphics::RGBA          Color		= Graphics::Color::Black;
		};


		UI::Template Button(
			int spacing = 5,
			Geometry::Size defsize = {100, 40},
			AreaParams normal_background = {},
			AreaParams hover_background  = {},
			AreaParams down_background   = {},
			TextParams normal_text       = {},
			TextParams hover_text        = TextParams{Graphics::Color::Blue},
			TextParams down_text         = {}
		);

		int Spacing = 5;
		int BorderRadius = 0;
		int CornerSmoothing = 0;

		std::string FontFilename;

		Graphics::StyledRenderer RegularFont;

	private:
		Graphics::GlyphRenderer *regularrenderer = nullptr;
		Containers::Collection<Graphics::Drawable> drawables;
		Containers::Collection<Graphics::AnimationProvider> providers;

	};

}}
