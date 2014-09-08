#pragma once

#include <vector>
#include <string>
#include <map>
#include <assert.h>

#include "../Types.h"
#include "../Containers/Collection.h"
#include "../Any.h"

namespace Gorgon {
	
	namespace Scripting {
		class Type;
		
		/// Tags define behavior of reflection objects
		enum Tag {
			/// Marks the object as optional
			OptionalTag,
			
			/// Marks the object as an array
			ArrayTag,
			
			/// Marks the object as a reference. When this tag affects types, it marks the
			/// type as a reference type, meaning it will always be moved around as a reference.
			/// When this tag affects parameters or variables, object becomes a reference to the
			/// parameter
			ReferenceTag,
			
			/// Marks the object as input
			InputTag,
			
			/// Marks the object as output. This may set ReferenceTag and unset InputTag.
			OutputTag,
			
			/// Denotes that a function has a method variant
			MethodTag,
			
			/// Marks an object as repeatable
			RepeatTag,
			
			/// Used only in functions with console dialect. When set, this tag allows last
			/// parameter to contain spaces
			StretchTag,
			
			/// Marks object as a keyword
			KeywordTag,
			
			/// Virtual machine may go into skipping mode due to various keywords. This tag
			/// marks this object not to be skipped by the virtual machine
			NeverSkipTag,
			
			/// Makes the object private, allowing only access from it parent
			PrivateTag,
			
			/// Makes the object public, allowing it to be accessed from all
			PublicTag,
		};
		
		typedef std::vector<Any> OptionList;
		
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
					 OptionList options, Params_ ...tags) : 
			name(name), type(&type), help(help), Options(options) {
				UnpackTags(tags...);
				using std::swap;
				swap(options, this->options);
			}
			
			///@cond INTERNAL
			Parameter(const std::string &name, const Type &type, const std::string &help) : 
			Parameter(name, type, help, OptionList{}) { }
			
			template <class ...Params_>
			Parameter(const std::string &name, const Type &type, const std::string &help, Tag firsttag,
					  Params_ ...tags) : 
			Parameter(name, type, help, OptionList{}, firsttag, std::forward<Params_>(tags)...) {
			}
			 
			Parameter(const std::string &name, const Type &type, const std::string &help, 
					  OptionList options, const std::vector<Tag> &tags) :
			Parameter(name, type, help, options) {
				for(auto tag : tags) {
					UnpackTags(tag);
				}
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
			const Type &GetType() const {
				return *type;
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
			const OptionList &Options;
			
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
			
			OptionList options;
			
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
			
			/// Function constructor fully constructs a function object. Both return type and tags are
			/// optional and its possible to skip return type and specify tags directly.
			template<class ...P_>
			Function(const std::string &name, const std::string &help, std::vector<Parameter> parameters,
					 const Type &returntype, P_ ...tags) :
			name(name), help(help), returntype(&returntype) 
			{
				using std::swap;
				
				swap(parameters, this->parameters);
				UnpackTags(tags...);
			}
			
			/// @cond INTERNAL
			template<class ...P_>
			Function(const std::string &name, const std::string &help, std::vector<Parameter> parameters,
					 Tag firsttag, P_ ...tags) : 
			name(name), help(help)
			{
				using std::swap;
				
				swap(parameters, this->parameters);
				UnpackTags(firsttag);
				UnpackTags(tags...);
			}
			
			Function(const std::string &name, const std::string &help, std::vector<Parameter> parameters) : 
			name(name), help(help)
			{
				using std::swap;
				
				swap(parameters, this->parameters);
			}
			/// @endcond
			
			/// Returns the name of this function.
			std::string GetName() const {
				return name;
			}
			
			/// Checks if this function returns anything
			bool HasReturnType() const {
				return returntype!=nullptr;
			}
			
			/// Returns the return type of this function. This value can be null, therefore, it should
			/// be checked before it is retrieved
			const Type &GetReturnType() const {
				assert(returntype && "This function does not have a return type");
				return *returntype;
			}
			
		protected:
			
			/// @cond INTERNAL
			void UnpackTags() {}
			
			template<class ...P_>
			void UnpackTags(Tag tag, P_ ...rest) {
				switch(tag) {
					default:
						assert(false && "Unknown tag");
				}
				
				UnpackTags(rest...);
			}
			/// @endcond
			
			/// Return type of the function could be nullptr denoting it is void.
			const Type *returntype = nullptr;
			
			/// List of parameters
			std::vector<Parameter> parameters;
			
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
		public:
			
		private:
			std::string name;
			std::string help;
			
			Type *type;
			
			bool array;
			bool reference;
			bool accessible;
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
			std::string name;
			std::string help;
			
			Any defaultvalue;
			
			Containers::Collection<Type> 		allowedconversions;
			std::map<std::string, Function> 	classfunctions;
			std::map<std::string, Function> 	instancefunctions;
			std::map<std::string, Function> 	operators;
			std::map<std::string, Event>		events;
			std::map<std::string, DataMember>	members;

			bool referencetype = false;
		};
	}
}