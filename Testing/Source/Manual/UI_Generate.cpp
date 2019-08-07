#include "GraphicsHelper.h"

#include <Gorgon/UI/ComponentStack.h>
#include <Gorgon/Widgets/Generator.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/Widgets/Button.h>
#include <Gorgon/Widgets/Checkbox.h>
#include <Gorgon/Widgets/RadioButtons.h>
#include <Gorgon/Widgets/Label.h>
#include <Gorgon/UI/RadioControl.h>

std::string helptext = 
    "Key list:\n"
    "d\tToggle disabled\n"
    "1-2\tChange Values\n"
	"esc\tClose\n"
;

using namespace Gorgon;

Graphics::Bitmap &prep(Graphics::Bitmap &bmp) {
    bmp.Prepare();
    
    return bmp;
}

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
    
    auto radtemp = gen.RadioButton();
    std::cout<<"Height: "<<gen.RegularFont.GetGlyphRenderer().GetSize('A').Height<<std::endl;
    
    Widgets::RadioButtons<int> rad(radtemp);
	
	rad.Add(0, "Americano");
	rad.Add(1, "Latte");
	rad.Add(2);

	app.wind.Add(rad);
	rad.Move(150, 4);
    
    rad.ChangedEvent.Register([](int val) {
        std::cout<<"Changed to "<<val<<std::endl;
    });

	auto chktemp = gen.Checkbox();

	Widgets::Checkbox chk(chktemp, "Sugar", [](bool state) {
		std::cout<<(state ? "with sugar" : "without sugar")<<std::endl;
	});

	app.wind.Add(chk);

	chk.Move(rad.GetLocation() + Gorgon::Geometry::Point(0, rad.GetSize().Height + 4));
    
    auto lbltemp = gen.Label();
    
    Widgets::Label lbl(lbltemp, "This is a label");
    
    app.wind.Add(lbl);
    
    lbl.Move(chk.GetLocation() + Gorgon::Geometry::Point(0, chk.GetSize().Height + 4));
    lbl.OwnIcon(prep(*new Graphics::Bitmap(Triangle(8, 8))));
    

    app.wind.Run();

	return 0;
}

