#include "GraphicsHelper.h"

#include <Gorgon/UI/Components.h>


void Init();

std::string helptext = 
    "Key list:\n"
	"esc\tClose\n"
	"space\tList formats\n"
;


int main() {
	Application app("clipboardtest", "Clipboard test", helptext);

	Graphics::Layer l;
    app.wind.Add(l);

	app.wind.KeyEvent.Register([&](Input::Key key, bool state) {
		if(key == 0x20) {
			l.Clear();
			auto list = WindowManager::GetClipboardFormats();
			std::string s = "Formats\n";

			for(auto g : list) {
				s += g.Name() + "\n";
			}

			app.sty.Print(l, s, 0, 0);

			return true;
		}

		return false;
	});

	while(true) {
		Gorgon::NextFrame();
	}

	return 0;
}


void Init() {

}
