#include <Gorgon/Window.h>
#include <Gorgon/WindowManager.h>
#include <Gorgon/OS.h>
#include <Gorgon/Resource/File.h>
#include <Gorgon/Resource/Image.h>
#include <Gorgon/Resource/Blob.h>
#include <Gorgon/Resource/Animation.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Main.h>
#include <Gorgon/Network/HTTP.h>

using namespace Gorgon;

int main() {
try {
	Initialize("Generic-Test");
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
	
	auto im1=new Resource::Image;
	auto im2=new Resource::Image;
	im1->ImportPNG("../Source/Manual/0.png");
	im2->ImportPNG("../Source/Manual/1.png");
	im1->Prepare();
	im2->Prepare();
	Resource::Animation anim;
	anim.Add(*im1, 800);
	anim.Add(*im2, 200);
	
	auto &a=anim.CreateAnimation(true);
	a.Draw(l, 0, 80);

	//f2.Root().Add(anim);
	//f2.Save("../Source/Manual/test2.gor");

	
 	f2.LoadFile("../Source/Manual/test2.gor");
 	f2.Prepare();
 	f2.Root().Get<Resource::Animation>(0).CreateAnimation(true).Draw(l, 60, 80);
	
	//std::cout<<f2.Root().Get<Resource::Blob>(0).GetData()<<std::endl;
	
	//Graphics::Layer l2;
	//Window wind2({400,0,300,200}, "test2");
	//wind2.Add(l2);
	//f.Root().Get<Resource::Image>(1).Draw(l2, 0,0);
	
	Network::HTTP::Nonblocking transfer;
	transfer.TextTransferCompletedEvent.Register([](const std::string &str) {
		std::cout<<str<<std::endl;
	});
	transfer.GetText("http://darkgaze.org/");
	
	while(1) {
		NextFrame();
	}
	
	return 0;
}
catch(const std::runtime_error &err) {
	std::cout<<"!!!"<<err.what()<<std::endl;
	throw;
}
}