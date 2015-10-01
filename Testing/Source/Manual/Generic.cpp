#include <Gorgon/Window.h>
#include <Gorgon/WindowManager.h>
#include <Gorgon/OS.h>
#include <Gorgon/Resource/File.h>
#include <Gorgon/Resource/Image.h>
#include <Gorgon/Resource/Blob.h>
#include <Gorgon/Graphics/Layer.h>

using namespace Gorgon;

int main() {
	WindowManager::Initialize();
	Resource::File f;
	f.LoadFile("../Source/Manual/test.gor");
	
	for(auto mon : WindowManager::Monitor::Monitors()) {
		std::cout<<mon.GetName()<<": "<<mon.GetSize()<<" + "<<mon.GetLocation()<<(mon.IsPrimary() ? "*" : "")<<std::endl;
	}
	
	Window wind({200,240}, "test");
	wind.ClosingEvent.Register([]{exit(0);});

	f.Prepare();
	Graphics::Layer l;
	wind.Add(l);
	f.Root().Get<Resource::Image>(1).Draw(l, 0,0);
	std::cout<<f.Root().Get<Resource::Image>(1).GetSize()<<std::endl;


	//Resource::Image im;
	//im.ImportPNG("../Source/Manual/0.png");
	//im.Prepare();
	//im.Draw(l, 20, 80);
	
	/*Resource::Blob b;
	char ch=32;
	for(auto &c : b.Ready(100)) {
		c=ch++;
	}*/
	
	Resource::File f2;
 	//f2.Root().SetName("abc");
 	//f2.Root().Add(b);
 	//f2.Save("../Source/Manual/test2.gor");
	
// 	f2.LoadFile("../Source/Manual/test2.gor");
// 	f2.Prepare();
// 	f2.Root().Get<Resource::Image>(0).Draw(l, 20, 80);
	
	//std::cout<<f2.Root().Get<Resource::Blob>(0).GetData()<<std::endl;
	
	//Graphics::Layer l2;
	//Window wind2({400,0,300,200}, "test2");
	//wind2.Add(l2);
	//f.Root().Get<Resource::Image>(1).Draw(l2, 0,0);
	
	while(1) {
		wind.Render();
		//wind2.Render();
		OS::processmessages();
	}
	
	return 0;
}
