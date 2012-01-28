//DESCRIPTION
//	This file is the basis for all gge::utils classes

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


#define UTILS_BASE_EXIST


#ifndef NULL
#	define	NULL	0
#endif

namespace gge {
	typedef unsigned char Byte;

	namespace utils {
		template<class T_>
		void CheckAndDelete(T_ *&elm) {
			if(elm) {
				delete elm;
				elm=NULL;
			}
		}

		template <class T_>
		class Range {
		public:
			T_ start, end;
			Range() {}
			Range(const T_ &start, const T_ &end) : start(start), end(end) {}

			T_ Difference() {
				return end-start;
			}
		};
	}

}
