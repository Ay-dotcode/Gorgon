#include <Gorgon/Window.h>
#include <Gorgon/WindowManager.h>
#include <Gorgon/OS.h>
#include <Gorgon/Resource/File.h>
#include <Gorgon/Resource/Image.h>
#include <Gorgon/Graphics/Layer.h>

using namespace Gorgon;

int main() {
	WindowManager::Initialize();
	Resource::File f;
	f.LoadFile("../Source/Manual/test.gor");
	
	for(auto mon : WindowManager::Monitor::Monitors()) {
		std::cout<<mon.GetName()<<": "<<mon.GetSize()<<" + "<<mon.GetLocation()<<(mon.IsPrimary() ? "*" : "")<<std::endl;
	}
	
	Window wind({105,97}, "test");
	wind.ClosingEvent.Register([]{exit(0);});

	f.Prepare();
	Graphics::Layer l;
	wind.Add(l);
	f.Root().Get<Resource::Image>(1).Draw(l, 0,0);
	std::cout<<f.Root().Get<Resource::Image>(1).GetSize()<<std::endl;

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
