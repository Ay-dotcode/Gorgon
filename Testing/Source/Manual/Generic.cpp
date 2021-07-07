#include "GraphicsHelper.h"
#include <Gorgon/Graphics/Bitmap.h>


std::string helptext = 
    "Key list:\n"
    "esc\tClose\n"
;

using namespace Gorgon::Graphics;


int main() {
    Application app("generictest", "Test", helptext, 10);
    app.wind.Resize(1000, 850);

    Graphics::Layer layer;
    app.wind.Add(layer);
    
    Bitmap bmp;
    bmp.Import("../../Resources/Logo-large.png");

    bmp = bmp.FlipX();
    bmp.Prepare();

    bmp.Draw(layer, 0,0);
    bmp.Export("test.png");


    while(true) {
        Gorgon::NextFrame();
    }

    return 0;
}
