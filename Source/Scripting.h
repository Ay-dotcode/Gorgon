#include <vector>

#include "Types.h"
#include "Enum.h"
#include "Containers/Collection.h"
#include "Any.h"

namespace Gorgon {

	/// This namespace contains Gorgon Script parser and reflection facilities.
	/// Gorgon Script allows applications to have embedded scripting capabilities. This
	/// scripting system has two dialects. First one is console dialect. This dialect
	/// allows fast command entry much like Bash script. Strings does not need to be
	/// quoted, function parameters are separeted by space. Nested functions should contain
	/// surrounding paranthesis. Console dialect has some limitations. For instance 
	/// functions cannot have typed parameters, it will be possible to create custom types, 
	/// etc...
	/// Second dialect is the programming dialect. 
	/// In this dialect strings should be quoted, function parameters should be placed inside 
	/// paranthesis, and they should be separated using comma. Additionally, scripting dialect
	/// allows lines to be terminated using semicolumn. Much like in Javascript, semicolumn 
	/// is not mandatory.
	namespace Scripting {
		
		/// This class contains information about a parse error. It is not intended to be
		/// used as an exception.
		class ParseError {
		public:
			/// This enumaration lists all parse error types.
			enum ErrorCode {
				MismatchedParanthesis,
			}; 
			
			/// The code of the error
			ErrorCode Code;
			
			/// The line that contains parse error
			int Line;
			
			/// The exact character that contains parse error. If it cannot be determined
			/// it will reported as -1.
			int Char;
		};
		
		DefineEnumStringsCM(Scripting, ErrorCode, 
			{Scripting::MismatchedParanthesis, "Mismatched paranthesis"}
		);
		
		/// This function parses the code and returns any syntax errors. This function
		/// cannot check parse errors that can be caused by type assignments. Additionally
		/// whether a function exists or not cannot be determined as functions can be
		/// dynamically defined at runtime. The given code will be tokenized into lines.
		/// Additonally, any refered files will also be parsed for errors.
		std::vector<ParseError> Parse(const std::string &code);
		
		class Data {
		};
		
		/// This class stores information about a function. Functions can be related to a type
		/// or stand alone functions in libraries. EmbeddedFunction class should be used to 
		/// functions that are pointing to C++ function stubs. These stubs should extract and
		/// forward parameters to real functions.
		class Function {
		public:
			
			
			virtual Data Execute(std::vector<Data> &data) = 0;
			
			/// The name of the function
			std::string Name;
			
			
		protected:
		};
		
		/// This class is used to create 
		class EmbeddedFunction : public Function {
		public:
			
			std::function<Data(std::vector<Data>&)> implementation;
		};
		
		
		///@cond INTERNAL
		class CustomFunction : public Function {
		public:
			
			std::string implementation;
		};
		///@endcond
		
		/// Data members that can be accessed through an instance of the a type. 
		class DataMember {
		};
		
		/// Stores information about a scope. Some scopes does not define regular variables to
		/// themselves. For instance a variable that is set in an if scope will be visible from
		/// outside unless specified as local. However, a variable defined in function will be
		/// stored inside its own scope
		class Scope {
		};
		
		/// This class stores information about types. Types can have their own functions, members,
		/// events and operators. Additionally some types can be converted to others, this information
		/// is also stored in this class.
		class Type {
			
		private:
			bool valuetype;
			Containers::Collection<Type> allowedconversions;
			std::vector<Function> classfunctions;
			std::vector<Function> instancefunctions;
			std::vector<Function> operators;
			std::vector<Event> events;
			std::map<std::string, Any> options;
		};
		
		/// Base class for input sources. This system allows different input sources to supply 
		/// code to virtual machine. Each input source has its own line numbering, a name and
		/// an index to identify it. For instance if the same file is read twice, first one will
		/// be named as file.gs#1 and the other will be file.gs#2
		class InputSource {
		};
		
		/// Events allow an easy mechanism to program logic into actions instead of checking actions
		/// continously. This system is vital for UI programming
		class Event {
		};
		
		/// Libraries are collection of types and functions. Every library has its own namespace.
		/// However, all functions in all libraries can be accessed without specifying the namespace.
		/// It is useful when names clash when they can be used to disambiguate function and type names
		class Library {
		public:
		};
		
		/// This class defines a virtual environment for scripts to run. It determines
		/// which additional functions the script can access, working directory, and safety
		/// level.
		class VirtualMachine {
		public:
			
			/// Executes a single statement in this virtual machine
			bool ExecuteStatement(const std::string &code);
			
			/// This method starts the virtual machine
			void Start();
			
			/// Includes a new library to be used in this virtual machine
			void AddLibrary(const Library &library);
			
		private:
			Containers::Collection<const Library> libraries;
			
		};
	}
}
