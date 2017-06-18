#include "GraphicsHelper.h"

#include <Gorgon/UI/Components.h>


void Init();

std::string helptext = 
    "Key list:\n"
	"esc\tClose\n"
	"space\tList formats\n"
	"1-9\tShow data\n"
	"t\tCopy text, from console\n"
	"h\tCopy html, from console\n"
;


int main() {
	Application app("clipboardtest", "Clipboard test", helptext);

	Graphics::Layer l;
	app.wind.Add(l);

	Graphics::Layer l2;
	app.wind.Add(l2);

	std::vector<Resource::GID::Type> list;

	app.wind.CharacterEvent.Register([&](Input::Keyboard::Char key) {
		if(key == 0x20) {
			l.Clear();
			l2.Clear();
			list = WindowManager::GetClipboardFormats();
			std::string s = "Formats\n";

			int ind = 1;
			for(auto g : list) {
				s += String::From(ind) + ". " + g.Name() + "\n";
				ind++;
			}

			app.sty.Print(l, s, 10, 100);

			return true;
		}
		else if(key > '0' && key < '1'+list.size()) {
			auto fmt = list[key-'1'];
			if(fmt == Resource::GID::Text || fmt == Resource::GID::HTML) {
				l2.Clear();
				app.sty.Print(l2, WindowManager::GetClipboardText(fmt), 100, 100);
			}
		}
		else if(key == 't') {
			std::string s;
			std::getline(std::cin, s);
			WindowManager::SetClipboardText(s);
		}
		else if(key == 'h') {
			std::string s;
			std::getline(std::cin, s);
			WindowManager::SetClipboardText(s, Resource::GID::HTML);
			WindowManager::SetClipboardText(s, Resource::GID::Text, true, true);
		}
		else if(key == 'H') {
			WindowManager::SetClipboardText(WindowManager::GetClipboardText(Resource::GID::HTML));
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
