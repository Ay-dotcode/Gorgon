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
#include <Gorgon/Audio.h>

using namespace Gorgon;

struct teststruct {
	int a;
	int b;
	
	Resource::Image *j;
	
	void f(int c) {}
	
	DefineStructMembers(teststruct, a, b, j);
};

int main() {
try {
	Audio::Log.InitializeConsole();
	Initialize("Generic-Test");


	
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
