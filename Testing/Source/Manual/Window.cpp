

#include <Gorgon/Window.h>
#include <Gorgon/WindowManager.h>
#include <Gorgon/Main.h>
#include "Gorgon/Graphics/Texture.h"
#include "Gorgon/Graphics/Bitmap.h"
#include "Gorgon/Graphics/Layer.h"
#include "Gorgon/Resource/Image.h"
#include "Gorgon/Encoding/JPEG.h"

using Gorgon::Window;
namespace Graphics = Gorgon::Graphics;
namespace Input = Gorgon::Input;
namespace WM = Gorgon::WindowManager;

int main() {
	
	Gorgon::Initialize("Window-test");
    
    auto &monitors = WM::Monitor::Monitors();
    for(auto &monitor : monitors) {
        std::cout<<monitor.GetName()<<": "<<monitor.GetArea()<<"  --  "<<monitor.GetUsable()<<std::endl;
    }
    
    std::cout<<WM::Monitor::Primary().GetName()<<": "<<WM::Monitor::Primary().GetLocation()<<std::endl;
    //std::cout<<WM::GetScreenRegion(0)<<std::endl;
    
	Window wind({200,200}, "windowtest");
	Graphics::Initialize();

	Graphics::Layer l;
	wind.Add(l);

	Graphics::Bitmap img;
	img.Import("test.png");

	img.Prepare();

	img.Draw(l, {0,0});
	
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
	
	while (1) {
		Gorgon::NextFrame();
	}

	return 0;
	
}
