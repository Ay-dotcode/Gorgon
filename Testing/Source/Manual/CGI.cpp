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
    
    Graphics::Bitmap bmp(800, 500, Graphics::ColorMode::Alpha);
    bmp.Clear();

    //{{2, 0}, {5, 0}, {5, 4}, {4, 4}, {4, 3}, {3, 2.5}, {1, 2.5}, {0, 3}}
    //{{0, 0}, {2, 0}, {2, 2}, {7, 3}, {7, 4}, {3, 4}, {3, 3}, {6, 2}, {6, 0}, {8, 0}, {8, 5}, {0, 5}}
    //std::vector<Geometry::PointList<Geometry::Pointf>> points = {{{0, 0}, {2, 0}, {2, 2}, {7, 3}, {7, 4}, {3, 4}, {3, 3}, {6, 2}, {6, 0}, {8, 0}, {8, 5}, {0, 5}}};
    
    Geometry::PointList<Geometry::Pointf> points;
    std::cout<<DefaultAA<<std::endl;
    
    Gorgon::Time::Timer tm;
    tm.Start();
    float maxr=10;
    float st = 0;
    float c = 9;
    int step=72;
    for(int i=st*step; i<=step*c; i+=1) {
        float a = Gorgon::PI * (i%step) / step*2;
        float r = maxr * i / step;
        
        points.Push({r * cos(a)+250, r * sin(a)+250});
    }
    
    points = {{20,50}, {50, 50}, {50, 20}};
    
    std::cout<<"Build time: "<<tm.Tick()<<std::endl;
    
    for(auto &p : points) {
        std::cout<<p.X<<","<<p.Y<<" ";
    }
    std::cout<<std::endl;
    
    //points += Geometry::Point{5, 5};
    //points *= 1;
    
    
    tm.Start();
    //for(int i=0; i<100; i++)
    CGI::DrawLines<DefaultAA>(bmp.GetData(), points, 14.0f, CGI::SolidFill<>(0x80ffffff));
    
    std::cout<<"Draw time: "<<tm.Tick()<<std::endl;
    
    auto bmp2 = bmp.ZoomMultiple(1);
    bmp2.Prepare();
    
    bmp2.Draw(l, 0,0, Gorgon::Graphics::Color::CanaryYellow);
    bmp2.ExportPNG("poly.png");
    
	while(true) {
		Gorgon::NextFrame();
	}
    
	return 0;
}
