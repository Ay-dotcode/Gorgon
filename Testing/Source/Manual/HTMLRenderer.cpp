//Copy Resources/Testing/Victoria to Testing/Runtime (in VS you need to fix running directory)
#include <unordered_map>

#include <Gorgon/Window.h>
#include <Gorgon/Main.h>
#include "Gorgon/Graphics/BitmapFont.h"
#include "Gorgon/Graphics/Layer.h"
#include "Gorgon/Graphics/EmptyImage.h"

#include "Gorgon/Graphics/HTMLRenderer.h"

using Gorgon::Window;
using Gorgon::Geometry::Point;
namespace Graphics = Gorgon::Graphics;
namespace Input = Gorgon::Input;
namespace Resource = Gorgon::Resource;


int main() {
	
	Gorgon::Initialize("HTMLRenderer-test");
    Graphics::HTMLRendererInternal::Logger.InitializeConsole();
	Window wind({800, 600}, "htmlrenderertest", "HTML Renderer Test");
	Graphics::Initialize();

	wind.DestroyedEvent.Register([]{
		exit(0);
	});

	Graphics::Layer layer;
	wind.Add(layer);
    
    Graphics::BitmapFont victoria, vicbold, vicboldlarge;
    victoria.ImportFolder("Victoria");
    vicbold.ImportFolder("VictoriaBold");
    vicboldlarge.ImportFolder("VictoriaBoldLarge");
    
    std::unordered_map<Graphics::FontFamily::Style,
                      Graphics::GlyphRenderer*,
                      Graphics::FontFamily::HashType> fonts;
          
    fonts.emplace(Graphics::FontFamily::Style::Normal, &victoria);
    fonts.emplace(Graphics::FontFamily::Style::Bold, &vicbold);
    fonts.emplace(Graphics::FontFamily::Style::Large, &vicboldlarge);
    
    Graphics::FontFamily family(fonts);
    
    Graphics::HTMLRenderer sty(family);

    
    sty.Print(layer, "<b>Annie, I bet you are okay...<br>Ann</b>ie?\?\?</br><b>Woaaaaa wth?\?\?\?</b>", 250, 230);
    
    sty.Print(layer, "<u color=\"black\"><b>AAAA</b> BBBB</u> <strike color=\"green\"><strong>CCCC</strong> DDDD</strike>", 250, 270);
    
    sty.Print(layer, "<u>AAAA <strong>BBBB</strong></u> <strike>CCCC <strong>DDDD</strong></strike>", 250, 300);
    
    
    sty.Print(layer, "<strong>ABCDEFG</strong>", 250, 330);
    sty.Print(layer, "<strike><strong>Annie, are you okay?</strong></strike>", 250, 360);
    
    sty.Print(layer, "<u><strike><strong>Are you okay Annie?</strong></strike></u>", 250, 390);
    
    sty.Print(layer, "<strong>the </strong><u><strike>quick <strong>brown</strong></strike> fox</u> jumps over the lazy dog", 250, 420);
    sty.Print(layer, "<u>the <strong>quick brown <strike>fox jumps</strong> over the lazy</u> dog</strike>", 250, 450);
    
    
    sty.Print(layer, "<u>the <strong>quick brown <strike>fox <br>jumps</strong> over the lazy</u> dog</strike>", 250, 480);
    
    
    sty.Print(layer, "the quick brown fox jumps over the lazy dog", 250, 510);
    sty.Print(layer, "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG", 250, 540);
    
    
    Graphics::BlankImage bimg(1, 1);
    bimg.Draw(layer, 245, 270);
    bimg.Draw(layer, 245, 300);
	bimg.Draw(layer, 245, 330);
    bimg.Draw(layer, 245, 360);
    bimg.Draw(layer, 245, 390);
	bimg.Draw(layer, 245, 420);
    bimg.Draw(layer, 245, 450);
    bimg.Draw(layer, 245, 480);
    bimg.Draw(layer, 245, 510);
    bimg.Draw(layer, 245, 540);
    
    
    while(true) {
		Gorgon::NextFrame();
	}
	
	return 0;
}
