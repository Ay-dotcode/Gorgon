//DESCRIPTION
//	This file contains definitions for basic graphics systems.

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

#include <string>
#include <sstream>

namespace gge { namespace utils {

	inline std::string Replace(const std::string str, const std::string find, const std::string replace) {
		std::string::size_type l=-1;
		std::string ret=str;
		while( (l=str.find(find, l+1)) != str.npos ) {
			ret=ret.replace(l, find.length(), replace);
		}

		return ret;
	}

	inline std::string Trim(std::string str, const std::string chars=" \t\n\r") {
		while(str.length() && chars.find_first_of(str[0])!=-1) {
			str=str.substr(1);
		}
		while(str.length() && chars.find_first_of(str[str.length()-1])!=-1) {
			str.resize(str.length()-1);
		}

		return str;
	}

	template <typename T>
	class has_stringoperator
	{
		typedef char one;
		struct two {
			char a[2];
		};

		template <typename C> static one test( decltype(((C*)(nullptr))->operator std::string()) aa ) {;}
		template <typename C> static two test(...){;}

	public:
		enum { value = sizeof(test<T>(""))==sizeof(char) };
	};

	template<class T_> 
	typename std::enable_if<has_stringoperator<T_>::value, std::string>::type ToString(const T_ &item) {
		return (std::string)item;
	}

	template<class T_> 
	typename std::enable_if<!has_stringoperator<T_>::value, std::string>::type ToString(const T_ &item) {
		std::stringstream ss;
		ss<<item;

		return ss.str();
	}

} }


