#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "Data.h"

namespace Gorgon {
	namespace Scripting {

		/// Describes the type of an instruction
		enum class InstructionType {
			/// Marks instruction as a function call.
			FunctionCall,

			/// Marks instruction as an assignment
			Assignment
		};

		/// Possible value types
		enum class ValueType {
			/// This value is a temporary and refers to a value calculated as the result
			/// of a function call
			Temp,
			
			/// This is a literal value
			Literal,
			
			/// This is a variable
			Variable,
			
			/// Marks this value as a constant
			Constant,
			
			/// For error checking purposes
			None,
		};
		
		/// This class contains a parsed value. A value can be a temporary, literal, variable of a contant
		/// for variables and constants, Name is stored. For a temporary, Result is stored, for a literal
		/// Value is stored.
		struct Value {
			/// Type of this value.
			ValueType Type;
			
			/// Used for variables and constants
			std::string Name;
			
			/// Used for temporary results
			unsigned long Result;
			
			/// Used for literal values
			Data Literal=Data::Invalid();
		};
		
		/**
		 * A single instruction. An instruction can either be a function call or an assignment.
		 *
		 * If it is a function call, Name field stores the name of the function. Store field decides
		 * whether the result of the instruction will be used and therefore, should be stored. Additionally
		 * Parameters stores the parameters of the function. If parameters contains expressions, they should
		 * be split into single instructions and their temporary values should be referenced.
		 *
		 * If the type of the instruction is assignment, Name field contains the name of the variable.
		 * Store field is not used and RHS field contains the right hand side value that should be assigned
		 * to the variable.
		 */
		struct Instruction {
			/// Type of the instruction
			InstructionType Type;
			
			/// Name of the function or variable
			Value Name;
			
			/// Whether to store the result of the function
			unsigned long Store;
			
			/// The value that will be assigned to the variable
			Value RHS;
			
			/// Parameters of the function.
			std::vector<Value> Parameters;
		};
		
		class ParserBase {
		public:
			
			virtual unsigned Parse(const std::string &input) = 0;
			
			std::vector<Instruction> List;
		};

		class IntermediateParser : public ParserBase {
		public:
			
			virtual unsigned Parse(const std::string &input) override;
			
private:
			void storedfn(const std::string &input, int &ch);
			
			void fncall(const std::string &input, int &ch);
			
			void varassign(const std::string &input, int &ch);
			
			Value parsevalue(const std::string &input, int &ch);
			
			unsigned long parsetemporary(const std::string &input, int &ch);
			
			std::string extractquotes(const std::string &input, int &ch);
			
			void eatwhite(const std::string &input, int &ch);
		};
	}
}
