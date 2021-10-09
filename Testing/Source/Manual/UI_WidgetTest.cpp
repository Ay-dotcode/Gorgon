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

int main() {
    basic_Application<UI::Window> app("uitest", "UI Widget Test", helptext, 1, 0x80);

    auto &org = app.wind.CreateOrganizer<UI::Organizers::Flow>();
    
    Widgets::TabPanel wgt1;
    
    org
        << wgt1;

    wgt1.Focus();

    wgt1.New("Tab 1", "Tab 1 long text");
    wgt1.New("Tab 2");
    wgt1.SetTabRollover(true);
    //wgt1.SetButtonTextWrap(true);

    Widgets::Button btn1("Hey tab 1");
    Widgets::Label lbl1("Hey tab 2");
    Widgets::Label lbl2("Tab control is now working, scroll down for the button");
    lbl2.SetAutosize(true, true);
    Widgets::Button btn2("Hey tab 2");

    wgt1["Tab 1"].Add(btn1);
    wgt1["Tab 2"].Add(lbl1);
    wgt1["Tab 2"].Add(lbl2);
    wgt1["Tab 2"].Add(btn2);
    lbl2.Location.Y = lbl1.GetBounds().Bottom + Widgets::Registry::Active().GetSpacing();
    btn2.Location.Y = 400;
    btn2.Location.X = Widgets::Registry::Active().GetUnitSize(3) + Widgets::Registry::Active().GetSpacing();

    wgt1.ActivateNext();
        
    
    app.wind.Run();

    return 0;
}
