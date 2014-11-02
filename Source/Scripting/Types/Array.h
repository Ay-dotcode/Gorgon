#pragma once


#include <vector>

#include "../../Any.h"
#include "../Reflection.h"
#include "../Exceptions.h"


namespace Gorgon {
	namespace Scripting {

		class Array {
		public:
			Array(const Type &type) : type(&type), Elements(elements) {}
			
			template<class T_>
			Array(const Type &type, const std::initializer_list<T_> &elements) : Array(type) {
				ASSERT(type.GetDefaultValue().TypeCheck<T_>(), "Given data type ("+Utils::GetTypeName<T_>()+
					   ") does not match with: "+type.GetName()+" ("+type.GetDefaultValue().GetTypeName()+")",
					   1, 4
				);
			}
			
			Data GetItemData(unsigned index) const {
				if(index>=elements.size()) {
					throw OutofBoundsException(index, elements.size(), "Array");
				}
				return {type, elements[index]};
			}
			
			void SetItemData(unsigned index, Data data) {
				if(index>=elements.size()) {
					throw OutofBoundsException(index, elements.size(), "Array");
				}
				if(type!=&data.GetType()) {
					//not implemented
					ASSERT(false, "Not implemented", 0, 16);
				}
				
				elements[index]=data.GetData();
			}
			
			template<class T_>
			void PushWithoutCheck(const T_ &elm) {
				elements.push_back(elm);
			}
			
			void Resize(unsigned size) {
				elements.resize(size);
			}
			
			unsigned GetSize() const {
				return elements.size();
			}
			
			const Type &GetType() const {
				return *type;
			}
			
			const std::vector<Any> &Elements;

		protected:
			std::vector<Any> elements;

			const Type *type;
		};

		Type *ArrayType();

	} 
}
