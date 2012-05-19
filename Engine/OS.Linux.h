#pragma once


#ifdef LINUX



#	define WM_USER                         0x0400


	namespace gge { namespace os {

		//TODO: Handle types
		////Handle for window
		typedef long 				WindowHandle;
		////Handle for icon
		typedef unsigned long	IconHandle;
		////Handle for cursor
		typedef unsigned long	CursorHandle;
		////Handle for Device Context
		typedef void*				DeviceHandle;

#		define	threadfncall 

		namespace filesystem {
			//TODO: directory enumerator struct
			class osdirenum {
			public:
				osdirenum();
				~osdirenum();
				
				void *dp;
				std::string pattern;

			};
		}

	} }




#endif
