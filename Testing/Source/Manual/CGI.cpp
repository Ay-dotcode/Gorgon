#include "GraphicsHelper.h"

#include <Gorgon/Geometry/PointList.h>
#include <Gorgon/CGI.h>
#include <Gorgon/CGI/Polygon.h>
#include <Gorgon/CGI/Line.h>
#include <Gorgon/CGI/Circle.h>
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
    int zoom = 20;
	Application app("generictest", "Test", helptext, zoom);

	Graphics::Layer l;
    ((Graphics::Layer&)app.wind.Children[0]).Clear();
    app.wind.Add(l);
    
    Graphics::Bitmap bmp(1000/zoom, 600/zoom, Graphics::ColorMode::RGBA);
    bmp.Clear();

    //{{2, 0}, {5, 0}, {5, 4}, {4, 4}, {4, 3}, {3, 2.5}, {1, 2.5}, {0, 3}}
    //{{0, 0}, {2, 0}, {2, 2}, {7, 3}, {7, 4}, {3, 4}, {3, 3}, {6, 2}, {6, 0}, {8, 0}, {8, 5}, {0, 5}}
    //std::vector<Geometry::PointList<Geometry::Pointf>> points = {{{0, 0}, {2, 0}, {2, 2}, {7, 3}, {7, 4}, {3, 4}, {3, 3}, {6, 2}, {6, 0}, {8, 0}, {8, 5}, {0, 5}}};
    
    //Geometry::PointList<Geometry::Pointf> points;
    std::cout<<DefaultAA<<std::endl;
    
    Gorgon::Time::Timer tm;
    tm.Start();
    /*float maxr=10;
    float st = 0.25;
    float c = 9;
    int step=72;
    for(int i=st*step; i<=step*c; i+=1) {
        float a = Gorgon::PI * (i%step) / step*2;
        float r = maxr * i / step;
        
        points.Push({r * cos(a)+250, r * sin(a)+250});
    }*/
    
    
	/*std::vector<Geometry::PointList<Geometry::Pointf>> points;
    points.push_back({{301.452,285.09}, {301.558,284.985}, {301.523,285.055}});
    points.push_back({{295.091,285.09}, {294.775,285.266}, {294.318,285.301}, {295.091,284.774}, {295.162,284.985}, {295.091,285.09}});*/
    
    /*for(auto &pl : points) {
        for(auto &p : pl) {
            p -= Geometry::Pointf{290, 280};
        }
    }*/
    /*points[0].Push(points[0].Front());
    points[0] -= Geometry::Pointf{15,15};
    //points[0] *= 0.4;
    points[1] -= Geometry::Pointf{15,15};*/
    //points[1] *= 0.4;
    
    //std::cout<<"Build time: "<<tm.Tick()<<std::endl;
    /*
    for(auto &p : points) {
        std::cout<<p.X<<","<<p.Y<<" ";
    }
    std::cout<<std::endl;
    
    //points += Geometry::Point{5, 5};
    //points *= 3;
    */
    std::vector<Geometry::PointList<Geometry::Pointf>> v;
    /*v.push_back(points.Duplicate());
    
    tm.Start();
    //for(int i=0; i<100; i++)*/
    //for(auto &p : points)
    /*CGI::DrawLines(bmp.GetData(), points, 1, CGI::SolidFill<>(0xffffffff));
    auto points2 = CGI::LinesToPolygons(points[1], 1);*/
    /*
    CGI::Circle<1>(bmp.GetData(), {30.f, 10.f}, 7, 2, CGI::SolidFill<>(Graphics::Color::ArmyGreen));
    
    CGI::Circle<>(bmp.GetData(), {30.f, 30.f}, 7, 2, CGI::SolidFill<>(Graphics::Color::White));
    
    Geometry::PointList<> cp;
    for(int i=0; i<21; i++) {
        Geometry::Pointf p(37.5, 10);
        Rotate(p, i*Gorgon::PI/10.f, {30.5f, 10.f});
        
        cp.Push(p);
    }
    
    Geometry::PointList<> cpo;
    for(int i=0; i<21; i++) {
        Geometry::Pointf p(39.5, 10);
        Rotate(p, i*Gorgon::PI/10.f, {30.5f, 10.f});
        
        cpo.Push(p);
    }
    
    
    */
    
    std::vector<Geometry::PointList<Geometry::Pointf>> points;
    points.push_back({{5,5.7}, {5.6,4.8}, {4,4.9}});
    points.push_back({{11.8,5.4},  {11.1,4.7}}); 
    for(auto &pl : points) {
        for(auto &p : pl) {
            p = (p-Geometry::Pointf(4, 4)) * 4;
        }
    }
    
    CGI::Polyfill<1>(bmp.GetData(), points, CGI::SolidFill<>(0xffffffff));
   /* std::cout<<"Draw time: "<<tm.Tick()<<std::endl;
    */
   auto bmp2 = bmp.ZoomMultiple(zoom);
   
   CGI::DrawLines(bmp2, points[0]*zoom, 1, CGI::SolidFill<>(0xa0ffffff));
   CGI::DrawLines(bmp2, points[1]*zoom, 1, CGI::SolidFill<>(0xa0ffffff));
   //CGI::DrawLines(bmp2, points[2]*zoom, 1, CGI::SolidFill<>(0xa0ffffff));
   /*CGI::DrawLines(bmp2, cpo*zoom, 2, CGI::SolidFill<>(0xa0ffffff));*/
   
   
    for(auto &pts : points)
    for(auto &p : pts) {
        bmp2.SetRGBAAt(p*zoom, Graphics::Color::Red);
        bmp2.SetRGBAAt(p*zoom+Geometry::Point{1,1}, Graphics::Color::Red);
        bmp2.SetRGBAAt(p*zoom+Geometry::Point{1,0}, Graphics::Color::Red);
        bmp2.SetRGBAAt(p*zoom+Geometry::Point{0,1}, Graphics::Color::Red);
    }
    
    /*for(auto &pts : points2) {
        for(auto &p : pts) {
            bmp2.SetRGBAAt(p*zoom, Graphics::Color::Green);
            bmp2.SetRGBAAt(p*zoom+Geometry::Point{1,1}, Graphics::Color::Green);
            bmp2.SetRGBAAt(p*zoom+Geometry::Point{1,0}, Graphics::Color::Green);
            bmp2.SetRGBAAt(p*zoom+Geometry::Point{0,1}, Graphics::Color::Green);
        }
        pts *= zoom;
    }*/
    
    //CGI::Polyfill(bmp2.GetData(), points2, CGI::SolidFill<>(Graphics::Color::Blue));
    /*for(auto &p : points) {
        bmp2.SetRGBAAt(p*zoom, Graphics::Color::Red);
    }
    auto col = Graphics::Color::LightGreen;
    for(auto &p : points2[0]) {
        auto m=Graphics::Color::Mustard;
        m.A = 25;
        col.Blend(m);
        bmp2.SetRGBAAt(p*zoom, col);
    }*/
    bmp2.Prepare();
    std::cout<<std::endl;
        /*for(auto &p : points2[0]) {
            std::cout<<"{"<<round(p.X*10)/10<<","<<round(p.Y*10)/10<<"}, ";
        }*/
    std::cout<<std::endl;
    
    bmp2.Draw(l, 0,0);
    bmp2.ExportPNG("poly.png");
    
	while(true) {
		Gorgon::NextFrame();
	}
    
	return 0;
}
