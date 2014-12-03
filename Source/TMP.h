/// @file TMP.h This file contains template metaprogramming methods and 
///             classes used throughout Gorgon Library.

#pragma once

#include <tuple>

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
			public FunctionTraits<R_(typename std::decay<C_>::type*, Args_...)>
		{ 
			static const bool IsMember = true;
		};
		
		template<class C_, class R_, class ...Args_>
		struct FunctionTraits<R_(C_::*)(Args_...) const> :
			public FunctionTraits<R_(const typename std::decay<C_>::type*, Args_...)> 
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
			using Type = T1_;
		};
		
		template<class T1_, class T2_>
		struct Choose<true, T1_, T2_> {
			using Type = T2_;
		};
		///@endcond
	}
};