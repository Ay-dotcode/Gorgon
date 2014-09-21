#pragma once

#include <vector>
#include <string>
#include <thread>

#include "Types.h"
#include "Enum.h"
#include "Containers/Collection.h"
#include "Any.h"

#include "Scripting/Reflection.h"
#include "Scripting/InputSource.h"

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
		
		
		/**
		 * Stores information about a scope. Some scopes does not define regular variables to
		 * themselves. For instance a variable that is set in an if scope will be visible from
		 * outside unless specified as local. However, a variable defined in function will be
		 * stored inside its own scope
		 */
		class Scope {		};
		
		
		///@cond INTERNAL
		class CustomFunction : public Function {
		public:
			
			std::string implementation;
		};
		///@endcond
		
		
		/** 
		 * Data describes a piece of data. It contains boxed data and the type. Additionally,
		 * a data can be an array, or reference to a variable. Data can have two tags: ArrayTag
		 * and ReferenceTag. It is possible to use both tags together to create an array of
		 * references. When a data is a reference, its type indicates which type it refers to.
		 * However, the type it refers to could also be an array. This allows two dimensional
		 * arrays to exist. However, reference to reference is not valid for now. Data is 
		 * non-mutable after construction.
		 */
		class Data {
		public:
			
			/// Constructs an invalid data object. Data object does not perform validity check
			/// therefore, use of this function should be very limited.
			static Data Invalid() { return {}; }
			
			/// Any constructor. Allows both data and type to be specified
			Data(const Type &type, Any data) : type(&type), data(data) {
			}
			
			/// Any constructor. Allows both data and type to be specified
			Data(const Type *type, Any data) : type(type), data(data) {
				assert(type!=nullptr && "Data type cannot be nullptr");
			}
			
			/// Default value constructor. Value of the data is determined from the type
			Data(Type &type);
			
			
			/// Returns the value of this data in the requested format
			template <class T_>
			T_ GetValue() const {
				return data.Get<T_>();
			}
			
			/// Returns the data contained in this data element
			Any GetData() const {
				return data;
			}
			
			/// Returns the type of the data
			const Type &GetType() const {
				assert(type && "Type is not set");
				
				return *type;
			}
			
		protected:
			Any   data;
			const Type *type = nullptr;
			
			
		private:
			/// Constructs an invalid data. Performing any operation on this data might cause
			/// crashes. Never use this constructor unless its absolutely necessary
			Data() {}
		};
		
		inline std::ostream &operator<<(std::ostream &out, const Data &data) {
			
			return out;
		}
		
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
		
		
		/// This class defines a virtual environment for scripts to run. It determines
		/// which additional functions the script can access, working directory, and safety
		/// level.
		class VirtualMachine {
		public:
			
			/// Default constructor
			VirtualMachine() : Libraries(libraries), output(&std::cout), input(&std::cin) { }
			
			/// Executes a single statement in this virtual machine
			bool ExecuteStatement(const std::string &code);
			
			/// This method starts the virtual machine
			void Start();
			
			/// Includes a new library to be used in this virtual machine
			void AddLibrary(const Library &library) { 
				libraries.Add(library);
			}
			
			/// Removes a library
			void RemoveLibrary(const Library &library) {
				libraries.Remove(library);
			}
			
			const Type &FindType(const std::string &name, const std::string &namespc="");
			
			const Function &FindFunction(const std::string &name, const std::string &namespc="");
			
			const Constant &FindConstant(const std::string &name, const std::string &namespc="");
			
			/// Sets the input source to read code lines from
			void SetSource(InputSource &source);
			
			/// Allows read-only access to libraries
			const Containers::Collection<const Library> &Libraries;
			
			static VirtualMachine &Get() {
				if(!activevms.Exists(std::this_thread::get_id())) {
					throw std::runtime_error("No active VMs for this thread.");
				}
				
				return activevms[std::this_thread::get_id()];
			}
			
			std::ostream &GetOutput() const {
				return *output;
			}
			
			std::istream &GetInput() const {
				return *input;
			}
			
			void Activate() {				
				activevms.Add(std::this_thread::get_id(), this);
			}
			
		private:
			Containers::Collection<const Library> libraries;
			//scopes
			//variables
			
			std::ostream *output;
			std::istream *input;
			
			static Containers::Hashmap<std::thread::id, VirtualMachine> activevms;
		};
	}
}
