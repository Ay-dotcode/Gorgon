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
		Type(name, help, def, new Any::Type<const T_>(), new Any::Type<T_*>(), new Any::Type<const T_*>(), false)
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
		Type(name, help, def, new Any::Type<const T_*>(), new Any::Type<T_**>(), new Any::Type<const T_**>(), true)
		{
		}
		
		MappedReferenceType(const std::string &name, const std::string &help) : MappedReferenceType(name, help, nullptr) {
		}
		
		
		/// Converts a data of this type to string. This function should never throw, if there is
		/// no data to display, recommended this play is either [ EMPTY ], or Typename #id
		virtual std::string ToString(const Data &data) const override {
			if(data.GetValue<T_*>()==nullptr) return "<null>";
			return ToString_(*data.GetValue<T_*>());
		}
		
		/// Parses a string into this data. This function is allowed to throw.
		virtual Data Parse(const std::string &str) const override {
			return Data(this, Parse_(str));
		}
		
	protected:
		virtual void deleteobject(const Data &obj) const override {
			if(obj.GetValue<T_*>()!=nullptr) {
				delete obj.GetValue<T_*>();
			}
		}
	};

	extern Type &Variant;
	
	template<class F_>
	class MappedFunction : public Scripting::Function::Variant {
		using variant = Scripting::Function::Variant;
		using traits  = TMP::FunctionTraits<F_>;
		template<int P_>
		using param = typename traits::template Arguments<P_>::Type;
	public:
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
		typename std::enable_if<!std::is_same<R_, void>::value && !std::is_reference<R_>::value, Data>::type
		callfn(TMP::Sequence<S_...>, const std::vector<Data> &parameters) const {
			Data data;
			
			ASSERT((!returnsref || returntype==Scripting::Variant), "Embedded function does not return a reference");
			
			data=Data(returntype, Any(
				std::bind(fn, cast<S_>(parameters)...)()
			), false, constant);
			
			ASSERT((!returnsref || (returntype==Scripting::Variant && data.GetValue<Data>().IsReference())), 
				   "Embedded function does not return a reference");
			
			return data;
		}

		template<class R_, int ...S_>
		typename std::enable_if<!std::is_same<R_, void>::value && std::is_reference<R_>::value, Data>::type
		callfn(TMP::Sequence<S_...>, const std::vector<Data> &parameters) const {
			Data data;
			if(returnsref) {
				data=Data(returntype, Any(
					&std::bind(fn, cast<S_>(parameters)...)()
				), true, constant);
			}
			else {
				data=Data(returntype, Any(
					std::bind(fn, cast<S_>(parameters)...)()
				), false, constant);
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
		}
		
		template<int ...S_>
		void check(TMP::Sequence<S_...>) {
			char dummy[] = {0, (checkparam<S_>(),'\0')...};
		}
		
		virtual void dochecks(bool ismethod) override {
			//sanity check
			if(ismethod) {
				ASSERT(returntype==nullptr, "Methods cannot return values. "+returntype->GetName()+" is given.");
			}
			
			//check return type
			//if void
			if(std::is_same<typename traits::ReturnType, void>::value) {
				//return type should be nullptr
				ASSERT(returntype==nullptr, "This function variant expects a return type of "+
				       returntype->GetName()+".");
			}
			else {
				ASSERT(returntype!=nullptr, "Return type is marked as void. Supplied function's return type is :"+
					   Utils::GetTypeName<typename traits::ReturnType>());
			}
			
			check(typename TMP::Generate<traits::Arity>::Type());
		}
		
		F_ fn;
	};
	
	template<class F_, class ...P_>
	Scripting::Function::Variant *MapFunction(F_ fn, const Type *returntype, ParameterList parameters, P_ ...tags) {
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
			
			Function::AddVariant(
				MapFunction(fn, returntype, 
					{
						Scripting::Parameter("rhs", "Right hand side of the operator", rhs)
					},
					ConstTag
				)
			);
		}
		
		template<class F_>
		void AddVariant(const Type *returntype, const Type *rhs, F_ fn) {
			ASSERT(returntype, "Operators should have a return type", 1, 1);
			
			Function::AddVariant(
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
		
		virtual void AddMethod(Variant &var) override {
			Utils::ASSERT_FALSE("Operators cannot have method variants");
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
	Scripting::Function::Variant *Map_Typecast(Type *from, Type *to, bool implicit=true) {
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