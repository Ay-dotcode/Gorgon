#pragma once

#include <vector>
#include <string>
#include <map>
#include <assert.h>
#include <functional>

#include "../Types.h"
#include "../Containers/Collection.h"
#include "../Containers/Hashmap.h"
#include "../Any.h"
#include "../Utils/Assert.h"

#include "Data.h"
#include "Exceptions.h"

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
			//OutputTag,
			
			/// Denotes that a function has a method variant
			MethodTag,
			
			/// Marks an object as repeatable
			RepeatTag,
			
			/// Used only in functions with console dialect. When set, this tag allows last
			/// parameter to contain spaces
			StretchTag,
			
			/// Marks object as a keyword
			KeywordTag,
			
			/// Makes the object private, allowing only access from it parent
			PrivateTag,
			
			/// Makes the object public, allowing it to be accessed from all
			PublicTag,
			
			/// Makes an object static
			StaticTag,
			
			/// Makes a function operator
			OperatorTag,
			
			/// Marks a parameter or a function constant
			ConstTag,
			
			/// Makes this parameter a variable accepting parameter. Variable parameters are type checked
			/// against supplied type, however, they are always passed as strings denoting the name of the variable
			VariableTag,
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
		 * Reference parameters are either passed as pointer or reference. A reference parameter can be
		 * Const as well.
		 * 
		 * **ConstTag**: This parameter is a constant and its value cannot be changed.
		 * 
		 * **VariableTag**: Marks this parameter as a variable. Useful to allow an undefined variable to be
		 * passed to the function.
		 */
		class Parameter {
		public:
			
			/// Constructs a new parameter. All information regarding to a parameter should be specified
			/// in the constructor. After construction parameter is non-mutable. Both options and tags
			/// are optional
			template <class ...Params_>
			Parameter(const std::string &name, const std::string &help, const Type *type, 
					 OptionList options, Params_ ...tags) : 
			name(name), type(type), help(help), Options(std::move(options)) {
				ASSERT((type!=nullptr), "Parameter type cannot be nullptr", 1, 2);
				
				UnpackTags(tags...);
			}
			
			/// @cond INTERNAL
			Parameter(const std::string &name, const std::string &help, const Type *type) : 
			Parameter(name, help, type, OptionList{}) { }
			
			template <class ...Params_>
			Parameter(const std::string &name, const std::string &help, const Type *type, Tag firsttag,
					  Params_ ...tags) : 
			Parameter(name, help, type, OptionList{}, firsttag, std::forward<Params_>(tags)...) {
			}
			 
			Parameter(const std::string &name, const std::string &help, const Type *type, 
					  OptionList options, const std::vector<Tag> &tags) :
			Parameter(name, help, type, options) {
				for(auto tag : tags) {
					UnpackTags(tag);
				}
			}
			/// @endcond
			
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
			/// its passed as a reference or pointer. Therefore, this value can be changed.
			/// Literal values cannot be passed as references.
			bool IsReference() const {
				return reference;
			}
			
			/// If true, this parameter is a variable and its name is given to the function as
			/// a string.
			bool IsVariable() const {
				return variable;
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
						break;
					case ConstTag:
						constant=true;
						break;
					case VariableTag:
						variable=true;
						break;
						
					default:
						Utils::ASSERT_FALSE("Unknown tag");
				}
				UnpackTags(tags...);
			}
			
			
			std::string name;
			std::string help;
			const Type *type;
			
			bool optional  = false;
			bool reference = false;
			bool constant  = false;
			bool variable  = false;
		};
		
		using ParameterList = Containers::Collection<const Parameter>;
		
		/** This class stores information about a function. Functions can be related to a type
		 * or stand alone functions in libraries. EmbeddedFunction class should be used to 
		 * functions that are pointing to C++ function stubs. These stubs should extract and
		 * forward parameters to real functions. Functions are non-mutable after construction.
		 * Function names should be unique in their respective scopes case-insensitively.
		 * 
		 * Function object allows following tags:
		 * 
		 * **KeywordTag**: Makes this function a keyword. Keywords acts differently in programming
		 * dialect. Instead of requiring parenthesis and commas between elements, it acts like in
		 * console dialect. This can be further exploited to have a single parameter with setting
		 * StretchTag. This way, a keyword can parse its own parameters. This allows function keyword
		 * to have `function name(type param, type param...)` returns type syntax
		 * 
		 * **StretchTag**: If this tag is set, in console dialect, spaces in the last parameter are not 
		 * treated as parameter separator as if it is in quotes. Helpful for functions like echo. But also 
		 * helpful for keywords that requires their own parsing.
		 * 
		 * **RepeatTag**: This tag makes the last parameter repeatable. This way, the function can be 
		 * called by as many parameters as user likes. In embedded functions, these parameters are also
		 * placed in the data list. In custom functions, the parameter that is marked as repeatable can 
		 * be accessed like an array. If both RepeatTag and StretchTag is set, stretch will be effective
		 * first. Therefore, for regular functions, in console dialect stretch will work, resulting a
		 * single parameter, however, in programming dialect, repeat will be in effect.
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
		 * 
		 * **OperatorTag**: Makes this function an operator. Operators could be symbols or regular identifiers.
		 * 
		 * **ConstTag**: Works only on member functions. This function becomes a constant, unable to change
		 * the contents of this object.
		 */
		class Function {
			friend class Type;
		public:
			
			/// Function constructor fully constructs a function object. Both return type and tags are
			/// optional and its possible to skip return type and specify tags directly.
			template<class ...P_>
			Function(const std::string &name, const std::string &help, const Type *returntype, 
					 const Type *parent, ParameterList parameters, P_ ...tags) :
			name(name), help(help), returntype(returntype), parent(parent), Parameters(this->parameters)
			{
				using std::swap;
				
				swap(parameters, this->parameters);
				UnpackTags(tags...);
				
				init();
			}

			Function(const Function &)=delete;
			
			
			/// Destructor frees all parameters.
			virtual ~Function() {
				parameters.DeleteAll();
			}

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
				ASSERT(returntype, "This function does not have a return type", 1, 2);
				return *returntype;
			}
			
			/// Returns if this function is actually a keyword.
			bool IsKeyword() const {
				return keyword;
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
			
			/// Returns whether this function is a constant
			bool IsConstant() const {
				return constant;
			}

			/// Returns if this function is a member function of a type.
			bool IsMember() const {
				return parent!=nullptr;
			}
			
			/// If this function is a member function, returns the owner object. If this function is not a
			/// member function, this function crashes.
			const Type &GetOwner() const {
				ASSERT(parent, "This class does not gave an owner.", 1,2);
				
				return *parent;
			}
			
			/// If this function is a member function, this function returns if it is publicly accessible
			bool IsAccessible() const {
				return accessible;
			}
			
			/// Returns if this function is an operator. All operators should be member functions
			bool IsOperator() const {
				return isoperator;
			}
			
			/// Returns if this function has a parent type, meaning it is a member function
			bool HasParent() const {
				return parent!=nullptr;
			}
			
			/// Returns the parent type. If this function is not a member function, this function will
			/// cause a runtime error
			const Type &GetParent() const {
				if(parent==nullptr) {
					throw std::runtime_error("This function is not a member function");
				}
				
				return *parent;
			}
			
			/** 
			 * Class the stub for this function. If ismethod parameter is set and method variant exists
			 * method variant is called. But if there is no method variant, it simply prints out the return
			 * of the function. When ismethod is set, this function will never return a value. 
			 */
			virtual Data Call(bool ismethod, const std::vector<Data> &parameters) const = 0;
			
			/// This function is only for scoped keywords
			virtual bool CallEnd(Data) const;
			
			/// This function is only for redirecting scoped keywords
			virtual void CallRedirect(Data,std::string &) const;
			
			
			/// Compares two functions
			bool operator ==(const Function &other) const {
				return this==&other;
			}
			
			bool operator ==(const Function *other) const {
				return this==other;
			}
			
			/// Compares two functions
			bool operator !=(const Function &other) const {
				return this!=&other;
			}
			
			bool operator !=(const Function *other) const {
				return this!=other;
			}
			
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
						break;;
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
						ASSERT(!isoperator, "A function cannot be a static operator");
						break;
					case PrivateTag:
						accessible=false;
						break;
					case PublicTag:
						accessible=true;
						break;
					case OperatorTag:
						isoperator=true;
						ASSERT(!staticmember, "A function cannot be a static operator");
						break;
					case ConstTag:
						constant=true;
						break;
					default:
						ASSERT(false, "Unknown tag", 2, 16);
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
			
			/// Makes this function an operator. All operators should be member functions
			bool isoperator = false;
			
			/// Makes this function constant. Only works on member functions.
			bool constant = false;
			
		private:
			void init();
			
			const Type *parent;
		};
		
		using FunctionList = Containers::Hashmap<std::string, const Function, &Function::GetName, std::map, String::CaseInsensitiveLess>;
		
		/**
		 * Constants are values that are fixed and can be accessed without $ sign. They can be a part of a
		 * type or a part of a library.
		 */
		class Constant {
		public:
			/// Constructor
			Constant(const std::string &name, const std::string &help, 
					 const Data &data) : 
			name(name), help(help), data(data) { }
			
			/// Returns the name of the constant
			std::string GetName() const {
				return name;
			}

			/// Returns the help related with this constant
			std::string GetHelp() const {
				return help;
			}
			
			/// Returns the type of the constant
			const Type &GetType() const {
				return data.GetType();
			}
			
			/// Returns the value of the constant
			Data GetData() const {
				return data;
			}
			
		private:
			
			/// The name of the constant
			std::string name;
			
			/// Help string of the constant
			std::string help;
			
			
			Data data;
		};
		
		using ConstantList = Containers::Hashmap<std::string, const Constant, &Constant::GetName, std::map, String::CaseInsensitiveLess>;
		
		/// Data members that can be accessed through an instance of the a type. 
		class DataMember {
			friend class Type;
		public:
			/// Constructor
			template<class ...P_>
			DataMember(const std::string &name, const std::string &help, const Type &type, P_ ...tags) :
			name(name), help(help), type(&type) {
				UnpackTags(tags...);
			}
			
			/// Constructor
			DataMember(const std::string &name, const std::string &help, const Type &type, 
					   const std::vector<Tag> &tags) :
			DataMember(name, help, type) {
				for(auto tag : tags)
					UnpackTags(tag);
			}

			/// Returns the name of this datamember.
			std::string GetName() const {
				return name;
			}

			/// Returns the help string. Help strings may contain markdown notation.
			std::string GetHelp() const {
				return help;
			}
			
			/// Returns if this function is static. Only meaningful when the function is a member function.
			bool IsStatic() const {
				return staticmember;
			}
			
			/// If this function is a member function, this function returns if it is publicly accessible
			bool IsAccessible() const {
				return accessible;
			}
			
			/// Returns the type of this data member
			const Type &GetType() const {
				return *type;
			}
			
			/// Gets data from the datamember
			virtual Data Get(const Data &source) const = 0;
			
			/// Sets the data of the data member, if the source is a reference,
			/// this function should perform in place replacement of the value
			virtual void Set(Data &source, const Data &value) const = 0;
			
			virtual ~DataMember() { }
			
		protected:
			/// @cond INTERNAL
			void UnpackTags() { }
			
			template <class ...P_>
			void UnpackTags(Tag tag, P_... rest) {
				switch(tag) {
					case PrivateTag:
						accessible = false;
						break;
					case StaticTag:
						staticmember = true;
						break;
					default:
						ASSERT(false, "Unknown tag", 2, 16);
				}
				
				UnpackTags(rest...);
			}
			/// @endcond
			
			virtual void typecheck(const Type *type) = 0;
			
			/// The name of the datamember
			std::string name;
			
			/// Help string of the datamember
			std::string help;
			
			/// Type of the datamember
			const Type *type;
			
			/// Whether this member is publicly accessible
			bool accessible = true;
			
			/// Whether this member is a static member that should be accessed from the type.
			bool staticmember = false;
		};
		
		using DataMemberList = Containers::Hashmap<std::string, const DataMember, &DataMember::GetName, std::map, String::CaseInsensitiveLess>;
		
		/// Events allow an easy mechanism to program logic into actions instead of checking actions
		/// continuously. This system is vital for UI programming. Events are basically function descriptors.
		/// Event handlers can access the object that is the source for event using $_eventsource variable.
		class Event {
		public:
			Event(const std::string &name, const std::string &help, ParameterList parameters) : 
			name(name), help(help), Parameters(this->parameters) {
				using std::swap;
				
				swap(parameters, this->parameters);
			}

			/// Returns the name of this event.
			std::string GetName() const {
				return name;
			}

			/// Returns the help event. Help strings may contain markdown notation.
			std::string GetHelp() const {
				return help;
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
		
		using EventList = Containers::Hashmap<std::string, const Event, &Event::GetName, std::map, String::CaseInsensitiveLess>;
		
		/** 
		 * This class stores information about types. Types can have their own functions, members,
		 * events and operators. Additionally some types can be converted to others, this information
		 * is also stored in this class. Inheritance is supported, however, as of now virtual functions
		 * are not supported. Therefore, inheritance is mostly useful to avoid code duplication.
		 * 
		 * Allows ReferenceTag to mark this type as a reference type. Reference types are stored as
		 * pointers within Data objects and therefore, will not copied around. Currently, user defined
		 * types cannot be reference type.
		 */
		class Type {
		public:
			
			class Inheritance {
				friend class Type;
				
			public:
				using ConversionFunction = std::function<Data(Data)>;
				
				Inheritance(const Type &target, ConversionFunction from, ConversionFunction to) : 
				target(target), from(from), to(to) { }
				
				const Type &target;
				
			private:
				ConversionFunction from, to;
			};
			
			/// Constructor, unlike other reflection objects, Type is not constructed fully. TypeInterace for pointer type
			/// can be constructed using Any::Type.
			Type(const std::string &name, const std::string &help, const Any &defaultvalue, 
				 Any::TypeInterface *consttype, Any::TypeInterface *ptrtype, Any::TypeInterface *constptrtype, bool isref);

			/// Returns the name of this type.
			std::string GetName() const {
				return name;
			}

			/// Returns the help string. Help strings may contain markdown notation.
			std::string GetHelp() const {
				return help;
			}
			
			/// Returns the value of the type
			Any GetDefaultValue() const {
				return defaultvalue;
			}
			
			/// Returns whether this type is a reference type.
			bool IsReferenceType() const {
				return referencetype;
			}
			
			/// Adds an inheritance parent. from and to function should handle reference and constness of the data.
			/// Inheritance should be added in order. After using a class as a parent, no parent should be added to that
			/// class
			void AddInheritance(const Type &type, Inheritance::ConversionFunction from, Inheritance::ConversionFunction to);
			
			/// Morphs the given data into the target type.
			Data MorphTo(const Type &type, Data source) const;
			
			/// Adds new datamembers to the type
			void AddDataMembers(std::initializer_list<DataMember*> elements) {
				for(auto element : elements) {
					ASSERT((element != nullptr), "Given element cannot be nullptr", 1, 2);
					ASSERT(!datamembers.Exists(element->GetName()), 
						   "Data member " + element->GetName() + " is already added as a data member.", 1, 2);
					ASSERT(!functions.Exists(element->GetName()), 
						   "Data member " + element->GetName() + " is already added as a function.", 1, 2);
					ASSERT(!constants.Exists(element->GetName()), 
						   "Data member " + element->GetName() + " is already added as a constant.", 1, 2);
					ASSERT(!events.Exists(element->GetName()), 
						   "Data member " + element->GetName() + " is already added as an event.", 1, 2);
					
					element->typecheck(this);
					datamembers.Add(element);
				}
			}
			
			/// Adds new functions to the type
			void AddFunctions(std::initializer_list<Function*> elements) {
				for(auto element : elements) {
					ASSERT((element != nullptr), "Given element cannot be nullptr", 1, 2);
					ASSERT(!datamembers.Exists(element->GetName()), 
						   "Function " + element->GetName() + " is already added as a data member.", 1, 2);
					ASSERT(!functions.Exists(element->GetName()), 
						   "Function " + element->GetName() + " is already added as a function.", 1, 2);
					ASSERT(!constants.Exists(element->GetName()), 
						   "Function " + element->GetName() + " is already added as a constant.", 1, 2);
					ASSERT(!events.Exists(element->GetName()), 
						   "Function " + element->GetName() + " is already added as an event.", 1, 2);
					ASSERT(element->parent==this,
						   "This type should be listed as the parent of this function", 1, 2);
					
					functions.Add(element);

				}
			}
			
			/// Adds new constructors to the type
			void AddConstructors(std::initializer_list<Function*> elements) {
				for(auto element : elements) {
					ASSERT((element != nullptr), "Given element cannot be nullptr", 1, 2);
					ASSERT(element->HasReturnType() && element->GetReturnType()==this,
						   "Given constructor should return this ("+name+") type", 1, 2);
					
					for(auto &c : constructors) {
						if(c.Parameters.GetCount()!=element->Parameters.GetCount()) continue;

						bool mismatched=false;
						for(int i=0; i<c.Parameters.GetCount(); i++) {
							if(c.Parameters[i].GetType()!=element->Parameters[i].GetType()) {
								mismatched=true;
								continue;
							}
						}
						if(mismatched) continue;

						ASSERT(false, "The given constructor already exists: "+element->GetName(), 1, 2);
					}
					
					constructors.Add(element);
				}
			}
			
			/// Adds new constants to the type
			void AddConstants(std::initializer_list<Constant*> elements) {
				for(auto element : elements) {
					ASSERT((element != nullptr), "Given element cannot be nullptr", 1, 2);
					ASSERT(!datamembers.Exists(element->GetName()), 
						"Constant " + element->GetName() + " is already added as a data member.", 1, 2);
					ASSERT(!functions.Exists(element->GetName()), 
						"Constant " + element->GetName() + " is already added as a function.", 1, 2);
					ASSERT(!constants.Exists(element->GetName()), 
						"Constant " + element->GetName() + " is already added as a constant.", 1, 2);
					ASSERT(!events.Exists(element->GetName()), 
						"Constant " + element->GetName() + " is already added as an event.", 1, 2);
					
					constants.Add(element);
				}
			}
			
			/// Adds new events to the type
			void AddEvents(std::initializer_list<Event*> elements) {
				for(auto element : elements) {
					ASSERT((element != nullptr), "Given element cannot be nullptr", 1, 2);
					ASSERT(!datamembers.Exists(element->GetName()), 
						"Event " + element->GetName() + " is already added as a data member.", 1, 2);
					ASSERT(!functions.Exists(element->GetName()), 
						"Event " + element->GetName() + " is already added as a function.", 1, 2);
					ASSERT(!constants.Exists(element->GetName()), 
						"Event " + element->GetName() + " is already added as a constant.", 1, 2);
					ASSERT(!events.Exists(element->GetName()), 
						"Event " + element->GetName() + " is already added as an event.", 1, 2);
					
					events.Add(element);
				}
			}
			
			/// Compares two types
			bool operator ==(const Type &other) const {
				return this==&other;
			}
			
			bool operator ==(const Type *other) const {
				return this==other;
			}
			
			/// Compares two types
			bool operator !=(const Type &other) const {
				return this!=&other;
			}
			
			bool operator !=(const Type *other) const {
				return this!=other;
			}
			
			/// Converts this type to its pointer
			operator const Type *() const {
				return this;
			}

			const Function *GetTypeCasting(const Type *other) const {
				for(const auto &ctor : constructors) {
					if(ctor.Parameters.GetCount()==1 && ctor.Parameters[0].GetType()==other) {
						return &ctor;
					}
				}

				return nullptr;
			}

			/// Constructs a new object from the given parameters
			Data Construct(const std::vector<Data> &parameters) const;

			/// Deletes the object
			void Delete(const Data &obj) const {
				ASSERT(referencetype, "Cannot delete non-reference type objects. They are automatically destroyed.", 1, 4);

				deleteobject(obj);
			}
			
			/// Converts a data of this type to string. This function should never throw, if there is
			/// no data to display, recommended this play is either [ EMPTY ], or Typename #id
			virtual std::string ToString(const Data &) const = 0;
			
			/// Parses a string into this data. This function is allowed to throw.
			virtual Data Parse(const std::string &) const = 0;
			
			
			/// Data members of this type. Notice that not every type has data members.
			const DataMemberList					&DataMembers;
			
			/// Contains the functions related with this type. These functions can be operators,
			/// or type casting functions.
			const FunctionList 						&Functions;
			
			/// Constructors of this type. They can also act like conversion from operators. Implicit
			/// conversion constructors should have their operator flag set.
			const Containers::Collection<Function> 	&Constructors;
			
			/// Constants related with this type. Constants can be of the same type as this one.
			const ConstantList    					&Constants;
			
			/// Events of this type.
			const EventList							&Events;
			
			/// Parents of this type. This includes indirect parents as well
			const std::map<const Type *, const Type *> &Parents;
			
			/// Inheritance list. 
			const std::map<const Type *, Inheritance> &InheritsFrom;
			
			/// Inherited symbols
			const Containers::Hashmap<std::string, const Type, nullptr, std::map, String::CaseInsensitiveLess> &InheritedSymbols;
			
			/// Type info for normal type
			const Any::TypeInterface * const TypeInterface;
			
			/// Type info for const type
			const Any::TypeInterface * const ConstTypeInterface;
			
			/// Type info for pointer type
			const Any::TypeInterface * const PtrTypeInterface;
			
			/// Type info for const pointer type
			const Any::TypeInterface * const ConstPtrTypeInterface;
			
			virtual ~Type() { 
				delete TypeInterface;
				delete PtrTypeInterface;
			}
			
		protected:

			/// This function should delete the given object.
			virtual void deleteobject(const Data &) const { }


		private:
			std::string name;
			
			std::string help;
			
			Any defaultvalue;
			
			
			/// Data members of this type. Notice that not every type has data members.
			DataMemberList						datamembers;

			/// Contains the functions related with this type. These functions can be operators,
			/// or type casting functions. Functions with the name of another type are called
			/// type casting functions. These functions are stored with [Library]Typename format.
			/// However, if the library is never specified they are listed as []Typename.
			FunctionList 						functions;
			
			/// Constructors of this type. They can also act like conversion from operators. Implicit
			/// conversion constructors should have their operator flag set.
			Containers::Collection<Function>	constructors;
			
			/// Constants related with this type. Constants can be of the same type as this one.
			ConstantList    					constants;
			
			/// Events of this type.
			EventList							events;
			
			/// Inheritance list. 
			std::map<const Type *, Inheritance> inheritsfrom;
			
			/// Inherited symbols
			Containers::Hashmap<std::string, const Type, nullptr, std::map, String::CaseInsensitiveLess> inheritedsymbols;
			
			/// This lists all parents of this type in the entire hierarchy.
			std::map<const Type *, const Type *> parents;
			
			bool referencetype = false;
		};
		
		inline std::ostream &operator <<(std::ostream &out, const Type &type) {
			out<<type.GetName();
			return out;
		}
		
		using TypeList = Containers::Hashmap<std::string, const Type, &Type::GetName, std::map, String::CaseInsensitiveLess>;
		
		/**
		 * This class represents a library. Libraries can be loaded into virtual machines to be used.
		 * Libraries contains types, functions and constants. This class is constant after construction.
		 * Name of a library is also its namespace.
		 */
		class Library {
		public:
			/// Constructor
			Library(const std::string &name, const std::string &help,
					TypeList types, FunctionList functions, ConstantList constants=ConstantList());
			
			/// For late initialization
			Library() : Types(this->types), Functions(this->functions), Constants(this->constants) { }
			
			Library(Library &&lib) : Types(this->types), Functions(this->functions), Constants(this->constants) {
				using std::swap;
				
				swap(types, lib.types);
				swap(functions, lib.functions);
				swap(constants, lib.constants);
				swap(name, lib.name);
				swap(help, lib.help);
			}
			
			Library &operator =(Library &&lib) {
				using std::swap;
				name="";
				help="";
				types.Destroy();
				functions.Destroy();
				constants.Destroy();
				
				swap(types, lib.types);
				swap(functions, lib.functions);
				swap(constants, lib.constants);
				swap(name, lib.name);
				swap(help, lib.help);
				
				return *this;
			}

			void AddFunctions(const std::initializer_list<Function*> &list) {
				for(auto &fn : list) {
					ASSERT(!SymbolExists(fn->GetName()), "Symbol "+fn->GetName()+" already exists", 1, 2);
					
					functions.Add(fn);
				}
			}
			
			void AddTypes(const std::initializer_list<Type*> &list) {
				for(auto &type : list) {
					ASSERT(!SymbolExists(type->GetName()), "Symbol "+type->GetName()+" already exists", 1, 2);
					
					types.Add(type);
				}
			}
			
			void AddConstants(const std::initializer_list<Constant*> &list) {
				for(auto &constant : list) {
					ASSERT(!SymbolExists(constant->GetName()), "Symbol "+constant->GetName()+" already exists", 1, 2);
					
					constants.Add(constant);
				}
			}
			
			void AddFunctions(const std::vector<Function*> &list) {
				for(auto &fn : list) {
					ASSERT(!SymbolExists(fn->GetName()), "Symbol "+fn->GetName()+" already exists", 1, 2);
					
					functions.Add(fn);
				}
			}

			void AddTypes(const std::vector<Type*> &list);
			
			void AddConstants(const std::vector<Constant*> &list) {
				for(auto &constant : list) {
					ASSERT(!SymbolExists(constant->GetName()), "Symbol "+constant->GetName()+" already exists", 1, 2);
					
					constants.Add(constant);
				}
			}
			
			bool SymbolExists(const std::string &name) const {
				return types.Exists(name) || functions.Exists(name) || constants.Exists(name);
			}
			
			SymbolType TypeOf(const std::string &name) const {
				if(functions.Exists(name)) return SymbolType::Function;
				if(types.Exists(name)) return SymbolType::Type;
				if(constants.Exists(name)) return SymbolType::Constant;
				
				return SymbolType::Unknown;
			}
			
			/// Returns the name of this library. Library names are also used as namespace specifiers
			std::string GetName() const {
				return name;
			}
			
			/// Returns the help string. Help strings may contain markdown notation.
			std::string GetHelp() const {
				return help;
			}
			
			/// List of types that this library contains
			const TypeList 		&Types;
			
			/// List of functions this library contains
			const FunctionList 	&Functions;
			
			/// List of constants this library contains
			const ConstantList 	&Constants;
			
		private:
			std::string name;
			std::string help;
			TypeList 	 types;
			FunctionList functions;
			ConstantList constants;
		};
	}
}
