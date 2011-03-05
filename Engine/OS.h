#pragma once

#include "../Utils/GGE.h"
#include "../Utils/Any.h"
#include "../Utils/EventChain.h"

#ifdef WIN32
#endif

#include "OS.Win32.h"

namespace gge {

	extern CursorHandle defaultcursor;
	extern bool ospointerdisplayed;

	////This function creates a window
	///@Name		: Identifier name for the window, should abide by variable naming rules
	///@Title		: Title of the window
	///@Icon		: The icon to be used, depends on OS
	///@Instance	: The handle for the application instance
	///@Left		: Distance of the window from the left of the screen
	///@Top			: Distance of the window from the top of the screen
	///@Width		: The width of the window
	///@Height		: The height of the window
	///@FullScreen	: Whether to create a fullscreen window
	WindowHandle CreateWin(string Name, string Title, IconHandle Icon, InstanceHandle Instance, int Left, int Top, int Width, int Height, int BitDepth, bool &FullScreen);
	////This function shows a OS message box to display errors, for other messages
	/// its better to use in-game dialogs
	void DisplayMessage(const char *Title, const char *Text);
	////Hides the pointer displayed by OS
	void HideOSPointer();
	////Shows the pointer displayed by OS
	void ShowOSPointer();
	////Quits the application while returning the given code, if no errors exists
	/// return value should be 0, other return codes should be arranged by application
	void Quit(int ret);
	////Gets the position of the cursor
	Point getMousePosition(WindowHandle Window);
	////This function processes OS messages
	void ProcessMessage();
	////Initializes OS subsystem by setting up events
	void InitializeOS();
	////Sleep function, should be in ms
	void Sleep(int);

	extern EventChain<Empty, empty_event_params> WindowActivateEvent;
	extern EventChain<Empty, empty_event_params> WindowDeactivateEvent;
	extern EventChain<Empty, empty_event_params> WindowDestroyedEvent;

}