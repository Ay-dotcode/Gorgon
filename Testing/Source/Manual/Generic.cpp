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
    bmp.Import("../../Resources/Logo-large.png");
    
    auto img = bmp.ReleaseData();
    bmp.Assume(*new Gorgon::Containers::Image(img.Scale({800, 800}, Gorgon::Containers::InterpolationMethod::Cubic)));
    
    bmp.Prepare();
    
    bmp.Draw(layer, 0,0);
    bmp.Export("test.png");
    
    while(true) {
        Gorgon::NextFrame();
    }

    return 0;
}
