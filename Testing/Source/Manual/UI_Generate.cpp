#include "GraphicsHelper.h"

#include <Gorgon/UI/ComponentStack.h>
#include <Gorgon/Widgets/Generator.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/Widgets/Button.h>
#include <Gorgon/Widgets/Checkbox.h>

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

	btn2.Move({0, btn.GetSize().Height + 5});
	app.wind.Add(btn2);
    
    auto chktemp = gen.Checkbox();
    
    Widgets::Checkbox chk(chktemp, "Milk?", true, [&]() { std::cout<<(chk ? "with milk." : "without milk")<<std::endl; });
    app.wind.Add(chk);
    chk.Move(200, 0);

    app.wind.Run();

	return 0;
}

