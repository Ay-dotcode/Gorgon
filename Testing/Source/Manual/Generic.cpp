//First run this file to check if window is displayed
//Then run font test and return here for instructions.

#include <Gorgon/Window.h>
#include <Gorgon/WindowManager.h>
#include <Gorgon/Main.h>
#include <Gorgon/Utils/Console.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Graphics/Bitmap.h>
#include <Gorgon/Graphics/BitmapFont.h>
#include <Gorgon/GL/FrameBuffer.h>
#include <Gorgon/GL.h>



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

int main() {
	
	Gorgon::Initialize("Generic-test");
    system("pwd");
    
    Gorgon::GL::log.InitializeConsole();
    
	Window wind({800, 600}, "windowtest");
	Graphics::Initialize();
    
    wind.ClosingEvent.Register([]{ exit(0); });

	Graphics::Layer l;
	wind.Add(l);
	l.Resize(150,150);
	//l.EnableClipping();

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

	Graphics::Bitmap bgimage({50, 50}, Graphics::ColorMode::Grayscale);

	for(int x = 0; x<bgimage.GetWidth(); x++)
		for(int y = 0; y<bgimage.GetHeight(); y++) {
			if((x/(bgimage.GetWidth()/2)) != (y/(bgimage.GetHeight()/2)))
				bgimage({x, y}, 0) = 0x10;
			else
				bgimage({x, y}, 0) = 0x30;
		}

	bgimage.Prepare();
	bgimage.DrawIn(l);
    
    auto circle = Circle(30);
    circle.Prepare();
    circle.Draw(l, 25,25, 0x80ffffff);
    
    auto trig = Triangle(25, 100);
    trig.Prepare();
    trig.Draw(l, 100,25, 0x80ffffff);
    
    auto rect = Rectangle(25, 100);
    rect.Prepare();
    rect.Draw(l, 175,25, 0x80ffffff);
    
    auto trig1 = Triangle1(25, 100);
    trig1.Prepare();
    trig1.Draw(l, 225,25, 0x80ffffff);
    
    auto trig2 = Triangle2(25, 100);
    trig2.Prepare();
    trig2.Draw(l, 275,25, 0x80ffffff);
    
    auto trig3 = Triangle3(25, 100);
    trig3.Prepare();
    trig3.Draw(l, 325,25, 0x80ffffff);
    
    
    auto trig4 = Triangle4(25, 100);
    trig4.Prepare();
    trig4.Draw(l, 375,25, 0x80ffffff);
    
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
    
    Gorgon::GL::FrameBuffer buffer(true);
    buffer.Use();
    
    Gorgon::NextFrame();
    buffer.RenderToScreen();
    
    Gorgon::Graphics::TextureImage tex(buffer.GetTexture(), Gorgon::Graphics::ColorMode::RGBA, Gorgon::Window::GetMinimumRequiredSize());
    //l.Clear();
	l.SetDrawMode(l.FrameBuffer);
    tex.Draw(l, 0, 70);

	while(true) {
		Gorgon::NextFrame();
	}

	return 0;
}
