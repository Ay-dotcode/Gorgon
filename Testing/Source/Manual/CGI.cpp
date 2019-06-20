#include "GraphicsHelper.h"

#include <Gorgon/Geometry/PointList.h>
#include <Gorgon/CGI.h>
#include <Gorgon/CGI/Polygon.h>
#include <Gorgon/Time/Timer.h>


std::string helptext = 
    "Key list:\n"
	"esc\tClose\n"
;


int main() {
	Application app("generictest", "Test", helptext);

	Graphics::Layer l;
    ((Graphics::Layer&)app.wind.Children[0]).Clear();
    app.wind.Add(l);
    
    Graphics::Bitmap bmp(400, 400, Graphics::ColorMode::Alpha);
    bmp.Clear();

    //{{2, 0}, {5, 0}, {5, 4}, {4, 4}, {4, 3}, {3, 2.5}, {1, 2.5}, {0, 3}}
    //{{0, 0}, {2, 0}, {2, 2}, {7, 3}, {7, 4}, {3, 4}, {3, 3}, {6, 2}, {6, 0}, {8, 0}, {8, 5}, {0, 5}}
    //std::vector<Geometry::PointList<Geometry::Pointf>> points = {{{0, 0}, {2, 0}, {2, 2}, {7, 3}, {7, 4}, {3, 4}, {3, 3}, {6, 2}, {6, 0}, {8, 0}, {8, 5}, {0, 5}}};
    
    std::vector<Geometry::PointList<Geometry::Pointf>> points;
    points.push_back({  });
    
    Gorgon::Time::Timer tm;
    tm.Start();
    float s=10;
    float st = 0;
    int c = 10;
    for(int i=st*36; i<=36*c; i+=1) {
        float a = Gorgon::PI * (i%36) / 18;
        float r = s * i / 36;
        
        points.back().Push({r * cos(a)+200, r * sin(a)+200});
    }
    
    for(int i=36*c; i>=st*36; i-=1) {
        float a = Gorgon::PI * (i%36) / 18;
        float r = s * i / 36 + 5;
        
        points.back().Push({r * cos(a)+200, r * sin(a)+200});
    }
    std::cout<<"Build time: "<<tm.Tick()<<std::endl;
    
    for(auto &p : points[0]) {
        std::cout<<p.X<<","<<p.Y<<" ";
    }
    std::cout<<std::endl;
    
    //points += Geometry::Point{5, 5};
    //points *= 1;
    
    tm.Start();
    CGI::Polyfill<16>(bmp.GetData(), points);
    std::cout<<"Draw time: "<<tm.Tick()<<std::endl;
    
    auto bmp2 = bmp.ZoomMultiple(1);
    bmp2.Prepare();
    
    bmp2.Draw(l, 0,0);
    
    
	while(true) {
		Gorgon::NextFrame();
	}
    
	return 0;
}
