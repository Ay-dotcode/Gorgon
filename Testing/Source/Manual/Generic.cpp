#include "GraphicsHelper.h"



std::string helptext = 
    "Key list:\n"
	"esc\tClose\n"
;


int main() {
	Application app("generictest", "Test", helptext);

	Graphics::Layer l;
    app.wind.Add(l);

    auto obj1 = Circle(15);
    obj1.Prepare();

    auto obj2 = Rectangle(30,30);
    obj2.Prepare();

    Graphics::BitmapAnimationProvider anim;
    anim.Add(obj1, 500);
    anim.Add(obj2, 500);
    auto &a = anim.CreateAnimation();
    
    Geometry::Pointf p(0,0);


	while(true) {
        p+={1,0};
        
        l.Clear();
        
        a.Draw(l, p, 0xff000080);

		Gorgon::NextFrame();
	}

	return 0;
}
