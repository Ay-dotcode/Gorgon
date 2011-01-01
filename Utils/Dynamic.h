#pragma once



#define DYNAMIC_EXIST

#include <stdexcept>


namespace gge {

	//Allows dynamic memory allocation for objects
	template <class T_>
	class Dynamic {
	public:

		//Dynamic(T_ obj) {
		//	object=new T_(obj);
		//}

		Dynamic(T_ &obj) {
			object=&obj;
		}

		Dynamic(T_ *obj) {
			object=obj;
		}

		Dynamic() : object(NULL) { }

		
		//Dynamic &operator = (T_ obj) {
		//	if(object)
		//		delete object;

		//	object=new T_(obj);
		//}

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


		int Size() {
			return sizeof(T_);
		}

		void Destroy() {
			delete object;
			object=NULL;
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

}