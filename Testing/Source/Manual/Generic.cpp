//First run this file to check if window is displayed
//Then run font test and return here for instructions.

#include <Gorgon/Window.h>
#include <Gorgon/WindowManager.h>
#include <Gorgon/Main.h>
#include <Gorgon/Utils/Console.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Graphics/Bitmap.h>
#include <Gorgon/Graphics/BitmapFont.h>
#include <Gorgon/Graphics/Line.h>
#include <Gorgon/Graphics/Rectangle.h>
#include <Gorgon/Graphics/MaskedObject.h>
#include <Gorgon/Graphics/ScalableObject.h>
#include <Gorgon/Graphics/TintedObject.h>
#include <Gorgon/Graphics/StackedObject.h>
#include <Gorgon/GL/FrameBuffer.h>
#include <Gorgon/GL.h>
#include "Gorgon/Resource/Line.h"
#include "Gorgon/Resource/Rectangle.h"
#include "Gorgon/Resource/MaskedObject.h"
#include "Gorgon/Resource/StackedObject.h"
#include "Gorgon/Resource/Image.h"
#include "Gorgon/Resource/File.h"



using Gorgon::Window;
using Gorgon::Geometry::Point;
namespace Graphics = Gorgon::Graphics;
namespace Input = Gorgon::Input;
namespace Mouse = Gorgon::Input::Mouse;
namespace WM = Gorgon::WindowManager;


Graphics::Bitmap Circle(int r) {
    Graphics::Bitmap b({r*2+3, r*2+3}, Graphics::ColorMode::Alpha);
    
    auto ri = (r-0.5)*(r-0.5);
    auto ro = (r+0.5)*(r+0.5);
    
    for(int y=0; y<b.GetHeight(); y++) {
        for(int x=0; x<b.GetWidth(); x++) {
            float xx = float(x - (r+1));
            float yy = float(y - (r+1));
            
            float ds = xx*xx + yy*yy;
            
            int v;
            
            if(ds < ri) {
                v = 0xff;
            }
            else if(ds <= ro) {
                v = int(0xff * (1 - (sqrt(ds) - (r-0.5))));
            }
            else {
                v = 0;
            }
            
            b(x,y,0) = v;
        }
    }
    
    return b;
}

Graphics::Bitmap Triangle(int w, int h) {
    Graphics::Bitmap b({2*w+1, h}, Graphics::ColorMode::Alpha);

    float cw = 0.5;
    float xinc = float(w)/h;
    for(int y=0; y<h; y++) {
        for(int x=0; x<b.GetWidth(); x++) {
            if(x>w-floor(cw) && x<w+floor(cw)) {
                b(x,y,0) = 255;
            }
            else if(x>w-ceil(cw) && x<w+ceil(cw)) {
                    b(x,y,0) = Gorgon::Byte(255*(cw-floor(cw)));
            }
            else {
                b(x,y,0) = 0;
            }
        }
        
        cw += xinc;
    }
    
    return b;
}

Graphics::Bitmap Triangle1(int w, int h) {
    Graphics::Bitmap b({w, h}, Graphics::ColorMode::Alpha);

    float cw = 1;
    float xinc = float(w)/h;
    for(int y=0; y<h; y++) {
        for(int x=0; x<b.GetWidth(); x++) {
            if(x<floor(cw)) {
                b(x,y,0) = 255;
            }
            else if(x<ceil(cw)) {
                    b(x,y,0) = Gorgon::Byte(255*(cw-floor(cw)));
            }
            else {
                b(x,y,0) = 0;
            }
        }
        
        cw += xinc;
    }
    
    return b;
}

Graphics::Bitmap Triangle2(int w, int h) {
    Graphics::Bitmap b({w, h}, Graphics::ColorMode::Alpha);

    float cw = 1;
    float xinc = float(w)/h;
    for(int y=0; y<h; y++) {
        for(int x=0; x<b.GetWidth(); x++) {
            if(x>w-floor(cw)) {
                b(x,y,0) = 255;
            }
            else if(x>w-ceil(cw)) {
                    b(x,y,0) = Gorgon::Byte(255*(cw-floor(cw)));
            }
            else {
                b(x,y,0) = 0;
            }
        }
        
        cw += xinc;
    }
    
    return b;
}
Graphics::Bitmap Triangle4(int w, int h) {
    Graphics::Bitmap b({w, h}, Graphics::ColorMode::Alpha);

    float cw = 1;
    float xinc = float(w)/h;
    for(int y=0; y<h; y++) {
        for(int x=0; x<b.GetWidth(); x++) {
            if(x<floor(cw)) {
                b(x,h-y-1,0) = 255;
            }
            else if(x<ceil(cw)) {
                    b(x,h-y-1,0) = Gorgon::Byte(255*(cw-floor(cw)));
            }
            else {
                b(x,h-y-1,0) = 0;
            }
        }
        
        cw += xinc;
    }
    
    return b;
}

Graphics::Bitmap Triangle3(int w, int h) {
    Graphics::Bitmap b({w, h}, Graphics::ColorMode::Alpha);

    float cw = 1;
    float xinc = float(w)/h;
    for(int y=0; y<h; y++) {
        for(int x=0; x<b.GetWidth(); x++) {
            if(x>w-floor(cw)) {
                b(x,h-y-1,0) = 255;
            }
            else if(x>w-ceil(cw)) {
                    b(x,h-y-1,0) = Gorgon::Byte(255*(cw-floor(cw)));
            }
            else {
                b(x,h-y-1,0) = 0;
            }
        }
        
        cw += xinc;
    }
    
    return b;
}

Graphics::Bitmap Rectangle(int w, int h) {
    Graphics::Bitmap b({w, h}, Graphics::ColorMode::Alpha);

    for(int y=0; y<h; y++) {
        for(int x=0; x<w; x++) {
            b(x,y,0) = 255;
        }
    }
    
    return b;
}

Graphics::Bitmap BGImage(int w, int h, Gorgon::Byte col1 = 0x10, Gorgon::Byte col2 = 0x30) {
	Graphics::Bitmap bgimage({w*2, h*2}, Graphics::ColorMode::Grayscale);

	for(int x = 0; x<w*2; x++) {
		for(int y = 0; y<h*2; y++) {
			if((x/w) != (y/h))
				bgimage({x, y}, 0) = 0x10;
			else
				bgimage({x, y}, 0) = 0x30;
		}
    }
    
    return bgimage;
}

int main() {

	std::cout<<"Current working directory: ";
#ifdef WIN32
	system("cd");
#else
	system("pwd");
#endif
	std::cout<<std::endl;

	Gorgon::Initialize("Generic-test");
    
    Gorgon::GL::log.InitializeConsole();
    
	Window wind({800, 600}, "windowtest", true);
	Graphics::Initialize();
    
    wind.ClosingEvent.Register([]{ exit(0); });

	Graphics::Layer l;
	wind.Add(l);

	Graphics::Bitmap img;
	if(!img.Import("test.png")) {
        std::cout<<"Test.png is not found"<<std::endl;
        exit(0);
    }

    img.Prepare();

	Graphics::Bitmap icon;
	if(icon.Import("icon.png")) {
        WM::Icon ico(icon.GetData());
        wind.SetIcon(ico);
    }

    auto bgimage = BGImage(25,25);
	bgimage.Prepare();
	//bgimage.DrawIn(l);

    Graphics::BitmapFont fnt;
	fnt.ImportFolder("Victoria", Graphics::BitmapFont::Automatic, 0, "", -1, true, false, false);
    fnt.Pack();
    

	Graphics::StyledRenderer sty(fnt);
	sty.UseFlatShadow({0.f, 1.0f}, {1.f, 1.f});
	sty.SetColor({0.6f, 1.f, 1.f});
	sty.JustifyLeft();
    
    sty.SetTabWidthInLetters(1.5f);
    sty.SetParagraphSpacing(2);
    sty.Print(l, 
        "Key list:\n"
        "esc\tClose\n"
        , 500, 10
    );
	
	wind.KeyEvent.Register([&wind](Input::Key key, bool state) {
		if (!state && (key == 27 || key == 65307))
			exit(0);

		else if(state && key == 13)
			std::cout<<"Pressed enter"<<std::endl;

		return false;
	});



	using Graphics::Bitmap;

	Graphics::BitmapRectangleProvider rectp(
		Triangle2(8, 8), Rectangle(8, 8), Triangle1(8, 8),
		Rectangle(8, 8), Rectangle(8, 8), Rectangle(8, 8),
		Triangle3(8, 8), Rectangle(8, 8), Triangle4(8, 8)
	);

    using namespace Gorgon::Graphics;

    rectp.OwnProviders();
    rectp.Prepare();

    auto c = Circle(4);
    c.Prepare();
    auto c2 = Circle(2);
    c.Prepare();
	Graphics::MaskedObjectProvider maskp(
		c, c2
	);

	Graphics::LineProvider linep(
		Graphics::Orientation::Vertical,
		*new Bitmap(Triangle(4, 4)), rectp, *new Bitmap(Triangle(4, 4).Rotate180())
	);
    
    TintedBitmapProvider tbp{Triangle(10, 20), 0xff6040ff};
    tbp.Prepare();
    ScalableObjectProvider sp{tbp, {SizeController::Integral_Smaller, SizeController::Integral_Smaller, Placement::MiddleLeft}};
    auto cc = new Bitmap(Circle(20));
    cc->Prepare();
    StackedObjectProvider stp{*cc, sp, {10,10}};

	Gorgon::Resource::File f;
	auto imgr = new Gorgon::Resource::Image(::Rectangle(8, 8));
	f.Root().Add(imgr);
	auto liner = new Gorgon::Resource::Line(linep);
	f.Root().Add(liner);
	auto rectr = new Gorgon::Resource::Rectangle(rectp);
	f.Root().Add(rectr);
	auto maskr = new Gorgon::Resource::MaskedObject(maskp);
	f.Root().Add(maskr);
    auto stbr = new Gorgon::Resource::StackedObject(stp);
    f.Root().Add(stbr);
	f.Save("linetest.gor");

	f.LoadFile("linetest.gor");
	f.Prepare();

	auto rp = f.Root().Get<Gorgon::Resource::AnimationStorage>(4).MoveOut();

	Graphics::MaskedObjectProvider mop(bgimage, rp);

	auto &r = rp.CreateAnimation();

	r.DrawIn(l, 100, 100, 300, 200);

	Graphics::BlankImage b(50, 50, 0.5f);

	b.Draw(l, 700, 10);


	while(true) {
		Gorgon::NextFrame();
	}

	return 0;
}
