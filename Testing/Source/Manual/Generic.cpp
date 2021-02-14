#include "GraphicsHelper.h"
#include <Gorgon/Graphics/ColorSpaces.h>


std::string helptext = 
    "Key list:\n"
    "esc\tClose\n"
;

using namespace Gorgon::Graphics;


int main() {
    Application app("generictest", "Test", helptext, 10);

    Graphics::Layer layer;
    app.wind.Add(layer);
    
    for(int c=0; c<=125; c+=25) {
        for(int l=0; l<=100; l+=10) {
            for(int h=0; h<=360; h+=15) {
                auto col = LChAf(l, c, h);
                layer.Draw(h, l+c/25*110, 15, 10, col);
                std::cout << col << RGBAf(col) << std::endl;
            }
        }
    }

    std::cout << LChAf(0xff0000ff) << std::endl;
    while(true) {
        Gorgon::NextFrame();
    }

    return 0;
}
