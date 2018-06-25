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
    Graphics::BlankImage btn_bgh(0.0f);
    Graphics::BlankImage tick(0.4f);
    Graphics::BlankImage tickhg(0.6f);

    auto trig = Triangle(6, 10);
    auto trig1 =trig.Rotate90();
    auto trig2 =trig.Rotate270();
    trig1.Prepare();
    trig2.Prepare();
    
    UI::Template lbtn;
	lbtn.SetSize({20, 20});
	auto &btnleft_n = lbtn.AddContainer(0, UI::ComponentCondition::Always);
	btnleft_n.Background.SetDrawable(btn_bg);
	btnleft_n.SetSize({100, UI::Dimension::Percent}, {100, UI::Dimension::Percent});

	lbtn.SetSize({20, 20});
	auto &btnleft_h = lbtn.AddContainer(0, UI::ComponentCondition::Hover);
	btnleft_h.Background.SetDrawable(btn_bgh);
	btnleft_h.SetSize({100, UI::Dimension::Percent}, {100, UI::Dimension::Percent});

	auto &btn_left_trig = lbtn.AddGraphics(1, UI::ComponentCondition::Always);
	btn_left_trig.Content.SetDrawable(trig1);
	btn_left_trig.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
	btnleft_n.AddIndex(1);
	btnleft_h.AddIndex(1);

	UI::Template temp;
	
	auto &outer = temp.AddContainer(0, UI::ComponentCondition::Always);
    outer.Background.SetAnimation(rect);
    outer.SetBorderSize(1);

	auto &lbtn_p = temp.AddPlaceholder(1, UI::ComponentCondition::Always);
	lbtn_p.SetTemplate(lbtn);
	lbtn_p.SetMargin(0, 0, 1, 0);
	lbtn_p.SetSize({20, UI::Dimension::Pixel}, {100, UI::Dimension::Percent});
	lbtn_p.SetTag(UI::ComponentTemplate::DecrementTag);
	outer.AddIndex(1);

    auto &indicator_n = temp.AddContainer(3, UI::ComponentCondition::Always);
    indicator_n.SetSize({100, UI::Dimension::Percent}, {100, UI::Dimension::Percent});
    indicator_n.SetTag(UI::ComponentTemplate::DragBarTag);
    outer.AddIndex(3);

    
    auto &indicator_sym = temp.AddGraphics(4, UI::ComponentCondition::Always);
    indicator_sym.Content.SetDrawable(btn_bg);
    indicator_sym.SetSize({20, UI::Dimension::Pixel}, {100, UI::Dimension::Percent});
    indicator_sym.SetValueModification(indicator_sym.ModifyX);
    indicator_sym.SetPositioning(indicator_sym.Absolute);
    indicator_sym.SetPosition(0, 0);
    indicator_sym.SetMargin(0, 1, 0, 1);
    indicator_sym.SetCenter(50, 50, UI::Dimension::Percent);
    indicator_sym.SetTag(UI::ComponentTemplate::DragTag);
    indicator_n.AddIndex(4);
    
    auto &indicatorh_sym = temp.AddGraphics(4, UI::ComponentCondition::Hover);
    indicatorh_sym.Content.SetDrawable(btn_bgh);
    indicatorh_sym.SetSize({20, UI::Dimension::Pixel}, {100, UI::Dimension::Percent});
    indicatorh_sym.SetValueModification(indicatorh_sym.ModifyX);
    indicatorh_sym.SetPositioning(indicatorh_sym.Absolute);
    indicatorh_sym.SetTag(UI::ComponentTemplate::DragTag);
    indicatorh_sym.SetCenter(50, 50, UI::Dimension::Percent);
    indicatorh_sym.SetPosition(0, 0);
    indicatorh_sym.SetMargin(0, 1, 0, 1);
    

    auto &tickn = temp.AddGraphics(7, UI::ComponentCondition::Always);
    tickn.Content.SetDrawable(tick);
    tickn.SetSize({2, UI::Dimension::Pixel}, {100, UI::Dimension::Percent});
    tickn.SetMargin(9, 2, 9, 2);
    tickn.SetRepeatMode(tickn.XTick);
    tickn.SetPositioning(tickn.Absolute);
    tickn.SetValueModification(tickn.ModifyX);
    indicator_n.AddIndex(7);

    /*auto &tickh = temp.AddGraphics(7, UI::ComponentCondition::Hover);
    tickh.Content.SetDrawable(tickhg);
    tickh.SetSize({2, UI::Dimension::Pixel}, {100, UI::Dimension::Percent});
    tickh.SetMargin(9, 2, 9, 2);
    tickh.SetRepeatMode(tickh.XTick);
    tickh.SetPositioning(tickh.Absolute);
    tickh.SetValueModification(tickh.ModifyX);*/

    
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
    
    
    std::array<float, 4> vs = {{0, 0, 0, 0}};
    auto &stinlay = stack.GetInputLayer(UI::ComponentTemplate::DragBarTag);
    stinlay.SetMove([&](Geometry::Point pnt) {
        vs = stack.CoordinateToValue(UI::ComponentTemplate::DragTag, pnt);
    });
    stinlay.SetClick([&]() {
        FitInto(vs[0], 0.f, 1.f);
        stack.SetValue(vs[0]);
    });
    
    app.wind.Add(stack);
    
    bool hover = false;
    app.wind.KeyEvent.Register([&](Input::Key key, bool state) {
        namespace Keycodes = Input::Keyboard::Keycodes;
        if(key == Keycodes::D && state) {
            /*if(hover)
                stack.RemoveCondition(UI::ComponentCondition::Disabled);
            else
                stack.AddCondition(UI::ComponentCondition::Normal__Disabled);
            */
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
        app.sty.Print(l, String::Concat("v1: ", std::round(v1*100), "\n", "p: ", std::round(vs[0]*100)), 10, 200);
	}

	return 0;
}

