#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "Data.h"

namespace Gorgon { namespace Scripting {
	
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
		intptr_t Result;
		
		/// Used for literal values
		Data Literal;
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
		
		/// The value that will be assigned to the variable
		Value RHS;
		
		/// Parameters of the function.
		std::vector<Value> Parameters;
	};

	class ParserBase {
	public:
		virtual Instruction Parse(const std::string &input) = 0;
	
	};
	
	/*class IntermediateParser {
	public:
		virtual Instruction Parse(std::string input) override {
			
			Instruction instruction;
			
			char current, previous;
			
			bool first = true;
			bool escaped = false;
			bool inquote = false;
			
			unsigned int quote = 0;
			unsigned int slash = 0;
			unsigned int parameter;
			
			ValueType type = ValueType::None;
			
			std::string accumulator;
			
			for(std::size_t i = 0; i < input.size(); i++) {
				first = i == 0 ? true : false;
				current = input[i];
				previous = !first ? input[i - 1];
				
				if(first && current == '#') break; // comment
								
				if(current == '"') {
					quote++;
					if(quote == 1) {
						if(!first && previous == '\\') {
							escaped = true;
							quote--;
						}
						else {
							inquote = true;
						}
					}
					else if(quote == 2) {
						if(slash) {
							slash--;
							escaped = true;
							quote--;
						}
						else {
							inquote = false;
							quote = 0;
							if(accumulator.empty()) {
								;// error
							}
						}
					}
				}
				
				else if(current == '\\') {
					slash++;
					if(escaped) {
						escaped = false;
						continue;
					}
					else if(!first && previous == '\\') {
						slash = 0;
						escaped = true;
					}
				} 
				
				else if(current == '=' && !inquote) {
					instruction.Type = InstructionType::Assignment;
					if(accumulator.empty()) {
						;// error
					}
					instruction.Name = accumulator;
					accumulator.clear();
				}
				else if(current = '' && !inquote) {
					if(!parameter) {
						// first chunk is parsed as function name
						instruction.Type = InstructionType::FunctionCall;
						if(accumulator.empty()) {
							;// error
						}
						instruction.Name = accumulator;
						accumulator.clear();
						paramstart = i + 1;
					}
					// not first chunk, meaning this is parameter 
					// last parameter is parsed outside of loop
					else {
						if(accumulator.empty()) {
							;// error
						}
						instruction.Parameters.push_back(extractvalue(accumulator, type));
						accumulator.clear();
					}
					parameter++;
					
				}
				
				else if(current == '$' && !inquote) { type = ValueType::Variable; }
				else if(current == '.' && !inquote) { type = ValueType::Temp; }
				else if(current == 'c' && !inquote) { type = ValueType::Constant; }
				else if((current == 's' || current == 'f' || current == 'i' || current == 'b') && !inquote) { type = ValueType::Literal; }
				
				
				if((!isspecial(current) && !inquote) || escaped) {
					accumulator += current;
				}
			}
		} 
	private:
		Value extractvalue(const std::string &str, ValueType type) {
			Value value;
			value.type = type;
			if(type == ValueType::Variable || type == VariableType::Constant) { value.Name = str; }
			// else if(type ==  ValueType::Temp) { value.Result = ?? }
			// else if(type == ValueType::Literal) { value.Data = ?? }
			else ; // error
		}
		
	}*/

} }
