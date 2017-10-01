//Copy Resources/Testing/Victoria to Testing/Runtime (in VS you need to fix running directory)
#include <map>

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
    
    std::map<Graphics::FontFamily::Style, Graphics::GlyphRenderer*> fonts = 
        { {Graphics::FontFamily::Style::Normal, &victoria},
          /*{Graphics::FontFamily::Style::Bold,   &vicbold},
          {Graphics::FontFamily::Style::Bold,   &vicboldlarge}*/ };
          
    Graphics::FontFamily family(fonts);
    
    Graphics::HTMLRenderer sty(family);

    
    sty.Print(layer, "<u><b>AAAA</b> BBBB</u> <strike><b>CCCC</b> DDDD</strike>", 250, 270);
    sty.Print(layer, "<u>AAAA <b>BBBB</b></u> <strike>CCCC <b>DDDD</b></strike>", 250, 300);
    
    
    sty.Print(layer, "<b>ABCDEFG</b>", 250, 330);
    sty.Print(layer, "<strike><b>Annie, are you okay?</b></strike>", 250, 360);
    
    sty.Print(layer, "<u><strike><b>Are you okay Annie?</b></strike></u>", 250, 390);
    
    sty.Print(layer, "<b>the<b/> <u><strike>quick <b>brown</b></strike> fox</u> jumps over the lazy dog", 250, 420);
    sty.Print(layer, "<u>the <b>quick brown <strike>fox jumps</b> over the lazy</u> dog</strike>", 250, 450);
    
    
    sty.Print(layer, "the quick brown fox jumps over the lazy dog", 250, 480);
    sty.Print(layer, "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG", 250, 510);
    
    
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
    
    while(true) {
		Gorgon::NextFrame();
	}
	
	return 0;
}
