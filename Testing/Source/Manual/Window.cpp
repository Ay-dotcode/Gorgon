

#include <Gorgon/Window.h>
#include <Gorgon/Main.h>

int main() {
	
	Gorgon::Initialize("Window-test");
	Gorgon::Window wind(Gorgon::Window::Fullscreen, "windowtest");
	
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