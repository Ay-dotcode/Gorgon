//Copy Resources/Testing/Victoria to Testing/Runtime (in VS you need to fix running directory)
#include <map>

#include <Gorgon/Window.h>
#include <Gorgon/Main.h>
#include "Gorgon/Graphics/BitmapFont.h"
#include "Gorgon/Graphics/Layer.h"

#include "Gorgon/Graphics/HTMLRenderer.h"

using Gorgon::Window;
using Gorgon::Geometry::Point;
namespace Graphics = Gorgon::Graphics;
namespace Input = Gorgon::Input;
namespace Resource = Gorgon::Resource;


int main() {
	
	Gorgon::Initialize("HTMLRenderer-test");
        
	Window wind({800, 600}, "htmlrenderertest", "HTML Renderer Test");
	Graphics::Initialize();

	wind.DestroyedEvent.Register([]{
		exit(0);
	});

	Graphics::Layer l;
	wind.Add(l);
    
    Graphics::BitmapFont victoria, vicbold;
    victoria.ImportFolder("Victoria");
    vicbold.ImportFolder("VictoriaBold");
    
    std::map<Graphics::FontFamily::Style, Graphics::GlyphRenderer*> fonts = 
        { {Graphics::FontFamily::Style::Normal, &victoria},
          {Graphics::FontFamily::Style::Bold,   &vicbold,} };
          
    Graphics::FontFamily family(fonts);
    
    family.GetGlyphRenderer(Graphics::FontFamily::Style::Bold);
    
	Graphics::HTMLRenderer sty(family);
	sty.Print(l, "<strike><b>AAAA</b>abc</strike><u>0000000000</u><strike>lulz</strike>", 250, 240);
    
	while(true) {
		Gorgon::NextFrame();
	}
	
	return 0;
}
