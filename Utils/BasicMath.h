//DESCRIPTION
//	This file contains some basic math functions which STDLib cannot
//	fully satisfy.

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


#include <cmath>


namespace gge { namespace utils {

	template <class T_>
	inline int Round(T_ num) {
		return (int)(num+0.5f); 
	}

	template <class T_>
	inline float Round(T_ num, int digits) {
		T_ d=std::pow(10.0f, digits);
		return ((int)(num*d+0.5f))/d;
	}

	template <class T_, class U_>
	T_ Max(T_ num1, U_ num2) { return num1>num2 ? num1 : num2; }

	template <class T_, class U_>
	T_ Min(T_ num1, U_ num2) { return num1<num2 ? num1 : num2; }


}}

