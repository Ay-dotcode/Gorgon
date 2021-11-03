#include "GraphicsHelper.h"

#include <Gorgon/Widgets/Generator.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/UI/Organizers/Flow.h>
#include <Gorgon/Widgets/Window.h>
#include <Gorgon/Widgets/TabPanel.h>
#include <Gorgon/Widgets/Button.h>
#include <Gorgon/UI/Dialog.h>
#include <Gorgon/Widgets/Label.h>

std::string helptext =
    "Key list:\n"
    "esc\tClose\n"
    ;

using namespace Gorgon;
using namespace Gorgon::UI::literals;

int main() {
    basic_Application<UI::Window> app("uitest", "UI Widget Test", helptext, 1, 0x80);

    //auto &org = app.wind.CreateOrganizer<UI::Organizers::Flow>();
    Widgets::Panel w;

    app.wind.Add(w);
    w.Location.X = 50_perc;
    
    Widgets::MarkdownLabel lbl2("This class is the base class for all **widget** containers.\n\nAll widgets require a layer to be placed on, to allow widget containers that are also widgets, this class is left abstract. You may derive from this class and WidgetBase at the same time. Visit <https://darkgaze.org> for more.\n\n[Working directory](.)");
    lbl2.SetAutosize(UI::Autosize::None, UI::Autosize::Automatic);
    w.Add(lbl2);
    w.SetInteriorHeight(6_u);
    std::cout << lbl2.GetCurrentHeight() << std::endl;

    app.wind.Run();

    return 0;
}
