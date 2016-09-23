

#include <Gorgon/Window.h>
#include <thread>
#include <Gorgon/WindowManager.h>
#include <Gorgon/Main.h>
#include "Gorgon/Graphics/Texture.h"
#include "Gorgon/Graphics/Bitmap.h"
#include "Gorgon/Graphics/Layer.h"
#include "Gorgon/Resource/Image.h"
#include "Gorgon/Encoding/JPEG.h"
#include "Gorgon/Input/Layer.h"
#include "Gorgon/Graphics/BitmapFont.h"
#include <Gorgon/Resource/File.h>
#include <Gorgon/Resource/Font.h>


#include <chrono>

using Gorgon::Window;
using Gorgon::Geometry::Point;
namespace Graphics = Gorgon::Graphics;
namespace Input = Gorgon::Input;
namespace Mouse = Gorgon::Input::Mouse;
namespace WM = Gorgon::WindowManager;
namespace Resource = Gorgon::Resource;

using namespace std::chrono_literals;

int main() {
	
	Gorgon::Initialize("Window-test");
    system("pwd");
    
    auto &monitors = WM::Monitor::Monitors();
    for(auto &monitor : monitors) {
        std::cout<<monitor.GetName()<<": "<<monitor.GetArea()<<"  --  "<<monitor.GetUsable()<<std::endl;
    }
    
    std::cout<<WM::Monitor::Primary().GetName()<<": "<<WM::Monitor::Primary().GetLocation()<<std::endl;
    //std::cout<<WM::GetScreenRegion(0)<<std::endl;
    
	Window wind({0, 0, 800, 600}, "windowtest");
	Graphics::Initialize();

	Graphics::Layer l;
	wind.Add(l);

	Graphics::Bitmap img;
	if(!img.Import("test.png")) {
        std::cout<<"Test.png is not found"<<std::endl;
        exit(0);
    }
	 
	img.Prepare();
	
	//img = Graphics::Bitmap({200, 200}, Graphics::ColorMode::Alpha);
	//for(int x = 0; x<200; x++)
	//	for(int y = 0; y<200; y++) {
	//		img({x, y}, 0) = x;
	//	}

	//img.Prepare();

	Graphics::Bitmap img2({3, 3}, Graphics::ColorMode::Alpha);

	for(int x = 0; x<3; x++)
		for(int y = 0; y<3; y++)
			for(int c=0; c<1; c++)
				img2({x, y}, c) = 0xff;

	img2.Prepare();

	Graphics::Bitmap img3({50, 50}, Graphics::ColorMode::Grayscale);

	for(int x = 0; x<img3.GetWidth(); x++)
		for(int y = 0; y<img3.GetHeight(); y++) {
			if((x/(img3.GetWidth()/2)) != (y/(img3.GetHeight()/2)))
				img3({x, y}, 0) = 0x10;
			else
				img3({x, y}, 0) = 0x30;
		}

	img3.Prepare();
	img3.DrawIn(l);

	
	for(int i=0; i<4; i++)
		img2.DrawStretched(l, 25+16+i*32, 0, 16, 400, {1.f, 1.f, 1.f, .3f});
		
	for(int i=0; i<10; i++)
		img.Draw(l, 150, 150);
	
	//img.Draw(l, 50, 50, {.2f, .2f, .8f, 1.f});
    
    Resource::File f;
    //f.LoadFile("test.gor");
    //f.Prepare();
    //f.Discard();
    Graphics::BitmapFont fnt;// = std::move(dynamic_cast<Graphics::BitmapFont&>(f.Root().Get<Resource::Font>(0).GetRenderer()));
	fnt.ImportFolder("Victoria", Graphics::BitmapFont::Automatic, 0, "", -1, true, false, false);
    fnt.Pack();
    
    std::cout<<"digit w: "<<fnt.GetDigitWidth()<<std::endl;
    std::cout<<"x-space: "<<fnt.GetGlyphSpacing()<<std::endl;
    std::cout<<"height: "<<fnt.GetHeight()<<std::endl;
    std::cout<<"line t: "<<fnt.GetLineThickness()<<std::endl;
    std::cout<<"max w: "<<fnt.GetMaxWidth()<<std::endl;
    std::cout<<"underline off: "<<fnt.GetUnderlineOffset()<<std::endl;
    std::cout<<"baseline: "<<fnt.GetBaseLine()<<std::endl;
    std::cout<<"is fixed: "<<fnt.IsFixedWidth()<<std::endl;
    std::cout<<"is ASCII: "<<fnt.IsASCII()<<std::endl;
    
//     Resource::File f;
//     Resource::Font fr(fnt);
//     f.Root().Add(fr);
//     f.Save("test.gor");
    
    wind.Minimize();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    wind.Restore();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    

	Graphics::StyledRenderer sty(fnt);
	sty.UseFlatShadow({0.f, 1.0f}, {1.f, 1.f});
	sty.SetColor({0.6f, 1.f, 1.f});
	sty.JustifyLeft();
	//sty.Strike({1.f, 0.7f, 0.3f, 1.f});
	//sty.Underline();
	
	//sty.Print(l, "a", 250, 200);

	sty.Print(l, "\xf0\x90\x8d\x88Lor|em ipsum\xe2\x80\xa8""folor sit amet, consecteturadipiscingelitseddoeiusmoftemporincididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.", 0, 0, 300);
	sty.Print(l, "abc\tfgh\n12-34 dsda\tasdf dsgh", 250, 200);
	sty.DisableShadow();
	sty.Print(l, "abc\tfgh\n12-34 dsda\tasdf dsgh", 250, 220);
	
	wind.KeyEvent.Register([&wind](Input::Key key, bool state) {
		if (!state && (key == 27 || key == 65307))
			exit(0);

		else if(state && key == 13)
			std::cout<<wind.GetMonitor().GetName()<<std::endl;

		else if(state && (key == 'R' || key == 'r'))
			wind.Restore();

		else if(state && (key == 'X' || key == 'x'))
			wind.Maximize();

		else if(state && (key == 'N' || key == 'n'))
			wind.Minimize();

		else if(state && (key == 'S' || key == 's'))
			wind.Resize({400,400});

		return false;
	});

	wind.MovedEvent.Register([&wind]() {
		if(wind.IsMaximized())
			std::cout<<"maxed"<<std::endl;
		else
			std::cout<<"moved to: "<<wind.GetPosition()<<std::endl;
	});

	wind.FocusedEvent.Register([&wind]() {
		std::cout<<"focus in: "<<wind.IsFocused()<<std::endl;
	});

	wind.MinimizedEvent.Register([&wind]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cout<<"min: "<<wind.IsMinimized()<<std::endl;
	});

	wind.RestoredEvent.Register([&wind]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cout<<"res: "<<wind.IsMinimized()<<std::endl;
	});

	wind.LostFocusEvent.Register([&wind]() {
		std::cout<<"focus out: "<<wind.IsFocused()<<std::endl;
	});
    
    //wind.Focus();

	//wind.Move({100, 100});

	wind.SetTitle("This is my window");
	std::cout<<"title\t\t: "<<wind.GetTitle()<<std::endl;
	std::cout<<"bounds\t\t: "<<wind.GetExteriorBounds()<<std::endl;
	std::cout<<"exterior size\t: "<<wind.GetExteriorBounds().GetSize()<<std::endl;
	std::cout<<"interior size\t: "<<wind.GetSize()<<std::endl;
	std::cout<<"position\t\t: "<<wind.GetPosition()<<std::endl;
	std::cout<<"monitor\t\t: "<<wind.GetMonitor().GetName()<<std::endl;

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
