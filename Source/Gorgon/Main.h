#pragma once

#include <string>

#include "Event.h"

///@page Design
///@htmlinclude "Design.html"

/// Root namespace for Gorgon Game Engine.
namespace Gorgon {

	/// @cond INTERNAL
	namespace internal {
		extern std::string systemname;
	}
	/// @endcond

	/// Defines the abstract class of Runner. Runners take the control of the code
	/// execution, calling any necessary functions as the events occur.
	class Runner {
	public:
		/// Takes the control of the execution until Quit is called.
		virtual void Run() = 0;

		/// Runs a single frame
		virtual void Step() = 0;

		/// Should quit after the current frame is completed for a graceful exit.
		virtual void Quit() = 0;
	};

	/// Initializes the entire system
	void Initialize(const std::string &systemname);

	/// Returns the name of the current system
	inline std::string GetSystemName() { return internal::systemname; }

	/// Performs various operations that are vital to system execution. These include
	/// OS message handling, animation and sound progressions, time progression and 
	/// delta time calculation. NextFrame function should be preferred if the frame delay
	/// is tolerable.
	void Tick();

	/// This function calls the starts the rendering pipeline. Rendering should be last
	/// operation of a frame.
	void Render();

	/// This function marks the end of current frame and starts the next one. This function calls
	/// the Tick function at start of the next frame. Additionally, this function calls end of
	/// frame tasks such as rendering. Before starting the next frame, a certain delay is performed.
	/// This delay aims to set each frame duration to 16ms, this duration sets the frames per second
	/// to 62.5. This delay greatly reduces the system load of simple games/applications.
	void NextFrame();

	/// This method works similar to next frame, however, no delay is done. This function allows an
	/// application to update the display and perform OS tasks while still continuing operation.
	inline void UpdateFrame();
	
	extern Event<> BeforeFrameEvent;
}
