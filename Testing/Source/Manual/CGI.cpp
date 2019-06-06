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
    app.wind.Add(l);
    
    Graphics::Bitmap bmp(20, 20, Graphics::ColorMode::Alpha);
    bmp.Clear();

    Geometry::PointList<Geometry::Pointf> points = {{0, 1}, {1, 1}, {1, 4}, {4, 4}, {4, 2}, {5, 2}, {5, 4}, {6, 4}, {6, 1}, {3, 1}, {3, 3}, {2, 3},
        {2, 0}, {7, 0}, {7, 5}, {0, 5}
    };
    
    //points += Geometry::Point{5, 5};
    
    CGI::Polyfill(bmp, points);
    
    auto bmp2 = bmp.ZoomMultiple(25);
    bmp2.Prepare();
    
    bmp2.Draw(l, 0,0);
    
    
	while(true) {
		Gorgon::NextFrame();
	}
    
	return 0;
}
