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
    namespace Color = Gorgon::Graphics::Color;
    using namespace Gorgon::Geometry;
    
    
    feenableexcept(FE_INVALID);
    int zoom = 1;
    Application app("generictest", "Test", helptext, zoom);

    Graphics::Layer l;
    ((Graphics::Layer&)app.wind.Children[0]).Clear();
    app.wind.Add(l);
    
    Graphics::Bitmap bmp(800/zoom, 600/zoom, Graphics::ColorMode::RGBA);
    bmp.ForAllPixels([&](int x, int y) { bmp.SetRGBAAt(x, y, Color::Transparent); });
    
    //PointList<> list = {{3, 2.5}, {3, 3}, {9, 3}, {9, 10}, {0, 10}, {0, 3}, {2, 3}, {2, 7}, {6, 7}, {6, 2.5}};
    //DrawLines<1>(bmp, list * 4+Pointf(4, 4), 2, SolidFill<>(Color::White));
    
    //PointList<> list = {{0, 0}, {1, 0}, {1, 2}, {6, 2}, {6, 5}, {3, 5}, {3, 0}, {4, 0}, {4, 4}, {5, 4}, {5, 3}, {1, 3}, {0, 2}};
    //Polyfill<1, 0>(bmp, list,SolidFill<>(Color::White));
    
    PointList<> list;
    for(int rs=120; rs<6000; rs++) {
        float r = rs/16.f;
        float ang = (fmod(r, 20.f)/10.f) * Gorgon::PI;
        
        list.Push({float(cos(ang) * r + 400), float(sin(ang) * r + 300)});
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    DrawLines<1>(bmp, list, 10, SolidFill<>(Color::White));
    
    double t = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000.0;
    std::cout << t << std::endl;

    auto bmp2 = bmp.ZoomMultiple(zoom);
    
    
    bmp2.Prepare();
    
    bmp2.Draw(l, 0,0);
    bmp2.ExportPNG("poly.png");
    
    while(true) {
        Gorgon::NextFrame();
    }
    
    return 0;
}
