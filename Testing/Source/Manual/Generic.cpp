#include "GraphicsHelper.h"
#include <Gorgon/Graphics/Bitmap.h>

#include <Gorgon/ImageProcessing/Kernel.h>
#include <Gorgon/ImageProcessing/Filters.h>

using namespace Gorgon::ImageProcessing;
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
    bmp.Import("read.png");
    bmp.StripAlpha();

    bmp.Assume(
        Convolution(
            Convolution(bmp.GetData(), Kernel::GaussianFilter(2,Gorgon::Axis::X, 2)),
            Kernel::GaussianFilter(2,Gorgon::Axis::Y, 2)
        )
    );
    bmp.Prepare();

    bmp.Draw(layer, 0,0);
    bmp.Export("test.png");


    while(true) {
        Gorgon::NextFrame();
    }

    return 0;
}
