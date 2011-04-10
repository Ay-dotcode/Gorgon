#pragma once

#include "../Utils/GGE.h"
#include "../Utils/Any.h"
#include "../Utils/EventChain.h"

#include "OS.Win32.h"

using std::string;

namespace gge { namespace os {
	////Initializes OS subsystem by setting up events
	void Initialize();
	////This function shows a OS message box to display errors, for other messages
	/// its better to use in-game dialogs
	void DisplayMessage(const char *Title, const char *Text);
	////Quits the application while returning the given code, if no errors exists
	/// return value should be 0, other return codes should be arranged by application
	void Quit(int ret);
	////Sleep function, should be in ms
	void Sleep(int);
	////Hides the pointer displayed by OS
	void HidePointer();
	////Shows the pointer displayed by OS
	void ShowPointer();

	namespace system {
		////This function processes OS messages
		void ProcessMessage();
		extern CursorHandle defaultcursor;
		extern bool pointerdisplayed;
	}

	namespace input {
		////Gets the position of the cursor
		Point getMousePosition(os::WindowHandle Window);
		inline bool isPointerDisplayed() { return system::pointerdisplayed; }
	}

	namespace window {
		extern utils::EventChain<> Activated;
		extern utils::EventChain<> Deactivated;
		extern utils::EventChain<> Destroyed;

		////This function creates a window
		///@Name		: Identifier name for the window, should abide by variable naming rules
		///@Title		: Title of the window
		///@Icon		: The icon to be used, depends on OS
		///@Instance	: The handle for the application instance
		///@Left		: Distance of the window from the left of the screen
		///@Top			: Distance of the window from the top of the screen
		///@Width		: The width of the window
		///@Height		: The height of the window
		///@FullScreen	: Whether to create a full screen window
		WindowHandle CreateWindow(string Name, string Title, os::IconHandle Icon, os::InstanceHandle Instance, int Left, int Top, int Width, int Height, int BitDepth, bool &FullScreen);
	}

} }