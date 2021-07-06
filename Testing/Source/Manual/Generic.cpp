#include "GraphicsHelper.h"
#include <Gorgon/Graphics/Bitmap.h>


std::string helptext = 
    "Key list:\n"
    "esc\tClose\n"
;

using namespace Gorgon::Graphics;


int main() {
    Application app("generictest", "Test", helptext, 10);

    Graphics::Layer layer;
    app.wind.Add(layer);
    
    Bitmap bmp;
    bmp.Import("../../Resources/Logo-small.png");

    bmp = bmp.Rotate(Gorgon::Angle(45));
    bmp.Prepare();

    bmp.Draw(layer, 0,0);
    bmp.Export("test.png");


    while(true) {
        Gorgon::NextFrame();
    }

    return 0;
}
