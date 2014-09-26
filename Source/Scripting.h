#pragma once

#include <vector>
#include <string>

#include "Types.h"
#include "Enum.h"
#include "Containers/Collection.h"
#include "Any.h"

#include "Scripting/Reflection.h"
#include "Scripting/InputSource.h"
#include "Scripting/Data.h"

namespace Gorgon {

	/** 
	 * This namespace contains Gorgon Script parser and reflection facilities.
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
	namespace Scripting {
		
		/// This class contains information about a parse error. It is not intended to be
		/// used as an exception.
		class ParseError {
		public:
			/// This enumeration lists all parse error types.
			enum ErrorCode {
				MismatchedParenthesis,
			}; 
			
			/// The code of the error
			ErrorCode Code;
			
			/// The line that contains parse error
			int Line;
			
			/// The exact character that contains parse error. If it cannot be determined
			/// it will reported as -1.
			int Char;
		};
		
		DefineEnumStringsCM(ParseError, ErrorCode, 
			{ParseError::MismatchedParenthesis, "Mismatched paranthesis"}
		);
		
		/** 
		 * This function parses the code and returns any syntax errors. This function
		 * cannot check parse errors that can be caused by type assignments. Additionally
		 * whether a function exists or not cannot be determined as functions can be
		 * dynamically defined at runtime. The given code will be tokenized into lines.
		 * Additionally, any referred files will also be parsed for errors.
		 */
		std::vector<ParseError> Parse(const std::string &code);
		
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
			void init_builtin();
			init_builtin();
		}
		
		/// This library requires Initialize to be called
		extern Library Integrals;
	}
}
