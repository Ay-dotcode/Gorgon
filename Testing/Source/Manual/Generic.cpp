#include <Source/OS.h>
#include <Source/Filesystem.h>
#include <Source/Resource/File.h>

#include <Source/Resource/Folder.h>
#include <Source/Resource/Blob.h>
#include <Source/Resource/Image.h>

#include <Source/Containers/Iterator.h>
#include <Source/Graphics.h>
#include <Source/Containers/Image.h>
#include "Source/Window.h"

int main() {
	Gorgon::Containers::Image img({100, 100}, Gorgon::Graphics::ColorMode::RGBA);
	
	Gorgon::Resource::File file;

	std::cout<<Gorgon::Filesystem::CurrentDirectory()<<std::endl;
	Gorgon::Filesystem::ChangeDirectory("../Testing/Source/Manual");

	Gorgon::Window win({400, 300}, "test");
	

	try {
		file.LoadFile("test.gor");
		file.Prepare();

		auto &data=file.Root().Get<Gorgon::Resource::Folder>(0).Get<Gorgon::Resource::Blob>(0).GetData();
		auto &image=file.Root().Get<Gorgon::Resource::Image>(1).CreateAnimation(true);

		std::cout<<image.GetSize()<<std::endl;
		std::cout<<image.ReleaseTexture().GetID()<<std::endl;

		std::cout<<std::string((char*)&data[0], data.size());
	}
	catch(const std::exception &ex) {
		std::cout<<ex.what()<<std::endl;
	}

	return 0;
}
