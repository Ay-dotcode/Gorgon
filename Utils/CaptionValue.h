//DESCRIPTION
//	This file contains class Any which is a container for any type and
//	supports boxing, unboxing and copying; does not use RTTI, use with
//	care. Best be used with built in types or POD structures

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

namespace gge { namespace utils {

	template<class T_>
	class CaptionValue {
	public:
		std::string caption;
		T_ value;

		CaptionValue(T_ value=T_(), std::string caption="") : value(value), caption(caption) 
		{ }

		//For listbox/combobox
		operator std::string() const {
			return caption;
		}

		bool operator ==(const CaptionValue &c) const {return c.value==value;}
		bool operator !=(const CaptionValue &c) const {return c.value!=value;}

		bool operator <(const CaptionValue &c) const {return c.value<value;}
		bool operator <=(const CaptionValue &c) const {return c.value<=value;}
		bool operator >(const CaptionValue &c) const {return c.value>value;}
		bool operator >=(const CaptionValue &c) const {return c.value>=value;}
	};

}}
