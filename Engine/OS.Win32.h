#pragma once


#ifdef WIN32



#	define WM_USER                         0x0400

extern "C" {
	struct _WIN32_FIND_DATAA;
}


	namespace gge { namespace os {

		////Handle for window
		typedef unsigned long	WindowHandle;
		////Handle for icon
		typedef unsigned long	IconHandle;
		////Handle for cursor
		typedef unsigned long	CursorHandle;
		////Handle for Device Context
		typedef unsigned long	DeviceHandle;

#		define	threadfncall __stdcall

		namespace filesystem {
			class osdirenum {
			public:
				osdirenum();

				~osdirenum();

				void *search_handle;

				bool valid;

				_WIN32_FIND_DATAA *data;
			};
		}

	} }




#endif
