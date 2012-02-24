#pragma once


#ifdef WIN32



#	define WM_USER                         0x0400




	namespace gge { namespace os {

		////Handle for window
		typedef unsigned long	WindowHandle;
		////Handle for icon
		typedef unsigned long	IconHandle;
		////Handle for cursor
		typedef unsigned long	CursorHandle;
		////Handle for Device Context
		typedef unsigned long	DeviceHandle;

	} }




#endif
