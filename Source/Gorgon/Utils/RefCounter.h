//DESCRIPTION
//	RefCounter is generic reference counter. It uses curiously recurring
//	template method to add reference counting to the classes that need without
//	any additional overhead.

//WARNING:
// Might cause problems with multiple inheritance.

//REQUIRES:
//	--

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


#define REFCOUNTER_EXIST

namespace gge { namespace utils {

	//RefCounter requires following methods to be implemented by client
	//classes:
	// void dealloc() : called when no reference left, should deallocate
	//                  buffers, set counts to 0, etc... object is still alive 
	//                  and accessible, so control variables can be left out
	// void destroy() : destroys everything, this is the last chance of freeing
	//                  resources, nothing should be left out
	//Copy constructor should increment counter, assignment should assign target ref 
	//count to object's refcount, destructor should decrement it.
	template <class C_>
	class RefCounter {
	  template<class C2_> friend class RefCounter;
	protected:
		RefCounter() : refcnt(new int(1)) {	
		}

		template <class C2_>
		RefCounter(const RefCounter<C2_> &ref) : refcnt(ref.refcnt) {
		}

		RefCounter(const RefCounter &ref) : refcnt(ref.refcnt) {
		}

		void refassign(const RefCounter &ref) {
			removeref();

			if(!refcheck()) {
				static_cast<C_*>(this)->destroy();
				delete refcnt;
			}

			refcnt=ref.refcnt;
			addref();
		}
	
		bool refcheck() const {
			return (*refcnt)>0;
		}

		void addref() const {
			(*refcnt)++;
		}

		void destructref() {
			removeref();

			if(!refcheck()) {
				static_cast<C_*>(this)->destroy();
				delete refcnt;
			}
		}

		void removeref() {
			if(*refcnt>0)
				(*refcnt)--;

			if(*refcnt==0) {
				static_cast<C_*>(this)->dealloc();
			}
		}

		void removeref() const {
			if(*refcnt>0)
				(*refcnt)--;

			if(*refcnt==0) {
				throw std::runtime_error("Cannot destroy a const object");
			}
		}

		int getrefcount() const {
			return *refcnt;
		}

		////Reference count
		mutable int *refcnt;
	};

} }
