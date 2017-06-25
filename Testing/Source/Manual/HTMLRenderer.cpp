//Copy Resources/Testing/Victoria to Testing/Runtime (in VS you need to fix running directory)

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
    
    
	Graphics::HTMLRenderer sty(vicbold);
	sty.Print(l, "<u>CENGIZ </u>KANDEMIR", 250, 240);
    
	while(true) {
		Gorgon::NextFrame();
	}
	
	return 0;
}
