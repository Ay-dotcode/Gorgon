

#include <Gorgon/Window.h>
#include <Gorgon/Main.h>
#include "Gorgon/Graphics/Texture.h"
#include "Gorgon/Graphics/Bitmap.h"
#include "Gorgon/Graphics/Layer.h"
#include "Gorgon/Resource/Image.h"

int main() {
	
	Gorgon::Initialize("Window-test");
	Gorgon::Window wind({200,200}, "windowtest");
	Gorgon::Graphics::Initialize();

	Gorgon::Graphics::Layer l;
	wind.Add(l);

	Gorgon::Graphics::Bitmap img;
	img.ImportPNG("test.png");

	img.Prepare();

	img.Draw(l, {0,0});
	
	wind.KeyEvent.Register([](Gorgon::Input::Key key, float amount) {
		if (key == 27 || key == 65307)
			exit(0);
		return false;
	});
	
	while (1) {
		Gorgon::NextFrame();
	}

	return 0;
	
}
