#include "GraphicsHelper.h"

#include <Gorgon/UI/ComponentStack.h>
#include <Gorgon/Widgets/Generator.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/Widgets/ColorPicker.h>
#include <Gorgon/UI/Organizers/Flow.h>
#include <Gorgon/Widgets/Window.h>
#include <Gorgon/Widgets/DialogWindow.h>

std::string helptext =
    "Key list:\n"
    "esc\tClose\n"
    ;

using namespace Gorgon;

int main() {
    basic_Application<UI::Window> app("uitest", "UI Widget Test", helptext, 1, 0x80);

    auto &org = app.wind.CreateOrganizer<UI::Organizers::Flow>();
    
    Widgets::ColorPicker wgt1;
    wgt1.ChangedEvent.Register([](Graphics::RGBAf color) {
        std::cout << color << std::endl;
    });
    wgt1.HueDensity = wgt1.High;
    //wgt1.LCDensity  = wgt1.Medium;
    //wgt1.Display    = wgt1.Hex;
    wgt1.Alpha = true;
    wgt1.PlaneSize.Height *= 1.2;
    wgt1.Location.Y = 500;
    
    org 
        << wgt1 ;
    
    
    app.wind.Run();

    return 0;
}
