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
		
		template<int ...S> struct Generate<0, S...>{ typedef Sequence<S...> Type; };
		
		
		template<class F_>
		struct FunctionTraits;
		
		template<class R_, class... Args_>
		struct FunctionTraits<R_(*)(Args_...)> : public FunctionTraits<R_(Args_...)>
		{};
		
		template<class R_, class ...Args_>
		struct FunctionTraits<R_(Args_...)> {
			using ReturnType = R_;
			
			static const int  Arity = sizeof...(Args_);
			
			static const bool IsMember = false;
			
			template<int N>
			struct Arguments {
				static_assert(N<sizeof...(Args_), "Argument index out of bounds");
				
				using Type = typename std::tuple_element<N, std::tuple<Args_...>>::type;
			};
		};
		
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
		
		/// This acts like ? operator between two types
		template<bool Cond_, class T1_, class T2_>
		struct Choose;
		
		template<class T1_, class T2_>
		struct Choose<false, T1_, T2_> {
			using Type = T1_;
		};
		
		template<class T1_, class T2_>
		struct Choose<true, T1_, T2_> {
			using Type = T2_;
		};
	}
};