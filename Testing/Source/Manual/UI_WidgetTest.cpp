#include "GraphicsHelper.h"

#include <Gorgon/UI/ComponentStack.h>
#include <Gorgon/Widgets/Generator.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/Widgets/ColorPlane.h>
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
    
    Widgets::ColorPlane wgt1;
    wgt1.ChangedEvent.Register([](Graphics::RGBAf color) {
        std::cout << color << std::endl;
    });
    
    Widgets::ColorPlane wgt2;
    wgt2.SetWidthInUnits(8);
    wgt2.ChangedEvent.Register([](Graphics::RGBAf color) {
        std::cout << color << std::endl;
    });
    wgt2.HueDensity = wgt2.High;
    
    Widgets::ColorPlane wgt3;
    wgt3.ChangedEvent.Register([](Graphics::RGBAf color) {
        std::cout << color << std::endl;
    });
    wgt3.HueDensity = wgt3.Low;
    wgt3.LCDensity  = wgt3.Low;

    Widgets::ColorPlane wgt4;
    wgt4.ChangedEvent.Register([](Graphics::RGBAf color) {
        std::cout << color << std::endl;
    });
    wgt4.HueDensity = wgt4.High;
    wgt4.LCDensity  = wgt4.High;

    Widgets::ColorPlane wgt5;
    wgt5.SetWidthInUnits(9);
    wgt5.ChangedEvent.Register([](Graphics::RGBAf color) {
        std::cout << color << std::endl;
    });
    wgt5.HueDensity = wgt5.VeryHigh;
    wgt5.LCDensity  = wgt5.High;

    Widgets::ColorPlane wgt6;
    wgt6.SetWidthInUnits(10);
    wgt6.SetHeight(Widgets::Registry::Active().GetUnitWidth(6));
    wgt6.ChangedEvent.Register([](Graphics::RGBAf color) {
        std::cout << color << std::endl;
    });
    wgt6.HueDensity = wgt6.VeryHigh;
    wgt6.LCDensity  = wgt6.VeryHigh;

    org 
        << wgt1 << wgt2 << wgt3 << org.Break
        << wgt5 << wgt6;

    app.wind.Run();

    return 0;
}
