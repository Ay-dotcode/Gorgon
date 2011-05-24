//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!! REQUIRES TESTING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//DESCRIPTION
//	CastableManagedBuffer is an array that manages reference counting
//	and auto destruction of its buffer if no variable references
//	to that buffer. It has copy semantics which links copied object
//	to the source object. The contents of the buffer can be accessed
//	as a different type, but in that case the buffer cannot be resized

//REQUIRES:
//	gge::utils::RefCount

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
	class CastableManagedBuffer : RefCounter<CastableManagedBuffer<T_> > {
		template<class O_>
		friend class CastableManagedBuffer;
		friend class RefCounter;
		friend void std::swap<T_>(CastableManagedBuffer<T_> &left, CastableManagedBuffer<T_> &right);
	public:
		CastableManagedBuffer() : sizefactor(1), noresizer(false) {
			data=new T_*();
			*data=NULL;
			size_=new int();
			*size_=0;
		}

		CastableManagedBuffer(int size) : sizefactor(1), noresizer(false) {
			data=new T_*();
			*data=NULL;
			this->size_=new int();
			Resize(size);
		}

		CastableManagedBuffer(const CastableManagedBuffer &buf) : RefCounter<CastableManagedBuffer>(buf), 
			sizefactor(1), noresizer(false) {

			data=buf.data;
			size_=buf.size_;
			sizefactor=buf.sizefactor;
		}


		//!CASTING CONSTRUCTORS REMOVED, USE CAST FUNCTION INSTEAD

		//works in O(1), no copying or conversion is done, 
		template<class O_>
		CastableManagedBuffer<O_> Cast() {
			//at least one of the sides should contain integral number of 
			//other type
			if( sizeof(T_)%sizeof(O_) && sizeof(O_)%sizeof(T_) ) 
				throw runtime_error("Cannot cast, size mismatch");

			return CastableManagedBuffer<O_>((factor*(float)sizeof(T_))/sizeof(O_), this);
		}

		//works in O(1), no copying or conversion is done, 
		template<class O_>
		const CastableManagedBuffer<O_> Cast() const {
			//at least one of the sides should contain integral number of 
			//other type
			if( sizeof(T_)%sizeof(O_) && sizeof(O_)%sizeof(T_) ) 
				throw runtime_error("Cannot cast, size mismatch");

			return const CastableManagedBuffer<O_>((factor*(float)sizeof(T_))/sizeof(O_), this);
		}

		void Resize(int size) {
			if(noresizer)
				throw std::runtime_error("Cannot resize");

			if(size==*(this->size_))
				return;

			if(*data) {
				*data=(T_*) std::realloc(*data, size*sizeof(T_));
			} else if(size==0) {
				std::free(*data);
				*data=NULL;
			} else {
				*data=(T_*) std::malloc(size*sizeof(T_));
			}

			*this->size_=size;
		}

		CastableManagedBuffer &operator =(const CastableManagedBuffer &buf) {
			if(this==&buf) return *this;

			refassign(buf);

			data=buf.data;
			size_=buf.size_;
			sizefactor=buf.sizefactor;
			noresizer=buf.noresizer;

			return *this;
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
				return (int)((*size_)*sizefactor);
			else
				return 0;
		}

		void AddReference() const {
			addref();
		}

		void RemoveReference() {
			removeref();
		}

		void RemoveReference() const {
			removeref();
		}

		int getReferenceCount() const {
			return getrefcount();
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

		inline const T_ &operator [] (int index) const {
#ifdef _DEBUG
#ifndef _DEBUG_FASTBUFFER
			if(index<0 || index>=GetSize()) {
				throw std::runtime_error("Index out of bounds");
			}
#endif
#endif

			return (*data)[index];
		}

		inline T_ *operator () (int index) {
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

		inline const T_ *operator () (int index) const {
			return &((*data)[index]);
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

		~CastableManagedBuffer() {
			destructref();
		}

//		std compatibility
		T_ *begin() {
			return *data;
		}

		T_ *end() {
			if(data==NULL)
				return NULL;
			return (*data)+GetSize();
		}

		const T_ *begin() const {
			return *data;
		}

		const T_ *end() const {
			if(data==NULL)
				return NULL;
			return (*data)+GetSize();
		}

		int size() const {
			return *size_;
		}

	protected:
		void dealloc() {
			Resize(0);
		}

		void destroy() {
			delete size_;

			if(data)
				delete *data;

			delete data;
		}


	private:
		template<class O_>
		CastableManagedBuffer(float factor, CastableManagedBuffer<O_> &buf) : 
			RefCounter<CastableManagedBuffer>(buf), sizefactor(factor) {

			noresizer = sizeof(O_) != sizeof(T_);
			data=(T_**)buf.data;
			size_=buf.size;
		}

		template<class O_>
		CastableManagedBuffer(float factor, CastableManagedBuffer<O_> *buf) :
			RefCounter<CastableManagedBuffer>(buf), sizefactor(factor) {

			noresizer = sizeof(O_) != sizeof(T_);
			data=(T_**)buf->data;
			size_=buf->size;
		}

		T_ **data;
		int *size_;

		bool noresizer;

		float sizefactor;

	};
}

namespace std {
	//untested for std compatibility, swaps to buffers in O(1)
	template<class T_>
	void swap(gge::CastableManagedBuffer<T_> &left,gge::CastableManagedBuffer<T_> &right) {
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
} }
