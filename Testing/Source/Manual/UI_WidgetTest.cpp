#include "GraphicsHelper.h"

#include <Gorgon/Widgets/Generator.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/UI/Organizers/Flow.h>
#include <Gorgon/Widgets/Window.h>
#include <Gorgon/Widgets/TabPanel.h>
#include <Gorgon/Widgets/Button.h>

std::string helptext =
    "Key list:\n"
    "esc\tClose\n"
    ;

using namespace Gorgon;

int main() {
    basic_Application<UI::Window> app("uitest", "UI Widget Test", helptext, 1, 0x80);

    auto &org = app.wind.CreateOrganizer<UI::Organizers::Flow>();
    
    Widgets::TabPanel wgt1;
    
    org 
        << wgt1 ;

    wgt1.New("Tab 1");
    wgt1.New("Tab 2");

    Widgets::Button btn1("Hey tab 1");
    Widgets::Button btn2("Hey tab 2");

    wgt1["Tab 1"].Add(btn1);
    wgt1["Tab 2"].Add(btn2);

        
    
    app.wind.Run();

    return 0;
}
