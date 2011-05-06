//DESCRIPTION
//	ManagedBuffer is an array that manages reference counting
//	and auto destruction of its buffer if no variable references
//	to that buffer. It has copy semantics which links copied object
//	to the source object. To access the contents of the buffer 
//	as a different type use CastableManagedBuffer instead

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
#include <stdexcept>
#include <cstdlib>

namespace gge { namespace utils {
	template <class T_>
	class ManagedBuffer {
		template<class O_>
		friend class ManagedBuffer;
		friend void std::swap<T_>(ManagedBuffer<T_> &left, ManagedBuffer<T_> &right);
	public:
		ManagedBuffer() : refcnt(new int(1)), data(new T_*(nullptr)), size_(new int(0)) 
		{ }

		ManagedBuffer(int size) : refcnt(new int(1)), data(new T_*(nullptr)), size(new int(0)) 
		{
			Resize(size);
		}

		ManagedBuffer(const ManagedBuffer &buf) : data(buf.data), refcnt(buf.refcnt), size_(buf.size_) {
			AddReference();
		}

		ManagedBuffer &operator =(const ManagedBuffer &buf) {
			if(this==&buf) return *this;
			if(this->data==buf->data) return *this;

			RemoveReference();

			if(!(*refcnt)) {
				delete refcnt;
				delete size_;
				delete data;
			}

			data=buf.data;
			refcnt=buf.refcnt;
			size_=buf.size_;
			AddReference();

			return *this;
		}

		void Resize(int size) {
			if(size==*(this->size_))
				return;

			if(size==0) {
				if(*data) {
					std::free(*data);
					*data=NULL;
				}
			} else if(*data) {
				*data=(T_*) std::realloc(*data, size*sizeof(T_));
			} else {
				*data=(T_*) std::malloc(size*sizeof(T_));
			}

			*this->size_=size;
		}

		inline T_ *GetBuffer() {
#ifdef _DEBUG
			if(*data==NULL) {
				throw std::runtime_error("Data is not set");
			}
#endif

			return *data;
		}

		inline const T_ *GetBuffer() const {
#ifdef _DEBUG
			if(*data==NULL) {
				throw std::runtime_error("Data is not set");
			}
#endif

			return *data;
		}

		int GetSize() const {
			if(*data)
				return (*size_);
			else
				return 0;
		}

		void AddReference() const {
			(*refcnt)++;
		}

		void RemoveReference() {
			if(*refcnt>0)
				(*refcnt)--;

			if(*refcnt==0 && *data) {
				std::free(*data);
				*data=NULL;
				*size_=0;
			}
		}

		void RemoveReference() const {
			if(*refcnt>0)
				(*refcnt)--;

			if(*refcnt==0 && *data) {
				throw std::runtime_error("Cannot destroy a const buffer");
			}
		}

		int getReferenceCount() const {
			return *refcnt;
		}

		inline T_ &operator [] (int index) {
#ifdef _DEBUG
#ifndef _DEBUG_FASTBUFFER
			if(index<0 || index>=GetSize()) {
				throw std::runtime_error("Index out of bounds");
			}
#endif
#endif

			return (*data)[index];
		}

		const inline T_ &operator [] (int index) const {
#ifdef _DEBUG
#ifndef _DEBUG_FASTBUFFER
			if(index<0 || index>=GetSize()) {
				throw std::runtime_error("Index out of bounds");
			}
#endif
#endif

			return (*data)[index];
		}

		inline T_ &Get(int index) {
			return (*this)[index];
		}

		inline const T_ &Get(int index) const {
			return (*this)[index];
		}

		inline T_ *operator () (int index) {
			return &((*data)[index]);
		}

		inline const T_ *operator () (int index) const {
			return &((*data)[index]);
		}

		operator T_*() {
#ifdef _DEBUG
			if(*data==NULL) {
				throw std::runtime_error("Managed Buffer: Data is not set");
			}
#endif
			return *data;
		}

		operator const T_*() const {
#ifdef _DEBUG
			if(*data==NULL) {
				throw std::runtime_error("Managed Buffer: Data is not set");
			}
#endif
			return *data;
		}

		T_* operator +(int offset) { return (*this)(offset); }

		const T_* operator +(int offset) const { return (*this)(offset); }

		~ManagedBuffer() {
			RemoveReference();

			if(!*data) {
				delete refcnt;
				delete size_;
				delete data;
			}
		}

		#pragma region "std compatibility"
		inline T_ *begin() {
			return *data;
		}

		T_ *end() {
			if(*data==NULL)
				return NULL;

			return (*data)+*size_;
		}

		const T_ *begin() const {
			return *data;
		}

		const T_ *end() const {
			if(*data==NULL)
				return NULL;

			return (*data)+*size_;
		}

		int size() const {
			return GetSize();
		}
#pragma endregion

	private:
		T_ **data;
		int *size_;

		////Reference count
		mutable int *refcnt;
	};
} }

namespace std {
	//untested for std compatibility, swaps to buffers in O(1)
	template<class T_>
	void swap(gge::utils::ManagedBuffer<T_> &left,gge::utils::ManagedBuffer<T_> &right) {
		T_** d;
		int *s;
		int *rc;
		bool nr;
		float sf;

		d=left.data;
		s=left.size;
		rc=left.refcnt;
		nr=left.noresizer;
		sf=left.sizefactor;

		left.data=right.data;
		left.size=right.size;
		left.refcnt=right.refcnt;
		left.noresizer=right.noresizer;
		left.sizefactor=right.sizefactor;

		right.data=d;
		right.size=s;
		right.refcnt=rc;
		right.noresizer=nr;
		right.sizefactor=sf;
	}
}
