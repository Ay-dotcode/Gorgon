

#include <Gorgon/Window.h>
#include <Gorgon/WindowManager.h>
#include <Gorgon/Main.h>
#include "Gorgon/Graphics/Texture.h"
#include "Gorgon/Graphics/Bitmap.h"
#include "Gorgon/Graphics/Layer.h"
#include "Gorgon/Resource/Image.h"
#include "Gorgon/Encoding/JPEG.h"
#include "Gorgon/Input/Layer.h"
#include "Gorgon/Graphics/BitmapFont.h"

#include <chrono>

using Gorgon::Window;
using Gorgon::Geometry::Point;
namespace Graphics = Gorgon::Graphics;
namespace Input = Gorgon::Input;
namespace Mouse = Gorgon::Input::Mouse;
namespace WM = Gorgon::WindowManager;

int main() {
	
	Gorgon::Initialize("Window-test");
    
    auto &monitors = WM::Monitor::Monitors();
    for(auto &monitor : monitors) {
        std::cout<<monitor.GetName()<<": "<<monitor.GetArea()<<"  --  "<<monitor.GetUsable()<<std::endl;
    }
    
    std::cout<<WM::Monitor::Primary().GetName()<<": "<<WM::Monitor::Primary().GetLocation()<<std::endl;
    //std::cout<<WM::GetScreenRegion(0)<<std::endl;
    
	Window wind({800, 800}, "windowtest");
	Graphics::Initialize();

	Graphics::Layer l;
	wind.Add(l);

	Graphics::Bitmap img;
	if(!img.Import("test.png")) {
        std::cout<<"Test.png is not found"<<std::endl;
        exit(0);
    }
	 
	img.Prepare();

	/*img = Graphics::Bitmap({200, 200}, Graphics::ColorMode::Alpha);
	for(int x = 0; x<200; x++)
		for(int y = 0; y<200; y++) {
			img({x, y}, 0) = x;
		}

	img.Prepare();*/
	
	Graphics::Bitmap img2({16, 400}, Graphics::ColorMode::RGB);

	for(int x = 0; x<16; x++)
		for(int y = 0; y<400; y++)
			for(int c=0;c<3;c++)
				img2({x, y}, c) = 0x80;

	img2.Prepare();
	//img2.Draw(l, 0, 0);

	
	for(int i=0; i<4; i++)
		img2.Draw(l, 25+16+i*32, 0);

	for(int i=0; i<10; i++)
		img.Draw(l, 150, 150);

	img.Draw(l, 50, 50, {.2f, .2f, .8f, 1.f});
    
    Graphics::BitmapFont fnt;
	fnt.ImportFolder("Victoria", Graphics::BitmapFont::Automatic, 0, "", -1, true, true, false);
    fnt.Pack();

	Graphics::StyledRenderer sty(fnt);
	sty.UseFlatShadow({0.f, 1.0f}, {.75, 0});
	sty.SetColor({0.5f, 0.8f, 0.8f});
	sty.JustifyLeft();
	
	//sty.Print(l, "a", 250, 200);

	sty.Print(l, "\xf0\x90\x8d\x88Lor|em ipsum\xe2\x80\xa8""dolor sit amet, consecteturadipiscingelitseddoeiusmodtemporincididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.", 0, 0, 120);
	sty.Print(l, "abc\tfgh\n1234 dsda\tasdf dsgh", 250, 200);
	sty.DisableShadow();
	sty.Print(l, "abc\tfgh\n1234 dsda\tasdf dsgh", 250, 220);

	wind.KeyEvent.Register([](Input::Key key, bool state) {
		if (!state && (key == 27 || key == 65307))
			exit(0);
		return false;
	});
	
	wind.CharacterEvent.Register([](Input::Key key) {
        if(key == '\r') std::cout<<std::endl;
		std::cout<<char(key);
        std::cout.flush();
        
		return true;
	});

	Gorgon::Input::Layer il;
	wind.Add(il);

    il.Move({25, 0});
	il.Resize({128, 0});

	il.SetHitCheck([](Point location) {
		return (location.X/16)%2 != 0;
	});

	il.SetClick([](Point location, Mouse::Button button) {
		std::cout<<button<<": "<<location<<std::endl;
	});

	il.SetDown([](Point location, Mouse::Button button) {
		std::cout<<"Down: "<<button<<": "<<location<<std::endl;
	});

	il.SetUp([](Point location, Mouse::Button button) {
		std::cout<<"Up: "<<button<<": "<<location<<std::endl;
	});

	il.SetOver([&wind]() {
		std::cout<<"Over"<<std::endl;
	});

	il.SetOut([]() {
		std::cout<<"Out"<<std::endl;
	});

	il.SetScroll([](Point location, float amount) {
		std::cout<<"Scrolled "<<amount<<" times at "<<location<<std::endl;
	});
	
	auto p = std::chrono::high_resolution_clock::now();

	for(int i=0; i<10; i++) {
		Gorgon::NextFrame();
	}

	auto ft = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()-p).count()/10000.f;

	std::cout<<"Frame time: "<<ft<<"ms, "<<1000.f/ft<<" fps"<<std::endl;

	while(true) {
		Gorgon::NextFrame();
	}

	return 0;
	
}
