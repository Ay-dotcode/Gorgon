#include "GraphicsHelper.h"

#include <Gorgon/Geometry/PointList.h>
#include <Gorgon/CGI.h>
#include <Gorgon/CGI/Polygon.h>
#include <Gorgon/CGI/Line.h>
#include <Gorgon/CGI/Circle.h>
#include <Gorgon/Time/Timer.h>
#include <fenv.h>


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
    int zoom = 1;
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
    feenableexcept(FE_INVALID);
    std::vector<Geometry::PointList<Geometry::Pointf>> points;
    points.push_back({{114,3.57143 }, {114,3.5714285373688 }, {113,3.7142856121063 }, {112.1428604126,3.7142856121063 }, {111.5714263916,3.7142856121063 }, {111.2857131958,3.5714285373688 }, {111.1428604126,3.2857143878937 }, {111.2857131958,3 }, {111.7142868042,2.7142856121063 }, {112.2857131958,2.2857143878937 }, {113,2 }, {113.7142868042,1.7142857313156 }, {114.4285736084,1.4285714626312 }, {115.1428604126,1.1428571939468 }, {115.8571395874,0.85714286565781 }, {116.4285736084,0.71428573131561 }, {117,0.85714286565781 }, {117.2857131958,1.1428571939468 }, {117.2857131958,1.4285714626312 }, {117,1.7142857313156 }, {116.5714263916,2.2857143878937 }, {115.8571395874,2.8571429252625 }, {115,3.2857143878937 }});
    
    points.push_back({{23.714284896851,3.5714285373688 }, {23.714284896851,3.5714285373688 }, {23.42857170105,3.7142856121063 }, {23,3.7142856121063 }, {22.285715103149,3.7142856121063 }, {21.42857170105,3.5714285373688 }, {20.57142829895,3.4285714626312 }, {19.714284896851,3.2857143878937 }, {18.8571434021,3.1428570747375 }, {18.1428565979,2.8571429252625 }, {17.285715103149,2.7142856121063 }, {16.42857170105,2.5714285373688 }, {15.57142829895,2.4285714626312 }, {14.714285850525,2.2857143878937 }, {13.857142448425,2.1428570747375 }, {13,2 }, {12,2 }, {11,2 }, {10,2 }, {9,2 }, {8,2 }, {7,2 }, {6,2.1428570747375 }, {5,2.2857143878937 }, {4,2.4285714626312 }, {3,2.5714285373688 }, {2.1428570747375,2.5714285373688 }, {1.4285714626312,2.4285714626312 }, {1,2.2857143878937 }, {0.85714286565781,2 }, {1,1.7142857313156 }, {1.4285714626312,1.4285714626312 }, {2.1428570747375,1.1428571939468 }, {3,1 }, {4,1 }, {5,1 }, {6,1 }, {7,1 }, {8,1 }, {9,1 }, {10,1 }, {11,1 }, {12,1 }, {13,1 }, {14,1 }, {15,1 }, {16,1 }, {16.8571434021,1.1428571939468 }, {17.714284896851,1.2857142686844 }, {18.57142829895,1.4285714626312 }, {19.42857170105,1.5714285373688 }, {20.285715103149,1.7142857313156 }, {21,2 }, {21.714284896851,2.2857143878937 }, {22.57142829895,2.4285714626312 }, {23.285715103149,2.7142856121063 }, {23.714284896851,3 }, {23.8571434021,3.2857143878937 }}); 
    
    for(auto &pl : points) {
        for(auto &p : pl) {
            p = (p+Geometry::Pointf(2, 2))/* * 4*/;
        }
    }
    
    CGI::DrawLines(bmp.GetData(), points, 3, CGI::SolidFill<>(0xffffffff));
    //CGI::Polyfill<1>(bmp.GetData(), points, CGI::SolidFill<>(0xffffffff));
   /* std::cout<<"Draw time: "<<tm.Tick()<<std::endl;
    */
   auto bmp2 = bmp.ZoomMultiple(zoom);
   
   /*CGI::DrawLines(bmp2, points[0]*zoom, 1, CGI::SolidFill<>(0xa0ffffff));
   CGI::DrawLines(bmp2, points[1]*zoom, 1, CGI::SolidFill<>(0xa0ffffff));*/
   //CGI::DrawLines(bmp2, points[2]*zoom, 1, CGI::SolidFill<>(0xa0ffffff));
   /*CGI::DrawLines(bmp2, cpo*zoom, 2, CGI::SolidFill<>(0xa0ffffff));*/
   
   /*
    for(auto &pts : points)
    for(auto &p : pts) {
        bmp2.SetRGBAAt(p*zoom, Graphics::Color::Red);
        bmp2.SetRGBAAt(p*zoom+Geometry::Point{1,1}, Graphics::Color::Red);
        bmp2.SetRGBAAt(p*zoom+Geometry::Point{1,0}, Graphics::Color::Red);
        bmp2.SetRGBAAt(p*zoom+Geometry::Point{0,1}, Graphics::Color::Red);
    }
    */
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
