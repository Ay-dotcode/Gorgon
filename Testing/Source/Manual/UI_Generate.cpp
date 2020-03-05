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
#include <Gorgon/UI/RadioControl.h>
#include <Gorgon/UI/Organizers/List.h>
#include <Gorgon/Graphics/BlankImage.h>

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
    
    Widgets::SimpleGenerator gen(15, "DejavuSans");
    Widgets::SimpleGenerator gen2(12);
    gen.Border.Radius = 3;
    gen.UpdateDimensions();
    gen2.Focus.Color = Graphics::Color::Red;
    gen2.Border.Radius = 2;


    std::cout << "font height: " << gen.RegularFont.GetGlyphRenderer().GetLetterHeight(true).second << std::endl;

    auto btntemp = gen.Button();
    auto radtemp = gen.RadioButton();
    auto chktemp = gen.Checkbox();
    auto chktemp2 = gen.CheckboxButton();
    auto icobtntemp = gen.IconButton();
    auto lbltemp = gen.Label();
    auto pnltemp = gen.BlankPanel();
    auto pnltemp2 = gen.Panel();
    auto inptemp = gen2.Inputbox();

    Widgets::Button btn(btntemp, "Helloo...", []() { std::cout<<"Hello..."<<std::endl; });
    btn.Move(5,5);
    
    app.wind.Add(btn);
    btn.Focus();
    
    Widgets::Button btn2(btntemp, "Exit", [&app]() { app.wind.Quit(); });

    btn2.Move({5, btn.GetSize().Height + 10});
    app.wind.Add(btn2);
    
    std::cout<<"Height: "<<gen.RegularFont.GetGlyphRenderer().GetSize('A').Height<<std::endl;
    
    Widgets::RadioButtons<int> rad(radtemp);
    
    rad.Add(0, u8"Ájmericano");
    rad.Add(1, "Latte");
    rad.Add(2);

    rad.Disable(2);

    app.wind.Add(rad);
    rad.Move(150, 4);
    
    

    Widgets::Checkbox chk(chktemp, "Sugar", [](bool state) {
        std::cout<<(state ? "with sugar" : "without sugar")<<std::endl;
    });

    rad.ChangedEvent.Register([&](int val) {
        std::cout<<"Changed to "<<val<<std::endl;
        chk.ToggleEnabled();
    });
    
    //chk.Hide();

    chk.BoundsChangedEvent.Register([] { 
        std::cout << "Bounds changed" << std::endl; 
    });
    rad.ChangedEvent.Register([&chk]{ chk.Show(); });
    chk.FocusEvent.Register([]{ std::cout << "Focus changed." << std::endl; });

    chk.Move(rad.GetLocation() + Gorgon::Geometry::Point(0, rad.GetSize().Height + 4));

    Widgets::Button ib(icobtntemp);
    auto ico = Graphics::BlankImage({16, 16}, Graphics::Color::Black);
    //ico.Prepare();
    ib.SetIcon(ico);
    ib.Move(chk.GetLocation() + Gorgon::Geometry::Point(0, chk.GetSize().Height + 4));
    app.wind.Add(ib);

    
    Widgets::Panel pnl(pnltemp);
    pnl.Resize(350, 300);
    Widgets::Panel mainpanel(pnltemp);
    mainpanel.Resize(350, 700);
    app.wind.Add(mainpanel);
    Widgets::Panel sub(pnltemp2);
    sub.Resize(300, 50);
    Widgets::Button increase(btntemp);
    Widgets::Button decrease(btntemp);


    mainpanel.Add(sub);
    sub.Move(5, 0);

    Widgets::Label valuelabel(lbltemp);
    sub.Add(valuelabel);
    valuelabel.SetText("Bug test : ");
    increase.SetText("+");
    decrease.SetText("-");
    
    sub.Add(increase);
    sub.Add(decrease);
    sub.CreateOrganizer<Gorgon::UI::Organizers::List>();
    
    sub.SetWidth(100);
    sub.SetScrollDistance(15);
    sub.SetOverscroll(200);
    
    increase.ClickEvent.Register([&]() {
        sub.ScrollBy(100);
    });
    
    decrease.ClickEvent.Register([&]() {
        sub.ScrollBy(-100);
        std::cout<<"-"<<std::endl;
    });
    decrease.ActivateClickRepeat();
    
    Widgets::Checkbox chk1(chktemp2, "Su?");
    
    //app.wind.Add(chk1);
    chk1.Move(300, 0);
    
    app.wind.Add(pnl);
    pnl.Move(150, 0);
    app.wind.Add(mainpanel);
    mainpanel.Move(0, 90);

    auto errortemp = gen.ErrorLabel();
    
    Widgets::Label lbl(lbltemp, "This is a label");
    Widgets::Label error(errortemp, "This is an Error label");
    
    Widgets::Pointbox inp(inptemp);
    inp={5, 2};
    pnl.Add(inp);
    inp.Move(5, 80);
    inp.SelectAll();

    pnl.Add(chk);
    pnl.Add(lbl);
    pnl.Add(error);
    pnl.Add(rad);
    pnl.Add(ib);
    pnl.SetHeight(500);
    mainpanel.Resize(140, 500);
    pnl.Disable();

    pnl.CreateOrganizer<Gorgon::UI::Organizers::List>();


    //lbl.Move(chk.GetLocation() + Gorgon::Geometry::Point(0, chk.GetSize().Height + 4));
    //error.Move(lbl.GetLocation().X,lbl.GetLocation().Y + 25);
    lbl.OwnIcon(prep(*new Graphics::Bitmap(Triangle(8, 8))));
    
    app.wind.Run();

    return 0;
}
