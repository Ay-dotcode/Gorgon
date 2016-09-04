

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
    
	Window wind({400, 400}, "windowtest");
	Graphics::Initialize();

	Graphics::Layer l;
	wind.Add(l);

	Graphics::Bitmap img;
	img.Import("test.png");

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


	for(int i=0; i<10; i++)
		img.Draw(l, 150, 150);

	for(int i=0; i<4; i++)
		img2.Draw(l, 25+16+i*32, 0);

	img.Draw(l, 50, 50, {.2f, .2f, .8f, 1.f});
    
    Graphics::Bitmap chr1({10, 16}, Graphics::ColorMode::Alpha);
    chr1.Clean();
    chr1({1, 8}, 0) = 255;
    chr1({2, 9}, 0) = 255;
    chr1({3, 9}, 0) = 255;
    chr1({4, 10}, 0) = 255;
    chr1({4, 11}, 0) = 255;
    chr1({5, 11}, 0) = 255;
    chr1({6, 11}, 0) = 255;
    chr1.Prepare();
   
    Graphics::Bitmap chr2({10, 16}, Graphics::ColorMode::Alpha);
    chr2.Clean();
    chr2({1, 8}, 0) = 255;
    chr2({2, 9}, 0) = 255;
    chr2({3, 9}, 0) = 255;
    chr2({4, 10}, 0) = 255;
    chr2({4, 11}, 0) = 255;
    chr2({5, 11}, 0) = 255;
    chr2({6, 11}, 0) = 255;
    chr2({6, 10}, 0) = 255;
    chr2({6, 9}, 0) = 255;
    chr2({6, 8}, 0) = 255;
    chr2.Prepare();
    
    Graphics::BitmapFont fnt(12);
    fnt.AddGlyph('a', chr1, 12);
    fnt.AddGlyph('b', chr2, 10);
    
    fnt.Print(l, "aababba\tabb\nbba\ta", 0, 0, Graphics::RGBAf(1.0f));

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
