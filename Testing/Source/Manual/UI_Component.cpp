#include "GraphicsHelper.h"

#include <Gorgon/UI/Template.h>
#include <Gorgon/UI/ComponentStack.h>
#include <Gorgon/Graphics/EmptyImage.h>
#include <Gorgon/Graphics/Rectangle.h>
#include <Gorgon/Graphics/TintedObject.h>


std::string helptext = 
    "Key list:\n"
    "h\tHover state\n"
	"esc\tClose\n"
;

int main() {
	Application app("uitest", "UI Component Test", helptext);

	Graphics::Layer l;
    app.wind.Add(l);
    
    Graphics::BlankImage border_col(1, 1, 0xff000000);
    auto border_bg = BGImage(16, 16, 0xa0, 0xd0);
    border_bg.Prepare();
    Graphics::RectangleProvider rect(border_col, border_col, border_col, border_col, border_bg, border_col, border_col, border_col, border_col);
    
    auto circle = Circle(15);
    circle.Prepare();
    Graphics::TintedObjectProvider t(circle, 0xff000000);

    UI::Template temp;
    
    
    auto &outer_normal = temp.AddContainer(0, UI::ComponentCondition::Always);
    
    outer_normal.Background.SetAnimation(rect);
    outer_normal.SetBorderSize(1);
    outer_normal.SetPadding(5);
    
    auto &icon = temp.AddGraphics(1, UI::ComponentCondition::Hover);
    icon.Content.SetAnimation(t);
    icon.SetSize(32, 32);
    
    outer_normal.AddIndex(1);

    UI::ComponentStack stack(temp, {160, 80});
    
    app.wind.Add(stack);
    
    app.wind.KeyEvent.Register([&](Input::Key key, bool state) {
        namespace Keycodes = Input::Keyboard::Keycodes;
        if(key == Keycodes::H) {
            stack.AddCondition(UI::ComponentCondition::Hover);
            return true;
        }
        
        return false;
    });


	while(true) {
		Gorgon::NextFrame();
	}

	return 0;
}

