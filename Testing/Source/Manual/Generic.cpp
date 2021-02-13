#include "GraphicsHelper.h"
#include <Gorgon/Graphics/ColorSpaces.h>


std::string helptext = 
    "Key list:\n"
    "esc\tClose\n"
;

using namespace Gorgon::Graphics;


int main() {
    Application app("generictest", "Test", helptext);

    Graphics::Layer l;
    app.wind.Add(l);
    
    auto c = LChAf(15, 7, 240);
    
    l.Draw(c);
    
    std::cout << RGBA(c) << std::endl;

    while(true) {
        Gorgon::NextFrame();
    }

    return 0;
}
