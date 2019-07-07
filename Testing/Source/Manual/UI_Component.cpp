#include "GraphicsHelper.h"

#include <Gorgon/UI/Template.h>
#include <Gorgon/UI/ComponentStack.h>
#include <Gorgon/Graphics/EmptyImage.h>
#include <Gorgon/Graphics/BlankImage.h>
#include <Gorgon/Graphics/Rectangle.h>
#include <Gorgon/Graphics/TintedObject.h>


std::string helptext = 
    "Key list:\n"
    "d\tToggle disabled\n"
    "1-2\tChange Values\n"
	"esc\tClose\n"
;

int main() {
	Application app("uitest", "UI Component Test", helptext);

	Graphics::Layer l;
    app.wind.Add(l);

    Graphics::BitmapAnimationProvider animprov;

    for(int i=0; i<25; i++) {
        Graphics::Bitmap bmp({25, 25}, Graphics::ColorMode::Alpha);
        bmp.Clear();
        for(int y = 0; y<i; y++) {
            for(int x = 0; x<25; x++) {
                bmp(x, y, 0) = 80;
            }
        }
        
        animprov.Add(std::move(bmp), 30+i*5);
    }
    
    animprov.Prepare();
    
    Graphics::BlankImage border_col(1, 1, 0xff000000);
    Graphics::BlankImage border_bg(0.8f);
    Graphics::RectangleProvider rect(border_col, border_col, border_col, border_col, border_bg, border_col, border_col, border_col, border_col);

    Graphics::BlankImage btn_bg(0.2f);
	Graphics::BlankImage btn_bgh(0.0f);
	Graphics::BlankImage btn_bghr({0.8f, 0.f, 0.f, 1.f});
	Graphics::BlankImage tick(0.4f);
	Graphics::BlankImage tick_h(Graphics::RGBAf{0.8f, 0.f, 0.f});

    auto trig = Triangle(6, 10);
    auto trig1 =trig.Rotate90();
    auto trig2 =trig.Rotate270();
    trig.Prepare();
    trig1.Prepare();
    trig2.Prepare();
    
    UI::Template lbtn;
	lbtn.SetConditionDuration(UI::ComponentCondition::Always, UI::ComponentCondition::Hover, 250);
	lbtn.SetConditionDuration(UI::ComponentCondition::Hover, UI::ComponentCondition::Always, 250);
	lbtn.SetConditionDuration(UI::ComponentCondition::Always, UI::ComponentCondition::Down, 250);
	lbtn.SetConditionDuration(UI::ComponentCondition::Down, UI::ComponentCondition::Always, 250);
	lbtn.SetSize({20, 20});
    
    
    UI::Template rbtn;
	rbtn.SetSize({20, 20});
	auto &btnright_n = rbtn.AddContainer(0, UI::ComponentCondition::Always);
	btnright_n.Background.SetDrawable(btn_bg);
	btnright_n.SetSize(100, 100, UI::Dimension::Percent);

	auto &btnright_h = rbtn.AddContainer(0, UI::ComponentCondition::Hover);
	btnright_h.Background.SetDrawable(btn_bgh);
	btnright_h.SetSize({100, UI::Dimension::Percent}, {100, UI::Dimension::Percent});

	auto &btn_right_trig = rbtn.AddPlaceholder(1, UI::ComponentCondition::Always);
	btn_right_trig.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
    btn_right_trig.SetDataEffect(UI::ComponentTemplate::Icon);
    
	btnright_n.AddIndex(1);
	btnright_h.AddIndex(1);


    UI::ComponentStack stack(rbtn, {300, 24});
    stack.HandleMouse();
    stack.SetData(UI::ComponentTemplate::Icon, trig);
    
    app.wind.Add(stack);
    

	while(true) {
		Gorgon::NextFrame();
		
	}

	return 0;
}

