#pragma once

//This utility depends on the game engine

#include <sstream>

#include "../Engine/OS.h"

#ifndef NOPERFUI
#	include "../Widgets/Dialogs/Message.h"
#endif

namespace gge { namespace utils {

	class PerformanceTimer {
	public:
		PerformanceTimer(unsigned passed=0) : passed(passed) {
			lasttick=os::GetTime();
		}

		void Start() {
			lasttick=os::GetTime();
		}

		PerformanceTimer &Tick() {
			unsigned current=os::GetTime();

			passed+=current-lasttick;
			lasttick=current;
			return *this;
		}

		unsigned Get() const {
			return passed;
		}

		void Reset() {
			passed=0;
			lasttick=os::GetTime();
		}

#ifndef NOPERFUI
		void ShowDialog(const std::string &name="Time passed", const std::string &title="Performance timer") const { 
			std::stringstream ss;
			ss<<name<<": "<<passed;

			gge::widgets::dialog::ShowMessage(ss.str(), title).SetIcon("Time");
		}
#endif

	private:
		unsigned lasttick;
		unsigned passed;
	};

} }