//Copy Resources/Testing/Victoria to Testing/Runtime (in VS you need to fix running directory)
#include <unordered_map>

#include <Gorgon/Main.h>
#include <Gorgon/Window.h>
#include <Gorgon/Graphics/BitmapFont.h>
#include <Gorgon/Graphics/BlankImage.h>
#include <Gorgon/Graphics/EmptyImage.h>
#include <Gorgon/Graphics/Font.h>
#include <Gorgon/Graphics/FreeType.h>
#include <Gorgon/Graphics/HTMLRenderer.h>
#include <Gorgon/Graphics/Layer.h>

using Gorgon::Window;
using Gorgon::Geometry::Point;
namespace Graphics = Gorgon::Graphics;
namespace Input = Gorgon::Input;
namespace Resource = Gorgon::Resource;

namespace
{
    class Printer
    {
    public:
        Printer(Graphics::Layer& layer, Graphics::HTMLRenderer& renderer, int x, int y)
            : layer(layer)
            , renderer(renderer)
            , x(x)
            , y(y)
            , bimg(2, 2)
        {}

        void Print(const std::string& str, int offset)
        {
            bimg.Draw(layer, x - 5, y + 7);
            renderer.Print(layer, str, x, y);
            y += offset;
        }

    private:
        Graphics::Layer& layer;
        Graphics::HTMLRenderer& renderer;
        int x;
        int y;
        Graphics::BlankImage bimg;
    };

    /*
    std::pair<Window, Graphics::Layer> init() {
        Gorgon::Initialize("HTMLRenderer-test");
        Graphics::HTMLRendererInternal::Logger.InitializeConsole();

        Window wind({800, 600}, "htmlrenderertest", "HTML Renderer Test");
        wind.DestroyedEvent.Register([]{
            exit(0);
        });

        Graphics::Layer layer;
        wind.Add(layer);

        Graphics::Initialize();

        return std::make_pair(std::move(wind), std::move(layer));
    }
    */
}


int main() {
    /*
    auto pair = init();
    auto& wind = pair.first;
    auto& layer = pair.second;
    */

    Gorgon::Initialize("HTMLRenderer-test");
    Graphics::HTMLRendererInternal::Logger.InitializeConsole();

    Window wind({800, 600}, "htmlrenderertest", "HTML Renderer Test");
    wind.DestroyedEvent.Register([]{
        exit(0);
    });

    Graphics::Layer layer;
    wind.Add(layer);

    Graphics::Initialize();

    Graphics::FreeType djv, djvbold, djvboldit;

    const int fontsize = 12;
    djv.LoadFile("dejavu/ttf/DejaVuSans.ttf");
    djv.LoadMetrics(fontsize);

    djvbold.LoadFile("dejavu/ttf//DejaVuSans-Bold.ttf");
    djvbold.LoadMetrics(fontsize);

    djvboldit.LoadFile("dejavu/ttf//DejaVuSans-BoldOblique.ttf");
    djvboldit.LoadMetrics(fontsize);

    std::unordered_map<Graphics::FontFamily::Style,
                       Graphics::GlyphRenderer*,
                       Graphics::FontFamily::HashType> fonts;

    fonts.emplace(Graphics::FontFamily::Style::Normal, &djv);
    fonts.emplace(Graphics::FontFamily::Style::Bold, &djvbold);
    fonts.emplace(Graphics::FontFamily::Style::Italic, &djvboldit);

    Graphics::FontFamily family(fonts);

    Graphics::HTMLRenderer sty(family);

    Printer printer(layer, sty, 250, 50);

    const int offset = 20;

    printer.Print("the quick brown fox jumps over the lazy dog", offset);
    printer.Print("THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG", offset);
    printer.Print("<b>the quick brown fox jumps over the lazy dog</b>", offset);
    printer.Print("<i>THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG</i>", offset);
    printer.Print("<strong>THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG</strong>", offset);

    printer.Print("<b>Annie, I bet you are okay...<br>Ann</b>ie?\?\?</br><b>Are you okay, Annie?</b>", offset * 4);

    printer.Print("<u color=\"black\"><b>AAAA</b> BBBB</u> <strike color=\"green\"><strong>CCCC</strong> DDDD</strike>", offset);
    printer.Print("<u>AAAA <strong>BBBB</strong></u> <strike>CCCC <strong>DDDD</strong></strike>", offset);

    printer.Print("<u><strike><strong>Are you okay, Annie?</strong></strike></u>", offset);
    printer.Print("<strong>the </strong><u><strike>quick <strong>brown</strong></strike> fox</u> jumps over the lazy dog", offset);
    printer.Print("<u>the <strong>quick brown <strike>fox jumps</strong> over the lazy</u> dog</strike>", offset);
    printer.Print("<u>the <strong>quick brown <strike>fox <br>jumps</strong> over the lazy</u> dog</strike>", offset);

    /* BUG: the way glyphs are printed seem to depend on prior prints, perhaps we do not clear the state properly after each print?
    printer.Print("the quick brown fox jumps over the lazy dog", offset);
    printer.Print("THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG", offset);
    printer.Print("<b>the quick brown fox jumps over the lazy dog</b>", offset);
    printer.Print("<i>THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG</i>", offset);
    printer.Print("<strong>THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG</strong>", offset);

    printer.Print("<b>Annie, I bet you are okay...<br>Ann</b>ie?\?\?</br><b>Are you okay, Annie?</b>", offset * 4);

    printer.Print("<u color=\"black\"><b>AAAA</b> BBBB</u> <strike color=\"green\"><strong>CCCC</strong> DDDD</strike>", offset);
    printer.Print("<u>AAAA <strong>BBBB</strong></u> <strike>CCCC <strong>DDDD</strong></strike>", offset);
    */

    while(true){
        Gorgon::NextFrame();
    }

    return 0;
}
