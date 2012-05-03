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

#include <cstring>
#include <cstdlib>
#include <stdexcept>

#include "UtilsBase.h"


#define ANY_EXIST



namespace gge { namespace utils {
	class Any {
	public:
		Any() : content(NULL),size(0) { }

		Any(const Any& any) {
			if(any.content) {
				size=any.size;
				content=malloc(size);
				std::memcpy(content, any.content, size);
			} else {
				content=NULL;
				size=0;
			}

		}

		template <class T_>
		explicit Any(T_ data) {
			size=sizeof(T_);
			content=std::malloc(size);
			*reinterpret_cast<T_*>(content)=data;
		}

		template <class T_>
		operator T_ &() {
			if(sizeof(T_)!=size) {
				throw std::runtime_error("Cannot cast, sizes are different");
			}
			return *reinterpret_cast<T_*>(content);
		}

		operator Any &() {
			return *this;
		}
	
		Any &operator =(const Any &any) {
			if(content)
				free(content);
			size=any.size;
			content=malloc(size);
			memcpy(content, any.content, size);

			return *this;
		}

		template <class T_>
		bool operator ==(const T_ &content) const  {
			return *reinterpret_cast<T_*>(this->content)==content;
		}

		template <class T_>
		bool operator !=(const T_ &content) const  {
			return *content!=content;
		}

		bool isSet() const  {
			return content!=NULL;
		}
		
		template <class T_>
		void SetData(const T_ &data) {
			size=sizeof(T_);
			content=std::malloc(size);
			*reinterpret_cast<T_*>(content)=data;
		}

		template <class T_>
		T_ *operator ->() {
			return reinterpret_cast<T_*>(content);
		}

		void Clear() {
			size=0;
			content=NULL;
		}

		~Any() {
			if(content)
				free(content);
		}

	protected:
		void *content;
		int size;
	};
} }
