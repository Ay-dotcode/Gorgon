//DESCRIPTION
//	ManagedBuffer is an array that manages reference counting
//	and auto destruction of its buffer if no variable references
//	to that buffer. It has copy semantics which links copied object
//	to the source object. The contents of the buffer can be accessed
//	as a different type, but in that case the buffer cannot be resized

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

namespace gge {
	template <class T_>
	class ManagedBuffer {
		template<class O_>
		friend class ManagedBuffer;
	public:
		ManagedBuffer() : sizefactor(1), noresizer(false) {
			refcnt=new int();
			*refcnt=1;
			data=new T_*();
			*data=NULL;
			size=new int();
			*size=0;
		}

		ManagedBuffer(int size) : sizefactor(1), noresizer(false) {
			refcnt=new int();
			*refcnt=1;
			data=new T_*();
			*data=NULL;
			this->size=new int();
			Resize(size);
		}

		ManagedBuffer(const ManagedBuffer &buf) : sizefactor(1), noresizer(false) {
			data=buf.data;
			refcnt=buf.refcnt;
			size=buf.size;
			sizefactor=buf.sizefactor;
			(*refcnt)++;
		}

		template<class O_>
		ManagedBuffer(float factor, ManagedBuffer<O_> &buf) : sizefactor(factor), noresizer(true) {
			data=(T_**)buf.data;
			refcnt=buf.refcnt;
			size=buf.size;
			(*refcnt)++;
		}

		template<class O_>
		ManagedBuffer(float factor, ManagedBuffer<O_> *buf) : sizefactor(factor), noresizer(true) {
			data=(T_**)buf->data;
			refcnt=buf->refcnt;
			size=buf->size;
			(*refcnt)++;
		}

		template<class O_>
		ManagedBuffer<O_> &Cast() {
			if((sizeof(T_)*(*size))%sizeof(O_))
				throw runtime_error("Cannot cast, size mismatch");

			ManagedBuffer<O_> *buf=new ManagedBuffer<O_>((float)sizeof(T_)/sizeof(O_), this);

			return *buf;
		}

		void Resize(int size) {
			if(noresizer)
				throw std::runtime_error("Cannot resize");

			if(size==*(this->size))
				return;

			if(*data) {
				*data=(T_*) std::realloc(*data, size*sizeof(T_));
			} else if(size==0) {
				std::free(*data);
				*data=NULL;
			} else {
				*data=(T_*) std::malloc(size*sizeof(T_));
			}

			*this->size=size;
		}

		ManagedBuffer &operator =(const ManagedBuffer &buf) {
			if(this==&buf) return *this;
			RemoveReference();

			if(!(*refcnt)) {
				delete refcnt;
				delete size;
				delete data;
			}

			data=buf.data;
			refcnt=buf.refcnt;
			size=buf.size;
			sizefactor=buf.sizefactor;
			noresizer=buf.noresizer;
			(*refcnt)++;

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

		void AddReference() {
			(*refcnt)++;
		}

		int GetSize() {
			if(*data)
				return (int)((*size)*sizefactor);
			else
				return 0;
		}

		void RemoveReference()
		{
			if(*refcnt>0)
				(*refcnt)--;

			if(*refcnt==0 && *data) {
				free(*data);
				*data=NULL;
				*size=0;
			}
		}
		~ManagedBuffer() {
			RemoveReference();
			if(!*data) {
				delete refcnt;
				delete size;
				delete data;
			}
		}
		int getReferenceCount() {
			if(*data)
				return *refcnt;
			else
				return 0;
		}

		ManagedBuffer<T_> &operator ++() {
			AddReference();

			return *this;
		}

		ManagedBuffer<T_> &operator --() {
			RemoveReference();

			return *this;
		}

		ManagedBuffer<T_> &operator ++(int) {
			AddReference();

			return *this;
		}

		ManagedBuffer<T_> &operator --(int) {
			RemoveReference();

			return *this;
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




	private:
		T_ **data;
		int *size;

		bool noresizer;

		float sizefactor;

		////Reference count
		int *refcnt;
	};
}
