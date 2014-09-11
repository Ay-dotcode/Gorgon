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
		class Data;
		class VirtualMachine;
		
		
		/// Tags define behavior of reflection objects
		enum Tag {
			/// Marks the object as optional
			OptionalTag,
			
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
			
			/// Makes an object static
			StaticTag,
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
			bool reference = false;
			bool input     = true;
			bool output    = false;
		};
		
		using ParameterList = std::vector<Parameter>;
		
		/** This class stores information about a function. Functions can be related to a type
		 * or stand alone functions in libraries. EmbeddedFunction class should be used to 
		 * functions that are pointing to C++ function stubs. These stubs should extract and
		 * forward parameters to real functions. Functions are non-mutable after construction.
		 * Function names should be unique in their respective scopes case-insensitively. Console
		 * dialect allows calling method functions lowercase (*this might get extended to programming
		 * dialect*).
		 * 
		 * Function object allows following tags:
		 * 
		 * **KeywordTag**: Makes this function a keyword. Keywords acts differently in programming
		 * dialect. Instead of requiring paranthesis and commas between elements, it acts like in
		 * console dialect. This can be further exploited to have a single parameter with setting
		 * StretchTag. This way, a keyword can parse its own parameters. This allows function keyword
		 * to have `function name(type param, type param...)` returns type syntax
		 * 
		 * **NeverSkipTag**: Due to control structures, some scopes are skipped by virtual machine.
		 * When this tag is attached to this function, virtual machine will never skip it. This is
		 * required for scoped keywords. It could also be helpful for writing debugging functions.
		 * A function marked with NeverSkipTag should always check current state of the virtual machine
		 * so that it will not perform any operation with side effects. If the machine is in skipping
		 * mode, a scoped keyword should increase scope depth, so that end keywords will match properly.
		 * A custom function that has scoping should have a section called `scopeend`.
		 * 
		 * **StretchTag**: If this tag is set, in console dialect, spaces in the last parameter are not 
		 * treated as parameter separator as if it is in quotes. Helpful for functions like echo. But also 
		 * helpful for keywords that requires their own parsing.
		 * 
		 * **RepeatTag**: This tag makes the last parameter repeatable. This way, the function can be 
		 * called by as many parameters as user likes. In embedded functions, these parameters are also
		 * placed in the data list. In custom functions, the parameter that is marked as repeatable can 
		 * be accessed like an array.
		 * 
		 * **MethodTag**: This tag denotes that this function has a method variant. Method variant is called 
		 * in console dialect when the return value is not queried. In programming dialect method version of a 
		 * function can be called using call function. Methods are expected to convey their output directly
		 * to the stdout. For instance, a function that normally returns elements of an object could list the 
		 * elements to stdout when invoked as a method.
		 * 
		 * **PrivateTag**: If this function is a member function, this tag will make it private. This tag is
		 * not useful for embedded functions. PublicTag can also be used for clarity.
		 * 
		 * **StaticTag**: Only works when this function is a class member. Marks the function as static. Static
		 * functions can be accessed from the type using scope resolution: [type]function
		 */
		class Function {
		public:
			
			/// Function constructor fully constructs a function object. Both return type and tags are
			/// optional and its possible to skip return type and specify tags directly.
			template<class ...P_>
			Function(const std::string &name, const std::string &help, ParameterList parameters,
					 const Type &returntype, P_ ...tags) :
			name(name), help(help), returntype(&returntype), Parameters(this->parameters)
			{
				using std::swap;
				
				swap(parameters, this->parameters);
				UnpackTags(tags...);
			}
			
			/// @cond INTERNAL
			template<class ...P_>
			Function(const std::string &name, const std::string &help, ParameterList parameters,
					 Tag firsttag, P_ ...tags) : 
			name(name), help(help), Parameters(this->parameters)
			{
				using std::swap;
				
				swap(parameters, this->parameters);
				UnpackTags(firsttag);
				UnpackTags(tags...);
			}
			
			Function(const std::string &name, const std::string &help, ParameterList parameters) : 
			name(name), help(help), Parameters(this->parameters)
			{
				using std::swap;
				
				swap(parameters, this->parameters);
			}

			Function(const std::string &name, const std::string &help, ParameterList parameters,
					 const Type &returntype, const std::vector<Tag> &tags) :
			name(name), help(help), returntype(&returntype), Parameters(this->parameters)
			{
				using std::swap;
				
				swap(parameters, this->parameters);
				for(auto tag : tags) {
					UnpackTags(tag);
				}
			}

			Function(const std::string &name, const std::string &help, ParameterList parameters,
					 const std::vector<Tag> &tags) :
			name(name), help(help), Parameters(this->parameters)
			{
				using std::swap;
				
				swap(parameters, this->parameters);
				for(auto tag : tags) {
					UnpackTags(tag);
				}
			}
			/// @endcond
			
			/// Returns the name of this function.
			std::string GetName() const {
				return name;
			}
			
			/// Returns the help string. Help strings may contain markdown notation.
			std::string GetHelp() const {
				return help;
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
			
			/// Returns if this function is actually a keyword.
			bool IsKeyword() const {
				return keyword;
			}
			
			/// Returns whether this function should never be skipped. This property can be true even if
			/// this function is not a keyword
			bool NeverSkip() const {
				return neverskip;
			}
			
			/// Returns if the last parameter of this function should be stretched. 
			/// If true, in console dialect, spaces in the last parameter are not treated as parameter
			/// separator as if it is in quotes. Helpful for functions like echo. But also helpful for
			/// keywords that requires their own parsing.
			bool StretchLast() const {
				return stretchlast;
			}
			
			/// Returns if the last parameter of this function should be repeated. If true last parameter 
			/// can be specified any number of times. This number can be obtained from data list supplied 
			/// to function stub. If both StretchTag and RepeatTag is specified, in console dialect stretch
			/// is used, while in programming dialect repeat is used.
			bool RepeatLast() const {
				return repeatlast;
			}
			
			/// Returns whether this function has a method variant. Method variant is called in console dialect
			/// when the return value is not queried. In programming dialect, method variant of a function
			/// can be called using call function. Methods are expected to convey their output directly
			/// to the stdout. For instance, a function that normally returns elements of an object could
			/// list the elements to stdout when invoked as a method. 
			bool HasMethod() const {
				return method;
			}
			
			/// Returns if this function is static. Only meaningful when the function is a member function.
			bool IsStatic() const {
				return staticmember;
			}

			/// Returns if this function is a member function of a type.
			bool IsMember() const {
				return parent!=nullptr;
			}
			
			/// If this function is a member function, returns the owner object. If this function is not a
			/// member function, this function crashes.
			const Type &GetOwner() const {
				assert(parent && "This class does not gave an owner.");
				
				return *parent;
			}
			
			/// If this function is a member function, this function returns if it is publically accessible
			bool IsAccessible() const {
				return accessible;
			}
			
			virtual Data Call(bool ismethod, VirtualMachine &machine, std::vector<Data> &parameters) = 0;
			
			
			/// Parameters that this function have
			const ParameterList &Parameters;
			
		protected:
			
			/// @cond INTERNAL
			void UnpackTags() {}
			
			template<class ...P_>
			void UnpackTags(Tag tag, P_ ...rest) {
				switch(tag) {
					case KeywordTag:
						keyword=true;
						break;
					case NeverSkipTag:
						neverskip=true;
						break;
					case StretchTag:
						stretchlast=true;
						break;
					case RepeatTag:
						repeatlast=true;
						break;
					case MethodTag:
						method=true;
						break;
					case StaticTag:
						staticmember=true;
						break;
					case PrivateTag:
						accessible=false;
						break;
					case PublicTag:
						accessible=true;
						break;
					default:
						assert(false && "Unknown tag");
				}
				
				UnpackTags(rest...);
			}
			/// @endcond
			
			/// Return type of the function could be nullptr denoting it is void.
			const Type *returntype = nullptr;
			
			/// List of parameters
			ParameterList parameters;
			
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
			
			/// If true last parameter can be specified any number of times. This number can be obtained
			/// from data list supplied to function stub.
			bool repeatlast = false;
			
			/// If true, this function has a method variant. Method variant is called in console dialect
			/// when the return value is not queried. In programming dialect method version of a function
			/// can be called using call function. Methods are expected to convey their output directly
			/// to the stdout. For instance, a function that normally returns elements of an object could
			/// list the elements to stdout when invoked as a method.
			bool method = false;
			
			/// Only meaningful in class member functions. If true, makes this function static.
			bool staticmember = false;
			
			/// Only meaningful in class member functions. If true this function can be access from outside
			/// the type itself
			bool accessible = true;
			
		private:
			const Type *parent;
		};
		
		/// Data members that can be accessed through an instance of the a type. 
		class DataMember {
		public:
			template<class ...P_>
			DataMember(const std::string &name, const std::string &help, const Type &type, P_ ...tags) :
			name(name), help(help), type(&type) {
				UnpackTags(tags...);
			}
			
		protected:
			/// @cond INTERNAL
			void UnpackTags() { }
			
			template <class ...P_>
			void UnpackTags(Tag tag, P_... rest) {
				switch(tag) {
					default:
						;
				}
				
				UnpackTags(rest...);
			}
			/// @endcond
			
			/// The name of the datamember
			std::string name;
			
			/// Help string of the datamember
			std::string help;
			
			/// Type of the datamember
			const Type *type;
			
			/// Whether this member is publically accessible
			bool accessible = true;
			
			/// Whether this member is a static member that should be accessed from the type.
			bool staticmember = false;
		};
		
		/// Events allow an easy mechanism to program logic into actions instead of checking actions
		/// continously. This system is vital for UI programming. Events are basically function descriptors.
		/// Event handlers can access the object that is the source for event using $_eventsource variable.
		class Event {
		public:
			Event(const std::string &name, const std::string &help, ParameterList parameters) : 
			name(name), help(help), Parameters(this->parameters) {
				using std::swap;
				
				swap(parameters, this->parameters);
			}
			
			/// Read only list of parameters
			const ParameterList &Parameters;
		protected:
			/// Name of the event
			std::string name;
			
			/// Help string identifying the event
			std::string help;
			
			/// Parameters that every event handler should accept. 
			ParameterList parameters;
		};
		
		/** This class stores information about types. Types can have their own functions, members,
		 *  events and operators. Additionally some types can be converted to others, this information
		 *  is also stored in this class. 
		 */
		class Type {
			
			
		private:
			std::string name;
			std::string help;
			
			Any defaultvalue;
			
			Containers::Collection<Type> 		allowedconversions;
			std::map<std::string, Function&> 	classfunctions;
			std::map<std::string, Function&> 	instancefunctions;
			std::map<std::string, Function&> 	operators;
			std::map<std::string, Event>		events;
			std::map<std::string, DataMember>	members;

			bool referencetype = false;
		};
	}
}
