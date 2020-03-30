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
    
    Widgets::SimpleGenerator gen;
    Widgets::SimpleGenerator gen2(12, "", false);
    gen.Init(15);
    gen.UpdateBorders();
    gen.UpdateDimensions();
    gen2.Focus.Color = Graphics::Color::Red;
    gen2.UpdateBorders(false);
    gen2.UpdateDimensions();
    
    std::cout << "font height: " << gen.RegularFont.GetGlyphRenderer().GetLetterHeight(true).second << std::endl;

    auto btntemp = gen.Button();
    auto radtemp = gen.RadioButton();
    auto chktemp = gen.Checkbox();
    auto chktemp2 = gen.CheckboxButton();
    auto icobtntemp = gen.IconButton();
    auto lbltemp = gen.Label();
    auto pnltemp = gen.BlankPanel();
    auto inptemp = gen2.Inputbox();

    Widgets::Progressbar progress;
    progress.Maximum = 30;
    progress.Minimum = 10;

    Widgets::Button btn("Helloo_...", [ &]() { std::cout<<"Hello..."<<std::endl; progress += 1; });
    btn.Move(5,5);
    btn.Size.Width *= 1.2;
    
    app.wind.Add(btn);
    btn.Focus();
    
    Widgets::Button btn2("Exit", [&app]() { app.wind.Quit(); });

    btn2.Move({5, btn.GetSize().Height + 10});
    app.wind.Add(btn2);
    
    std::cout<<"Height: "<<gen.RegularFont.GetGlyphRenderer().GetSize('A').Height<<std::endl;
    
    Widgets::RadioButtons<int> rad;
    
    rad.Add(0, u8"Ájmericano");
    rad.Add(1, "Latte");
    rad.Add(2);

    //rad.Disable(2);

    app.wind.Add(rad);
    rad.Location = {150, 4};
    
    

    Widgets::Checkbox chk("Sugar", [](bool state) {
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

    Widgets::Button ib(Widgets::Registry::Button_Icon);
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
    Widgets::Panel sub(Widgets::Registry::Panel_Top);
    sub.Resize(300, 50);
    Widgets::Button increase(btntemp);
    Widgets::Button decrease(btntemp);


    mainpanel.Add(sub);
    sub.Move(0, 0);

    Widgets::Label valuelabel(lbltemp), l2(lbltemp, "Hello"), l3(lbltemp, "Another");
    valuelabel.SetText("Bug test : ");
    increase.SetText("+");
    decrease.SetText("-");
    
    sub.Add(increase);
    sub.Add(decrease);
    sub.Add(valuelabel);
    sub.Add(l2);
    sub.Add(l3);
    sub.CreateOrganizer<Gorgon::UI::Organizers::List>();
    
    sub.SetWidth(100);
    sub.SetScrollDistance(15);
    sub.SetOverscroll(50);
    
    increase.ClickEvent.Register([&]() {
        sub.ScrollBy(100);
    });
    
    decrease.ClickEvent.Register([&]() {
        sub.ScrollBy(-100);
        std::cout<<"-"<<std::endl;
    });
    decrease.ActivateClickRepeat();
    
    Widgets::Checkbox chk1("?", Widgets::Registry::Checkbox_Button);
    
    app.wind.Add(chk1);
    chk1.Move(500, 80);
    
    app.wind.Add(pnl);
    pnl.Move(150, 0);
    app.wind.Add(mainpanel);
    mainpanel.Move(0, 90);

    auto errortemp = gen.ErrorLabel();
    
    Widgets::Label lbl("This is a label");
    Widgets::Label error("This is an Error label", Gorgon::Widgets::Registry::Label_Error);
    
    Widgets::Sizebox inp;
    inp.Width = 5;
    pnl.Add(inp);
    inp.Location = {5, 80};
    inp.SelectAll();
    inp.ChangedEvent.Register([](Geometry::Size val) {
        std::cout << val << std::endl;
    });
    //inp.Readonly = true;
    
    app.wind.Add(progress);
    
    pnl.Add(progress);
    pnl.Add(chk);
    pnl.Add(lbl);
    pnl.Add(error);
    pnl.Add(rad);
    pnl.Add(ib);
    pnl.SetHeight(500);
    mainpanel.Resize(140, 550);
    //pnl.Disable();
    
    pnl.CreateOrganizer<Gorgon::UI::Organizers::List>();


    //lbl.Move(chk.GetLocation() + Gorgon::Geometry::Point(0, chk.GetSize().Height + 4));
    //error.Move(lbl.GetLocation().X,lbl.GetLocation().Y + 25);
    lbl.OwnIcon(prep(*new Graphics::Bitmap(Triangle(8, 8))));
    
    
    app.wind.Run();

    return 0;
}
