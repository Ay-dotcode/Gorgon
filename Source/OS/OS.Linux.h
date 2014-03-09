#pragma once

#ifdef LINUX

#include "../Utils/RefCounter.h"



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
			class osdirenum : public utils::RefCounter<osdirenum> {
				friend class RefCounter<osdirenum>;
			public:
				osdirenum();
				osdirenum(const osdirenum &);
				~osdirenum();
				osdirenum &operator =(const osdirenum &);

				void* dp;
				std::string pattern;

			protected:
				void dealloc() {}
				void destroy();
			};
		}

	} }




#endif
