#include "GraphicsHelper.h"

#include <Gorgon/Widgets/Generator.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/UI/Organizers/Flow.h>
#include <Gorgon/Widgets/Window.h>
#include <Gorgon/Widgets/TabPanel.h>
#include <Gorgon/Widgets/Button.h>
#include <Gorgon/Widgets/Label.h>

std::string helptext =
    "Key list:\n"
    "esc\tClose\n"
    ;

using namespace Gorgon;
using namespace Gorgon::UI::literals;

int main() {
    basic_Application<UI::Window> app("uitest", "UI Widget Test", helptext, 1, 0x80);

    auto &org = app.wind.CreateOrganizer<UI::Organizers::Flow>();
    
    Widgets::TabPanel wgt1;
    
    org
        << wgt1;

    wgt1.Focus();

    wgt1.New("Tab 1");
    wgt1.New("Tab 2");
    wgt1.New("Tab 3");
    wgt1.New("Tab 4");
    wgt1.SetTabRollover(true);
    //wgt1.SetButtonTextWrap(true);

    Widgets::Button btn1("Hey tab 1");
    Widgets::Label lbl1("Hey tab 2");
    Widgets::Label lbl2("Tab control is now working, scroll down for the button");
    lbl2.SetAutosize(UI::Autosize::Automatic, UI::Autosize::Automatic);
    Widgets::Button btn2("Hey tab 2");

    wgt1["Tab 1"].Add(btn1);
    wgt1["Tab 2"].Add(lbl1);
    wgt1["Tab 2"].AddUnder(lbl2);
    wgt1["Tab 2"].Add(btn2);

    btn2.Location.Y = 400_px;
    btn2.Location.X = 3_u;

    wgt1.ActivateNext();
        
    
    app.wind.Run();

    return 0;
}
