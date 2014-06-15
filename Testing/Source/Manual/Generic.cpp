#include <Source/OS.h>
#include <Source/Filesystem.h>
#include <Source/Resource/File.h>

#include <Source/Resource/Folder.h>
#include <Source/Resource/Blob.h>

#include <Source/Containers/Iterator.h>
#include <Source/Graphics.h>

int main() {
	Gorgon::Resource::File file;

	std::cout<<Gorgon::Filesystem::CurrentDirectory()<<std::endl;
	Gorgon::Filesystem::ChangeDirectory("../Testing/Source/Manual");

	try {
		file.LoadFile("test.gor");
		file.Prepare();

		auto &data=file.Root().Get<Gorgon::Resource::Folder>(0).Get<Gorgon::Resource::Blob>(0).GetData();
		
		std::cout<<std::string((char*)&data[0], data.size());
	}
	catch(const std::exception &ex) {
		std::cout<<ex.what()<<std::endl;
	}

	return 0;
}
