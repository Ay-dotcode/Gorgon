#include "Generator.h"
#include "../OS.h"
#include "../Filesystem.h"
#include "../Graphics/FreeType.h"
#include "../Graphics/BitmapFont.h"
#include "../Graphics/BlankImage.h"
#include "../Graphics/Rectangle.h"

namespace Gorgon { namespace Widgets { 

	Generator::Generator(int fontsize /* = 12 */, std::string fontname /* = "" */) {
#ifdef WIN32
		FontFilename = Filesystem::Join(OS::GetEnvVar("WINDIR"), "Fonts/tahoma.ttf");
#else
		bool found = false;
		std::streambuf *buf;
		OS::Start("fc-match", buf, {"-v", fontname == "" ? "sans" : fontname});

		if(buf) {
			std::istream in(buf);
			std::string line;
			while(getline(in, line)) {
				line = String::Trim(line);
				auto name = String::Extract(line, ':', true);
				if(name == "file") {
					String::Extract(line, '"', true);
					auto fname = String::Extract(line, '"', true);
					FontFilename = fname;
					found = true;
					break;
				}
			}
		}

		if(!found)
			FontFilename = "/usr/share/fonts/gnu-free/FreeSans.ttf";
#endif

		auto &regular = *new Graphics::FreeType();
		regular.LoadFile(FontFilename, fontsize);

		RegularFont.SetGlyphRenderer(regular);

		if(!regular.HasKerning()) {
			auto &bmpfnt = *new Graphics::BitmapFont(regular.MoveOutBitmap());
			RegularFont.SetGlyphRenderer(bmpfnt);
			bmpfnt.AutoKern();
			regularrenderer = &bmpfnt;

			delete &regular;
		}
		else {
			regularrenderer = &regular;
		}

	}

	Generator::~Generator() {
		if(regularrenderer)
			delete regularrenderer;

		providers.DeleteAll();
		drawables.DeleteAll();
	}

	UI::Template Generator::Button(int spacing, Geometry::Size defsize, 
								   AreaParams normal_background, AreaParams hover_background, AreaParams down_background, 
								   TextParams normal_text, TextParams hover_text, TextParams down_text) {

		UI::Template temp;
		temp.SetSize(defsize);

		auto &bg_n = temp.AddContainer(0, UI::ComponentCondition::Always);
		{
			//assuming border radius = 0
			auto &bi = *new Graphics::BlankImage({normal_background.BorderWidth, normal_background.BorderWidth}, normal_background.BorderColor);
			auto &ci = *new Graphics::BlankImage({32, 32}, normal_background.BackgroundColor);

			auto &rect = *new Graphics::RectangleProvider(bi, bi, bi, bi, ci, bi, bi, bi, bi);
			rect.OwnProviders();

			bg_n.Background.SetAnimation(rect);
			providers.Add(rect);
			bg_n.SetPadding(spacing);
			bg_n.AddIndex(1);
		}

		auto &txt_n = temp.AddTextholder(1, UI::ComponentCondition::Always);
		txt_n.SetRenderer(RegularFont);
		txt_n.SetColor(normal_text.Color);
		txt_n.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
		txt_n.SetDataEffect(UI::ComponentTemplate::Text);
		txt_n.SetSizing(UI::ComponentTemplate::Automatic);

		auto &bg_h = temp.AddContainer(0, UI::ComponentCondition::Hover);
		{
			//assuming border radius = 0
			auto &bi = *new Graphics::BlankImage({normal_background.BorderWidth, normal_background.BorderWidth}, normal_background.BorderColor);
			auto &ci = *new Graphics::BlankImage({32, 32}, normal_background.BackgroundColor);

			auto &rect = *new Graphics::RectangleProvider(bi, bi, bi, bi, ci, bi, bi, bi, bi);
			rect.OwnProviders();

			bg_h.Background.SetAnimation(rect);
			providers.Add(rect);
			bg_h.SetPadding(spacing);
			bg_h.AddIndex(1);
		}

		auto &txt_h = temp.AddTextholder(1, UI::ComponentCondition::Hover);
		txt_h.SetRenderer(RegularFont);
		txt_h.SetColor(hover_text.Color);
		txt_h.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
		txt_h.SetDataEffect(UI::ComponentTemplate::Text);
		txt_h.SetSizing(UI::ComponentTemplate::Automatic);

		return temp;
	}

}}
