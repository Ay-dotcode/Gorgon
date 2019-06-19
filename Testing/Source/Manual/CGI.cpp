#include "GraphicsHelper.h"

#include <Gorgon/Geometry/PointList.h>
#include <Gorgon/CGI.h>
#include <Gorgon/CGI/Polygon.h>


std::string helptext = 
    "Key list:\n"
	"esc\tClose\n"
;


int main() {
	Application app("generictest", "Test", helptext);

	Graphics::Layer l;
    ((Graphics::Layer&)app.wind.Children[0]).Clear();
    app.wind.Add(l);
    
    Graphics::Bitmap bmp(22, 22, Graphics::ColorMode::Alpha);
    bmp.Clear();

    //{{2, 0}, {5, 0}, {5, 4}, {4, 4}, {4, 3}, {3, 2.5}, {1, 2.5}, {0, 3}}
    //{{0, 0}, {2, 0}, {2, 2}, {7, 3}, {7, 4}, {3, 4}, {3, 3}, {6, 2}, {6, 0}, {8, 0}, {8, 5}, {0, 5}}
    //std::vector<Geometry::PointList<Geometry::Pointf>> points = {{{0, 0}, {2, 0}, {2, 2}, {7, 3}, {7, 4}, {3, 4}, {3, 3}, {6, 2}, {6, 0}, {8, 0}, {8, 5}, {0, 5}}};
    
    std::vector<Geometry::PointList<Geometry::Pointf>> points;
    points.push_back({ {10,0}, {20,20}, {0,15} });
    //points.push_back({ {1,1}, {2,1}, {2,2}, {1,2} });
    
    //points += Geometry::Point{5, 5};
    //points *= 1;
    
    CGI::Polyfill<16>(bmp.GetData(), points);
    
    auto bmp2 = bmp.ZoomMultiple(1);
    bmp2.Prepare();
    
    bmp2.Draw(l, 0,0);
    
    
	while(true) {
		Gorgon::NextFrame();
	}
    
	return 0;
}
