#include "GraphicsHelper.h"

#define GORGON_DEFAULT_SUBDIVISIONS 8

#include <Gorgon/Geometry/PointList.h>
#include <Gorgon/CGI.h>
#include <Gorgon/CGI/Polygon.h>
#include <Gorgon/CGI/Line.h>
#include <Gorgon/CGI/Circle.h>
#include <Gorgon/CGI/Bezier.h>
#include <Gorgon/Time/Timer.h>
#include <fenv.h>

std::string helptext = 
    "Key list:\n"
    "esc\tClose\n"
;

int main() {
    using namespace Gorgon::CGI;
    using namespace Gorgon::Graphics::Color;
    
    int zoom = 5;
    Application app("generictest", "Test", helptext, zoom);

    Graphics::Layer l;
    ((Graphics::Layer&)app.wind.Children[0]).Clear();
    app.wind.Add(l);
    
    Graphics::Bitmap bmp(1000/zoom, 600/zoom, Graphics::ColorMode::RGBA);
    bmp.ForAllPixels([&](int x, int y) { bmp.SetRGBAAt(x, y, DarkGrey); });
    
    Curves c;
    c.SetStartingPoint({0, 40});
    c.Push({{0, 40}, {40, 10}, {80, 80}, {100, 60}});
    c.Push({90, 100});
    c.Push({45, 50}, {0, 100});
    auto b = c.Get(0);
    
    for(int i=0; i<c.GetCount(); i++)
        DrawLines(bmp, c[i].Flatten(), 1, SolidFill<>(Cyan));

    feenableexcept(FE_INVALID);

    auto bmp2 = bmp.ZoomMultiple(zoom);
    
    auto markbezier = [&](auto b) {
        Circle(bmp2, b.P0*zoom, 3, SolidFill<>(Green));
        Circle(bmp2, b.P1*zoom, 3, SolidFill<>(Blue));
        Circle(bmp2, b.P2*zoom, 3, SolidFill<>(Blue));
        Circle(bmp2, b.P3*zoom, 3, SolidFill<>(Green));
        
        DrawLines(bmp2, {b.P0*zoom, b.P1*zoom}, 0.75, SolidFill<>(LightBlue));
        DrawLines(bmp2, {b.P2*zoom, b.P3*zoom}, 0.75, SolidFill<>(LightBlue));
    };
    
    
    Circle(bmp2, {45.f*zoom, 50.f*zoom}, 3, SolidFill<>(Magenta));
    Polyfill(bmp2, c.Flatten(0.72), SolidFill<>({DarkGreen, 0.5}));
    DrawLines<8>(bmp2, c.Flatten(0.72), 4, SolidFill<>(White));
    
    std::cout << "Release points: " << c.Flatten(0.72).GetCount() << std::endl;
        
    for(int i=0; i<c.GetCount(); i++) {
    
        std::cout << c[i].Flatten().GetCount() << "\t: ";
        for(auto &p : c[i].Flatten()) {
            std::cout << p << ", ";
            Circle(bmp2, p*zoom, 3, SolidFill<>(White));
        }
        std::cout << std::endl;
        
        markbezier(c[i]);
    }
    
    bmp2.Prepare();
    
    bmp2.Draw(l, 0,0);
    bmp2.ExportPNG("poly.png");
    
    while(true) {
        Gorgon::NextFrame();
    }
    
    return 0;
}
