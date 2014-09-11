#pragma once

#include <vector>
#include <string>

#include "Types.h"
#include "Enum.h"
#include "Containers/Collection.h"
#include "Any.h"

#include "Scripting/Reflection.h"

namespace Gorgon {

	/** This namespace contains Gorgon Script parser and reflection facilities.
	 * Gorgon Script allows applications to have embedded scripting capabilities. This
	 * scripting system has two dialects. First one is console dialect. This dialect
	 * allows fast command entry much like Bash script. Strings does not need to be
	 * quoted, function parameters are separeted by space. Nested functions should contain
	 * surrounding paranthesis.
	 * Second dialect is the programming dialect. 
	 * In this dialect strings should be quoted, function parameters should be placed inside 
	 * paranthesis, and they should be separated using comma. Additionally, scripting dialect
	 * allows lines to be terminated using semicolumn. Much like in Javascript, semicolumn 
	 * is not mandatory.
	 */
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
		
		
		/// Stores information about a scope. Some scopes does not define regular variables to
		/// themselves. For instance a variable that is set in an if scope will be visible from
		/// outside unless specified as local. However, a variable defined in function will be
		/// stored inside its own scope
		class Scope {
		};
		
		/// This class is used to create linking to an embedded function. You may use MakeStub
		/// to create stub function that handles parameter unboxing
		class EmbeddedFunction : public Function {
		public:
			
			/// Implementation for this function
			std::function<Data(std::vector<Data>&)> Implementation;
		};
		
		
		///@cond INTERNAL
		class CustomFunction : public Function {
		public:
			
			std::string implementation;
		};
		///@endcond
		
		/** Data describes a piece of data. It contains boxed data and the type. Additionally,
		 * a data can be an array, or reference to a variable. Data can have two tags: ArrayTag
		 * and ReferenceTag. It is possible to use both tags together to create an array of
		 * references. When a data is a reference, its type indicates which type it refers to.
		 * However, the type it refers to could also be an array. This allows two dimentional
		 * arrays to exist. However, reference to reference is not valid for now. Data is 
		 * non-mutable after construction.
		 */
		class Data {
		public:
			
			/// Any constructor. Allows both data and type to be specified
			template <class ...P_>
			Data(Type &type, Any data, P_ ...args);
			
			/// Default value constructor. Value of the data is determined from the type
			Data(Type &type);
			
			/// Default value constructor. Value of the data is determined from the type
			template <class ...P_>
			Data(Type &type, Tag firsttag, P_ ...args);
			
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
			Type &GetType() const {
				return type;
			}
			
		protected:
			/// @cond INTERNAL
			void UnpackTags() {}
			
			template<class ...P_>
			void UnpackTags(Tag first, P_ ...rest) {
				switch(first) {
					case ArrayTag:
						array=true;
						break;
					case ReferenceTag:
						reference=true;
						break;
				}
				
				UnpackTags(rest...);
			}
			/// @endcond
			
			Any   data;
			Type *type;
			
			bool array     = false;
			bool reference = false;
		};
		
		/// This class represents a variable. It contains the data and the name of the variable.
		class Varible : public Data {
		public:
			
			/// Sets the data contained in this variable without changing its type
			void Set(Any value) {
				data=value;
			}
			
			/// Sets the data contained in this variable by modifying its type. Also this function
			/// resets the tags unless they are respecified
			template<class ...P_>
			void Set(Type &type, Any value, P_ ...tags) {
				data=value;
				this->type=&type;
				
				array=false;
				reference=false;
				
				UnpackTags(tags...);
			}
			
			
		protected:
			std::string name;
		};
		
		/// Libraries are collection of types and functions. Every library has its own namespace.
		/// However, all functions in all libraries can be accessed without specifying the namespace.
		/// It is useful when names clash when they can be used to disambiguate function and type names
		class Library {
		public:
			std::string Name;
			
			Containers::Collection<Type> Types;
			Containers::Collection<Function> Functions;
		};
		
		/// This class defines a virtual environment for scripts to run. It determines
		/// which additional functions the script can access, working directory, and safety
		/// level.
		class VirtualMachine {
		public:
			
			/// Default constructor
			VirtualMachine() : Libraries(libraries) { }
			
			/// Executes a single statement in this virtual machine
			bool ExecuteStatement(const std::string &code);
			
			/// This method starts the virtual machine
			void Start();
			
			/// Includes a new library to be used in this virtual machine
			void AddLibrary(const Library &library);
			
			/// Removes a library
			void RemoveLibrary(const Library &library);
			
			/// Sets the input source to read code lines from
			void SetSource(InputSource &source);
			
			/// Allows read-only access to libraries
			const Containers::Collection<const Library> &Libraries;
			
		private:
			Containers::Collection<const Library> libraries;
			
		};
	}
}
