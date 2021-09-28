#include "GraphicsHelper.h"

#include <Gorgon/Widgets/Generator.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/UI/Organizers/Flow.h>
#include <Gorgon/Widgets/Window.h>
#include <Gorgon/Widgets/TabPanel.h>

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
        
    
    app.wind.Run();

    return 0;
}
