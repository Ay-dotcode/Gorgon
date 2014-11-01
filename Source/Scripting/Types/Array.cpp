#include "../Embedding.h"

#include "Array.h"

namespace Gorgon {
	namespace Scripting {

		// to avoid name clashes
		namespace {
			std::string ArrayToStr(const Array &array) {
				std::string ret;
				for(auto a : array.elements) {
					if(ret!="") ret+=", ";
					else ret=array.GetType().GetName()+"[";

					ret+=array.GetType().ToString({array.GetType(), a});
				}
				ret+="]";
			}
		}

		Type *ArrayType() {
			static Type *array = nullptr;

			if(!array) {
				array=new MappedReferenceType<Array, ArrayToStr>("Array", "This is an array", (Array*)nullptr);
				
				//array->AddConstructors()
			}
			
			return array;
		}
	}
}
