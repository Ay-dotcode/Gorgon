#include "GraphicsHelper.h"

#include <Gorgon/UI/ComponentStack.h>
#include <Gorgon/Widgets/Generator.h>

std::string helptext = 
    "Key list:\n"
    "d\tToggle disabled\n"
    "1-2\tChange Values\n"
	"esc\tClose\n"
;

using namespace Gorgon;

int main() {
	Application app("uitest", "UI Generator Test", helptext, 25, 0x40);

	Graphics::Layer l;
    app.wind.Add(l);

	Widgets::SimpleGenerator gen;

	auto temp = gen.Button();

	UI::ComponentStack st(temp, temp.GetSize());
	app.wind.Add(st);
	st.Move(30, 30);
	st.SetData(UI::ComponentTemplate::Text, "Click me");
	st.HandleMouse();


	while(true) {
		Gorgon::NextFrame();
		
	}

	return 0;
}

