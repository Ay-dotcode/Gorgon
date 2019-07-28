#include "GraphicsHelper.h"

#include <Gorgon/UI/ComponentStack.h>
#include <Gorgon/Widgets/Generator.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/Widgets/Button.h>

std::string helptext = 
    "Key list:\n"
    "d\tToggle disabled\n"
    "1-2\tChange Values\n"
	"esc\tClose\n"
;

using namespace Gorgon;

int main() {
	basic_Application<UI::Window> app("uitest", "UI Generator Test", helptext, 1, 0x40);
	Graphics::Layer l;
	app.wind.Add(l);
	Widgets::SimpleGenerator gen;
	gen.Forecolor.Regular.Blend(Gorgon::Graphics::Color::AquaBlue);
	gen.Forecolor.Down.Blend(Gorgon::Graphics::Color::AquaGreen);
	gen.Forecolor.Hover.Blend(Gorgon::Graphics::Color::Red);
	auto temp = gen.Button();
	Widgets::Button btn(temp, "Helloo", []() { std::cout<<"Hello..."<<std::endl; });

	app.wind.Add(btn);
	btn.Move({100,100});
	
    app.wind.Run();

	return 0;
}

