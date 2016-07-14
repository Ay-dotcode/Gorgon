

#include <Gorgon/Window.h>
#include <Gorgon/Main.h>
#include "Gorgon/Graphics/Texture.h"
#include "Gorgon/Graphics/Bitmap.h"
#include "Gorgon/Graphics/Layer.h"
#include "Gorgon/Resource/Image.h"

int main() {
	
	Gorgon::Initialize("Window-test");
	Gorgon::Window wind(Gorgon::Window::Fullscreen, "windowtest");
	Gorgon::Graphics::Initialize();

	Gorgon::Graphics::Layer l;
	wind.Add(l);

	Gorgon::Resource::Image *img=new Gorgon::Resource::Image;
	img->ImportPNG("test.png");

	img->Prepare();

	img->Draw(l, {0,0});
	
	wind.KeyEvent.Register([](Gorgon::Input::Key key, float amount) {
		if (key == 27)
			exit(0);
		return false;
	});
	
	while (1) {
		Gorgon::NextFrame();
	}

	return 0;
	
}