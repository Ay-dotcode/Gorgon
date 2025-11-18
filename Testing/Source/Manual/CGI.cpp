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
    int zoom = 20;
    Application app("generictest", "Test", helptext, zoom, 0x10, {1000, 850});

    Graphics::Layer l;
    ((Graphics::Layer&)app.wind.Children[0]).Clear();
    app.wind.Add(l);
    
    Graphics::Bitmap bmp(1000/zoom, 850/zoom, Graphics::ColorMode::RGBA);
    bmp.ForAllPixels([&](int x, int y) { bmp.SetRGBAAt(x, y, Color::Transparent); });
    
    //PointList<> list = {{3, 2.5}, {3, 3}, {9, 3}, {9, 10}, {0, 10}, {0, 3}, {2, 3}, {2, 7}, {6, 7}, {6, 2.5}};
    //DrawLines<1>(bmp, list * 4+Pointf(4, 4), 2, SolidFill<>(Color::White));
    
    //PointList<> list = {{0, 0}, {1, 0}, {1, 2}, {6, 2}, {6, 5}, {3, 5}, {3, 0}, {4, 0}, {4, 4}, {5, 4}, {5, 3}, {1, 3}, {0, 2}};
    //Polyfill<1, 0>(bmp, list,SolidFill<>(Color::White));

    PointList<> list = {{27, 31},{13, 30},{9, 23},{21, 24},{20, 18},{25, 19},{29, 26},{31, 23},{32, 26},{34, 25},{50, 14},{43, 24},{36, 29},{40, 31},{47, 37},{39, 37},{44, 49},{31, 32},{31, 48},{28, 40},{24, 40},{17, 43},{21, 35},{27, 31}};

    for(auto &p : list) {
        p -= Pointf(9, 14);
    }
    
    /*PointList<> list;
    for(int rs=120; rs<6000; rs++) {
        float r = rs/16.f;
        float ang = (fmod(r, 20.f)/10.f) * Gorgon::PI;
        
        list.Push({float(cos(ang) * r + 400), float(sin(ang) * r + 300)});
    }*/
    
    auto start = std::chrono::high_resolution_clock::now();

    
    //DrawLines<1>(bmp, list, 10, SolidFill<>(Color::White));
    Polyfill(bmp, list, SolidFill<>(Color::White));
    bmp.ForAllValues([](auto &v){
        v = v > 64 ? 255 : 0;
    });

    double t = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000.0;
    std::cout << t << std::endl;

    auto bmp2 = bmp.ZoomMultiple(zoom);

    auto step = 1.0f / list.GetSize();
    auto cur = 0;
    for(auto p : list) {
        bmp2.SetRGBAAt(p*zoom, Graphics::RGBA(Color::Green, Color::Red, cur));
        cur += step;
    }

    DrawLines(bmp2, list*zoom, 2, SolidFill<>(Color::Blue));


    
    bmp2.Prepare();
    
    bmp2.Draw(l, 0,0);
    bmp2.ExportPNG("poly.png");
    
    while(true) {
        Gorgon::NextFrame();
    }
    
    return 0;
}
