/// @file Runtime.h This file contains classes that stores runtime and end programmer defined objects.

#pragma once

#include <ostream>
#include <string>


#include "Reflection.h"
#include "Data.h"

namespace Gorgon {
	
	namespace Scripting {
		
		
		/// This class represents a variable. It contains the data and the name of the variable.
		class Variable : public Data {
		public:
			
			Variable(const std::string &name, Type &type, Any value) : name(name), Data(type, value) {
			}
			
			Variable(const std::string &name, Type &type) : name(name), Data(type) {
			}
			
			/// Sets the data contained in this variable without changing its type
			void Set(Any value) {
				data=value;
			}
			
			/// Sets the data contained in this variable by modifying its type. Also this function
			/// resets the tags unless they are re-specified
			void Set(Type &type, Any value) {
				data=value;
				this->type=&type;
			}
			
		protected:
			std::string name;
		};
		
		/**
		 * This class stores information about a scope that is built by a scoped keyword.
		 */
		class KeywordScope { };
		
		/**
		 * 
		 */
		class VariableScope { 
		public:
		};
		
		class InputScope { };
		
		
		///@cond INTERNAL
		class CustomFunction : public Function {
		public:
			
			std::string implementation;
		};
		///@endcond
		
		
	}
}
