#include "Generator.h"
#include "../OS.h"
#include "../Filesystem.h"
#include "../Graphics/FreeType.h"
#include "../Graphics/BitmapFont.h"
#include "../Graphics/BlankImage.h"
#include "../Graphics/Rectangle.h"

//This should be fixed
#ifdef LINUX
#include <unistd.h>
#include <wait.h>

namespace Gorgon { namespace OS {
    bool Start(const std::string &name, std::streambuf *&buf, const std::vector<std::string> &args);
} }
#endif

namespace Gorgon { namespace Widgets { 

	SimpleGenerator::SimpleGenerator(int fontsize, std::string fontname) {
#ifdef WIN32
		fontname = Filesystem::Join(OS::GetEnvVar("WINDIR"), "Fonts/tahoma.ttf");
#else
        bool found = false;
        
        try {
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
                        fontname = fname;
                        found = true;
                        break;
                    }
                }
            }
        }
        catch(...) {
            found = false;
        }

		if(!found)
			fontname = "/usr/share/fonts/gnu-free/FreeSans.ttf";
#endif

		auto &regular = *new Graphics::FreeType();
		regular.LoadFile(fontname, fontsize);

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

	SimpleGenerator::~SimpleGenerator() {
		if(regularrenderer)
			delete regularrenderer;

		providers.DeleteAll();
		drawables.DeleteAll();
	}

	UI::Template SimpleGenerator::Button(Geometry::Size defsize) {

		UI::Template temp;
		temp.SetSize(defsize);

        auto &bi = *new Graphics::BlankImage({Border.Width, Border.Width}, Border.Color);
        drawables.Add(bi);

		{
			auto &bg_n = temp.AddContainer(0, UI::ComponentCondition::Always);
			bg_n.SetSizing(UI::ComponentTemplate::Automatic);
			//assuming border radius = 0
			auto &ci = *new Graphics::BlankImage({32, 32}, Background.Regular);
            drawables.Add(ci);

			auto &rect = *new Graphics::RectangleProvider(bi, bi, bi, bi, ci, bi, bi, bi, bi);

			bg_n.Background.SetAnimation(rect);
			providers.Add(rect);
			bg_n.SetPadding(Spacing);
			bg_n.AddIndex(1);
		}

		{
			auto &txt_n = temp.AddTextholder(1, UI::ComponentCondition::Always);
			txt_n.SetRenderer(RegularFont);
			txt_n.SetColor(Forecolor.Regular);
			txt_n.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
			txt_n.SetDataEffect(UI::ComponentTemplate::Text);
			//txt_n.SetClip(true);
			txt_n.SetSize(100, 100, UI::Dimension::Percent);
			txt_n.SetSizing(UI::ComponentTemplate::ShrinkOnly);
		}

		{
            auto &bg_h = temp.AddContainer(0, UI::ComponentCondition::Hover);
			bg_h.SetSizing(UI::ComponentTemplate::Automatic);
            
			//assuming border radius = 0
            auto c = Background.Regular;
            c.Blend(Background.Hover);
			auto &ci = *new Graphics::BlankImage({32, 32}, c);
            drawables.Add(ci);

			auto &rect = *new Graphics::RectangleProvider(bi, bi, bi, bi, ci, bi, bi, bi, bi);

			bg_h.Background.SetAnimation(rect);
			providers.Add(rect);
			bg_h.SetPadding(Spacing);
			bg_h.AddIndex(1);
		}

		{
            auto &txt_h = temp.AddTextholder(1, UI::ComponentCondition::Hover);
            
            auto c = Forecolor.Regular;
            c.Blend(Forecolor.Hover);
            txt_h.SetRenderer(RegularFont);
            txt_h.SetColor(c);
            txt_h.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
            txt_h.SetDataEffect(UI::ComponentTemplate::Text);
			txt_h.SetClip(true);
			txt_h.SetSize(100, 100, UI::Dimension::Percent);
			txt_h.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
		}

		{
			auto &bg_d = temp.AddContainer(0, UI::ComponentCondition::Down);
			bg_d.SetSizing(UI::ComponentTemplate::Automatic);
            
			//assuming border radius = 0
            auto c = Background.Regular;
            c.Blend(Background.Down);
			auto &ci = *new Graphics::BlankImage({32, 32}, c);
            drawables.Add(ci);

			auto &rect = *new Graphics::RectangleProvider(bi, bi, bi, bi, ci, bi, bi, bi, bi);

			bg_d.Background.SetAnimation(rect);
			providers.Add(rect);
			bg_d.SetPadding(Spacing);
			bg_d.AddIndex(1);
		}

		{
            auto &txt_d = temp.AddTextholder(1, UI::ComponentCondition::Down);
            
            auto c = Forecolor.Regular;
            c.Blend(Forecolor.Down);
            txt_d.SetRenderer(RegularFont);
            txt_d.SetColor(c);
            txt_d.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
            txt_d.SetDataEffect(UI::ComponentTemplate::Text);
			txt_d.SetClip(true);
			txt_d.SetSize(100, 100, UI::Dimension::Percent);
			txt_d.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        }

		return temp;
	}

	UI::Template SimpleGenerator::Checkbox(Geometry::Size defsize) {
		UI::Template temp;
		temp.SetSize(defsize);

		return temp;
	}

}}
