#include "GraphicsHelper.h"

#include <Gorgon/Scene.h>


void Init();

std::string helptext = 
    "Key list:\n"
	"esc\tClose\n"
;


int main() {
	Application app("scenetest", "Scene Test", helptext);

	Graphics::Layer l;
    app.wind.Add(l);
    
    
    

	return 0;
}


void Init() {

}
