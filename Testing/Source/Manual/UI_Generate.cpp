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

    ///Blank Panel & elements with Registry & Regulars 

    Widgets::Panel blank(Gorgon::Widgets::Registry::Panel_Blank);
    blank.Resize({ 100,600 });
    Gorgon::Widgets::Button btn("press",Gorgon::Widgets::Registry::Button_Regular);
    Gorgon::Widgets::Button icnbtn("+", Gorgon::Widgets::Registry::Button_Icon);
    Gorgon::Widgets::Button icnbtn2("-", Gorgon::Widgets::Registry::Button_Icon);

    Gorgon::Widgets::Label l("Coffee:");

    Gorgon::Widgets::RadioButtons<int> radio(Gorgon::Widgets::Registry::Radio_Regular);
    radio.Add(0,"Americano");
    radio.Add(1,"Latte");

    Gorgon::Widgets::Inputbox<std::string> input;
   

    Gorgon::Widgets::Checkbox chk("Black",Gorgon::Widgets::Registry::Checkbox_Regular);
    Gorgon::Widgets::Checkbox chk2("Latte");
    Gorgon::Widgets::Checkbox chkbutton("Cbutton", Gorgon::Widgets::Registry::Checkbox_Button);
    Gorgon::Widgets::Progressbar bar(Gorgon::Widgets::Registry::Progress_Regular);
    Gorgon::Widgets::Panel toppanel(Gorgon::Widgets::Registry::Panel_Top);
    toppanel.Resize({ 30,30 });
    Gorgon::Widgets::Label toplabel("Panel Top");
    Gorgon::Widgets::Button topbtn("regular",Gorgon::Widgets::Registry::Button_Regular);
    toppanel.Add(toplabel);
    toppanel.Add(topbtn);
    topbtn.Move(toplabel.GetLocation().X, toplabel.GetLocation().Y + toplabel.GetSize().Height);

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
        bar.Set(bar.Get() + 10);
    });

    icnbtn2.PressEvent.Register([&] {
        bar.Set(bar.Get() - 10);
    });
    blank.CreateOrganizer<Gorgon::UI::Organizers::List>().SetSpacing(Gorgon::Widgets::Registry::Active().GetSpacing());
    blank.Add(btn);
    blank.Add(icnbtn);
    blank.Add(icnbtn2);
    blank.Add(l);
    blank.Add(radio);
    blank.Add(input);
    blank.Add(chk);
    blank.Add(chk2);
    blank.Add(chkbutton);
    blank.Add(bar);
    blank.Add(toppanel);
    blank.Add(bottompanel);
    blank.Add(leftpanel);
    blank.Add(rightpanel);


    

    
    ///Generator 1  background.panel color = red focus color = grey, forecolor.regular = green border = blue,  fontsize = 12 defualt;
    Widgets::SimpleGenerator gen;
    gen.Init(12);
    gen.Background.Panel = Gorgon::Graphics::Color::Red;
    gen.Border.Color = Gorgon::Graphics::Color::Blue;
    gen.Forecolor.Regular = Gorgon::Graphics::Color::Green;
    gen.Focus.Color = Gorgon::Graphics::Color::Grey;
    gen.UpdateBorders();
    gen.UpdateDimensions();
    
    auto gentemp = gen.BlankPanel();
    auto btntemp = gen.Button();
    auto radtemp = gen.RadioButton();
    auto chktemp = gen.Checkbox();
    auto chktemp2 = gen.CheckboxButton();
    auto icobtntemp = gen.IconButton();
    auto lbltemp = gen.Label();
    auto pnltemp = gen.BlankPanel();
    auto inptemp = gen.Inputbox();
    auto progresstmp = gen.Progressbar();
    auto toppaneltmp = gen.TopPanel();
    auto bottompaneltmp = gen.BottomPanel();
    auto leftpaneltmp = gen.LeftPanel();
    auto rightpaneltmp = gen.RightPanel();
    
    Gorgon::Widgets::Panel panelgen1(gentemp);
    Gorgon::Widgets::Button btngen1(btntemp,"press");
    Gorgon::Widgets::Button icnbtngen1(icobtntemp,"+");
    Gorgon::Widgets::Button icnbtn2gen1(icobtntemp, "-");
    Gorgon::Widgets::Label labelgen1(lbltemp, "Coffee:");
    
    Gorgon::Widgets::RadioButtons<int> radiogen1(radtemp);
    radiogen1.Add(0, "Americano");
    radiogen1.Add(1, "Latte");

    Gorgon::Widgets::Inputbox<std::string> inputgen1(inptemp);
    Gorgon::Widgets::Checkbox chkgen1(chktemp, "Black");
    Gorgon::Widgets::Checkbox chk2gen1(chktemp, "Latte");
    Gorgon::Widgets::Checkbox chkbtngen1(chktemp2, "Cbutton");
    Gorgon::Widgets::Progressbar bargen1(progresstmp);

    Gorgon::Widgets::Panel toppanelgen1(toppaneltmp);
    toppanelgen1.Resize({ 50,80 });
    Gorgon::Widgets::Label toplabelgen1(lbltemp,"Panel Top");
    Gorgon::Widgets::Button topbtngen1(btntemp,"button");
    toppanelgen1.Add(toplabelgen1);
    toppanelgen1.Add(topbtngen1);
    topbtngen1.Move(toplabelgen1.GetLocation().X, toplabelgen1.GetLocation().Y + toplabelgen1.GetSize().Height);
    Gorgon::Widgets::Label bottomlabelgen1(lbltemp, "Panel bottom");

    Gorgon::Widgets::Panel bottompanelgen1(bottompaneltmp);
    bottompanelgen1.Resize({ 30,30 });
    bottompanelgen1.Add(bottomlabelgen1);
    Gorgon::Widgets::Panel leftpanelgen1(leftpaneltmp);
    Gorgon::Widgets::Label leftlabelgen1(lbltemp, "Panel left");
    leftpanelgen1.Add(leftlabelgen1);
    leftpanelgen1.Resize({ 30,30 });
    Gorgon::Widgets::Panel rightpanelgen1(rightpaneltmp);
    Gorgon::Widgets::Label rightlabelgen1(lbltemp, "Panel right");
    rightpanelgen1.Add(rightlabelgen1);
    rightpanelgen1.Resize({ 30,30 });
    






    inputgen1.ChangedEvent.Register([&] {
        std::cout << inputgen1.GetText() << std::endl;
    });

    icnbtngen1.PressEvent.Register([&] {
        bargen1.Set(bargen1.Get() + 10);
    });

    icnbtn2gen1.PressEvent.Register([&] {
        bargen1.Set(bargen1.Get() - 10);
    });



    panelgen1.Resize(100, 600);
    panelgen1.CreateOrganizer<Gorgon::UI::Organizers::List>().SetSpacing(Gorgon::Widgets::Registry::Active().GetSpacing());
    panelgen1.Add(btngen1);
    panelgen1.Add(icnbtngen1);
    panelgen1.Add(icnbtn2gen1);
    panelgen1.Add(labelgen1);
    panelgen1.Add(radiogen1);
    panelgen1.Add(inputgen1);
    panelgen1.Add(chkgen1);
    panelgen1.Add(chk2gen1);
    panelgen1.Add(chkbtngen1);
    panelgen1.Add(bargen1);
    panelgen1.Add(toppanelgen1);
    panelgen1.Add(bottompanelgen1);
    panelgen1.Add(leftpanelgen1);
    panelgen1.Add(rightpanelgen1);





    ///Generator 2  background.panel color = grey focus color = blue, forecolor.regular = purple border = white,  fontsize = 20 defualt;
    Widgets::SimpleGenerator gen2;
    gen2.Init(16);
    gen2.Background.Panel = Gorgon::Graphics::Color::Grey;
    gen2.Border.Color = Gorgon::Graphics::Color::White;
    gen2.Forecolor.Regular = Gorgon::Graphics::Color::Purple;
    gen2.Focus.Color = Gorgon::Graphics::Color::Blue;
    gen2.UpdateBorders();
    gen2.UpdateDimensions();

    auto gentemp2 = gen2.BlankPanel();
    auto btntemp2 = gen2.Button();
    auto radtemp2 = gen2.RadioButton();
    auto chktempgen2 = gen2.Checkbox();
    auto chktempbtn2 = gen2.CheckboxButton();
    auto icobtntemp2 = gen2.IconButton();
    auto lbltemp2 = gen2.Label();
    auto pnltemp2 = gen2.BlankPanel();
    auto inptemp2 = gen2.Inputbox();
    auto progresstmp2 = gen2.Progressbar();
    auto toppaneltmp2 = gen2.TopPanel();
    auto bottompaneltmp2 = gen2.BottomPanel();
    auto leftpaneltmp2 = gen2.LeftPanel();
    auto rightpaneltmp2 = gen2.RightPanel();

    Gorgon::Widgets::Panel panelgen2(gentemp2);
    Gorgon::Widgets::Button btngen2(btntemp2, "press");
    Gorgon::Widgets::Button icnbtngen2(icobtntemp2, "+");
    Gorgon::Widgets::Button icnbtn2gen2(icobtntemp2, "-");
    Gorgon::Widgets::Label labelgen2(lbltemp2, "Coffee:");

    Gorgon::Widgets::RadioButtons<int> radiogen2(radtemp2);
    radiogen2.Add(0, "Americano");
    radiogen2.Add(1, "Latte");

    Gorgon::Widgets::Inputbox<std::string> inputgen2(inptemp2);
    Gorgon::Widgets::Checkbox chkgen2(chktemp2, "Black");
    Gorgon::Widgets::Checkbox chk2gen2(chktemp2, "Latte");
    Gorgon::Widgets::Checkbox chkbtngen2(chktempgen2, "Cbutton");
    Gorgon::Widgets::Progressbar bargen2(progresstmp2);

    Gorgon::Widgets::Panel toppanelgen2(toppaneltmp2);
    toppanelgen2.Resize({ 50,80 });
    Gorgon::Widgets::Label toplabelgen2(lbltemp2, "Panel Top");
    Gorgon::Widgets::Button topbtngen2(btntemp2, "button");
    toppanelgen2.Add(toplabelgen2);
    toppanelgen2.Add(topbtngen2);
    topbtngen2.Move(toplabelgen2.GetLocation().X, toplabelgen2.GetLocation().Y + toplabelgen2.GetSize().Height);
    Gorgon::Widgets::Label bottomlabelgen2(lbltemp2, "Panel bottom");

    Gorgon::Widgets::Panel bottompanelgen2(bottompaneltmp2);
    bottompanelgen2.Resize({ 30,30 });
    bottompanelgen2.Add(bottomlabelgen2);
    Gorgon::Widgets::Panel leftpanelgen2(leftpaneltmp2);
    Gorgon::Widgets::Label leftlabelgen2(lbltemp2, "Panel left");
    leftpanelgen2.Add(leftlabelgen2);
    leftpanelgen2.Resize({ 30,30 });
    Gorgon::Widgets::Panel rightpanelgen2(rightpaneltmp2);
    Gorgon::Widgets::Label rightlabelgen2(lbltemp2, "Panel right");
    rightpanelgen2.Add(rightlabelgen2);
    rightpanelgen2.Resize({ 30,30 });







    inputgen2.ChangedEvent.Register([&] {
        std::cout << inputgen2.GetText() << std::endl;
    });

    icnbtngen2.PressEvent.Register([&] {
        bargen2.Set(bargen2.Get() + 10);
    });

    icnbtn2gen2.PressEvent.Register([&] {
        bargen2.Set(bargen2.Get() - 10);
    });



    panelgen2.Resize(100, 600);
    panelgen2.CreateOrganizer<Gorgon::UI::Organizers::List>().SetSpacing(Gorgon::Widgets::Registry::Active().GetSpacing());
    panelgen2.Add(btngen2);
    panelgen2.Add(icnbtngen2);
    panelgen2.Add(icnbtn2gen2);
    panelgen2.Add(labelgen2);
    panelgen2.Add(radiogen2);
    panelgen2.Add(inputgen2);
    panelgen2.Add(chkgen2);
    panelgen2.Add(chk2gen2);
    panelgen2.Add(chkbtngen2);
    panelgen2.Add(bargen2);
    panelgen2.Add(toppanelgen2);
    panelgen2.Add(bottompanelgen2);
    panelgen2.Add(leftpanelgen2);
    panelgen2.Add(rightpanelgen2);






    app.wind.Add(blank);
    app.wind.Add(panelgen1);
    app.wind.Add(panelgen2);
    panelgen1.Move(blank.GetBounds().Right + 5,blank.GetLocation().Y);
    panelgen2.Move(panelgen1.GetBounds().Right + 5, blank.GetLocation().Y);

    /*
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
    btn.Text = "assdfa";
    btn.Size *= 1.2;
    
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
    rad.ChangeValue(0, 2);

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
    inp += {3, 2};
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
    
    */
    app.wind.Run();

    return 0;
}
