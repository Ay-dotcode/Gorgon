//First run this file to check if window is displayed
//Then run font test and return here for instructions.

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
#include <Gorgon/Resource/Pointer.h>
#include <Gorgon/Graphics/Pointer.h>


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
	Gorgon::GL::log.InitializeConsole();
    
    auto &monitors = WM::Monitor::Monitors();
    for(auto &monitor : monitors) {
        std::cout<<monitor.GetName()<<": "<<monitor.GetArea()<<"  --  "<<monitor.GetUsable()<<std::endl;
    }
    
    std::cout<<WM::Monitor::Primary().GetName()<<": "<<WM::Monitor::Primary().GetLocation()<<std::endl;
    //std::cout<<WM::GetScreenRegion(0)<<std::endl;
    
	Window wind({800, 600}, "windowtest", true);
	Graphics::Initialize();
    
    std::cout<<WM::GetClipboardText()<<std::endl;
    WM::SetClipboardText("copied!");

	Graphics::Layer l;
	wind.Add(l);

	/*Graphics::Bitmap img;
	if(!img.Import("test.png")) {
        std::cout<<"Test.png is not found"<<std::endl;
        exit(0);
    }

    img.Prepare();*/

	//Graphics::Bitmap icon;
	//icon.Import("icon.png");

	//WM::Icon ico(icon.GetData());
	//wind.SetIcon(ico);
    
    Graphics::Bitmap cursor1({16,16}, Graphics::ColorMode::RGBA);
    cursor1.ForAllPixels([&](int x, int y) {
        if(y%2 && (x == y/2 || x == cursor1.GetWidth()-y/2-1)) {
            cursor1(x, y, 0) = 255;
            cursor1(x, y, 1) = 255;
            cursor1(x, y, 2) = 255;
            cursor1(x, y, 3) = 128;
        }
        else if(x>=y/2 && x<=cursor1.GetWidth()-y/2-1) {
            cursor1(x, y, 0) = 255;
            cursor1(x, y, 1) = 255;
            cursor1(x, y, 2) = 255;
            cursor1(x, y, 3) = 255;
        }
        else {
            cursor1(x, y, 3) = 0;
        }
    });
    
    Graphics::Bitmap cursor2({16,16}, Graphics::ColorMode::RGBA);
    cursor2.ForAllPixels([&](int x, int y) {
        int yy = y;
        if(y>=8) {
            yy = 16-y-1;
        }
        if(x>=yy/2 && x<=cursor2.GetWidth()-yy/2-1) {
            cursor2(x, y, 0) = 255;
            cursor2(x, y, 1) = 255;
            cursor2(x, y, 2) = 255;
            cursor2(x, y, 3) = 255;
        }
        else {
            cursor2(x, y, 3) = 0;
        }
    });
    
    Graphics::Bitmap cursor3({1,16}, Graphics::ColorMode::Alpha);
    cursor3.ForAllPixels([&](Gorgon::Byte &b) {
        b = 255;
    }, 0);
    
    cursor1.Prepare();
    Graphics::DrawablePointer pointer1(cursor1, 8,15);
    
    cursor2.Prepare();
    Graphics::DrawablePointer pointer2(cursor2, 8,8, Gorgon::Graphics::PointerType::Wait);
    
    cursor3.Prepare();
    Graphics::DrawablePointer pointer3(cursor3, 1,8, Gorgon::Graphics::PointerType::Text);
    
    Resource::File fsave;
    auto ptrsave = new Resource::Pointer;
    
    ptrsave->Add(cursor1);
    ptrsave->SetType(Graphics::PointerType::Arrow);
    fsave.Root().Add(ptrsave);
    fsave.Save("ptrtest.gor");
    
    Resource::File f;
    f.LoadFile("ptrtest.gor");
    f.Prepare();
    
	auto ptr = f.Root().Get<Resource::Pointer>(0).CreatePointer();
    wind.Pointers.Add(Graphics::PointerType::Arrow, ptr);
    //wind.Pointers.Add(Graphics::PointerType::Arrow, pointer1);
    wind.Pointers.Add(Graphics::PointerType::Wait, pointer2);
    wind.Pointers.Add(Graphics::PointerType::Text, pointer3);
    

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
	wind.SetBackground(img3);
    
    cursor3.Draw (l, 500, 400);
    pointer1.Draw(l, 500, 400);
    
    Graphics::Layer ptrlayer;
    wind.Add(ptrlayer);

	
	for(int i=0; i<4; i++)
		img2.DrawStretched(l, 25+16+i*32, 0, 16, 400, {1.f, 1.f, 1.f, .3f});
		
	//for(int i=0; i<10; i++)
		//img.Draw(l, 150, 150);
	
	//img.Draw(l, 50, 50, {.2f, .2f, .8f, 1.f});
    
    Graphics::BitmapFont fnt;
	fnt.ImportFolder("Victoria", Graphics::BitmapFont::Automatic, 0, "");
    
    wind.Minimize();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    wind.Restore();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    Graphics::PointerStack::Token ptrtoken;
    

	Graphics::StyledPrinter sty(fnt);
	sty.UseFlatShadow({0.f, 1.0f}, {1.f, 1.f});
	sty.SetColor({0.6f, 1.f, 1.f});
	sty.JustifyLeft();
    
    sty.SetTabWidthInLetters(1.5f);
    sty.SetParagraphSpacing(2);
    sty.Print(l, 
        "Key list:\n"
        "x\tMaximize window\n"
        "r\tRestore window\n"
        "n\tMinimize window\n"
        "s\tResize window to 400x400\n"
        "c\tCenter window\n"
        "e\tToggle window resize\n"
        "h\tKeep pressed to hide pointer\n"
        "w\tToggle local pointers\n"
        "b\tBusy pointer (local only)\n"
        "t\tText pointer (local only)\n"
        "p\tPop pointer (returns to default)\n"
        , 500, 10
    );
	
	wind.KeyEvent.Register([&wind, &ptrtoken](Input::Key key, bool state) {
        static bool localptr = false;
        static bool resizable = false;

		namespace Keycodes = Gorgon::Input::Keyboard::Keycodes;
        
        std::cout<<Keycodes::GetName(key)<<": "<<state<<std::endl;
        
		if (!state && (key == Keycodes::Escape))
			exit(0);

		if(!state)
			return false;

		else if(Keycodes::IsEnter(key))
			std::cout<<wind.GetMonitor().GetName()<<std::endl;

		else if(key == Keycodes::R)
			wind.Restore();

		else if(key == Keycodes::X)
			wind.Maximize();

		else if(key == Keycodes::N)
			wind.Minimize();

		else if(key == Keycodes::S)
			wind.Resize({400,400});

		else if(key == Keycodes::C)
			wind.Center();

		else if(key == Keycodes::E) {
			if(resizable) {
				wind.PreventResize();
				resizable = false;
			}
			else {
				wind.AllowResize();
				resizable = true;
			}
		}

		else if(key == Keycodes::H)
			wind.HidePointer();

		else if(key == Keycodes::H)
			wind.ShowPointer();

		else if(key == Keycodes::W) {
			if(localptr) {
				wind.SwitchToWMPointers();
				localptr = false;
			}
			else {
				wind.SwitchToLocalPointers();
				localptr = true;
			}
		}

		else if(key == Keycodes::B)
			ptrtoken = wind.Pointers.Set(Graphics::PointerType::Wait);

		else if(key == Keycodes::T)
			ptrtoken = wind.Pointers.Set(Graphics::PointerType::Text);

		else if(key == Keycodes::P)
			wind.Pointers.Reset(ptrtoken);

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
    
    wind.ResizedEvent.Register([&]() {
		std::cout<<"resized: "<<wind.GetSize()<<std::endl;
		//img3.DrawIn(l);
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

	std::ofstream outfile("test.txt");

	wind.CharacterEvent.Register([&outfile](Gorgon::Char key) {
        if(key == '\r') 
            std::cout<<std::endl;
        
        std::string s="\"";
        Gorgon::String::AppendUnicode(s, key);
        
		std::cout<<s<<"\""<<std::endl;
        
		return true;
	});

	Gorgon::Input::Layer il;
    il.Move(0, 25);
	il.setname("Input");
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

    Gorgon::Geometry::Point last = {0,0};
	il.SetMove([&last](Point location) {
        if(last == location)
            return;
        last = location;
		std::cout<<"Move: " << location <<std::endl;
	});

	il.SetOut([]() {
		std::cout<<"Out"<<std::endl;
	});

	il.SetScroll([](Point location, float amount) {
		std::cout<<"Scrolled "<<amount<<" times at "<<location<<std::endl;

        return true;
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
