#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>


#ifdef GGE_XMLSERVICES
#include "../External/XmlParser/xmlParser.h"
#endif


#define SIZE2D_EXISTS



namespace gge {
	template<class T_>
	struct basic_Size2D {
		basic_Size2D(T_ W, T_ H) {
			Width=W;
			Height=H;
		}
		basic_Size2D() { }

		template <class O_>
		basic_Size2D(basic_Size2D<O_> &size) : Width(size.Width), Height(size.Height) {  }

		template <class O_>
		basic_Size2D(basic_Point2D<O_> &size) : Width(size.x), Height(size.y) {  }

		T_ Width;
		T_ Height;

		template <class O_>
		basic_Size2D &operator =(basic_Point2D<O_> &size) { Width=size.x; Height=size.y; }

		bool operator ==(basic_Size2D  &size) { 
			return Width==size.Width && Height==size.Height;
		}

		bool operator !=(basic_Size2D  &size) { 
			return Width!=size.Width || Height!=size.Height;
		}
		basic_Size2D operator +(basic_Size2D  &size) { 
			return basic_Size2D(size.Width+Width, size.Height+Height);
		}
		basic_Size2D operator -(basic_Size2D  &size) { 
			return basic_Size2D(size.Width-Width, size.Height-Height);
		}
		template<class _U>
		basic_Size2D operator *(_U size) { 
			return basic_Size2D(size.Width*size, size.Height*size); 
		}
		template<class _U>
		basic_Size2D operator /(_U size) { 
			return basic_Size2D(size.Width/size, size.Height/size); 
		}
		basic_Size2D operator +=(basic_Size2D  &size) { 
			Width=size.Width  +Width;
			Height=size.Height+Height;

			return *this;
		}
		basic_Size2D operator -=(basic_Size2D  &size) { 
			Width=size.Width  -Width;
			Height=size.Height-Height;

			return *this;
		}
		template<class _U>
		basic_Size2D operator *=(_U size) { 
			Width=size.Width  *Width;
			Height=size.Height*Height;

			return *this;
		}
		template<class _U>
		basic_Size2D operator /=(_U size) { 
			Width=size.Width  /Width;
			Height=size.Height/Height;

			return *this;
		}

		T_ Area() { return Width*Height; }

	};

	typedef basic_Size2D<int> Size2D;

}