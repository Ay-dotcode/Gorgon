

#include <Gorgon/Window.h>
#include <Gorgon/Main.h>
#include "Gorgon/Graphics/Texture.h"
#include "Gorgon/Graphics/Bitmap.h"
#include "Gorgon/Graphics/Layer.h"
#include "Gorgon/Resource/Image.h"
#include "Gorgon/Encoding/JPEG.h"

int main() {
	
	Gorgon::Initialize("Window-test");
	Gorgon::Window wind({200,200}, "windowtest");
	Gorgon::Graphics::Initialize();

	Gorgon::Graphics::Layer l;
	wind.Add(l);

	Gorgon::Graphics::Bitmap img;
	img.Import("test.png");

	img.Prepare();

	img.Draw(l, {0,0});
	
	wind.KeyEvent.Register([](Gorgon::Input::Key key, bool state) {
		if (!state && (key == 27 || key == 65307))
			exit(0);
		return false;
	});
	
	wind.CharacterEvent.Register([](Gorgon::Input::Key key) {
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
