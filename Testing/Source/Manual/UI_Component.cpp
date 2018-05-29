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
    
    Graphics::BlankImage border_col(1, 1, 0xff000000);
    Graphics::BlankImage border_bg(0.8f);
    Graphics::RectangleProvider rect(border_col, border_col, border_col, border_col, border_bg, border_col, border_col, border_col, border_col);

    Graphics::BlankImage btn_bg(0.2f);
    Graphics::BlankImage tick(0.4f);

    auto trig = Triangle(6, 10);
    auto trig1 =trig.Rotate90();
    auto trig2 =trig.Rotate270();
    trig1.Prepare();
    trig2.Prepare();
    
    UI::Template temp;
    
    
    auto &outer = temp.AddContainer(0, UI::ComponentCondition::Always);
    outer.Background.SetAnimation(rect);
    outer.SetBorderSize(1);
    
    auto &btnleft_n = temp.AddContainer(1, UI::ComponentCondition::Always);
    btnleft_n.Background.SetDrawable(btn_bg);
    btnleft_n.SetSize({20, UI::Dimension::Pixel}, {100, UI::Dimension::Percent});
    btnleft_n.SetMargin(0, 0, 1, 0);
    outer.AddIndex(1);
    
    auto &btn_left_trig = temp.AddGraphics(2, UI::ComponentCondition::Always);
    btn_left_trig.Content.SetDrawable(trig1);
    btn_left_trig.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
    btnleft_n.AddIndex(2);

    auto &indicator_n = temp.AddContainer(3, UI::ComponentCondition::Always);
    indicator_n.SetSize({100, UI::Dimension::Percent}, {100, UI::Dimension::Percent});
    outer.AddIndex(3);

    
    auto &indicator_sym = temp.AddGraphics(4, UI::ComponentCondition::Always);
    indicator_sym.Content.SetDrawable(btn_bg);
    indicator_sym.SetSize({20, UI::Dimension::Pixel}, {100, UI::Dimension::Percent});
    indicator_sym.SetValueModification(indicator_sym.ModifyX);
    indicator_sym.SetPositioning(indicator_sym.Absolute);
    indicator_sym.SetPosition(0, 0);
    indicator_sym.SetMargin(0, 1, 0, 1);
    indicator_n.AddIndex(4);
    

    auto &tickn = temp.AddGraphics(7, UI::ComponentCondition::Always);
    tickn.Content.SetDrawable(tick);
    tickn.SetSize({2, UI::Dimension::Pixel}, {100, UI::Dimension::Percent});
    tickn.SetMargin(9, 2, 9, 2);
    tickn.SetRepeatMode(tickn.XTick);
    tickn.SetPositioning(tickn.Absolute);
    tickn.SetValueModification(tickn.ModifyX);
    indicator_n.AddIndex(7);

    
    auto &btnright_n = temp.AddContainer(5, UI::ComponentCondition::Always);
    btnright_n.Background.SetDrawable(btn_bg);
    btnright_n.SetSize({20, UI::Dimension::Pixel}, {100, UI::Dimension::Percent});
    btnright_n.SetMargin(1, 0, 0, 0);
    outer.AddIndex(5);
    
    auto &btn_right_trig = temp.AddGraphics(6, UI::ComponentCondition::Always);
    btn_right_trig.Content.SetDrawable(trig2);
    btn_right_trig.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
    btnright_n.AddIndex(6);
    

    UI::ComponentStack stack(temp, {300, 20});
    stack.HandleMouse();
    
    for(auto i = 0; i<=10; i++) {
        stack.AddRepeat(tickn.XTick, i/10.f);
    }
    
    float v1 = 0;
    
    app.wind.Add(stack);
        
    bool hover = false;
    app.wind.KeyEvent.Register([&](Input::Key key, bool state) {
        namespace Keycodes = Input::Keyboard::Keycodes;
        if(key == Keycodes::D && state) {
            if(hover)
                stack.RemoveCondition(UI::ComponentCondition::Disabled);
            else
                stack.AddCondition(UI::ComponentCondition::Normal__Disabled);
            
            hover = !hover;
            
            return true;
        }
		else if(key == Keycodes::Number_1 && state) {
			v1 -= 0.1f;
			stack.SetValue(v1);
		}
		else if(key == Keycodes::Number_2 && state) {
			v1 += 0.1f;
			stack.SetValue(v1);
		}

        return false;
    });
    

	while(true) {
		Gorgon::NextFrame();
		
		l.Clear();
        app.sty.Print(l, String::Concat("v1: ", std::round(v1*100), "\n"), 10, 200);
	}

	return 0;
}

