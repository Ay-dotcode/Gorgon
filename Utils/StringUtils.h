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

namespace gge { namespace utils {

	std::string Replace(const std::string str, const std::string find, const std::string replace) {
		std::string::size_type l=0;
		std::string ret=str;
		while( (l=str.find(find, l+1)) != str.npos ) {
			ret=ret.replace(l, find.length(), replace);
		}

		return ret;
	}

} }


