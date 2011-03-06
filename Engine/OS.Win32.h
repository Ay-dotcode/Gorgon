#pragma once


#ifdef WIN32



#	define WM_USER                         0x0400


	extern "C" {
		__declspec(dllimport) unsigned long __stdcall timeGetTime(void);
	}


	namespace gge { namespace os {

		////Handle for window
		typedef unsigned long	WindowHandle;
		////Handle for icon
		typedef unsigned long	IconHandle;
		////Handle for application instance
		typedef unsigned long	InstanceHandle;
		////Handle for cursor
		typedef unsigned long	CursorHandle;
		////Handle for Device Context
		typedef unsigned long	DeviceHandle;

		////Window message to process video operation
#		define WM_VIDEO_NOTIFY  WM_USER+3

		////This function returns the current time
		inline unsigned int GetTime() { return timeGetTime(); }

	} }




#endif
