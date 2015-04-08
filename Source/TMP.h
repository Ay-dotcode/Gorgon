/// @file TMP.h This file contains template metaprogramming methods and 
///             classes used throughout Gorgon Library.

#pragma once

#include <tuple>
#include <typeinfo>
#include "Utils/Compiler.h"

namespace Gorgon {
	namespace TMP {

		/// A sequence element, can be used to represent a sequence of integer numbers
		template<int ...> struct Sequence {};
		
		/// Generates a sequence from 0 to the given value.
		template<int N, int ...S> struct Generate : Generate<N-1, N-1, S...> {};
		
		///@cond
		template<int ...S> struct Generate<0, S...>{ typedef Sequence<S...> Type; };
		
		template<class F_>
		struct FunctionTraits;
		
		template<class R_, class... Args_>
		struct FunctionTraits<R_(*)(Args_...)> : public FunctionTraits<R_(Args_...)>
		{};
		
		///@endcond
		
		/** 
		 * Determines traits of a function. Works with regular, member and std::function.
		 * First argument of the member function is mapped as a pointer to the class. If the
		 * function is a const member function class pointer is set to be const
		 *
		 * Some examples:
		 * @code
		 * typename FunctionTraits<decltype(myfn)>::ReturnType ret = myfn();
		 * 
		 * typename FunctionTraits<decltype(myfn)>::template Arguments<1> secondarg;
		 * 
		 * template<class F_>
		 * typename std::enable_if<FunctionTraits<F_>::IsMember>::type MembersOnly(F_ fn) { ... }
		 * 
		 * int params = FunctionTraits<decltype(&A::myfn)>::Arity;
		 * @endcode
		 */
		template<class R_, class ...Args_>
		struct FunctionTraits<R_(Args_...)> {
			/// Return type of the function
			using ReturnType = R_;
			
			/// Number of arguments of the function
			static const int  Arity = sizeof...(Args_);
			
			/// Whether this function is a member function
			static const bool IsMember = false;
			
			/// This struct allow access to types of individual arguments. Do not forget to use template
			/// keyword before arguments.
			template<int N>
			struct Arguments {
				static_assert(N<sizeof...(Args_), "Argument index out of bounds");
				
				/// Type of the argument.
				using Type = typename std::tuple_element<N, std::tuple<Args_...>>::type;
			};
		};
		
		/// @cond
		template<class C_, class R_, class ...Args_>
		struct FunctionTraits<R_(C_::*)(Args_...)> : 
			public FunctionTraits<R_(typename std::decay<C_>::type&, Args_...)>
		{ 
			static const bool IsMember = true;
		};
		
		template<class C_, class R_, class ...Args_>
		struct FunctionTraits<R_(C_::*)(Args_...) const> :
			public FunctionTraits<R_(const typename std::decay<C_>::type&, Args_...)> 
		{ 
			static const bool IsMember = true;
		};
		
		template<class F_>
		struct FunctionTraits
		{
		private:
			using innertype = FunctionTraits<decltype(&F_::operator())>;
		public:
			using ReturnType = typename innertype::ReturnType;
			
			static const int Arity = innertype::Arity - 1;

			static const bool IsMember = false;
			
			template <int N>
			struct Arguments
			{
				static_assert(N<Arity, "Argument index out of bounds");
				using Type = typename innertype::template Arguments<N+1>::Type;
			};
		};
		
		template<class F_>
		struct FunctionTraits<F_&> : public FunctionTraits<F_>
		{};
		
		template<class F_>
		struct FunctionTraits<F_&&> : public FunctionTraits<F_>
		{};
		
		///@endcond
		
		/// This acts like ? operator between two types
		template<bool Cond_, class T1_, class T2_>
		struct Choose;
		
		/// @cond
		template<class T1_, class T2_>
		struct Choose<false, T1_, T2_> {
			using Type = T2_;
		};
		
		template<class T1_, class T2_>
		struct Choose<true, T1_, T2_> {
			using Type = T1_;
		};
		///@endcond
		
		///This class contains information about a runtime type
		class RTTI {
		public:
			///Returns if this type is pointer
			virtual bool IsPointer() const = 0;
			
			///Returns if this type is a reference
			virtual bool IsReference() const = 0;
			
			///Returns if this type is a constant
			virtual bool IsConstant() const = 0;
			
			///Compares the type stored with this service to the given type info
			virtual bool  IsSameType(const std::type_info &) const = 0;
			
			///Compares the type stored with this service to the given type
			template<class T_>
			bool IsSameType() const {
				return IsSameType(typeid(T_));
			}
			
			bool operator ==(const std::type_info &info) const {
				return IsSameType(info);
			}
			
			bool operator !=(const std::type_info &info) const {
				return IsSameType(info);
			}
			
			operator const std::type_info &() const {
				return TypeInfo();
			}
			
			///Returns the size of the object
			virtual long  GetSize() const = 0;
			
			///@name TypeInfo
			///@{
			virtual const std::type_info &TypeInfo() const = 0;
			///@}
			
			///Returns human readable name of the type
			std::string Name() const { return Utils::GetTypeName(TypeInfo()); }
		};
		
		/// This class contains runtime type services that allows dealing with unknown type.
		/// RTTS should be constructed for RTT class. This class is not designed to work with
		/// array types.
		class RTTS : public RTTI {
		public:
			///Duplicates this service
			virtual RTTS *Duplicate() const = 0;
			
			///Creates a new instance of the object. If this type is a reference, pointer of this type
			///is created, this function can return nullptr
			virtual void *Create() const = 0;
			
			///Clones the given object
			virtual void *Clone(const void* const obj) const = 0;
			
			///Deletes the given object
			virtual void  Delete(void* obj) const = 0;
			
			
			///Destructor
			virtual ~RTTS() { }
		};
		
		///Runtime Type. This class implements both RTTS and RTTI
		template<class T_>
		class RTT : public RTTS {
			using StorageType = typename Choose<std::is_reference<T_>::value, typename std::remove_reference<T_>::type*, T_>::Type*;
			using NewType = typename std::remove_const<typename Choose<std::is_reference<T_>::value, typename std::remove_reference<T_>::type*, T_>::Type>::type;
			using CloneType = const typename Choose<std::is_reference<T_>::value, typename std::remove_reference<T_>::type*, T_>::Type* const;
			
			virtual RTTS *Duplicate() const {
				return new RTT<T_>();
			}
			
			virtual void *Create() const {
				return new NewType();
			}
			
			virtual void *Clone(const void* const obj) const override {
				auto n = new NewType(*reinterpret_cast<CloneType>(obj));
				
				return n;
			}
			
			virtual void Delete(void *obj) const override {
				delete static_cast<StorageType>(obj);
			}
			
			virtual bool IsSameType(const std::type_info &info) const override {
				return info==typeid(T_);
			}
			
			virtual const std::type_info &TypeInfo() const override {
				return typeid(T_);
			}
			
			virtual long GetSize() const override { return sizeof(T_); }
			
			virtual bool IsPointer() const override { return std::is_pointer<T_>::value; }

			virtual bool IsReference() const override { return std::is_reference<T_>::value; }

			virtual bool IsConstant() const override { return std::is_const<T_>::value; }
		};
		
		///Runtime Type. This class implements both RTTS and RTTI
		template<class T_>
		class AbstractRTT : public RTTS {
			using StorageType = typename Choose<std::is_reference<T_>::value, typename std::remove_reference<T_>::type*, T_>::Type*;
			using NewType = typename std::remove_const<typename Choose<std::is_reference<T_>::value, typename std::remove_reference<T_>::type*, T_>::Type>::type;
			using CloneType = const typename Choose<std::is_reference<T_>::value, typename std::remove_reference<T_>::type*, T_>::Type* const;
			
			virtual RTTS *Duplicate() const {
				return new AbstractRTT<T_>();
			}
			
			virtual void *Create() const {
				return nullptr;
			}
			
			virtual void *Clone(const void* const obj) const override {
				return nullptr;
			}
			
			virtual void Delete(void *obj) const override {
				delete static_cast<StorageType>(obj);
			}
			
			virtual bool IsSameType(const std::type_info &info) const override {
				return info==typeid(T_);
			}
			
			virtual const std::type_info &TypeInfo() const override {
				return typeid(T_);
			}
			
			virtual long GetSize() const override { return sizeof(T_); }
			
			virtual bool IsPointer() const override { return std::is_pointer<T_>::value; }

			virtual bool IsReference() const override { return std::is_reference<T_>::value; }

			virtual bool IsConstant() const override { return std::is_const<T_>::value; }
		};
		
		
		///Runtime Type Hierarchy
		class RTTH {
		public:
			virtual ~RTTH() {
				delete &NormalType;
				delete &ConstType;
				delete &RefType;
				delete &ConstRefType;
				delete &PtrType;
				delete &ConstPtrType;
			}
			
			RTTS &NormalType, &ConstType, &RefType, &ConstRefType, &PtrType, &ConstPtrType;
			
		protected:
			RTTH(RTTS &normaltype, RTTS &consttype, RTTS &reftype, RTTS &constreftype, RTTS &ptrtype, RTTS &constptrtype) :
			NormalType(normaltype), ConstType(consttype), RefType(reftype), ConstRefType(constreftype),
			PtrType(ptrtype), ConstPtrType(constptrtype)
			{ }
		};
		
		/// Runtime type class, implements RTTH
		template<class T_>
		class RTTC : public RTTH {
		public:
			using BaseType = typename std::remove_const<typename std::remove_reference<T_>::type>::type;
			
			RTTC() : 
			RTTH(
				*new RTT<BaseType >(), *new RTT<const BaseType >(),
				*new RTT<BaseType&>(), *new RTT<const BaseType&>(),
				*new RTT<BaseType*>(), *new RTT<const BaseType*>()
			) 
			{ }
		};
		
		/// Runtime type class, implements RTTH
		template<class T_>
		class AbstractRTTC : public RTTH {
		public:
			using BaseType = typename std::remove_const<typename std::remove_reference<T_>::type>::type;
			
			AbstractRTTC() : 
			RTTH(
				*new AbstractRTT<BaseType >(), *new AbstractRTT<const BaseType >(),
				*new AbstractRTT<BaseType&>(), *new AbstractRTT<const BaseType&>(),
				*new RTT<BaseType*>(), *new RTT<const BaseType*>()
			) 
			{ }
		};
	}
};
