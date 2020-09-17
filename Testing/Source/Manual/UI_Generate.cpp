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
#include <Gorgon/Graphics/TintedObject.h>


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
    /*Widgets::SimpleGenerator generator;
    generator.Init(13);
    //generator.Border.Radius = 0;
    generator.UpdateBorders();
    generator.UpdateDimensions();
    generator.Activate();*/

    Widgets::Panel blank/*(Gorgon::Widgets::Registry::Panel_Blank)*/;
    blank.SetHeight(600);
    Gorgon::Widgets::Button btn("Save Âj",Gorgon::Widgets::Registry::Button_Regular);
    Gorgon::Widgets::Button icnbtn("+", Gorgon::Widgets::Registry::Button_Icon);
    Gorgon::Widgets::Button icnbtn2("Âj", Gorgon::Widgets::Registry::Button_Icon);
    Gorgon::Widgets::Button icnbtn3("X", Gorgon::Widgets::Registry::Button_Icon);
    auto icon = Triangle(5, 10);
    icon.Prepare();
    Graphics::TintedBitmapProvider icon2(icon.Rotate270(), Graphics::Color::Charcoal);
    icon2.Prepare();
    btn.OwnIcon(icon2.CreateAnimation());
    
    icnbtn.OwnIcon(icon2.CreateAnimation());

    Gorgon::Widgets::Label l("Coffee:");
    Gorgon::Widgets::Label l2("Error", Gorgon::Widgets::Registry::Label_Error);

    Gorgon::Widgets::RadioButtons<int> radio(Gorgon::Widgets::Registry::Radio_Regular);
    radio.Add(0,"Americano");
    radio.Add(1,"Latte");

    Gorgon::Widgets::Inputbox<std::string> input;
    
    Gorgon::Widgets::Checkbox chk("Black",Gorgon::Widgets::Registry::Checkbox_Regular);
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
    //blank.CreateOrganizer<Gorgon::UI::Organizers::List>().SetSpacing(Gorgon::Widgets::Registry::Active().GetSpacing());
    
    auto addme = [&](auto &pnl, UI::WidgetBase &w) {
        Geometry::Point offsetx = {Widgets::Registry::Active().GetSpacing(), 0};
        if(pnl.UI::WidgetContainer::begin() != pnl.UI::WidgetContainer::end()) {
            auto &last = *(pnl.UI::WidgetContainer::end() - 1);
            auto lastb = last.GetBounds();
            if(lastb.Right + w.GetWidth() <= pnl.GetInteriorSize().Width)
                w.Move(last.GetBounds().TopRight() + offsetx);
            else
                w.Move(0, last.GetBounds().Bottom + Widgets::Registry::Active().GetSpacing());
        }
        
        pnl.Add(w);
    };
    
    app.wind.KeyEvent.Register([&](Gorgon::Input::Key key, float amount) {
        namespace Keycodes = Gorgon::Input::Keyboard::Keycodes;
        
        switch(key) {
        case Keycodes::D:
            blank.ToggleEnabled();
            return true;
        }
            
        return false;
    });
    
    app.wind.Add(blank);
    addme(blank, btn);
    addme(blank, icnbtn);
    addme(blank, icnbtn2);
    addme(blank, icnbtn3);
    addme(blank, l);
    addme(blank, radio);
    addme(blank, l2);
    addme(blank, input);
    addme(blank, chk);
    addme(blank, chk2);
    addme(blank, chkbutton);
    addme(blank, bar);
    addme(blank, toppanel);
    addme(blank, bottompanel);
    addme(blank, leftpanel);
    addme(blank, rightpanel);
/*
    
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
    icnbtngen1.SetIcon(icon);
    
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



    panelgen1.Resize(180, 600);
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
    icnbtngen2.SetIcon(icon);
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



    panelgen2.Resize(180, 600);
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



*/


    /*app.wind.Add(panelgen1);
    app.wind.Add(panelgen2);
    panelgen1.Move(blank.GetBounds().Right + 5,blank.GetLocation().Y);
    panelgen2.Move(panelgen1.GetBounds().Right + 5, blank.GetLocation().Y);

   */
    app.wind.Run();

    return 0;
}
