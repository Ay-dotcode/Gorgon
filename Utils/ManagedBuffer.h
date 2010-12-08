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
			noresize=new int();
			*noresize=0;
		}

		ManagedBuffer(int size) : sizefactor(1), noresizer(false) {
			refcnt=new int();
			*refcnt=1;
			data=new T_*();
			*data=NULL;
			this->size=new int();
			noresize=new int();
			*noresize=0;
			Resize(size);
		}

		ManagedBuffer(ManagedBuffer &buf) : sizefactor(1), noresizer(false) {
			data=buf.data;
			refcnt=buf.refcnt;
			size=buf.size;
			noresize=noresize;
			sizefactor=buf.sizefactor;
		}

		template<class O_>
		ManagedBuffer(float factor, ManagedBuffer<O_> *buf) : sizefactor(factor), noresizer(true) {
			data=(T_**)buf->data;
			refcnt=buf->refcnt;
			size=buf->size;
			noresize=buf->noresize;

			(*noresize)++;
		}

		template<class O_>
		ManagedBuffer<O_> &Cast(O_ *none=NULL) {
			if((sizeof(T_)*(*size))%sizeof(O_))
				throw runtime_error("Cannot cast, size mismatch");

			ManagedBuffer<O_> *buf=new ManagedBuffer<O_>((float)sizeof(T_)/sizeof(O_), this);

			return *buf;
		}

		void Resize(int size) {
			if(noresizer)
				throw std::runtime_error("Cannot resize");
			if(*data)
				*data=(T_*) std::realloc(*data, size*sizeof(T_));
			else {
				*data=(T_*) std::malloc(size*sizeof(T_));
			}

			*this->size=size;
		}

		ManagedBuffer &operator =(ManagedBuffer &buf) {
			RemoveReference();

			if(!(*refcnt)) {
				delete refcnt;
				delete size;
				delete data;
				delete noresize;
			}

			data=buf.data;
			refcnt=buf.refcnt;
			size=buf.size;
			noresize=buf.noresize;
			sizefactor=buf.sizefactor;

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
				if(noresizer)
					(*noresize)--;
				delete noresize;
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
			if(index<0 || index>=GetSize()) {
				throw std::runtime_error("Index out of bounds");
			}
#endif

			return (*data)[index];
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
		int *noresize;

		bool noresizer;

		float sizefactor;

		////Reference count
		int *refcnt;
	};
}
