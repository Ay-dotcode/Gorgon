//DESCRIPTION
//	This file contains SGuid, short GUID class which is an 8-byte
//	unique identifier, may not be unique between instances however,
//	while operating on same data SGUID advances its serial to the
//	highest known serial, in addition to that current time and a
//	random component guarantees uniqueness.

//REQUIRES:
//	GGE/Utils/Random

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


#include "SGuid.h"
#include <sstream>

namespace gge { namespace utils {

	unsigned SGuid::serial = 0;
	const SGuid SGuid::Empty = SGuid(0,0);



	SGuid::operator std::string() const {
		std::stringstream ss;

		ss
			<<std::setfill('0')<<std::hex
			<<std::setw(6)<<((ints[1]>>8) & 0xffffff)<<"-"
			<<std::setw(6)<<(*(int*)(bytes+2) & 0xffffff)<<"-"
			<<std::setw(4)<<(ints[0] & 0xffff)
			;

		return ss.str();
	}

} }
