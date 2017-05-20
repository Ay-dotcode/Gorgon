#include "GraphicsHelper.h"
#include <Gorgon/Graphics/EmptyImage.h>
#include <Gorgon/Input/Layer.h>
#include <Gorgon/Input/DnD.h>
#include <Gorgon/Resource/GID.h>


void Init();

std::string helptext = 
    "Key list:\n"
	"esc\tClose\n"
;

namespace Input = Gorgon::Input;
namespace GID = Gorgon::Resource::GID;

int main() {
	Application app("generictest", "Test", helptext);

	Graphics::Layer l;
    app.wind.Add(l);


	auto bmp = Pattern(2);
	bmp.Prepare();

	Graphics::BlankImage bg;

	bg.DrawIn(l, 40, 40, 100, 100, 0xffffa090);
	bmp.DrawIn(l, 40, 40, 100, 100, 0x80000000);

	bg.DrawIn(l, 200, 200, 100, 100, 0xff606060);

	Input::Layer inlayer;
	app.wind.Add(inlayer);
	inlayer.Move({200,200});
	inlayer.Resize(100,100);

	inlayer.SetOver([] {
		std::cout<<"Mouse over."<<std::endl;
	});
	
	inlayer.SetDown([] () {
		Input::BeginDrag("AAa");
	});

	Input::DragStarted.Register([](Input::DragInfo &info) {
		std::cout<<"Drag operation has started with ";
		if(info.HasData(GID::Text)) {
			std::cout<<info.GetData(GID::Text).Text()<<std::endl;
		}
		else if(info.HasData(GID::File)) {
			std::cout<<info.GetData(GID::File).Text()<<std::endl;
		}
		else {
			throw std::logic_error("A problem with data exchange.");
		}
	});

	Input::DragEnded.Register([](Input::DragInfo &info, bool status) {
		std::cout<<"Drag operation has "<<(status ? "successfully finished":"failed")<<" with ";
		if(info.HasData(GID::Text)) {
			std::cout<<info.GetData(GID::Text).Text()<<std::endl;
		}
		else if(info.HasData(GID::File)) {
			std::cout<<info.GetData(GID::File).Text()<<std::endl;
		}
		else {
			throw std::logic_error("A problem with data exchange.");
		}
	});

	Input::DropTarget target;
	app.wind.Add(target);
	target.Move(40,40);
	target.Resize(100,100);

	target.SetOver([](Input::DragInfo &info) {
		std::cout<<"Over"<<std::endl;

		return true;
	});

	target.SetOut([](Input::DragInfo &info) {
		std::cout<<"Out"<<std::endl;
	});

	target.SetDrop([](Input::DragInfo &info) {
		if(info.HasData(GID::Text)) {
			std::cout<<"Accepted: "<<info.GetData(GID::Text).Text()<<std::endl;
		}
		else if(info.HasData(GID::File)) {
			std::cout<<"Accepted: "<<info.GetData(GID::File).Text()<<std::endl;
		}

		return true;
	});

	while(true) {
		Gorgon::NextFrame();
	}

	return 0;
}

