#include "GraphicsHelper.h"

#include <Gorgon/Geometry/PointList.h>
#include <Gorgon/CGI.h>
#include <Gorgon/CGI/Polygon.h>
#include <Gorgon/CGI/Line.h>
#include <Gorgon/Time/Timer.h>


std::string helptext = 
    "Key list:\n"
	"esc\tClose\n"
;

#ifdef NDEBUG
    const int DefaultAA = 8;
#else 
    const int DefaultAA = 4; //Due to non-linear complexity, AA=4 in debug and AA=8 in release are very similar in speed
#endif
    
int main() {
	Application app("generictest", "Test", helptext);

	Graphics::Layer l;
    ((Graphics::Layer&)app.wind.Children[0]).Clear();
    app.wind.Add(l);
    
    Graphics::Bitmap bmp(40, 30, Graphics::ColorMode::RGBA);
    bmp.Clear();

    //{{2, 0}, {5, 0}, {5, 4}, {4, 4}, {4, 3}, {3, 2.5}, {1, 2.5}, {0, 3}}
    //{{0, 0}, {2, 0}, {2, 2}, {7, 3}, {7, 4}, {3, 4}, {3, 3}, {6, 2}, {6, 0}, {8, 0}, {8, 5}, {0, 5}}
    //std::vector<Geometry::PointList<Geometry::Pointf>> points = {{{0, 0}, {2, 0}, {2, 2}, {7, 3}, {7, 4}, {3, 4}, {3, 3}, {6, 2}, {6, 0}, {8, 0}, {8, 5}, {0, 5}}};
    
    Geometry::PointList<Geometry::Pointf> points;
    std::cout<<DefaultAA<<std::endl;
    
    Gorgon::Time::Timer tm;
    tm.Start();
    float maxr=10;
    float st = 0.25;
    float c = 9;
    int step=72;
    for(int i=st*step; i<=step*c; i+=1) {
        float a = Gorgon::PI * (i%step) / step*2;
        float r = maxr * i / step;
        
        points.Push({r * cos(a)+250, r * sin(a)+250});
    }
    
    
	points = {{3, 6}, {7, 10}, {11, 7.8}, {15, 10}, {19, 10}, {23, 10}, {27, 4}};
    //points.Push(points.Front());
    //points += Geometry::Pointf{0,0.1};
    
    std::cout<<"Build time: "<<tm.Tick()<<std::endl;
    
    for(auto &p : points) {
        std::cout<<p.X<<","<<p.Y<<" ";
    }
    std::cout<<std::endl;
    
    //points += Geometry::Point{5, 5};
    //points *= 3;
    
    std::vector<Geometry::PointList<Geometry::Pointf>> v;
    v.push_back(points.Duplicate());
    
    tm.Start();
    //for(int i=0; i<100; i++)
    CGI::DrawLines<8>(bmp.GetData(), points, 2, CGI::SolidFill<>(0xffffffff));
    auto points2 = CGI::LinesToPolygons<8>(points, 2);
    
    std::cout<<"Draw time: "<<tm.Tick()<<std::endl;
    
    auto bmp2 = bmp.ZoomMultiple(25);
    for(auto &p : points) {
        bmp2.SetRGBAAt(p*25, Graphics::Color::Red);
    }
    auto col = Graphics::Color::LightGreen;
    for(auto &p : points2[0]) {
        auto m=Graphics::Color::Mustard;
        m.A = 25;
        col.Blend(m);
        bmp2.SetRGBAAt(p*25, col);
    }
    bmp2.Prepare();
    std::cout<<std::endl;
        for(auto &p : points2[0]) {
            std::cout<<"{"<<round(p.X*10)/10<<","<<round(p.Y*10)/10<<"}, ";
        }
    std::cout<<std::endl;
    
    bmp2.Draw(l, 0,0);
    bmp2.ExportPNG("poly.png");
    
	while(true) {
		Gorgon::NextFrame();
	}
    
	return 0;
}
