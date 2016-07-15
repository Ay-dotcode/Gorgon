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
#include <Gorgon/Audio/Controllers.h>

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
	//Audio::Log.InitializeConsole();
	Initialize("Generic-Test");
	system("pwd");

	auto &devices = Audio::Device::Devices();
	for(auto dev : devices) {
		std::cout<<dev.GetName()<<std::endl;
	}
	
	int freq = 200;
	int rate = 8000;
	float duration = 2;
	float amp = 0.5;
	float pi = 3.1415f;
	
	Containers::Wave wave(duration * rate, rate, {Audio::Channel::LowFreq, Audio::Channel::FrontRight});
	
	int ind = 0;
	for(auto elm : wave) {
		elm[0] = amp*sin(2*pi*ind/(rate/freq));
		elm[1] = amp*sin(4*pi*ind/(rate/freq));
		ind++;
		ind = ind % (rate/freq);
		/*if(ind == 0)
			freq++;*/
	}
	
	Containers::Wave wave2;
    std::cout<<wave2.ImportWav("test.wav")<<std::endl;
	
	
	Audio::BasicController c(wave);
	//c.Loop();
	
	Audio::BasicController c2(wave2);
	c2.Play();
	
	while(1) {
		NextFrame();
        std::this_thread::yield();
	}
	
	return 0;
}
catch(const std::runtime_error &err) {
	std::cout<<"!!!"<<err.what()<<std::endl;
	throw;
}
}
