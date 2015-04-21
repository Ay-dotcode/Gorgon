#pragma once

#include <tuple>
#include <type_traits>

#include "Reflection.h"
#include "VirtualMachine.h"
#include "Exceptions.h"
#include "../Scripting.h"
#include "../Utils/Assert.h"
#include "../TMP.h"



namespace Gorgon { namespace Scripting {
		
	template<class T_>
	using StringFromFn = std::string(*)(const T_ &);
	
	template<class T_>
	using ParseFn = T_(*)(const std::string &);
	
	template <
	class T_, 
	StringFromFn<T_> ToString_=String::From<T_>, 
	ParseFn<T_> Parse_=String::To<T_>
	>
	class MappedValueType;
	
	template<class T_>
	std::string ToEmptyString(const T_ &) {
		return "";
	}
	
	/**
		* This class allows embedded types to become scripting types that are passed around
		* as values. This class requires T_ to be copy constructable.
		*/
	template <
		class T_, 
		StringFromFn<T_> ToString_, 
		ParseFn<T_> Parse_
	>
	class MappedValueType : public Type {
	public:
		MappedValueType(const std::string &name, const std::string &help, const T_ &def) :
		Type(name, help, def, new TMP::AbstractRTTC<T_>(), false)
		{
		}
		
		MappedValueType(const std::string &name, const std::string &help) : MappedValueType(name, help, T_()) {
		}
		
		
		/// Converts a data of this type to string. This function should never throw, if there is
		/// no data to display, recommended this play is either [ EMPTY ], or Typename #id
		virtual std::string ToString(const Data &data) const override {
			return ToString_(data.GetValue<T_>());
		}
		
		/// Parses a string into this data. This function is allowed to throw.
		virtual Data Parse(const std::string &str) const override {
			return Data(this, Parse_(str));
		}
	};
	
	template<class T_>
	T_ ParseThrow(const std::string &) { throw std::runtime_error("This type cannot be parsed."); }
	
	/**
		* This class allows embedded types to become scripting types that are passed around
		* as references. Parsing requires a pointer, therefore, a regular parse function will not
		* be sufficient. Default parsing function throws.
		*/
	template <
	class T_, 
	std::string(*ToString_)(const T_ &)=&String::From<T_>, 
	T_*(*Parse_)(const std::string &)=&ParseThrow<T_*>
	>
	class MappedReferenceType : public Type {
	public:
		MappedReferenceType(const std::string &name, const std::string &help, T_ *def) :
		Type(name, help, def, new TMP::AbstractRTTC<T_>(), true)
		{
		}
		
		MappedReferenceType(const std::string &name, const std::string &help) : MappedReferenceType(name, help, nullptr) {
		}
		
		
		/// Converts a data of this type to string. This function should never throw, if there is
		/// no data to display, recommended this play is either [ EMPTY ], or Typename #id
		virtual std::string ToString(const Data &data) const override {
			if(data.IsConstant()) {
				if(data.ReferenceValue<const T_*>()==nullptr) return "<null>";
				return ToString_(*data.ReferenceValue<const T_*>());
			}
			else {
				if(data.ReferenceValue<T_*>()==nullptr) return "<null>";
				return ToString_(*data.ReferenceValue<T_*>());
			}
		}
		
		/// Parses a string into this data. This function is allowed to throw.
		virtual Data Parse(const std::string &str) const override {
			return Data(this, Parse_(str));
		}
		
	protected:
		virtual void deleteobject(const Data &obj) const override {
			T_ *ptr;
			if(obj.IsConstant()) {
				//force to non-const
				ptr=const_cast<T_*>(obj.ReferenceValue<const T_*>());
			}
			else {
				ptr=obj.ReferenceValue<T_*>();
			}
			if(ptr!=nullptr) {
				delete ptr;
			}
		}
	};

	
	/// This class wraps a C++ function into an overload. It can be constructed using MapFunction.
	template<class F_>
	class MappedFunction : public Scripting::Function::Overload {
		using variant = Scripting::Function::Overload;
		using traits  = TMP::FunctionTraits<F_>;
		template<int P_>
		using param = typename traits::template Arguments<P_>::Type;
	public:

		/// Constructor
		template<class ...P_>
		MappedFunction(F_ fn, const Scripting::Type *returntype, Scripting::ParameterList parameters, P_ ...tags) :
		variant(returntype, std::move(parameters), tags...), fn(fn)
		{ }

		virtual Data Call(bool ismethod, const std::vector<Data> &parameters) const override {
			auto ret=callfn<typename traits::ReturnType>(typename TMP::Generate<traits::Arity>::Type(), parameters);

			if(ismethod) {
				if(ret.IsValid()) {
					VirtualMachine::Get().GetOutput()<<ret<<std::endl<<std::endl;

					return Data::Invalid();
				}
			}

			return ret;
		}

	private:

		template<class T_>
		struct extractvector {
			enum { isvector = false };
			
			using inner = T_;
		};

		template<template<class, class> class V_, class T_, class A_>
		struct extractvector<V_<T_, A_>> {
			enum { isvector = std::is_same<V_<T_, A_>, std::vector<T_, A_>>::value };

			using inner = T_;
		};

		template<class T_>
		struct is_nonconstref {
			enum { value = std::is_reference<T_>::value && !std::is_const<typename std::remove_reference<T_>::type>::value };
		};

		template<class T_>
		inline typename std::enable_if<
			is_nonconstref<T_>::value, 
			T_
		>::type castto(const Data &d) const {
			///... do the logic to turn underlying type to T_
			return d.ReferenceValue<T_>();
		}

		template<class T_>
		inline typename std::enable_if<!is_nonconstref<T_>::value, T_>::type castto(const Data &d) const {
			///... do the logic to turn underlying type to T_
			return d.GetValue<T_>();
		}

		template<int P_>
		typename std::enable_if<!extractvector<param<P_>>::isvector, param<P_>>::type
		accumulatevector(const std::vector<Data> &parameters) const {
			Utils::ASSERT_FALSE("Invalid accumulation");
		}

		template<int P_>
		typename std::enable_if<extractvector<param<P_>>::isvector, param<P_>>::type
		accumulatevector(const std::vector<Data> &parameters) const {
			param<P_> v;
			for(unsigned i=P_; i<parameters.size(); i++) {
				v.push_back(castto<typename extractvector<param<P_>>::inner>(parameters[i]));
			}

			return v;
		}

		template<int P_>
		inline typename TMP::Choose<
			is_nonconstref<param<P_>>::value && !extractvector<param<P_>>::isvector,
			std::reference_wrapper<typename std::remove_reference<param<P_>>::type>,
			param<P_>
		>::Type cast(const std::vector<Data> &parameters) const {
			ASSERT(parameters.size()>P_, "Number of parameters does not match");

			if(P_-(parent->IsMember() && !parent->IsStatic())==this->parameters.size()-1 && repeatlast) {
				ASSERT(extractvector<param<P_>>::isvector, "Repeating parameter should be a vector");

				return accumulatevector<P_>(parameters);
			}
			
			return castto<param<P_>>(parameters[P_]);
		}

		template<class R_, int ...S_>
		typename std::enable_if<!std::is_same<R_, void>::value && !std::is_reference<R_>::value && !std::is_pointer<R_>::value, Data>::type
		callfn(TMP::Sequence<S_...>, const std::vector<Data> &parameters) const {
			Data data;
			
			ASSERT((!returnsref || returntype==Types::Variant()), "Embedded function does not return a reference");
			
			data=Data(returntype, Any(
				std::bind(fn, cast<S_>(parameters)...)()
			), false, returnsconst);
			
			ASSERT((!returnsref || (returntype==Types::Variant() && data.GetValue<Data>().IsReference())),
				   "Embedded function does not return a reference");
			
			if(returnsref && returntype==Types::Variant()) {
				if(!data.GetValue<Data>().IsReference()) {
					throw CastException("Non-reference variant", "Reference variant", "While returning value from "+parent->GetName());
				}
			}
			
			return data;
		}
		
		template<class R_, int ...S_>
		typename std::enable_if<!std::is_same<R_, void>::value && std::is_reference<R_>::value && !std::is_pointer<R_>::value, Data>::type
		callfn(TMP::Sequence<S_...>, const std::vector<Data> &parameters) const {
			Data data;
			if(returnsref || returntype->IsReferenceType()) {
				data=Data(returntype, Any(
					&std::bind(fn, cast<S_>(parameters)...)()
				), true, returnsconst);
			}
			else {
				Utils::ASSERT_FALSE("Cannot happen");
			}
			
			return data;
		}
		
		template<class R_, int ...S_>
		typename std::enable_if<!std::is_same<R_, void>::value && std::is_pointer<R_>::value, Data>::type
		callfn(TMP::Sequence<S_...>, const std::vector<Data> &parameters) const {
			Data data;
			if(returnsref || returntype->IsReferenceType()) {
				data=Data(returntype, Any(
					std::bind(fn, cast<S_>(parameters)...)()
				), true, returnsconst);
			}
			else {
				Utils::ASSERT_FALSE("Cannot happen");
			}
			
			return data;
		}
		
		template<class R_, int ...S_>
		typename std::enable_if<std::is_same<R_, void>::value, Data>::type
		callfn(TMP::Sequence<S_...>, const std::vector<Data> &parameters) const {
			std::bind(fn, cast<S_>(parameters)...)();

			return Data::Invalid();
		}

		template<int P_>
		void checkparam() {
			using T=param<P_>;
			TMP::AbstractRTTC<typename std::remove_pointer<T>::type> rtt;
			
			bool ismember=parent->IsMember() && !parent->IsStatic();

			//** Constant and reference checks
			//nonconst ptr or reference
			if(is_nonconstref<T>::value || (std::is_pointer<T>::value && !std::is_const<typename std::remove_pointer<T>::type>::value)) {
				//this pointer
				if(P_==0 && ismember) {
					//should not be a constant
					ASSERT(
						!IsConstant(), 
						"This function variant is marked as const, yet its implementation requires non-const "
						"pointer or reference\n"
						"in function "+parent->GetName(), 4, 3
					);
				}
				else {
					const auto &param=parameters[P_-ismember];
					
					//repeating parameters cannot be a non-const reference or a pointer
					ASSERT(
						P_-ismember!=parameters.size()-1 || !repeatlast, 
						"Repeating parameter vectors cannot be non-const references"
						"in function "+parent->GetName(), 4, 3
					);
					
					//parameter should be a reference
					ASSERT(param.IsReference(),
						"Parameter #"+String::From(P_-ismember)+" is not declared as reference, "
						"yet its implementation is\n"
						"in function "+parent->GetName(), 4, 3
					);
					
					//and should not be a constant
					ASSERT(!param.IsConstant(),
						"Parameter #"+String::From(P_-ismember)+" is declared as constant, "
						"yet its implementation is not const\n"
						"in function "+parent->GetName(), 4, 3
					);
				}
			}
			//const pointer
			else if(std::is_pointer<T>::value) {
				//this pointer
				if(P_==0 && ismember) {
#ifdef TEST
					//it is ok if the function is not marked as constant, but this might be a mistake
					if(!IsConstant()) {
						std::cout<<"This function variant is not marked as const, yet its implementation requires const "
						"pointer\n"
						"in function "+parent->GetName()<<std::endl;
					}
#endif
				}
				else {
					const auto &param=parameters[P_-ismember];
					
					//a repeating parameter cannot be a pointer
					ASSERT(
						P_-ismember!=parameters.size()-1 || !repeatlast, 
						"Repeating parameter vectors cannot be a pointer"
					);
					
					//parameter should be a reference, since type is a pointer
					ASSERT(param.IsReference(),
						   "Parameter #"+String::From(P_-ismember+1)+" is not declared as reference, "
						   "yet its implementation is\n"
						   "in function "+parent->GetName(), 4, 3
					);
					
#ifdef TEST
					//it is ok if the parameter is not marked as constant, but this might be a mistake
					if(!param.IsConstant()) {
						std::cout<< "Parameter #"+String::From(P_-ismember+1)+" is not marked as const, "
						"yet its implementation requires const pointer\n"
						"in function "+parent->GetName()<<std::endl;
					}
#endif
				}
			}
			else if(std::is_reference<T>::value) { //const ref can be anything
				//nothing to do
			}
			else if(std::is_const<T>::value) { //constant value type
				//this pointer
				if(P_==0 && ismember) {
					//should not a be constant, this behavior can be supported but it might lead to more problems
					ASSERT(
						IsConstant(), 
						"This function variant is marked as a non-const member function, "
						"yet its implementation requests a constant value which cannot modify the this pointer.\n"
						"in function "+parent->GetName(), 4, 3
					);
				}
				else {
					const auto &param=parameters[P_-ismember];
					
					//if not the repeating parmeter
					if(P_-ismember!=parameters.size()-1 || !repeatlast) {
						ASSERT(!param.IsReference(),
							"Parameter #"+String::From(P_-ismember+1)+" is declared as reference "
							"yet its implementation not\n"
							"in function "+parent->GetName(), 4, 3
						);
						
#ifdef TEST
						//it is ok if the parameter is not marked as constant, but this might be a mistake
						if(!param.IsConstant()) {
							std::cout<<"Parameter #"+String::From(P_-ismember+1)+" is not marked as const, yet its implementation requires const "
							"value\n"
							"in function "+parent->GetName()<<std::endl;
						}
#endif
					}
					//else const std::vector<...> is allowed
				}
			}
			else { //normal value type
				//this pointer
				if(P_==0 && ismember) {
					//if passed by value, this function cannot modify this pointer, and therefore, should be constant
					ASSERT(
						IsConstant(), 
						"This function variant is marked as a non-const member function, "
						"yet its implementation requests a value which cannot modify the this pointer.\n"
						"in function "+parent->GetName(), 4, 3
					);
				}
				else {
					const auto &param=parameters[P_-ismember];
					
					//if not the repeating parameter
					if(P_-ismember!=parameters.size()-1 || !repeatlast) {
						//cannot be a reference as it is passed by value
						ASSERT(!param.IsReference(),
							"Parameter #"+String::From(P_-ismember+1)+" is declared as reference, "
							"yet its implementation not\n"
							"in function "+parent->GetName(), 4, 3
						);
						
						//for the sake of clarity, if a function requires const, it should be marked as const
						ASSERT(!param.IsConstant(),
							"Parameter #"+String::From(P_-ismember+1)+" is declared as constant, "
							"yet its implementation is not const\n"
							"in function "+parent->GetName(), 4, 3
						);
					}
					//else std::vector<...> is allowed
				}
			}
			
			//**type check
			//** 
			//this pointer
			if(P_==0 && ismember) {
				ASSERT(
					(rtt.NormalType==parent->GetOwner().TypeInterface.NormalType),
					"The declared type ("+parent->GetOwner().GetName()+", "+
					parent->GetOwner().TypeInterface.NormalType.Name()+") of "
					"parameter #"+String::From(P_-ismember+1)+" does not match with the function type ("+
					rtt.NormalType.Name()+")\n"+
					"in function "+parent->GetName(), 4, 3
				);
			}
			//repeating parameter
			else if(P_-ismember==parameters.size()-1 && repeatlast) {
				const auto &param=parameters[P_-ismember];
				
				ASSERT(
					(TMP::RTT<typename extractvector<T>::inner>()==param.GetType().TypeInterface.NormalType),
					"The declared type ("+parent->GetOwner().GetName()+", "+
					parent->GetOwner().TypeInterface.NormalType.Name()+") of "
					"parameter #"+String::From(P_-ismember+1)+" does not match with the function type ("+
					rtt.NormalType.Name()+")\n"+
					"in function "+parent->GetName(), 4, 3
				);
			}
			//regular parameters
			else {
				const auto &param=parameters[P_-ismember];
				
				ASSERT(
					rtt.NormalType==param.GetType().TypeInterface.NormalType,
					"The declared type ("+param.GetType().GetName()+", "+param.GetType().TypeInterface.NormalType.Name()+") of "
					"parameter #"+String::From(P_-ismember+1)+" does not match with the function type ("+
					rtt.NormalType.Name()+")\n"
					"in function "+parent->GetName(), 4, 3
				);
			}
		}
		
		template<int ...S_>
		void check(TMP::Sequence<S_...>) {
			char dummy[] = {0, (checkparam<S_>(),'\0')...};
		}
		
		virtual void dochecks(bool ismethod) override {
			Function::Overload::dochecks(ismethod);

			//check return type
			//if void
			if(std::is_same<typename traits::ReturnType, void>::value) {
				//return type should be nullptr
				ASSERT(
					returntype==nullptr, 
					"This function variant expects a return type of "+
				    returntype->GetName()+"\n"
					"in function "+parent->GetName(), 4, 3
				);
			}
			else {
				ASSERT(
					returntype!=nullptr, 
					"Return type is marked as void. Supplied function's return type is :"+
					Utils::GetTypeName<typename traits::ReturnType>()+
					"in function "+parent->GetName(), 4, 3
				);
				
				TMP::AbstractRTT<typename traits::ReturnType> returnrtt;

				//**return type check

				//returns a reference value
				if(returnsref || returntype->IsReferenceType()) {
					//in case of const
					if(returnsconst) {
						//can either return const ptr or const ref of the given type
						ASSERT(
							returntype->TypeInterface.ConstPtrType==returnrtt ||
							returntype->TypeInterface.ConstRefType==returnrtt,
							"Return type of the function ("+returntype->GetName()+", "+
							returntype->TypeInterface.ConstPtrType.Name()+") does not match "
							"with its implementation ("+returnrtt.Name()+")"
							"in function "+parent->GetName(), 4, 3
						);
					}
					else {
						//can either return non-const ptr or non-const ref of the given type
						ASSERT(
							returntype->TypeInterface.PtrType==returnrtt ||
							returntype->TypeInterface.RefType==returnrtt,
							"Return type of the function ("+returntype->GetName()+", "+
							returntype->TypeInterface.PtrType.Name()+") does not match "
							"with its implementation ("+returnrtt.Name()+")"
							"in function "+parent->GetName(), 4, 3
						);
					}
				}
				//returns a value
				else {
					//in case of const
					if(returnsconst) {
						//should return const of that type, non-const can also be supported but
						//no point in doing so
						ASSERT(
							returntype->TypeInterface.ConstType==returnrtt,
							"Return type of the function ("+returntype->GetName()+", "+
							returntype->TypeInterface.ConstType.Name()+") does not match "
							"with its implementation ("+returnrtt.Name()+")"
							"in function "+parent->GetName(), 4, 3
						);
					}
					else {
						//should return the type, const can also be supported but
						//no point in doing so
						ASSERT(
							returntype->TypeInterface.NormalType==returnrtt,
							"Return type of the function ("+returntype->GetName()+", "+
							returntype->TypeInterface.NormalType.Name()+") does not match "
							"with its implementation ("+returnrtt.Name()+")"
							"in function "+parent->GetName(), 4, 3
						);
					}
				}
			}
			
			check(typename TMP::Generate<traits::Arity>::Type());
		}
		
		F_ fn;
	};
	
	template<class F_, class ...P_>
	Scripting::Function::Overload *MapFunction(F_ fn, const Type *returntype, ParameterList parameters, P_ ...tags) {
		return new MappedFunction<F_>(fn, returntype, std::move(parameters), tags...);
	}

	/**
		* This class makes working with operators easier.
		*/
	class MappedOperator : public Scripting::Function {
	public:
		/// Constructor, returntype and parent could be nullptr, tags are optional. 
		template<class F_>
		MappedOperator(const std::string &name, const std::string &help, const Type *returntype, 
					   const Type *parent, const Type *rhs, F_ fn) :
		Function( name, help, *parent, {}, Scripting::OperatorTag ) {
			ASSERT(returntype, "Operators should have a return type", 1, 1);
			ASSERT(parent, "Operators should belong a class", 1, 1);
			
			Function::AddOverload(
				MapFunction(fn, returntype, 
					{
						Scripting::Parameter("rhs", "Right hand side of the operator", rhs)
					},
					ConstTag
				)
			);
		}
		
		/// Adds a new operator overload
		template<class F_>
		void AddOverload(const Type *returntype, const Type *rhs, F_ fn) {
			ASSERT(returntype, "Operators should have a return type", 1, 1);
			
			Function::AddOverload(
				MapFunction(fn, returntype, 
					{
						Scripting::Parameter("rhs", 
							"Right hand side of the operator", rhs
						)
					}, 
					ConstTag
				)
			);
		}
		
	private:
	};
	
	/**
	* Creates a comparison function
	*/
	#define MAP_COMPARE(opname, op, mappedtype, cpptype) \
		new MappedOperator( #opname, \
			"Compares two "#mappedtype" types.", \
			Bool, mappedtype, mappedtype, [](cpptype l, cpptype r) { return l op r; } \
		)
	
	/**
	* Maps a constructor for type casting, works for value types
	*/
	template<class from_, class to_>
	Scripting::Function::Overload *MapTypecast(Type *from, Type *to, bool implicit=true) {
		if(implicit) {
			return MapFunction(
				[](from_ val) { 
					return to_(val); 
				}, to, 
				{ 
					Parameter("value", "", from) 
				},
				ImplicitTag
			);
		}
		else {
			return MapFunction(
				[](from_ val) { 
					return to_(val); 
				}, to, 
				{ 
					Parameter("value", "", from) 
				}
			);
		}
	}

	
	/**
	* Maps a constructor for type casting, works for polymorphic types
	*/
	template<class from_, class to_>
	Scripting::Function::Overload *MapDynamiccast(Type *from, Type *to, bool implicit=true) {
		if(implicit) {
			return MapFunction(
				[](from_ &val) { 
					return dynamic_cast<to_&>(val); 
				}, to, 
				{ 
					Parameter("value", "", from) 
				},
				ImplicitTag
			);
		}
		else {
			return MapFunction(
				[](from_ &val) { 
					return dynamic_cast<to_&>(val); 
				}, to, 
				{ 
					Parameter("value", "", from) 
				}
			);
		}
	}
		
	/**
	* Maps a constructor for type casting, works for polymorphic types
	*/
	template<class from_, class to_>
	Scripting::Function::Overload *MapConstDynamiccast(Type *from, Type *to, bool implicit=true) {
		if(implicit) {
			return MapFunction(
				[](from_ &val) { 
					return dynamic_cast<to_&>(val); 
				}, to, 
				{ 
					Parameter("value", "", from) 
				},
				ImplicitTag
			);
		}
		else {
			return MapFunction(
				[](from_ &val) { 
					return dynamic_cast<to_&>(val); 
				}, to, 
				{ 
					Parameter("value", "", from) 
				}
			);
		}
	}

	/**
	* Maps a constructor for type casting, works for reference types
	*/
	template<class from_, class to_>
	Scripting::Function::Overload *MapStaticcast(Type *from, Type *to, bool implicit=true) {
		if(implicit) {
			return MapFunction(
				[](const from_ &val) { 
					return static_cast<const to_&>(val); 
				}, to, 
				{ 
					Parameter("value", "", from) 
				},
				ImplicitTag
			);
		}
		else {
			return MapFunction(
				[](const from_ &val) { 
					return static_cast<const to_&>(val); 
				}, to, 
				{ 
					Parameter("value", "", from) 
				}
			);
		}
	}
	
	/**
	* Maps a constructor for type casting, works for const reference types
	*/
	template<class from_, class to_>
	Scripting::Function::Overload *MapConstStaticcast(Type *from, Type *to, bool implicit=true) {
		if(implicit) {
			return MapFunction(
				[](const from_ &val) { 
					return static_cast<const to_&>(val); 
				}, to, 
				{ 
					Parameter("value", "", from) 
				},
				ImplicitTag
			);
		}
		else {
			return MapFunction(
				[](const from_ &val) { 
					return static_cast<const to_&>(val); 
				}, to, 
				{ 
					Parameter("value", "", from) 
				}
			);
		}
	}
	
	/**
	* This class allows a one to one mapping of a data member to a c++ data member. First template
	* parameter is the type of the object and the second is the type of the data member
	*/
	template<class C_, class T_>
	class MappedData : public DataMember {
	public:
		/// Constructor
		template<class ...P_>
		MappedData(T_ C_::*member, const std::string &name, const std::string &help, const Type *type, P_ ...tags) :
		DataMember(name, help, *type, tags...), member(member) {
			ASSERT(type, "Type cannot be nullptr", 1, 2);
		}
		
		virtual Data Get(const Data &data) const override {
			return {GetType(), data.GetValue<C_>().*member};
		}
		
		/// Sets the data of the data member
		virtual void Set(Data &source, const Data &value) const override {
			if(source.IsReference()) {					
				C_ &obj  = source.ReferenceValue<C_&>();
				obj.*member = value.GetValue <T_>();
			}
			else {
				C_ obj  = source.GetValue<C_>();
				obj.*member = value.GetValue <T_>();
				source={source.GetType(), obj};
			}
		}
		
	protected:
		void typecheck(const Type *type) {
			ASSERT(type->GetDefaultValue().TypeCheck<C_>(), "The type of mapped data does not match with the type "
				"it has placed in.", 2, 2);
		}
		
	private:
		T_ C_::*member;
	};
	
	/**
		* This class allows a one to one mapping of a data member to a c++ data member
		*/
	template<class C_, class T_>
	class MappedData<C_*, T_> : public DataMember {
	public:
		/// Constructor
		template<class ...P_>
		MappedData(T_ C_::*member, const std::string &name, const std::string &help, const Type *type, P_ ...tags) :
		DataMember(name, help, *type, tags...), member(member) {
			ASSERT(type, "Type cannot be nullptr", 1, 2);
			ASSERT(type->GetDefaultValue().TypeCheck<T_>(), "Reported type "+type->GetName()+
					"does not match with c++ type: "+Utils::GetTypeName<T_>(), 1, 2);
		}
		
		virtual Data Get(const Data &data) const override {
			C_ *obj=data.GetValue<C_*>();
			if(obj==nullptr) {
				throw NullValueException("", "The value of the object was null while accessing to " + GetName() + " member.");
			}
			return {GetType(), obj->*member};
		}			
		
		/// Sets the data of the data member
		virtual void Set(Data &source, const Data &value) const override {
			C_ *obj=source.GetValue<C_*>();
			if(obj==nullptr) {
				throw NullValueException("", "The value of the object was null while accessing to " + GetName() + " member.");
			}
			obj->*member = value.GetValue <T_>();
		}			
		
	protected:
		void typecheck(const Type *type) {
			ASSERT(type->GetDefaultValue().TypeCheck<C_*>(), "The type of mapped data does not match with the type "
			"it has placed in.", 2, 2);
		}
		
	private:
		T_ C_::*member;
	};
	
	
	/**
		* This class maps a data accessor functions to a data member
		*/
	template <class C_, class T_>
	class DataAccessor : public DataMember {
	public:
		
		/// Constructor
		template<class ...P_>
		DataAccessor(std::function<T_(const C_ &)> getter, std::function<void(C_ &, const T_ &)> setter, const std::string &name, const std::string &help, const Type *type, P_ ...tags) :
		DataMember(name, help, *type, tags...), getter(getter), setter(setter) {
			ASSERT(type, "Type cannot be nullptr", 1, 2);
			ASSERT(type->GetDefaultValue().TypeCheck<T_>(), "Reported type "+type->GetName()+
					"does not match with c++ type: "+Utils::GetTypeName<T_>(), 1, 2);
		}
		
		virtual Data Get(const Data &data) const override {
			return {GetType(), getter(data.GetValue<C_>())};
		}			
		
		/// Sets the data of the data member
		virtual void Set(Data &source, const Data &value) const override {
			C_ obj = source.GetValue<C_>();
			setter(obj, value.GetValue<T_>());
			source= {source.GetType(), obj};
		}
		
	protected:
		void typecheck(const Type *type) {
			ASSERT(type->GetDefaultValue().TypeCheck<C_>(), "The type of mapped data does not match with the type "
			"it has placed in.", 2, 2);
		}
		
	private:
		std::function<T_(const C_ &)> getter;
		std::function<void(C_ &, const T_ &)> setter;
	};
	
	/**
		* This class maps a data accessor functions to a data member
		*/
	template <class C_, class T_>
	class DataAccessor<C_*, T_> : public DataMember {
	public:
		
		/// Constructor
		template<class ...P_>
		DataAccessor(std::function<T_(const C_ &)> getter, std::function<void(C_ &, const T_ &)> setter, const std::string &name, const std::string &help, const Type *type, P_ ...tags) :
		DataMember(name, help, *type, tags...), getter(getter), setter(setter) {
			ASSERT(type, "Type cannot be nullptr", 1, 2);
			ASSERT(type->GetDefaultValue().TypeCheck<T_>(), "Reported type "+type->GetName()+
					"does not match with c++ type: "+Utils::GetTypeName<T_>(), 1, 2);
		}
		
		virtual Data Get(const Data &data) const override {
			C_ *obj=data.GetValue<C_*>();
			if(obj==nullptr) {
				throw NullValueException("", "The value of the object was null while accessing to " + GetName() + " member.");
			}
			return {GetType(), getter(*obj)};
		}
		
		/// Sets the data of the data member
		virtual void Set(Data &source, const Data &value) const override {
			C_ *obj = source.GetValue<C_*>();
			if(obj==nullptr) {
				throw NullValueException("", "The value of the object was null while accessing to " + GetName() + " member.");
			}
			setter(*obj, value.GetValue<T_>());
		}
		
	protected:
		void typecheck(const Type *type) {
			ASSERT(type->GetDefaultValue().TypeCheck<C_*>(), "The type of mapped data does not match with the type "
			"it has placed in.", 2, 2);
		}
		
	private:
		std::function<T_(const C_ &)> getter;
		std::function<void(C_ &, const T_ &)> setter;
	};
	
} }