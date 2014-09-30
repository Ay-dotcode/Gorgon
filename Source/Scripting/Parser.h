#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "Data.h"

namespace Gorgon { namespace Scripting {
	
	/// Describes the type of an instruction
	enum InstructionType {
		/// Marks instruction as a function call.
		FunctionCall,
		
		/// Marks instruction as an assignment
		Assignment
	};
	
	/// Possible value types
	enum ValueType {
		/// This value is a temporary and refers to a value calculated as the result
		/// of a function call
		Temp,
		
		/// This is a literal value
		Literal,
		
		/// This is a variable
		Variable,
		
		/// Marks this value as a constant
		Constant,
	};
	
	/// This class contains a parsed value. A value can be a temporary, literal, variable of a contant
	/// for variables and constants, Name is stored. For a temporary, Result is stored, for a literal
	/// Value is stored.
	struct Value {
		/// Type of this value.
		ValueType Type;
		
		union {
			/// Used for variables and constants
			std::string Name;
			
			/// Used for temporary results
			intptr_t Result;
			
			/// Used for literal values
			Data Value;
		};
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
		bool Store;
		
		union {
			/// The value that will be assigned to the variable
			Value RHS;
			
			/// Parameters of the function.
			std::vector<Value> Parameters;
		};
	};

} }
