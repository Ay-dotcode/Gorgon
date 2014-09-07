#pragma once

#include <vector>
#include <string>
#include <assert.h>

#include "../Types.h"
#include "../Containers/Collection.h"
#include "../Any.h"

namespace Gorgon {
	
	namespace Scripting {
		class Type;
		
		enum Tag {
			OptionalTag,
			ArrayTag,
			ReferenceTag,
			InputTag,
			OutputTag,
			MethodTag,
			RepeatTag,
			StretchTag,
			KeywordTag,
			NeverSkipTag,
		};
		
		/**This class represents a function parameter description. It does not contain any data. 
		 * Parameter is non-mutable after construction
		 * 
		 * A parameter can be controlled using tags. Parameters can have following tags: 
		 * 
		 * **OptionalTag**: This parameter becomes an optional parameter. Optional parameters should
		 * either be at the beginning or end of the parameter list.
		 * 
		 * **ArrayTag**: This parameter becomes an array. An array parameter can be specified using 
		 * square brackets.
		 * 
		 * **ReferenceTag**: This parameter becomes bidirectional reference. That is, it needs to exist
		 * before the call to the function. The value of the parameter can be modified or left as is.
		 * 
		 * **OutputTag**: This parameter becomes output only reference. Output only references can be
		 * read after it is set first time. The function is free not to set the value of an output
		 * reference, however, this may cause confusion to the user.
		 */
		class Parameter {
		public:
			
			/// Constructs a new parameter. All information regarding to a parameter should be specified
			/// in the constructor. After construction parameter is non-mutable. Both options and tags
			/// are optional
			template <class ...Params_>
			Parameter(const std::string &name, const Type &type, const std::string &help, 
					  std::vector<const Any> options, Params_ ...tags) : 
			name(name), type(&type), help(help), Options(options) {
				UnpackTags(tags);
				using std::swap;
				swap(options, this->options);
			}
			
			///@cond INTERNAL
			Parameter(const std::string &name, const Type &type, const std::string &help) : 
			Parameter(name, type, help, std::vector<Any>{}) { }
			
			template <class ...Params_>
			Parameter(const std::string &name, const Type &type, const std::string &help, Tag firsttag,
					  Params_ ...tags) : 
			Parameter(name, type, help, std::vector<Any>{}, firsttag, std::forward<Params_>(tags)...) {
			 }
			///@endcond
			
			/// Returns the name of the parameter
			std::string GetName() const {
				return name;
			}
			
			/// Returns the help related with this parameter
			std::string GetHelp() const {
				return help;
			}
			
			/// Returns the type of the parameter
			Type &GetType() const {
				return type;
			}
			
			/// Checks if the parameter is optional
			bool IsOptional() const {
				return optional;
			}
			
			/// Checks if the parameter is a reference. When a parameter is a reference,
			/// instead of the value of the variable the variable itself is passed. Making
			/// a parameter reference automatically make the parameter output. However,
			/// unless OutputTag is specified, parameter will be bidirectional, meaning
			/// it should exist. If the variable is output only, then an undefined
			/// variable can also be sent.
			bool IsReference() const {
				return reference;
			}
			
			/// Checks if changes to this parameter is propagated. Always returns same value
			/// as IsReference
			bool IsOutput() const {
				return output;
			}
			
			/// Checks if the parameter value will be read. If a parameter is not a reference,
			/// IsInput is always true. When ReferenceTag is set, IsInput might be true or false
			/// depending on whether OutputTag is sent.
			bool IsInput() const {
				return input;
			}
			
			/// Checks if this parameter is an array instead of a single value. 
			bool IsArray() const {
				return array;
			}
			
			/// Allowed values for this parameter
			const std::vector<const Any> &Options;
			
		private:
			void UnpackTags() {}
			
			template<class ...Params_>
			void UnpackTags(Tag tag, Params_ ...tags) {
				switch(tag) {
					case OptionalTag:
						optional=true;
						break;
					case ArrayTag:
						array=true;
						break;
					case ReferenceTag:
						reference=true;
						output=true;
						break;
					case OutputTag:
						reference=true;
						output=true;
						input=false;
						break;
					default:
						assert(false && "Unknown tag");
				}
				UnpackTags(tags...);
			}
			
			
			std::string name;
			std::string help;
			const Type *type;
			
			std::vector<const Any> options;
			
			bool optional  = false;
			bool array     = false;
			bool reference = false;
			bool input     = true;
			bool output    = false;
		};
		
		
		
		/** This class stores information about a function. Functions can be related to a type
		 * or stand alone functions in libraries. EmbeddedFunction class should be used to 
		 * functions that are pointing to C++ function stubs. These stubs should extract and
		 * forward parameters to real functions. Functions are non-mutable after construction.
		 * Function names should be unique in their respective scopes case-insensitively. Console
		 * dialect allows calling method functions lowercase (*this might get extended to programming
		 * dialect*).
		 */
		class Function {
		public:
			
			
		protected:
			/// Return type of the function could be nullptr denoting it is void.
			Tag *returntype = nullptr;
			
			/// The name of the function
			std::string name;
			
			/// Help string of the function
			std::string help;
			
			/// This function is treated as a keyword. That is, it does not require parenthesis in
			/// programming dialect and its parameters are always separated by space. 
			bool keyword = false;
			
			/// Never skip functions are always called, even if code execution is being skipped. However,
			/// to avoid any side effects, they should mind whether the virtual machine is currently
			/// skipping. All scoped functions (if, for, etc...) should set both keyword and neverskip
			/// tags.
			bool neverskip = false;
			
			/// If true, in console dialect, spaces in the last parameter are not treated as parameter
			/// separator as if it is in quotes. Helpful for functions like echo
			bool stretchlast = false;
			
			/// If true last parameter can be specified any number of times. This parameter should be
			/// an array for this tag to work correctly.
			bool repeatlast = false;
			
			/// If true, this function has a method version. Method version is called in console dialect
			/// when the return value is not queried. In programming dialect method version of a function
			/// can be called using call function. Methods are expected to convey their output directly
			/// to the stdout. For instance, a function that normally returns elements of an object could
			/// list the elements to stdout when invoked as a method.
			bool method = false;
			
		};
		
		/// Data members that can be accessed through an instance of the a type. 
		class DataMember {
		};
		
		/// Events allow an easy mechanism to program logic into actions instead of checking actions
		/// continously. This system is vital for UI programming
		class Event {
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
		};		
	}
}