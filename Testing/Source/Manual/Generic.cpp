#include <Source/OS.h>
#include <Source/Filesystem.h>
#include <Source/Resource/File.h>

int main() {
	Gorgon::Resource::File file;

	std::cout<<Gorgon::Filesystem::CurrentDirectory()<<std::endl;
	Gorgon::Filesystem::ChangeDirectory("../Testing/Source/Manual");

	try {
		file.LoadFile("test.gor");
		file.Prepare();
	}
	catch(const std::exception &ex) {
		std::cout<<ex.what()<<std::endl;
	}

	return 0;
}
