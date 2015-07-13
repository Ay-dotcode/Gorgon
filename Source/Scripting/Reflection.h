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
			/// When this tag affects parameters, object becomes a reference to the parameter
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
			
			/// Denotes a function that returns const
			ReturnsConstTag,
			
			/// Makes this parameter a variable accepting parameter. Variable parameters are type checked
			/// against supplied type, however, they are always passed as strings denoting the name of the variable
			VariableTag,
			
			/// Makes a constructor implicit
			ImplicitTag,
			
			/// Allows a parameter to be NULL
			AllowNullTag,
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
		 * 
		 * **AllowNullTag**: Allows a parameter that requires a reference type to accept a Null value.
		 * Null values are dangerous when combined with references.
		 */
		class Parameter {
		public:
			
			/// Constructs a new parameter. All information regarding to a parameter should be specified
			/// in the constructor. After construction parameter is non-mutable. options, tags, and defaultvalue
			/// are optional
			template <class ...Params_>
			Parameter(const std::string &name, const std::string &help, const Type *type, 
					 Data defaultvalue, OptionList options, Params_ ...tags) : 
			name(name), type(type), help(help), defaultvalue(defaultvalue) {
				ASSERT((type!=nullptr), "Parameter type cannot be nullptr", 1, 2);
				using std::swap;

				swap(options, this->Options);

				UnpackTags(tags...);
			}
			
			/// @cond INTERNAL
			Parameter(const std::string &name, const std::string &help, const Type *type, Data defaultvalue=Data::Invalid()) : 
			Parameter(name, help, type, defaultvalue, OptionList()) { }
			
			template <class ...Params_>
			Parameter(const std::string &name, const std::string &help, const Type *type, Tag firsttag,
					  Params_ ...tags) : 
			Parameter(name, help, type, Data::Invalid(), OptionList(), firsttag, std::forward<Params_>(tags)...) {
			}
			
			template <class ...Params_>
			Parameter(const std::string &name, const std::string &help, const Type *type, Data defaultvalue,
					  Tag firsttag, Params_ ...tags) : 
			Parameter(name, help, type, defaultvalue, OptionList(), firsttag, std::forward<Params_>(tags)...) {
			}
			 
			Parameter(const std::string &name, const std::string &help, const Type *type, 
					  Data defaultvalue, OptionList options, bool reference, bool constant, bool variable, bool allownull) :
			Parameter(name, help, type, defaultvalue, options)
			{
				this->reference = reference;
				this->constant  = constant;
				this->variable  = variable;
				this->optional  = defaultvalue.IsValid();
				this->allownull = allownull;
			}
			/// @endcond
			
			/// Copy assignment
			Parameter &operator =(const Parameter &p) {
				name		= p.name		;
				help		= p.help		;
				type		= p.type		;
				defaultvalue= p.defaultvalue;
				Options		= p.Options		;
				optional	= p.optional	;
				reference	= p.reference	;
				constant	= p.constant	;
				variable	= p.variable	;
				allownull	= p.allownull	;
				
				return *this;
			}
			
			/// Compares two parameters, not very reliable, it does not check defaultvalue and options
			bool operator ==(const Parameter &p) const {
				return 
					name		== p.name		&&
					help		== p.help		&&
					type		== p.type		&&
					optional	== p.optional	&&
					reference	== p.reference	&&
					constant	== p.constant	&&
					variable	== p.variable	&&
					allownull   == p.allownull  ;
			}
			
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
			bool IsReference() const;
			
			/// Checks if this parameter accepts a constant value
			bool IsConstant() const {
				return constant;
			}
			
			/// If true, this parameter is a variable and its name is given to the function as
			/// a string.
			bool IsVariable() const {
				return variable;
			}
			
			/// If true, a null reference can be passed to this parameter
			bool AllowsNull() const {
				return allownull;
			}
			
			/// Returns the default value for this parameter
			Data GetDefaultValue() const {
				ASSERT(optional, name+" parameter does not have default value");
				
				return defaultvalue;
			}
			
			/// Allowed values for this parameter
			OptionList Options;
			
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
						
					case AllowNullTag:
						allownull=true;
						break;
						
					default:
						Utils::ASSERT_FALSE("Unknown tag");
				}
				UnpackTags(tags...);
			}
			
			
			std::string name;
			std::string help;
			const Type *type;
			Data defaultvalue=Data::Invalid();
			
			bool optional  = false;
			bool reference = false;
			bool constant  = false;
			bool variable  = false;
			bool allownull = false;
		};
		
		using ParameterList = std::vector<Parameter>;
		

		/**
		 * Represents a function. Functions contains overloads that vary in parameters and/or traits. Overloads
		 * can be C++ functions or functions that are defined by scripting. Overloads can be added after the object
		 * is constructed.
		 * 
		 * A function can act as a keyword. While keywords do not have special traits they have additional 
		 * restrictions. Keywords cannot return values and cannot be member functions. Keywords may have special
		 * syntax based on the dialect that is effect. For instance, in programming dialect, keywords do not require
		 * parenthesis to be called. A function can be marked as a keyword using **KeywordTag**.
		 * 
		 * If a owner (or parent) is set, the function will become a **member function**. Member functions becomes a part
		 * of the owner type. A member function can also be static using **StaticTag**. A static member function do not
		 * have this pointer and cannot be access from a member of the type. Instead it can be accessed using `Type:Fn`
		 * syntax. Constructor functions are also marked as static. Member functions can be called either by issuing 
		 * `Object.fn(...)` or `fn(Object, ...)`. Latter case is useful for calling member functions on literals. 
		 * 
		 * A member function can act as an operator if **OperatorTag** is set. Operator functions must have a single 
		 * parameter and cannot be static. While performing overload resolution, if ambiguity occurs, the overload that
		 * has same parameter type as the owner type is favored. This solves cases where the right hand side type do
		 * not have a specific overload but can be converted to multiple type that have overloads.
		 */
		class Function {
			friend class Type;
		public:
			
			/**
			 * Represents a function overload. Function overloads can either be C++ functions or defined by scripting.
			 * A C++ function can be embedded using MapFunction. 
			 * 
			 * Return value of an overload is specified by returntype. A `nullptr` value marks the function void. If the
			 * overload returns a reference, **ReferenceTag** must be set. An embedded function can return reference
			 * by reference or a pointer. Reference types should always be returned as a reference or pointer. If the
			 * returned reference is constant, **ReturnsConstTag** should be set.
			 * 
			 * A constructor overload should return the object that is constructed. A constructor that has a single
			 * parameter can be used as a casting operator if the variant is marked with **ImplicitTag**. Otherwise,
			 * all constructors should be invoked manually.
			 * 
			 * If a function is a member function, an overload of this function can be made private using **PrivateTag**.
			 * A private function can only be called from the function of the owner type or any type that is descendant of
			 * it. Private overloads are useful in scripting defined types. 
			 * 
			 * A member function overload can be marked as constant by using **ConstTag**. If an embedded constant overload
			 * is a member function, it should be const. If it is a namespace function, it should take this pointer as either
			 * `const Type *` or `const Type &`. It is also possible to use `Type` for value types, however, this usage is
			 * discouraged.
			 * 
			 * If an overload is marked with **RepeatLastTag**, the last parameter can be repeated as many times as needed.
			 * However, unless **OptionalTag** of the last parameter is set, at least a single parameter is required for
			 * these repeating overloads. Embedded functions should receive these parameters as either a const reference
			 * a normal std::vector of the specified type.
			 */
			class Overload {
				friend class Function;
			public:
				/// Regular constructor that can take as many tags as needed.
				template<class ...P_>
				Overload(const Type *returntype, ParameterList parameters, P_ ...tags) :
				returntype(returntype), Parameters(this->parameters)
				{
					using std::swap;
					swap(parameters, this->parameters);
					
					unpacktags(tags...);
				}
				
				/// A full constructor
				Overload(const Type *returntype, ParameterList parameters, bool stretchlast, bool repeatlast, 
						bool accessible, bool constant, bool returnsref, bool returnsconst, bool implicit) :
				returntype(returntype), stretchlast(stretchlast), repeatlast(repeatlast), 
				accessible(accessible), constant(constant), returnsref(returnsref), returnsconst(returnsconst),
				implicit(implicit), Parameters(this->parameters)
				{
					using std::swap;
					swap(parameters, this->parameters);
				}
				
				virtual ~Overload() { }
				
				/// Compares two variants if they have the same signature
				bool IsSame(const Overload &var) const;
				
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
				
				/// Returns whether this function is a constant
				bool IsConstant() const {
					return constant;
				}
				
				/// This function variant returns a reference to a value rather than the value itself
				bool ReturnsRef() const {
					return returnsref;
				}
				
				/// This function variant returns a constant
				bool ReturnsConst() const {
					return returnsconst;
				}
				
				/// Returns the function this variant belongs
				const Function &GetParent() const {
					ASSERT(parent, "Parent is not set");
					
					return *parent;
				}
				
				/// Returns whether this variant returns a value
				bool HasReturnType() const {
					return returntype!=nullptr;
				}
				
				/// Returns the type this variant returns
				const Type &GetReturnType() const {
					ASSERT(returntype, "This function does not return a value");
					
					return *returntype;
				}
				
				/// Returns if this variant can be used as implicit conversion
				bool IsImplicit() const {
					return implicit;
				}
		
				/** 
				* Class the stub for this function. If ismethod parameter is set and method variant exists
				* method variant is called. But if there is no method variant, it simply prints out the return
				* of the function. When ismethod is set, this function will never return a value. 
				*/
				virtual Data Call(bool ismethod, const std::vector<Data> &parameters) const = 0;
				
				/// The parameters of this overload
				const ParameterList &Parameters;
				
			protected:
			
				/// @cond INTERNAL
				void unpacktags() {}
				
				template<class ...P_>
				void unpacktags(Tag tag, P_ ...rest) {
					switch(tag) {
						case ConstTag:
							constant=true;
							break;
							
						case StretchTag:
							stretchlast=true;
							break;
							
						case RepeatTag:
							repeatlast=true;
							break;
							
						case PublicTag:
							accessible=true;
							break;
							
						case PrivateTag:
							accessible=false;
							break;
							
						case ReferenceTag:
							returnsref=true;
							break;
							
						case ReturnsConstTag:
							returnsconst=true;
							break;
							
						case ImplicitTag:
							implicit=true;
							break;
							
						default:
							ASSERT(false, "Unknown tag", 2, 16);
					}
					
					unpacktags(rest...);
				}
				/// @endcond
				
				/// This function should perform validity checks on the variant. The base function
				/// should be called unless similar checks are repeated
				virtual void dochecks(bool ismethod);
				
				/// Modifiable parameters of this overload
				ParameterList parameters;
				
				/// Return type of this function variant. If nullptr this function does not return a value.
				const Type *returntype = nullptr;
			
				/// If true, in console dialect, spaces in the last parameter are not treated as parameter
				/// separator as if it is in quotes. Helpful for functions like echo
				bool stretchlast = false;
				
				/// If true last parameter can be specified any number of times. This number can be obtained
				/// from data list supplied to function stub.
				bool repeatlast = false;
				
				/// Only meaningful in class member functions. If true this function can be access from outside
				/// the type itself
				bool accessible = true;
				
				/// Makes this function constant. Only works on member functions.
				bool constant = false;
				
				/// This function variant returns a reference
				bool returnsref = false;
				
				/// This function variant returns a constant, useful with references
				bool returnsconst = false;
				
				/// The parent function of this variant
				Function *parent;
				
				/// If the parent function is constructor, marks this variant as an implicit type conversion
				bool implicit = false;
			};
			
			/// Regular constructor with both overloads and methods specified a long with at least a single tag
			template<class ...P_>
			Function(const std::string &name, const std::string &help, const Type *parent, 
					 const Containers::Collection<Overload> &overloads, const Containers::Collection<Overload> &methods, 
					 Tag tag, P_ ...tags) : 
			name(name), help(help), parent(parent), Overloads(this->overloads), Methods(this->methods)
			{
				for(auto &variant : overloads) {
					AddOverload(variant);
				}
				
				for(auto &variant : methods) {
					AddMethod(variant);
				}
				
				unpacktags(tag);
				unpacktags(tags...);

				init();
			}
			
			/// Regular constructor with overloads specified a long with at least a single tag
			template<class ...P_>
			Function(const std::string &name, const std::string &help, const Type *parent, 
					 const Containers::Collection<Overload> &overloads, Tag tag, P_ ...tags) :
			name(name), help(help), parent(parent), Overloads(this->overloads), Methods(this->methods)
			{
				for(auto &overload : overloads) {
					AddOverload(overload);
				}
				
				unpacktags(tag);
				unpacktags(tags...);

				init();
			}
			
			/// Regular constructor with both overloads and methods without any tags
			template<class ...P_>
			Function(const std::string &name, const std::string &help, const Type *parent,
					 const Containers::Collection<Overload> &overloads, const Containers::Collection<Overload> &methods=Containers::Collection<Overload>()
			) : 
			name(name), help(help), parent(parent), Overloads(this->overloads), Methods(this->methods)
			{ 
				for(auto &overload : overloads) {
					AddOverload(overload);
				}
				
				for(auto &overload : methods) {
					AddMethod(overload);
				}

				init();
			}

			/// Full constructor
			template<class ...P_>
			Function(const std::string &name, const std::string &help, const Type *parent, 
					 bool keyword, bool isoperator, bool staticmember) : 
			Function(name, help, parent, Containers::Collection<Overload>()) {
				this->keyword=keyword;
				this->isoperator=isoperator;
				this->staticmember=staticmember;

				init();
			}
			
			Function(const Function &) = delete;

			virtual ~Function() { 
				overloads.Destroy();
				methods.Destroy();
			} 
			
			/// Returns the name of this function.
			std::string GetName() const {
				return name;
			}

			/// Returns the help string. Help strings may contain markdown notation.
			std::string GetHelp() const {
				return help;
			}
			
			/// Returns if this function is actually a keyword.
			bool IsKeyword() const {
				return keyword;
			}
			
			/// Returns if this function is static. Only meaningful when the function is a member function.
			bool IsStatic() const {
				return staticmember;
			}

			/// Returns if this function is a member function of a type.
			bool IsMember() const {
				return parent!=nullptr;
			}
				
			/// Returns if this function is an operator
			bool IsOperator() const {
				return isoperator;
			}
			
			/// If this function is a member function, returns the owner object. If this function is not a
			/// member function, this function crashes.
			const Type &GetOwner() const {
				ASSERT(parent, "This function does not have an owner.", 1, 5);
				
				return *parent;
			}
			
			/// Adds the given overload to this function after performing necessary checks
			virtual void AddOverload(Overload &overload) {
				ASSERT(
					!isoperator || overload.parameters.size()==1,
					"Operators should have only a single parameter\n"					
					"in function "+name, 1, 3
				);

				overload.parent=this;
				for(const auto &v : overloads) {
					if(overload.IsSame(v)) {
						throw AmbiguousSymbolException(name, SymbolType::Function, "Ambiguous function variant");
					}
				}
				
#ifndef NDEBUG
				overload.dochecks(false);
#endif
				overloads.Push(overload);
			}
			
			/// Adds the given overload as a method to this function after performing necessary checks
			virtual void AddMethod(Overload &overload) {
				ASSERT(!isoperator, "Operators cannot be methods\n in function "+name, 1, 3);
				
				overload.parent=this;
#ifndef NDEBUG
				for(const auto &v : methods) {
					ASSERT(!overload.IsSame(v), "Ambiguous function variant\n in function "+name, 1, 3);
				}
				
				overload.dochecks(true);
#endif
				methods.Push(overload);
			}
			
			/// Adds the given overload to this function after performing necessary checks
			virtual void AddOverload(Overload *overload) {
				ASSERT(overload, "Empty variant\n in function "+name);

				AddOverload(*overload);
			}
			
			/// Adds the given overload as a method to this function after performing necessary checks
			virtual void AddMethod(Overload *overload) {
				ASSERT(overload, "Empty variant\n in function "+name);

				AddMethod(*overload);
			}
			
			/// The list of overloads this function has
			const Containers::Collection<Overload> &Overloads;
			
			/// The list of methods this function has
			const Containers::Collection<Overload> &Methods;
			
		private:
			
			/// @cond INTERNAL
			void unpacktags() {}
			
			template<class ...P_>
			void unpacktags(Tag tag, P_ ...rest) {
				switch(tag) {
					case KeywordTag:
						keyword=true;
						break;
						
					case StaticTag:
						staticmember=true;
						ASSERT(!isoperator, "A function cannot be a static operator");
						break;
						
					case OperatorTag:
						isoperator=true;
						ASSERT(!staticmember, "A function cannot be a static operator");
						ASSERT(parent, "Operators should be member functions");

						break;
						
					default:
						ASSERT(false, "Unknown tag", 2, 16);
				}
				
				unpacktags(rest...);
			}

			void init();
			/// @endcond

			
			const Type *parent = nullptr;
			
			std::string name;
			
			std::string help;
			
			bool keyword = false;
			
			bool isoperator = false;
			
			bool staticmember = false;
			
			Containers::Collection<Overload> overloads;
			
			Containers::Collection<Overload> methods;
		};
		
		//-use unordered_map
		/// The type to store list of functions
		using FunctionList = Containers::Hashmap<std::string, const Function, &Function::GetName, std::map, String::CaseInsensitiveLess>;
		
		/**
		 * Constants are values that are fixed and can be accessed without $ sign. They can be a part of a
		 * type or a part of a library.
		 */
		class Constant {
		public:
			/// Constructor
			Constant(const std::string &name, const std::string &help, 
					 const Type *type, Any data) : 
			name(name), help(help), data(type, data, false, true) { 
				ASSERT(type, "Type cannot be nullptr");
			}
			
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
		
		/// The type to store list of constants
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
			
			/// Returns if this data member is static. Only meaningful when the function is a member.
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
			virtual Data Get(      Data &source) const = 0;
			
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
			
			/// There are multiple ways to morph a type to another, this enumeration holds these types
			enum MorphType {
				/// Morphing is not possible
				NotPossible,
				
				/// This is an upcasting, but not a direct one
				UpCasting,
				
				/// This is a down casting
				DownCasting,
				
				/// This is a type casting
				TypeCasting
			};
			
			enum SubType {
				Regular,
				Event,
				Enum,
				Library
			};
			
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
			
			/// Constructor, unlike other reflection objects, Type is not constructed fully.
			Type(const std::string &name, const std::string &help, const Any &defaultvalue, 
				 TMP::RTTH *typeinterface, bool isref);

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
			
			virtual SubType GetSubType() const { return Regular; }
			
			/// Adds an inheritance parent. from and to function should handle reference and constness of the data.
			/// Inheritance should be added in order. After using a class as a parent, no parent should be added to that
			/// class
			void AddInheritance(const Type &type, Inheritance::ConversionFunction from, Inheritance::ConversionFunction to);
			
			/// Morphs the given data into the target type.
			Data MorphTo(const Type &type, Data source, bool allowtypecast=true) const;
			
			/// Check if it is possible to morph this type to the other
			MorphType CanMorphTo(const Type &type) const;
			
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
					ASSERT(element->parent==this,
						   "This type should be listed as the parent of this function", 1, 2);
					
					functions.Add(element);

				}
			}
			
			/// Adds new constructors to the type
			void AddConstructors(std::initializer_list<Function::Overload*> elements) {
				for(auto element : elements) {
					ASSERT((element != nullptr), "Given element cannot be nullptr", 1, 2);
					ASSERT(element->HasReturnType() && element->GetReturnType()==this,
						   "Given constructor should return this ("+name+") type", 1, 2);
					
					constructor.AddOverload(*element);
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
					
					constants.Add(element);
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

			const Function::Overload *GetTypeCastingFrom(const Type *other, bool implicit=true) const {
				for(const auto &ctor : constructor.Overloads) {
					if(ctor.Parameters.size()==1 && ctor.Parameters[0].GetType()==other && 
						(!implicit || ctor.IsImplicit())
					) {
						return &ctor;
					}
				}

				return nullptr;
			}

			/// !Unsafe. Constructs a new object from the given parameters. Requires parameters to be the exact type.
			Data Construct(const std::vector<Data> &parameters) const;

			/// Deletes the object
			void Delete(const Data &obj) const {
				deleteobject(obj);
			}
			
			/// This function compares two instances of this type. Both left and right should of this type.
			bool Compare(const Data &l, const Data &r) const;
			
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
			/// conversion constructors should have their flag set.
			const Function	 						&Constructor;
			
			/// Constants related with this type. Constants can be of the same type as this one.
			const ConstantList    					&Constants;
			
			/// Parents of this type. This includes indirect parents as well
			const std::map<const Type *, const Type *> &Parents;
			
			/// Inheritance list. 
			const std::map<const Type *, Inheritance> &InheritsFrom;
			
			/// Inherited symbols
			const Containers::Hashmap<std::string, const Type, nullptr, std::map, String::CaseInsensitiveLess> &InheritedSymbols;
			
			TMP::RTTH &TypeInterface;
			
			virtual ~Type() {
				delete &TypeInterface;
			}
			
		protected:

			/// This function should delete the given object.
			virtual void deleteobject(const Data &) const=0;
			
			/// This function should compare two instances of the type. Not required for reference types as pointers
			/// will be compared
			virtual bool compare(const Data &l, const Data &r) const { throw std::runtime_error("These elements cannot be compared"); }


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
			/// conversion constructors should have their flag set.
			Function							constructor;
			
			/// Constants related with this type. Constants can be of the same type as this one.
			ConstantList    					constants;
			
			/// Inheritance list. 
			std::map<const Type *, Inheritance> inheritsfrom;
			
			/// Inherited symbols
			Containers::Hashmap<std::string, const Type, nullptr, std::map, String::CaseInsensitiveLess> inheritedsymbols;
			
			/// This lists all parents of this type in the entire hierarchy.
			std::map<const Type *, const Type *> parents;
			
			bool referencetype = false;
		};
		
		/// Events allow an easy mechanism to program logic into actions instead of checking actions
		/// continuously. This system is vital for UI programming. Events are basically function descriptors.
		/// Event handlers can access the object that is the source for event using $_eventsource variable.
		class Event : virtual public Type {
		public:
			Event(const std::string &name, const std::string &help, const Any &defaultvalue, 
				  TMP::RTTH *typeinterface, Type *ret, ParameterList parameters) : 
			Type(name, help, defaultvalue, typeinterface, true), Parameters(this->parameters), returntype(ret) {
				using std::swap;
				
				swap(parameters, this->parameters);
			}
			
			virtual SubType GetSubType() const override {
				return Type::Event;
			}
			
			/// Returns whether event handlers should return a value
			bool HasReturnType() const {
				return returntype!=nullptr;
			}
			
			/// Returns the return type expected from the handlers.
			const Type &ReturnType() const {
				ASSERT(returntype, "This event does not allow returns");
				
				return *returntype;
			}
			
			/// Read only list of parameters
			const ParameterList &Parameters;
			
		protected:
			/// Parameters that every event handler should accept. 
			ParameterList parameters;
			
			/// The return type of this event, if it is allowed to return a value
			const Type *returntype;
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
					TypeList types, FunctionList functions, 
					ConstantList constants=ConstantList());
			
			Library(const std::string &name, const std::string &help) : Library(name, help, {}, {}) { }
			
			/// For late initialization
			Library() : Types(this->types), Functions(this->functions), Constants(this->constants) { }
			
			Library(Library &&lib) : Types(this->types), Functions(this->functions), 
			Constants(this->constants) {
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
			
			void AddTypes(const std::initializer_list<Type*> &list);
			
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
			
			/// List of functions that this library contains
			const FunctionList 	&Functions;
			
			/// List of constants that this library contains
			const ConstantList 	&Constants;
			
		private:
			std::string name;
			std::string help;
			TypeList 	 types;
			FunctionList functions;
			ConstantList constants;
			DataMemberList datamembers;
		};
	}
}
