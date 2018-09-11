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
	Graphics::BlankImage btn_bghr({0.8f, 0.f, 0.f, 1.f});
	Graphics::BlankImage tick(0.4f);
	Graphics::BlankImage tick_h(Graphics::RGBAf{0.8f, 0.f, 0.f});

    auto trig = Triangle(6, 10);
    auto trig1 =trig.Rotate90();
    auto trig2 =trig.Rotate270();
    trig1.Prepare();
    trig2.Prepare();
    
    UI::Template lbtn;
	lbtn.SetConditionDuration(UI::ComponentCondition::Always, UI::ComponentCondition::Hover, 250);
	lbtn.SetConditionDuration(UI::ComponentCondition::Hover, UI::ComponentCondition::Always, 250);
	lbtn.SetConditionDuration(UI::ComponentCondition::Always, UI::ComponentCondition::Down, 250);
	lbtn.SetConditionDuration(UI::ComponentCondition::Down, UI::ComponentCondition::Always, 250);
	lbtn.SetSize({20, 20});
    
    
	auto &btnleft_n = lbtn.AddContainer(0, UI::ComponentCondition::Always);
	btnleft_n.Background.SetDrawable(btn_bg);
	btnleft_n.SetSize({100, UI::Dimension::Percent}, {100, UI::Dimension::Percent});

    //this handles normal to hover, hover and hover to normal
	auto &btnleft_n_h = lbtn.AddContainer(2, UI::ComponentCondition::Always, UI::ComponentCondition::Hover);
	btnleft_n_h.Background.SetDrawable(btn_bgh);
    btnleft_n_h.SetValueModification(btnleft_n_h.ModifySize, btnleft_n_h.UseTransition);
    btnleft_n_h.SetPositioning(btnleft_n_h.Absolute);
	btnleft_n_h.SetSize({100, UI::Dimension::Percent}, {100, UI::Dimension::Percent});
	btnleft_n_h.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleRight, UI::Anchor::MiddleRight);
    btnleft_n_h.SetReversible(true);

	/*auto &btnleft_h = lbtn.AddContainer(2, UI::ComponentCondition::Hover);
	btnleft_h.Background.SetDrawable(btn_bghr);
	btnleft_h.SetSize({100, UI::Dimension::Percent}, {100, UI::Dimension::Percent});
	btnleft_h.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleRight, UI::Anchor::MiddleRight);*/
    
	auto &btn_left_trig = lbtn.AddGraphics(1, UI::ComponentCondition::Always);
	btn_left_trig.Content.SetDrawable(trig1);
	btn_left_trig.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);

	auto &btn_left_trig_d = lbtn.AddGraphics(1, UI::ComponentCondition::Always, UI::ComponentCondition::Down);
	btn_left_trig_d.Content.SetDrawable(trig1);
	btn_left_trig_d.SetColor(1.f);
	btn_left_trig_d.SetValueModification(UI::ComponentTemplate::BlendColor, UI::ComponentTemplate::UseTransition);
	btn_left_trig_d.SetTargetColor({1.f, 0.2f, 0.1f});
	btn_left_trig_d.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
	btn_left_trig_d.SetReversible(true);

	btnleft_n.AddIndex(2);
	btnleft_n.AddIndex(1);
	//btnleft_h.AddIndex(1);
    
    UI::Template rbtn;
	rbtn.SetSize({20, 20});
	auto &btnright_n = rbtn.AddContainer(0, UI::ComponentCondition::Always);
	btnright_n.Background.SetDrawable(btn_bg);
	btnright_n.SetSize(100, 100, UI::Dimension::Percent);

	auto &btnright_h = rbtn.AddContainer(0, UI::ComponentCondition::Hover);
	btnright_h.Background.SetDrawable(btn_bgh);
	btnright_h.SetSize({100, UI::Dimension::Percent}, {100, UI::Dimension::Percent});

	auto &btn_right_trig = rbtn.AddGraphics(1, UI::ComponentCondition::Always);
	btn_right_trig.Content.SetDrawable(trig2);
	btn_right_trig.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
	btnright_n.AddIndex(1);
	btnright_h.AddIndex(1);

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
    indicator_sym.SetSize({20, UI::Dimension::Pixel}, {0, UI::Dimension::Pixel});
	indicator_sym.SetSizing(UI::ComponentTemplate::GrowOnly, UI::ComponentTemplate::Fixed);
    indicator_sym.SetValueModification(indicator_sym.ModifyHeight);
    indicator_sym.SetPositioning(indicator_sym.Absolute);
    indicator_sym.SetPosition(0, 0);
    indicator_sym.SetMargin(0, 1, 0, 1);
    indicator_sym.SetCenter(50, 50, UI::Dimension::Percent);
	indicator_sym.SetTag(UI::ComponentTemplate::DragTag);
	indicator_n.AddIndex(4);
    
    auto &indicatorh_sym = temp.AddGraphics(4, UI::ComponentCondition::Hover);
    indicatorh_sym.Content.SetDrawable(btn_bgh);
	indicatorh_sym.SetSize({20, UI::Dimension::Pixel}, {100, UI::Dimension::Percent});
	indicatorh_sym.SetSizing(UI::ComponentTemplate::GrowOnly, UI::ComponentTemplate::Fixed);
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

	auto &tickh = temp.AddGraphics(7, UI::ComponentCondition::Hover);
	tickh.Content.SetDrawable(tick_h);
	tickh.SetSize({2, UI::Dimension::Pixel}, {100, UI::Dimension::Percent});
	tickh.SetMargin(9, 2, 9, 7);
	tickh.SetRepeatMode(tickh.XTick);
	tickh.SetPositioning(tickh.Absolute);
	tickh.SetValueModification(tickh.ModifyX);

    auto styc = app.sty;
    styc.AlignCenter();
	auto &tickt = temp.AddTextholder(8, UI::ComponentCondition::Always);
	tickt.SetRenderer(styc);
    tickt.SetDataEffect(tickt.ValueText1);
	tickt.SetSize({16, UI::Dimension::Pixel}, {100, UI::Dimension::Percent});
	tickt.SetMargin(1, 0, 2, 7);
	tickt.SetRepeatMode(tickt.XTick);
	tickt.SetPositioning(tickt.Absolute);
	tickt.SetValueModification(tickt.ModifyX);
	indicator_n.AddIndex(8);
    
	auto &rbtn_p = temp.AddPlaceholder(5, UI::ComponentCondition::Always);
	rbtn_p.SetTemplate(rbtn);
	rbtn_p.SetMargin(1, 0, 0, 0);
	rbtn_p.SetSize({20, UI::Dimension::Pixel}, {100, UI::Dimension::Percent});
	rbtn_p.SetTag(UI::ComponentTemplate::IncrementTag);
	outer.AddIndex(5);
    
    UI::ComponentStack button(lbtn, {30, 30});
	button.HandleMouse();
    button.Move(0, 100);
    app.wind.Add(button);


    UI::ComponentStack stack(temp, {300, 24});
    stack.HandleMouse();
    
    for(auto i = 0; i<=10; i++) {
        stack.AddRepeat(tickn.XTick, i/10.f);
    }
    
    stack.SetValueToText([](auto, auto, auto val) {
        return std::to_string((int)std::round(val[0]*10));
    });

	stack.SetValueTransitionSpeed({{2, 0, 0, 0}});
    
    float v1 = 0;
    
    std::array<float, 4> vs = {{0, 0, 0, 0}};

	stack.SetMouseMoveEvent([&](UI::ComponentTemplate::Tag tag, Geometry::Point pnt) {
		if(tag == UI::ComponentTemplate::NoTag) {
			int ind = stack.ComponentAt(pnt);

			if(stack.ComponentExists(ind))
				tag = stack.GetTemplate(ind).GetTag();
		}

		vs = stack.CoordinateToValue(UI::ComponentTemplate::DragTag, pnt);
		FitInto(vs[0], 0.f, 1.f);
		vs[0] = round(vs[0]*20)/20;

		stack.RemoveAllConditionsOf(tickn.XTick);
		if(tag == UI::ComponentTemplate::DragBarTag) {
			int v = int(std::round(vs[0] * 20));
			if(v%2) {
				stack.SetConditionOf(tickn.XTick, v/2, UI::ComponentCondition::Hover);
				stack.SetConditionOf(tickn.XTick, v/2+1, UI::ComponentCondition::Hover);
			}
			else
				stack.SetConditionOf(tickn.XTick, v/2, UI::ComponentCondition::Hover);
		}
	});

	stack.SetMouseOutEvent([&](UI::ComponentTemplate::Tag tag) {
		stack.RemoveAllConditionsOf(tickn.XTick);
	});

    stack.SetClickEvent([&](UI::ComponentTemplate::Tag tag, Geometry::Point location, Input::Mouse::Button btn) {
		std::cout<<"Click at "<<location<<std::endl;

        if(btn != Input::Mouse::Button::Left) 
            return;

		if(tag == UI::ComponentTemplate::NoTag) {
			int ind = stack.ComponentAt(location);

			if(stack.ComponentExists(ind))
				tag = stack.GetTemplate(ind).GetTag();
		}
        
		if(tag == UI::ComponentTemplate::DragBarTag) {
			vs = stack.CoordinateToValue(UI::ComponentTemplate::DragTag, location);
			FitInto(vs[0], 0.f, 1.f);
			vs[0] = round(vs[0]*20)/20;
			stack.SetValue(vs[0]);
		}
		else if(tag == UI::ComponentTemplate::DecrementTag) {
			float v = stack.GetValue()[0];
			v -= 0.1f;
			FitInto(v, 0.f, 1.f);
			stack.SetValue(v);
		}
		else if(tag == UI::ComponentTemplate::IncrementTag) {
			float v = stack.GetValue()[0];
			v += 0.1f;
			FitInto(v, 0.f, 1.f);
			stack.SetValue(v);
		}
    });
    
    app.wind.Add(stack);
    
    bool hover = false;
    app.wind.KeyEvent.Register([&](Input::Key key, bool state) {
        namespace Keycodes = Input::Keyboard::Keycodes;
        if(key == Keycodes::D && state) {
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

