#include <thread>

#include "Main.h"

#include "Filesystem.h"
#include "WindowManager.h"
#include "Window.h"
#include "OS.h"
#include "Time.h"
#include "Animation.h"
#include "Resource.h"
#include "Audio.h"
#include "Graphics.h"

#ifdef SCRIPTING
#	include "Scripting.h"
#endif


/**
 * @page GDB Pretty Printing
 * For GDB pretty printing to work with Gorgon objects
 * simply copy Resource/Gorgon-gdb.py file to the startup
 * directory of your program.
 */

#ifdef __GNUC__
//For GDB pretty printing
#define DEFINE_GDB_SCRIPT(name) \
asm("\
    .pushsection \".debug_gdb_scripts\", \"MS\",@progbits,1\n\
    .byte 1\n\
    .asciz \"" name "\"\n\
    .popsection \n\
");

DEFINE_GDB_SCRIPT ("Gorgon-gdb.py")
#endif

namespace Gorgon {
	
	Event<> BeforeFrameEvent;
    
    std::vector<std::function<void()>> once;

	bool exiting = false;

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
#ifdef AUDIO
		Audio::Initialize();
#endif
		Resource::Initialize();
	}

	void Tick() {
		static bool inited=false;
		auto ctime=Time::GetTime();
		if(!inited) {
			Time::internal::framestart=ctime;
			inited=true;
		}

		Time::internal::deltatime=ctime-Time::internal::framestart;
		Time::internal::framestart=ctime;

		Animation::Animate();
        
        for(auto &fn : once) {
            fn();
        }
        
        once.clear();

		OS::processmessages();
	}

	void Render() {
		for(auto &w : Window::Windows) {
			w.Render();
		}
	}

	void NextFrame() {
		Render();

		auto ctime=Time::GetTime();
		auto currentdelta=ctime-Time::FrameStart();
        
		if(currentdelta<16) {
			std::this_thread::sleep_for(std::chrono::duration<unsigned long, std::milli>(15-currentdelta));
		}

		Tick();
		
		BeforeFrameEvent();

		OS::processmessages();
	}
	
	void UpdateFrame() {
		Render();
		
		Tick();
		
		BeforeFrameEvent();

		OS::processmessages();
	}
	
	void RegisterOnce(std::function<void()> fn) {
        once.push_back(std::move(fn));
    }
}
