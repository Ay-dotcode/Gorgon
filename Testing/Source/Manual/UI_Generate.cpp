#include "GraphicsHelper.h"

#include <Gorgon/UI/ComponentStack.h>
#include <Gorgon/Widgets/Generator.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/Widgets/Button.h>
#include <Gorgon/Widgets/Checkbox.h>
#include <Gorgon/UI/RadioControl.h>

std::string helptext = 
    "Key list:\n"
    "d\tToggle disabled\n"
    "1-2\tChange Values\n"
	"esc\tClose\n"
;

using namespace Gorgon;

int main() {
	basic_Application<UI::Window> app("uitest", "UI Generator Test", helptext, 1, 0x80);

	Graphics::Layer l;
    app.wind.Add(l);

	Widgets::SimpleGenerator gen;

	auto btntemp = gen.Button();

	Widgets::Button btn(btntemp, "Helloo...", []() { std::cout<<"Hello..."<<std::endl; });

	app.wind.Add(btn);

	btn.Focus();

	Widgets::Button btn2(btntemp, "Exit", [&app]() { app.wind.Quit(); });

	btn2.Move({0, btn.GetSize().Height + 4});
	app.wind.Add(btn2);
    
    auto chktemp = gen.Checkbox();
    
    UI::RadioControl<> rad({
        {0, new Widgets::Checkbox(chktemp, "Milk")},
        {1, new Widgets::Checkbox(chktemp, "Sugar")},
    }, 0);
    
    rad.PlaceIn(app.wind, {200, 4}, 4);
    
    rad.ChangedEvent.Register([](int val) {
        std::cout<<"Changed to "<<val<<std::endl;
    });
    

    app.wind.Run();

	return 0;
}

