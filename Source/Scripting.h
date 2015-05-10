#pragma once

#include <vector>
#include <string>
#include <set>

#include "Types.h"
#include "Enum.h"
#include "Containers/Collection.h"
#include "Any.h"

#include "Scripting/Reflection.h"
#include "Scripting/Data.h"

namespace Gorgon {

	/** 
	 * This namespace contains Gorgon Script parser and reflection facilities.
	 * @see GScript
	 */
	namespace Scripting {
		
		/**
		 * @page GScript Gorgon Script
		 * 
		 * Gorgon::Scripting allows embedded scripting capabilities to the users of the library. It has multiple 
		 * dialects and an easy method for embedding functions.
		 * 
		 * Gorgon Script allows applications to have embedded scripting capabilities. This
		 * scripting system has two dialects. First one is console dialect. This dialect
		 * allows fast command entry much like Bash script. Strings does not need to be
		 * quoted, function parameters are separated by space. Nested functions should contain
		 * surrounding parenthesis.
		 * 
		 * Second dialect is the programming dialect. 
		 * In this dialect strings should be quoted, function parameters should be placed inside 
		 * parenthesis, and they should be separated using comma. Additionally, scripting dialect
		 * allows lines to be terminated using semicolon. Much like in Javascript, semicolon 
		 * is not mandatory.
		 */
		
		/**
		 * @page GScript
		 * @subpage GScript-todo
		 * 
		 */
		
		 /** 
		 * @page GScript-todo TODO
		 * Tasks that are left for later:
		 * * Save compiled instructions to a file
		 * * Events
		 * * Custom types
		 * * Range operator (..)
		 * 
		 */
		
		/** 
		 * This function parses the code and returns any syntax errors. This function
		 * cannot check parse errors that can be caused by type assignments. Additionally
		 * whether a function exists or not cannot be determined as functions can be
		 * dynamically defined at runtime. The given code will be tokenized into lines.
		 * Additionally, any referred files will also be parsed for errors.
		 */
		//!might be removed
		std::vector<ParseError> Parse(const std::string &code);

		/// @cond INTERNAL
		void init_builtin();
		/// @endcond
		
		/// Prints out a data
		inline std::ostream &operator<<(std::ostream &out, const Data &data) {
			if(!data.IsValid()) {
				out<<"[ INVALID ]";
			}
			else {
				out<<data.GetType().ToString(data);
			}
			
			return out;
		}
		
		/// Initializes the scripting system
		inline void Initialize() {
			init_builtin();
		}
		
		/// This library requires Initialize to be called
		extern Library Integrals;
		extern Library Keywords;
		extern Library Reflection;
		
		extern std::set<std::string, String::CaseInsensitiveLess> KeywordNames;
		
		/// Allows easy and fast access to integral types
		namespace Types {
#define DEFTYPE(name) inline const Type &name() { static const Scripting::Type *type = Integrals.Types[#name]; return *type; }

			DEFTYPE(Variant);
			DEFTYPE(String);
			DEFTYPE(Int);
			DEFTYPE(Float);
			DEFTYPE(Bool);
			DEFTYPE(Double);
			DEFTYPE(Char);
			DEFTYPE(Byte);
			DEFTYPE(Unsigned);

			DEFTYPE(Array);

#undef DEFTYPE
			
#define DEFTYPE(name) inline const Scripting::Type &name() { static const Scripting::Type *type = Reflection.Types[#name]; return *type; }
			
			DEFTYPE(Type);
			DEFTYPE(Function);
			
#undef DEFTYPE
		}
	}
}
