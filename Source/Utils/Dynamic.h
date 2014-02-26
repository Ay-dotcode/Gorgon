//DESCRIPTION
//	Allows dynamic memory allocation for objects. This class is used for
//	convenience. Since it is not logical for const references, this
//	class is not const correct.

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
//	

//USAGE
// For function SomeType &Funct(); which creates an non-copyable object
// Dynamic SomeVar(Funct());
// This will destroy the generated object when they run out of scope.

#pragma once



#define DYNAMIC_EXIST

#include <stdexcept>


namespace gge { namespace utils {

	template <class T_>
	class Dynamic {
	public:

		Dynamic(T_ &obj) {
			object=&obj;
		}

		Dynamic(T_ *obj) {
			object=obj;
		}

		Dynamic() : object(NULL) { }


		Dynamic &operator = (T_ &obj) {
			if(object)
				delete object;

			object=&obj;
		}

		Dynamic &operator = (T_ *obj) {
			if(object)
				delete object;

			object=obj;
		}


		T_* operator -> () {
#ifdef _DEBUG
			if(!object)
				throw std::bad_cast();
#endif
			return object;
		}

		operator T_&() {
#ifdef _DEBUG
			if(!object)
				throw std::bad_cast();
#endif
			return *object;
		}

		operator T_*() {
			return object;
		}


		int Size() const {
			return sizeof(T_);
		}

		void Destroy() {
			if(object) {
				delete object;
				object=NULL;
			}
		}

		~Dynamic() {
			Destroy();
		}


	protected:
		T_ *object;

	private:
		Dynamic(Dynamic<T_> &obj);
		Dynamic &operator = (Dynamic &);


	};

} }
