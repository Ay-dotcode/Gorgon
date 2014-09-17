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
		
		template<class R_, class ...Args_>
		struct FunctionTraits<R_(*)(Args_...)> {
			using ReturnType = R_;
			
			static const int Arity = sizeof...(Args_);
			
			template<int N>
			struct Arguments {
				static_assert(N<sizeof...(Args_), "Argument index out of bounds");
				
				using Type = typename std::tuple_element<N, std::tuple<Args_...>>::type;
			};
		};
		
	}
};