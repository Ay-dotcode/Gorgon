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
#include <Gorgon/Containers/Wave.h>
#include <Gorgon/Encoding/URI.h>

using namespace Gorgon; 

constexpr std::chrono::seconds operator ""_sec(unsigned long long s) {
	return std::chrono::seconds(s);
}
constexpr std::chrono::duration<long double> operator ""_sec(long double s) {
	return std::chrono::duration<long double>(s);
}

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
	

	auto &devices = Audio::Device::Devices();
	for(auto dev : devices) {
		std::cout<<dev.GetName()<<std::endl;
	}
	
	Containers::Wave wave(5 * 8000, 8000);
	
	int ind = 0;
	for(auto elm : wave) {
		elm[0] = sin(2*3.14159f*ind/20);
		ind++;
		ind = ind % 20;
	}
	
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
