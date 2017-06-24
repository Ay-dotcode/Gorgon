//Copy Resources/Testing/Victoria to Testing/Runtime (in VS you need to fix running directory)

#include <Gorgon/Window.h>
#include <Gorgon/Main.h>
#include "Gorgon/Graphics/Bitmap.h"
#include "Gorgon/Graphics/Layer.h"
#include <Gorgon/Resource/File.h>
#include <Gorgon/Resource/Font.h>


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
    
    //load to use
    Resource::File f2;
    f2.LoadFile("font-test.gor");
    f2.Prepare();
    f2.Discard();
    
    Graphics::BitmapFont fnt = std::move(dynamic_cast<Graphics::BitmapFont&>(f2.Root().Get<Resource::Font>(0).GetRenderer()));
    
	Graphics::HTMLRenderer sty(fnt);
	sty.Print(l, "<u>CENGIZ </u>KANDEMIR", 250, 240);
    
	while(true) {
		Gorgon::NextFrame();
	}
}
