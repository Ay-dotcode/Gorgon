#include <typeinfo>

#include <Gorgon/Window.h>
#include <Gorgon/WindowManager.h>
#include <Gorgon/OS.h>
#include <Gorgon/Resource/File.h>
#include <Gorgon/Resource/Image.h>
#include <Gorgon/Resource/Data.h>
#include <Gorgon/Resource/Blob.h>
#include <Gorgon/Resource/Animation.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Main.h>
#include <Gorgon/Network/HTTP.h>
#include <Gorgon/Struct.h>

using namespace Gorgon;

struct teststruct {
	int a;
	int b;
	
	void f(int c) {}
	
	DefineStructMembers(teststruct, a, b);
};

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
	
	Resource::File f2;
	
	teststruct t;
	Resource::Data d;
	d.Append("a", 5);
	d.Append("b", 2);
	d.Append("c", 2.2f);
	d.Append("d", "texty text");
	d.Append("e", Geometry::Point{3, 7});

	//f2.Root().Add(d);
	//f2.Save("../Source/Manual/test2.gor");
	//f2.Root().Remove(d);

	for(auto &data : d) {
		std::cout<<data.Name<<": "<<data<<std::endl;
	}

	
 	f2.LoadFile("../Source/Manual/test2.gor");
 	//f2.Prepare();

	//std::cout<<"c:"<<f2.Root().Get<Resource::Data>(0).Get<float>(2)<<std::endl;

 	//f2.Root().Get<Resource::Animation>(0).CreateAnimation(true).Draw(l, 60, 80);
	
	//std::cout<<f2.Root().Get<Resource::Blob>(0).GetData()<<std::endl;
	
	//Graphics::Layer l2;
	//Window wind2({400,0,300,200}, "test2");
	//wind2.Add(l2);
	//f.Root().Get<Resource::Image>(1).Draw(l2, 0,0);
	
	Network::HTTP::Nonblocking transfer;
	transfer.TextTransferCompletedEvent.Register([&transfer](const std::string &str) {
		std::cout<<str<<std::endl;
		transfer.GetFile("http://www.google.com/", "index.html");
	});
	transfer.TransferErrorEvent.Register([&transfer](Network::HTTP::Error err) {
		std::cout<<err.error<<": "<<err.what()<<std::endl;
	});
	//transfer.GetText("http://darkgaze.org/");
	
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
