#include <Gorgon/Main.h>
#include <Gorgon/Window.h>
#include <Gorgon/Graphics/TextureAnimation.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Resource/File.h>
#include <Gorgon/Resource/Animation.h>

using Gorgon::Window;
using Gorgon::Geometry::Point;
namespace Graphics = Gorgon::Graphics;
namespace Resource = Gorgon::Resource;

int main() {
    Gorgon::Initialize("anim-test");
        
	Window wind({800, 600}, "animtest", "Animation test", true);
	Graphics::Initialize();

	Graphics::Layer l;
	wind.Add(l);

	wind.DestroyedEvent.Register([]{
		exit(0);
	});

	Graphics::Bitmap img3({50, 50}, Graphics::ColorMode::Grayscale);

	for(int x = 0; x<img3.GetWidth(); x++)
		for(int y = 0; y<img3.GetHeight(); y++) {
			if((x/(img3.GetWidth()/2)) != (y/(img3.GetHeight()/2)))
				img3({x, y}, 0) = 0x10;
			else
				img3({x, y}, 0) = 0x30;
		}

	img3.Prepare();
	img3.DrawIn(l);

        
    Graphics::Bitmap anim1({30, 30}, Graphics::ColorMode::Alpha), anim2({30, 30}, Graphics::ColorMode::Alpha);
    
    anim1.Clear();
    anim2.Clear();
    
    for(int i = 0; i<30; i++) {
        anim1(i,i,0) = 255;
        anim2(i, 30-i-1, 0) = 255;
    }
    anim1.Prepare();
    anim2.Prepare();
    
    Graphics::BitmapAnimationProvider animprov;
    animprov.Add(anim1, 1000);
    animprov.Add(anim2, 1000);
    
	Resource::Animation *aa = new Resource::Animation(animprov.Duplicate());
	Graphics::ConstBitmapAnimationProvider cbp = aa->MoveOutAsBitmap();

	std::cout<<"!..."<<animprov.GetDuration()<<std::endl;
	std::cout<<"!..."<<cbp.GetDuration()<<std::endl;

    const Graphics::RectangularAnimation &anim = cbp.CreateAnimation(true);
    
    anim.Draw(l, 100, 100);

    
	while(true) {
		Gorgon::NextFrame();
	}
}
