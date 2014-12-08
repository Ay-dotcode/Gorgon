#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "Data.h"
#include "../Types.h"

namespace Gorgon {
	namespace Scripting {
		namespace internal { struct node; void testlexer(const std::string &input, std::ostream *cases); };

		
		
		/// Describes the type of an instruction
		enum class InstructionType {

			/// Marks the start of a function call, intended for keywords only
			Mark = 1,

			/// Marks instruction as a regular function call. Regular function calls can also be member functions,
			/// however, they cannot be data members.
			FunctionCall,

			/// Marks this instruction as a member function call. This means the function is either a data member
			/// and will return or set the value of the member or a member function that will be called.
			MemberFunctionCall,	

			/// Marks this instruction as a method call.
			MethodCall,


			/// Marks this instruction as a member method call. This means the function is either a data member
			/// and will return or set the value of the member or a member function that will be called.
			MemberMethodCall,

			/// Marks instruction as an assignment
			Assignment,
			
			/// Marks instruction as a removal of a temporary. Temporary index should be stored in the Store member
			RemoveTemp,
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
		
		/// This class contains a parsed value. A value can be a temporary, literal, variable or a constant.
		/// For variables and constants, Name is stored. For a temporary, Result is stored, for a literal
		/// Value is stored.
		struct Value {
			/// Used for variables and constants
			std::string Name;

			/// Used for literal values
			Data Literal=Data::Invalid();

			/// Type of this value.
			ValueType Type;

			/// Used for temporary results
			Byte Result;
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
			
			/// The value that will be assigned to the variable
			Value RHS;
			
			/// Parameters of the function.
			std::vector<Value> Parameters;

			/// Whether to store the result of the function
			Byte Store;
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
			
			void fncall(const std::string &input, int &ch, bool allowmethod=true);
			
			void varassign(const std::string &input, int &ch);
			
			Value parsevalue(const std::string &input, int &ch);
			
			unsigned long parsetemporary(const std::string &input, int &ch);
			
			void eatwhite(const std::string &input, int &ch);
		};


		class ProgrammingParser: public ParserBase {
		public:

			virtual unsigned Parse(const std::string &input) override { return 0;  }

			internal::node *parse(const std::string &input);

		private:			

		};
	}
}
