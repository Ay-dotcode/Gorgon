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

    Geometry::PointList<Geometry::Point> points = {{0, 0}, {4, 0}, {4, 4}, {0, 4}};
    
    points += Geometry::Point{5, 5};
    
    CGI::Polyfill(bmp, points);
    
    auto bmp2 = bmp.ZoomMultiple(25);
    bmp2.Prepare();
    
    bmp2.Draw(l, 0,0);
    
    
	while(true) {
		Gorgon::NextFrame();
	}
    
	return 0;
}
