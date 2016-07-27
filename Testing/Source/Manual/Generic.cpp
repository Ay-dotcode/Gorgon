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
#include "Gorgon/Encoding/FLAC.h"

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
    system("pwd");
	Audio::Log.InitializeConsole();
	Initialize("Generic-Test");

	auto &devices = Audio::Device::Devices();
	for(auto dev : devices) {
		std::cout<<dev.GetName()<<std::endl;
	}

	std::cout<<std::endl<<"*** Current device ***"<<std::endl;
	std::cout<<Audio::Current.GetName()<<std::endl;
	std::cout<<"Headphones: "<<Audio::Current.IsHeadphones()<<std::endl;

	std::cout<<std::endl<<"*** Default device ***"<<std::endl;
	std::cout<<Audio::Device::Default().GetName()<<std::endl;

	int freq = 400;
	int rate = 12000;
	float duration = 2;
	float amp = 0.5;
	float pi = 3.1415f;
	
	Containers::Wave wave(int(duration * rate), rate, {Audio::Channel::Mono});
	
	int ind = 0;
	for(auto elm : wave) {
		elm[0] = amp*sin(2*pi*ind/(rate/freq));
		//elm[1] = amp*sin(4*pi*ind/(rate/freq));
		ind++;
		ind = ind % (rate/freq);
		/*if(ind == 0)
			freq++;*/
	}
	
	//Encoding::Flac.Encode(wave, "out.flac", 24);
    
    std::vector<Byte> data;
	std::ifstream ifile("test.flac", std::ios::binary);

    char chr;
    while(ifile.read(&chr, 1)) {
        data.push_back(chr);
        
        if(ifile.eof()) break;
    }
    
	Containers::Wave wave2;// = Encoding::Flac.Decode("test.flac");
    std::cout<<"Load file: "<<wave2.ImportWav("test.wav")<<std::endl;

	//std::ofstream ofile("out.flac", std::ios::binary);
	//Encoding::Flac.Encode(wave2, ofile);
	//ofile.close();
	
	
	Audio::BasicController c(wave);
	//c.Loop();
	
	Audio::PositionalController c2(wave);
    c2.SetVolume(1);
	c2.Loop();
    
    Geometry::Point3Df loc = {5, 0.2f, 0};
    
    c2.Move(loc);
	
	
	while(1) {
		NextFrame();
        loc = loc - Geometry::Point3Df(Time::DeltaTime()/2000.f, 0,0);
        std::cout<<loc.X<<std::endl;
        c2.Move(loc);
        std::this_thread::yield();
	}
	
	return 0;
}
catch(const std::runtime_error &err) {
	std::cout<<"!!!"<<err.what()<<std::endl;
	throw;
}
}
