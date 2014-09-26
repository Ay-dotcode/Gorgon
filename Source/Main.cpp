#include <thread>

#include "Main.h"

#include "Filesystem.h"
#include "WindowManager.h"
#include "OS.h"
#include "Time.h"
#include "Animation.h"

#ifdef SCRIPTING
#	include "Scripting.h"
#endif

namespace Gorgon {

	namespace internal {
		std::string systemname;
	}

	namespace Animation {
		void Animate();
	}

	void Initialize(const std::string &name) {
		internal::systemname=name;
		
		Filesystem::Initialize();
		WindowManager::Initialize();
		
#ifdef SCRIPTING
		Scripting::Initialize();
#endif
	}

	void Tick() {
		auto ctime=Time::GetTime();
		Time::internal::deltatime=ctime-Time::internal::framestart;
		Time::internal::framestart=ctime;

		Animation::Animate();

		OS::processmessages();
	}

	void Render() {

	}

	void NextFrame() {
		Render();

		auto ctime=Time::GetTime();
		auto currentdelta=ctime-Time::FrameStart();
		if(currentdelta<16) {
			std::this_thread::sleep_for(std::chrono::duration<unsigned long, std::milli>(16-currentdelta));
		}

		Tick();
	}
}
