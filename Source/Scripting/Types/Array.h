#pragma once


#include <vector>

#include "../../Any.h"
#include "../Reflection.h"


namespace Gorgon {
	namespace Scripting {

		class Array {
		public:
			Array(const Type &type) : type(&type) {}

			const Type &GetType() const {
				return *type;
			}

			std::vector<Any> elements;

		protected:
			const Type *type;
		};

		Type *ArrayType();

	} 
}