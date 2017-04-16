#include "GraphicsHelper.h"

#include <Gorgon/UI/Components.h>


void Init();

std::string helptext = 
    "Key list:\n"
	"esc\tClose\n"
;


int main() {
	Application app("generictest", "Test", helptext);

	Graphics::Layer l;
    app.wind.Add(l);




	while(true) {
		Gorgon::NextFrame();
	}

	return 0;
}


void Init() {

}
