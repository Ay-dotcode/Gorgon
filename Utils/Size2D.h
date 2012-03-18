//DESCRIPTION
//	This file contains the class Size2D which defines size of a 2D object
//	Missing some operators.

//REQUIRES:
//	---

//LICENSE
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the Lesser GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//	Lesser GNU General Public License for more details.
//
//	You should have received a copy of the Lesser GNU General Public License
//	along with this program. If not, see < http://www.gnu.org/licenses/ >.

//COPYRIGHT
//	Cem Kalyoncu, DarkGaze.Org (cemkalyoncu[at]gmail[dot]com)

#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include "Point2D.h"


#ifdef GGE_XMLSERVICES
#include "../External/XmlParser/xmlParser.h"
#endif
#include <iosfwd>


#define SIZE2D_EXISTS



namespace gge { namespace utils {
	template<class T_>
	struct basic_Size2D {
		basic_Size2D(T_ W, T_ H) {
			Width=W;
			Height=H;
		}
		basic_Size2D() { }

		template <class O_>
		basic_Size2D(const basic_Size2D<O_> &size) : Width(size.Width), Height(size.Height) {  }

		template <class O_>
		basic_Size2D(const basic_Point2D<O_> &size) : Width(size.x), Height(size.y) {  }

		T_ Width;
		T_ Height;

		template <class O_>
		basic_Size2D &operator =(const basic_Point2D<O_> &size) { Width=size.x; Height=size.y; }

		bool operator ==(const basic_Size2D  &size) const { 
			return Width==size.Width && Height==size.Height;
		}

		bool operator !=(const basic_Size2D  &size) const { 
			return Width!=size.Width || Height!=size.Height;
		}
		basic_Size2D operator +(const basic_Size2D  &size) const { 
			return basic_Size2D(size.Width+Width, size.Height+Height);
		}
		basic_Size2D operator -(const basic_Size2D  &size) const { 
			return basic_Size2D(Width-size.Width, Height-size.Height);
		}
		basic_Size2D operator +=(const basic_Size2D  &size) { 
			Width=size.Width  +Width;
			Height=size.Height+Height;

			return *this;
		}
		basic_Size2D operator -=(const basic_Size2D  &size) { 
			Width=Width-size.Width;
			Height=Height-size.Height;

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
			Width/=size.Width;
			Height/=size.Height;

			return *this;
		}

		operator basic_Point2D<T_>() const {
			return basic_Point2D<T_>(Width, Height);
		}

		T_ Cells() const { return Width*Height; }
		T_ Area() const { return Width*Height; }

	};

	template<class T_>
	static std::ostream &operator <<(std::ostream &out, const basic_Size2D<T_> &size) {
		out<<size.Width<<"x"<<size.Height;

		return out;
	}

	template<class T_>
	inline basic_Point2D<T_> operator *(const basic_Point2D<T_> &l, const basic_Size2D<T_> &r) {
		return basic_Point2D<T_>(l.x*r.Width,l.y*r.Height);
	}

	template<class T_>
	inline basic_Point2D<T_> operator /(const basic_Point2D<T_> &l, const basic_Size2D<T_> &r) {
		return basic_Point2D<T_>(l.x/r.Width,l.y/r.Height);
	}
	template<class T_, class _U>
	basic_Size2D<T_> operator *(const basic_Size2D<T_> &l, const _U &r) { 
		return basic_Size2D<T_>(T_(l.Width*r), T_(l.Height*r)); 
	}
	template<class T_, class _U>
	basic_Size2D<T_> operator /(const basic_Size2D<T_> &l, const _U &r) { 
		return basic_Size2D<T_>(T_(l.Width/r), T_(l.Height/r)); 
	}
	template<class T_>
	inline basic_Size2D<T_> operator *(const basic_Size2D<T_> &l, const basic_Size2D<T_> &r) { 
		return basic_Size2D<T_>(l.Width*r.Width, l.Height*r.Height); 
	}
	template<class T_>
	inline basic_Size2D<T_> operator /(const basic_Size2D<T_> &l, const basic_Size2D<T_> &r) { 
		return basic_Size2D<T_>(l.Width/r.Width, l.Height/r.Height); 
	}

	typedef basic_Size2D<int> Size;
	typedef basic_Size2D<FloatingPoint> Size2D;
} }
