#include "GraphicsHelper.h"

#include <Gorgon/UI/ComponentStack.h>
#include <Gorgon/Widgets/Generator.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/Widgets/Button.h>
#include <Gorgon/Widgets/Checkbox.h>
#include <Gorgon/Widgets/RadioButtons.h>
#include <Gorgon/Widgets/Label.h>
#include <Gorgon/Widgets/Panel.h>
#include <Gorgon/Widgets/Textbox.h>
#include <Gorgon/Widgets/Numberbox.h>
#include <Gorgon/Widgets/GeometryBoxes.h>
#include <Gorgon/Widgets/Progressbar.h>
#include <Gorgon/Widgets/Scrollbar.h>
#include <Gorgon/Widgets/Composer.h>
#include <Gorgon/Widgets/Listbox.h>
#include <Gorgon/Widgets/ListItem.h>
#include <Gorgon/Widgets/ColorPlane.h>
#include <Gorgon/UI/RadioControl.h>
#include <Gorgon/UI/Organizers/List.h>
#include <Gorgon/UI/Organizers/Flow.h>
#include <Gorgon/Graphics/BlankImage.h>
#include <Gorgon/Graphics/TintedObject.h>
#include <Gorgon/Widgets/Dropdown.h>
#include <Gorgon/Widgets/Window.h>
#include <Gorgon/Widgets/DialogWindow.h>
#include <Gorgon/UI/Dialog.h>

enum DaysOfWeek {
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
};


DefineEnumStrings(DaysOfWeek, {
    {Monday, "Monday"},
    {Tuesday, "Tuesday"},
    {Wednesday, "Wednesday"},
    {Thursday, "Thursday"},
    {Friday, "Friday"},
    {Saturday, "Saturday"},
    {Sunday, "Sunday"}
});


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


float StringDiv(std::string left, std::string min, std::string max) {
    if(min.length() == max.length()) return 0.f;

    return float(left.length() - min.length()) / float(max.length() - min.length());
}

std::string StringVal(float value, std::string min, std::string max) {
    return max.substr(0, int((max.length() - min.length()) * value + min.length()));
}

// Do not forget to define enum and Enum Strings before the create your DropDown with the enum
enum CoffeeType {
    Americano, 
    Latte, 
    Cappuccino, 
    Espresso
};

DefineEnumStrings(CoffeeType, {
    {Americano,"Americano"},
    {Latte,"Latte"},
    {Cappuccino,"Cappuccino"},
    {Espresso,"Espresso"}
});

int main() {
    basic_Application<UI::Window> app("uitest", "UI Generator Test", helptext, 1, 0x80);

    ///Blank Panel & elements with Registry & Regulars
//     Widgets::SimpleGenerator generator;
//     generator.Density = 5;
//     generator.Init(16, "freesans");
//     generator.UpdateBorders();
//     generator.Border.Width = 2;
//     generator.Border.Radius = 4;
//     generator.Border.Divisions = 0;
//     generator.UpdateDimensions();
//     generator.Activate();

    Widgets::Window blank;
    blank.Move(5, 50);
    blank.SetHeight(300);
    auto icon = Triangle(5, 10);
    icon.Prepare();
    Graphics::TintedBitmapProvider icon2(icon.Rotate270(), Graphics::Color::Charcoal);
    icon2.Prepare();

    Gorgon::Widgets::Button btn("Save Âj", Gorgon::Widgets::Registry::Button_Regular);
    Gorgon::Widgets::Button icnbtn("+", Gorgon::Widgets::Registry::Button_Icon);
    Gorgon::Widgets::Button icnbtn2("Âj", Gorgon::Widgets::Registry::Button_Icon);
    Gorgon::Widgets::Button icnbtn3("X", Gorgon::Widgets::Registry::Button_Icon);
    btn.OwnIcon(icon2.CreateAnimation());

    icnbtn.OwnIcon(icon2.CreateAnimation());

    Gorgon::Widgets::Label l("\tOne:");
    Gorgon::Widgets::Label l2("Error", Gorgon::Widgets::Registry::Label_Subtitle);

    Gorgon::Widgets::RadioButtons<int> radio(Gorgon::Widgets::Registry::Radio_Regular);
    radio.Add(0,"One");
    radio.Add(1,"Two");
    
    radio.SetColumns(2);

    Gorgon::Widgets::Textbox input;

    Gorgon::Widgets::Checkbox chk("One",Gorgon::Widgets::Registry::Checkbox_Regular);
    Gorgon::Widgets::Checkbox chk2("Lattej");
    Gorgon::Widgets::Checkbox chkbutton("C", Gorgon::Widgets::Registry::Checkbox_Button);
    Gorgon::Widgets::Progressbar bar(Gorgon::Widgets::Registry::Progress_Regular);
    Gorgon::Widgets::Panel toppanel(Gorgon::Widgets::Registry::Panel_Top);
    toppanel.Resize({ 30,30 });
    Gorgon::Widgets::Label toplabel("Panel Top");
    Gorgon::Widgets::Button topbtn("regular",Gorgon::Widgets::Registry::Button_Regular);
    toppanel.Add(toplabel);
    toppanel.Add(topbtn);
    topbtn.Move(toplabel.GetLocation().X, toplabel.GetLocation().Y + toplabel.GetSize().Height);
    
    chkbutton.SetIconProvider(icon2);

    Gorgon::Widgets::Panel leftpanel(Gorgon::Widgets::Registry::Panel_Left);
    leftpanel.Resize({ 30,30 });
    Gorgon::Widgets::Label leftlabel("Panel Left");
    leftpanel.Add(leftlabel);

    Gorgon::Widgets::Panel rightpanel(Gorgon::Widgets::Registry::Panel_Right);
    Gorgon::Widgets::Label rightlabel("Panel Right");
    rightpanel.Add(rightlabel);
    rightpanel.Resize({ 30,30 });

    Gorgon::Widgets::Panel bottompanel(Gorgon::Widgets::Registry::Panel_Bottom);
    Gorgon::Widgets::Label bottomlabel("Panel bottom");
    bottompanel.Add(bottomlabel);

    input.ChangedEvent.Register([&] {
        std::cout << input.GetText() << std::endl;
    });

    icnbtn.PressEvent.Register([&] {
        bar += 10;
    });

    icnbtn2.PressEvent.Register([&] {
        bar -= 10;
    });

    app.wind.KeyEvent.Register([&](Gorgon::Input::Key key, float amount) {
        namespace Keycodes = Gorgon::Input::Keyboard::Keycodes;

        switch(key) {
        case Keycodes::D:
            blank.ToggleEnabled();
            return true;
        }

        return false;
    });
    Widgets::VScrollbar scroll1(20);
    scroll1 = 100;
    scroll1.Range = 20;

    
    Widgets::HScrollbar scroll2(200);
    scroll2.Range = 20;
    //scroll2.SetValue(200, false);

    Gorgon::Widgets::Inputbox<Graphics::RGBAf> colorin;
    colorin.ChangedEvent.Register([&] {
        std::cout << "size f changed " << colorin.GetText();
    });
    colorin.AutoSelectAll = true;
    
    Widgets::MultiListbox<std::string> list;
    //list.SetSelectionMethod(list.UseCtrl);
    list.SetEventMethod(list.Once);
    //list.SetSelectionFollowsFocus(false);
    list.Add("5");
    list.Add("Âj");
    list.Add("9");
    list.Add("Hello");
    list.Add("World");
//     for(int i=0; i<10; i++)
//         list.Add("Welcome", "to", "the", "wonderful", "Gorgon", "Library", "!");
//     list.Add("World");
    list.AddToSelection(3);
    list.AddToSelection(1, 4);
    list.InvertSelection();
    list.SetWidthInUnits(5);
    list.ChangedEvent.Register([&](long index, bool status) {
        std::cout << "Selected items: ";
        for(auto &s : list.Selection) {
            std::cout << s << "\t";
        }
        
        std::cout << std::endl;
        
        if(index == -1)
            return;
        
        std::cout << list[index] << ": " << status << std::endl;
        
        if(index >= 1)
            list.Remove(index - 1);
    });
    list.EnsureVisible(11);
    list.EnsureVisible(1);
    list.Clear();
    list.FitHeight(list.GetHeight());
    list.Tooltip = "This is the list";
    
    Widgets::DropdownList<DaysOfWeek> dlist(begin(Enumerate<DaysOfWeek>()), end(Enumerate<DaysOfWeek>()));
    dlist.List.SetSelectedIndex(1);
    dlist.ChangedEvent.Register([](long index) {
        std::cout << "Dropdown index: " << index << std::endl;
    });
    dlist.Tooltip = "Choose your coffee";
   
    //...
    // Define your DropDown with : 
    Widgets::DropdownList<CoffeeType> Coffee(begin(Enumerate<CoffeeType>()), end(Enumerate<CoffeeType>()));
    
    // Select Default value
    Coffee = Latte;
    // or index
    Coffee.List.SetSelectedIndex(1);
    
    // Use ChangedEvent.Register to handle selection event
    Coffee.ChangedEvent.Register([&](long index){
        //this can throw if nothing is selected, check index, it should be > -1
        //this can only happen if you remove the selected item from the list.
        if(index == -1) {
            std::cout << "Nothing is selected" << std::endl;
        }
        else {
            std::cout << "Your coffee is " << Coffee << std::endl;
        }
    });
    
    //Compare using if...
    if(Coffee == Latte) {
        std::cout << "Working ";
    }
    //...or switch
    switch(Coffee) {
    case Latte:
        std::cout << "for sure" << std::endl;
        break;
    default:
        std::cout << "not really" << std::endl;
        break;
    }
    
    
    auto &org = blank.CreateOrganizer<UI::Organizers::Flow>();

    //org.SetAlignment(org.Center);
    
    Widgets::ColorPlane plane;
    
    org << 9 << plane << org.Break
        << 2 << "Label" << 2 << "" << Coffee << l2
        << list << 2 << org.Break
        << icnbtn2 << 1 << "" << dlist
        << icnbtn3 << radio << icnbtn << 5 << "Hello"
        << chk << chk2 << chkbutton
        << input
        << bar
        << scroll1 << scroll2
        << colorin
    ;
    
    org << org.Action("Ok", [&]() { std::cout << "Ok clicked" << std::endl; });
    btn.SetHorizonalAutosize(Gorgon::UI::Autosize::Unit);

    
    Widgets::DialogWindow wind("My window", {200, 300});
    wind.Add(btn);
    wind.OwnIcon(icon.CreateAnimation());
    int closetrycount = 0;
    wind.ClosingEvent.Register([&](bool &allow) {
        allow = closetrycount++;
        if(!allow)
            std::cout << "Click once more to close." << std::endl;
    });
    btn.Move(0,0);
    Widgets::Checkbox enableclosebtn("Enable close button", true);
    enableclosebtn.SetAutosize(Gorgon::UI::Autosize::Unit, Gorgon::UI::Autosize::Automatic);
    enableclosebtn.ChangedEvent.Register([&] { wind.SetCloseButtonEnabled(bool(enableclosebtn)); });
    wind.Add(enableclosebtn);
    wind.CreateOrganizer<UI::Organizers::List>() 
        .Add("Try resize")
        .Add("Click close twice")
    ;
    wind.AllowResize();
    std::vector<std::string> opts = {"Zero", "One", "Two"};
    wind.AddButton("?", [&]{
        //UI::MultipleChoice<DaysOfWeek>("Select one", [](DaysOfWeek ind) { std::cout << ind << std::endl; }, UI::CloseOption::Cancel);
        UI::AskYesNo("Exit", "Are you certain?", [&app] { 
            app.wind.Quit(); 
        });
    }).SetHorizonalAutosize(Gorgon::UI::Autosize::Automatic);
    
    wind.AddButton("Msg", []{
        UI::ShowMessage("Hello", "This class is the base class for all widget containers.\n\nAll widgets require a layer to be placed on, to allow widget containers that are also widgets, this class is left abstract. You may derive from this class and WidgetBase at the same time.");
    }).SetHorizonalAutosize(Gorgon::UI::Autosize::Automatic);
    
    wind.AddButton("Inp", []{
        UI::Input<std::string>("??", "Please enter your name below", "Name", 
            [](const std::string &reply) {
                UI::ShowMessage("Hello "+reply);
            }, 
            [](const std::string &reply) {
                return reply.length()>2;
            }
        );
    }).SetHorizonalAutosize(Gorgon::UI::Autosize::Automatic);
    
    wind.AddButton("Sel", []{
        UI::Select<DaysOfWeek>("Holiday", "Please select an option",
            [](DaysOfWeek v) {
                UI::ShowMessage("We are going holiday on " + String::From(v));
            }, 2
        );
    }).SetHorizonalAutosize(Gorgon::UI::Autosize::Automatic);
    
    app.wind.Tooltips.SetSetText([](const std::string &text) {
        std::cout << "Tooltip: " << text << std::endl;
    });

    blank.Tooltip = "This is the container";
    app.wind.Add(blank);
    app.wind.Run();

    return 0;
}
