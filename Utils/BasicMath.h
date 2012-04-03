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

#pragma once

#include <cmath>
#include <cstdlib>
#include <type_traits>
#include "Iterator.h"


namespace gge { namespace utils {

	template <class T_>
	inline T_ Round(T_ num) {
		return std::floor(num+0.5f); 
	}

	template <class T_>
	inline T_ Round(const T_ &num, int digits) {
		T_ d=std::pow(10.0f, digits);
		return std::floor(num*d+0.5f)/d;
	}

	//accepts both terminals as in range
	template <class T_, class U_, class V_>
	bool InRange(const T_ &num, const U_ &n1, const V_ &n2) {
		if(n1<n2)
			return num>=n1 && num<=n2;
		else
			return num>=n2 && num<=n1;
	}

	template <class T_, class U_>
	T_ Max(const T_ &num1, const U_ &num2) { return num1>num2 ? num1 : num2; }

	template <class T_, class U_>
	T_ Min(const T_ &num1, const U_ &num2) { return num1<num2 ? num1 : num2; }

	template <class T_, class P_>
	typename std::enable_if<is_iterator<T_>::value, typename T_::value_type>::type 
		Max(const T_ &num1, const T_ &num2, P_ pred=std::greater<T_>()) { 
		if(num1==num2)
			return typename T_::value_type();

		typename T_::value_type m=*num1;
		T_ it=num1;
		it++;
		for(;it!=num2;++it) {
			if(pred(*it,m))
				m=*it;
		}

		return m;
	}


	template <class T_, class P_>
	typename std::enable_if<is_iterator<T_>::value, typename T_::value_type>::type 
		Min(const T_ &num1, const T_ &num2, P_ pred=std::less<T_>()) { 
		if(num1==num2)
			return typename T_::value_type();

		typename T_::value_type m=*num1;
		T_ it=num1;
		it++;
		for(;it!=num2;++it) {
			if(pred(*it,m))
				m=*it;
		}

		return m;
	}


	template <class T_>
	inline T_ PositiveMod(const T_ &num, const T_ &mod) {
		return num<0 ? mod + (num%mod) : num%mod;
	}

	template <class T_>
	T_ StrToNumber(const std::string &value, int base=10) {
		char *n;
		return std::strtol(value.c_str(), &n, base);
	}

	template <>
	inline float StrToNumber<float>(const std::string &value, int base) {
		return (float)std::atof(value.c_str());
	}

	template <>
	inline double StrToNumber<double>(const std::string &value, int base) {
		return std::atof(value.c_str());
	}


}}

