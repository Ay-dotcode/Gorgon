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
#include <typeinfo>



#define ANY_EXIST



namespace gge { namespace utils {
	class TypeInterface {
	public:
		virtual void *New() const=0;
		virtual void  Delete(void* obj) const=0;
		virtual void *Clone(const void* const obj) const=0;
		virtual bool  IsSameType(const std::type_info &) const=0;
	};

	template<class T_> class Type : public TypeInterface {
	public:
		virtual void* New() const {
			return new T_;
		}
		virtual void Delete(void *obj) const {
			delete static_cast<T_*>(obj);
		}
		virtual void *Clone(const void* const obj) const {
			T_ *n = new T_;
			*n = *static_cast<const T_* const>(obj);
			return n;
		}
		virtual bool IsSameType(const std::type_info &info) const {
			return info==typeid(T_);
		}
	};

	class Any {
	public:
		Any() : content(NULL),type(NULL) { }

		Any(const Any& any) {
			if(any.content) {
				type=(TypeInterface*)malloc(sizeof(TypeInterface));
				std::memcpy(type, any.type, sizeof(TypeInterface));
				content=type->Clone(any.content);
			} else {
				content=NULL;
				type=NULL;
			}

		}

		template <class T_>
		explicit Any(const T_ &data) {
			type=new Type<T_>;
			content=type->Clone(&data);
		}

		template <class T_>
		operator T_() const {
#ifdef _DEBUG
			if(!type || !content) {
				throw std::runtime_error("Any storage is empty");
			}
#endif
			if(!type->IsSameType(typeid(T_))) {
				throw std::runtime_error("Cannot cast: not the same type");
			}
			return *static_cast<T_*>(content);
		}

		template <class T_>
		bool TypeCheck() const {
			return type->IsSameType(typeid(T_));
		}

		operator Any() {
			return *this;
		}

		Any &operator =(const Any &any) {
			if(content) {
				type->Delete(content);
				delete type;
			}
			if(any.type) {
				type=(TypeInterface*)malloc(sizeof(TypeInterface));
				std::memcpy(type, any.type, sizeof(TypeInterface));
				content=type->Clone(any.content);
			}
			else {
				type=NULL;
				content=NULL;
			}

			return *this;
		}

		template <class T_>
		bool operator ==(const T_ &content) const  {
#ifdef _DEBUG
			if(!type || !content) {
				throw std::runtime_error("Any storage is empty");
			}
#endif

			return *static_cast<T_*>(this->content)==content;
		}

		template<class T_>
		bool IsEqual(const Any &any) const  {
			if(any.content==NULL && content==NULL) return true;

#ifdef _DEBUG
			if(!type || !content) {
				throw std::runtime_error("Any storage is empty");
			}
#endif

			return any==*static_cast<T_*>(content);
		}

		template <class T_>
		bool operator !=(const T_ &content) const  {
#ifdef _DEBUG
			if(!type || !content) {
				throw std::runtime_error("Any storage is empty");
			}
#endif

			return *static_cast<T_*>(this->content)==content;
		}

		bool isSet() const  {
			return content!=NULL;
		}

		template <class T_>
		void SetData(const T_ &data) {
			if(content) {
				type->Delete(content);
				delete type;
			}
			type=new Type<T_>;
			content=type->Clone(&data);
		}

		void Clear() {
			if(content) {
				type->Delete(content);
				delete type;
				type=NULL;
				content=NULL;
			}
		}

		~Any() {
			if(content) {
				type->Delete(content);
				delete type;
			}
		}

	protected:
		void *content;
		TypeInterface *type;
	};
} }
