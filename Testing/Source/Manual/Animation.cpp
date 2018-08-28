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

    Graphics::BitmapAnimationProvider animprov;

    for(int i=0; i<15; i++) {
        Graphics::Bitmap bmp({30,30}, Graphics::ColorMode::Alpha);;
        bmp.Clear();
        for(int y = 0; y<i; y++) {
            for(int x = 0; x<30; x++) {
                bmp(x, y, 0) = 255;
            }
        }
        
        animprov.Add(std::move(bmp), 60);
    }
    
    animprov.Prepare();
    
	Resource::AnimationStorage *aa = new Resource::Animation(animprov.Duplicate());
	auto cbp = Gorgon::Animation::AnimationCast<Graphics::DiscreteAnimationProvider>(aa->MoveOut());

	std::cout<<"!..."<<animprov.GetDuration()<<std::endl;
	std::cout<<"!..."<<cbp.GetDuration()<<std::endl;

    const Graphics::RectangularAnimation &anim = cbp.CreateAnimation(true);
    
    anim.Draw(l, 100, 100);

    
	while(true) {
        l.Clear();
        
        for(int i=0; i<20; i++)
            for(int j=0; j<20; j++)
                anim.Draw(l, 10+35*i, 10+35*j);
        
		Gorgon::NextFrame();
	}
}
