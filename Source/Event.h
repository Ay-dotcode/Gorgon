/// @file Event.h contains event distribution mechanism
#pragma once

#include <type_traits>
#include <vector>
#include <functional>

#include "Types.h"
#include "Container/Collection.h"


namespace Gorgon {

	/// @cond INTERNAL
	namespace internal  {
		template<class Source_, typename... Params_>
		struct HandlerBase {
			virtual void Fire(Source_ *, Params_...) = 0;
		};

		template<class Source_, typename... Params_>
		struct EmptyHandlerFn : public HandlerBase<Source_, Params_...> {
			EmptyHandlerFn(std::function<void()> fn) : fn(fn) { }

			virtual void Fire(Source_ *, Params_...) {
				fn();
			}

			std::function<void()> fn;
		};

		template<class Source_, typename... Params_>
		struct ArgsHandlerFn : public HandlerBase<Source_, Params_...> {
			ArgsHandlerFn(std::function<void(Params_...)> fn) : fn(fn) { }

			virtual void Fire(Source_ *, Params_... args) {
				fn(args...);
			}

			std::function<void(Params_...)> fn;
		};

		template<class Source_, typename... Params_>
		struct FullHandlerFn : public HandlerBase<Source_, Params_...> {
			FullHandlerFn(std::function<void(Source_&, Params_...)> fn) : fn(fn) { }

			virtual void Fire(Source_ *source, Params_... args) {
				fn(*source, args...);
			}

			std::function<void(Source_&, Params_...)> fn;
		};

		template <typename U>
		struct has_parop {
			typedef char yes;
			struct no { char _[2]; };
			template<typename T, typename L=decltype(&T::operator())>
			static yes impl(T*) { return yes(); }
			static no  impl(...) { return no(); }

			enum { value = sizeof(impl(static_cast<U*>(0))) == sizeof(yes) };
		};

		template<class Source_, class... Params_>
		static HandlerBase<Source_, Params_...>& createhandlerfn(void(*fn)()) {
			return *new EmptyHandlerFn<Source_, Params_...>(fn);
		}

		template<class Source_, class... Params_>
		static HandlerBase<Source_, Params_...>& createhandlerfn(void(*fn)(Params_...)) {
			return *new ArgsHandlerFn<Source_, Params_...>(fn);
		}

		template<class Source_, class... Params_>
		static HandlerBase<Source_, Params_...>& createhandlerfn(void(*fn)(Source_ &, Params_...)) {
			return *new FullHandlerFn<Source_, Params_...>(fn);
		}

		template <typename T>
		struct argscount : public argscount<decltype(&T::operator())> { };

		template <typename ClassType, typename ReturnType, typename... Args>
		struct argscount<ReturnType(ClassType::*)(Args...) const> {
			enum { value = sizeof...(Args) };
		};

		template<class F_, class Source_, int N, class... Params_>
		static typename std::enable_if<argscount<F_>::value==0, HandlerBase<Source_, Params_...>&>::type createhandler(F_ fn) {
			return *new EmptyHandlerFn<Source_, Params_...>(fn);
		}

		template<class F_, class Source_, int N, typename... Params_>
		static typename std::enable_if<argscount<F_>::value!=0 && argscount<F_>::value==N, HandlerBase<Source_, Params_...>&>::type 
		createhandler(F_ fn) {
			return *new ArgsHandlerFn<Source_, Params_...>(fn);
		}

		template<class F_, class Source_, int N, class... Params_>
		static typename std::enable_if<argscount<F_>::value==N+1, HandlerBase<Source_, Params_...>&>::type createhandler(F_ fn) {
			return *new FullHandlerFn<Source_, Params_...>(fn);
		}

		template<class F_, class Source_, class... Params_>
		typename std::enable_if<has_parop<F_>::value, HandlerBase<Source_, Params_...>&>::type 
		create_handler(F_ fn) {
			return createhandler<F_, Source_, sizeof...(Params_), Params_...>(fn);
		}

		template<class F_, class Source_, class... Params_, typename N=void>
		typename std::enable_if<!has_parop<F_>::value, HandlerBase<Source_, Params_...>&>::type 
		create_handler(F_ fn) {
			return createhandlerfn<Source_, Params_...>(fn);
		}


		/// Solution by Peter Dimov-5: http://std.2283326.n4.nabble.com/bind-Possible-to-use-variadic-templates-with-bind-td2557818.html
		template<int...> struct int_tuple { };

		// make indexes impl is a helper for make indexes 
		template<int I, typename IntTuple, typename... Types>
		struct make_indexes_impl;

		template<int I, int... Indexes, typename T, typename... Types>
		struct make_indexes_impl<I, int_tuple<Indexes...>, T, Types...> {
			typedef typename make_indexes_impl<I+1,
				int_tuple<Indexes..., I>,
				Types...>::type type;
		};

		template<int I, int... Indexes>
		struct make_indexes_impl<I, int_tuple<Indexes...> > {
			typedef int_tuple<Indexes...> type;
		};

		template<typename... Types>
		struct make_indexes : make_indexes_impl<0, int_tuple<>, Types...> { };

		template< class T, class... Args, int... Indexes >
		std::function< void(Args...) > make_func_helper(void (T::* pm) (Args...),
			T * that, int_tuple< Indexes... >) {
			return std::bind(pm, that, std::_Ph<Indexes+1>()...);
		}

		template< class T, class... Args >
		std::function< void(Args...) > make_func(void (T::* pm) (Args...), T *
			that) {
			return make_func_helper(pm, that, typename
				make_indexes<Args...>::type());
		}
	}

	/// @endcond
	
	/// This class provides event mechanism. Different function signatures are allowed to
	/// as event handlers. These are:
	///
	/// * <b>`void fn()`</b> neither event source nor event parameters are supplied. 
	/// * <b>`void fn(Source_ &source)`</b> event source will be passed
	/// * <b>`void fn(Params_... params)`</b> parameters will be passed
	/// * <b>`void fn(Source_ &source, Params_... params)`</b> both the source and parameters
	///   will be passed
	/// 
	/// Class members or lambda functions can also be used as event handlers. An
	/// event handler can be registered using Register function. If necessary, 
	/// RegisterNamespace, RegisterMember and RegisterLambda methods can be used.
	template<class Source_=Empty, typename... Params_>
	class Event {		
	public:		
		
		Event() : source(nullptr) {
			static_assert( std::is_same<Source_, Empty>::value , "Empty constructor cannot be used." );
		}
		
		Event(Source_ &source) : source(&source) {
			static_assert( !std::is_same<Source_, Empty>::value, "Filling constructor is not required, use the default." );
		}

		~Event() { handlers.Destroy(); }
		
		Event(const Event &) = delete;
		
		Event &operator =(const Event &) = delete;

		template<class F_>
		void Register(F_ fn) {
			handlers.Add(internal::create_handler<F_, Source_, Params_...>(fn));
		}

		template<class C_, typename... A_>
		void Register(C_ &c, void(C_::*fn)(A_...)) {
			std::function<void(A_...)> f=internal::make_func(fn, &c);

			handlers.Add(internal::create_handler<decltype(f), Source_, Params_...>(f));
		}


		void operator()(Params_... args) {
			for(auto &h : handlers) {
				h.Fire(source, args...);
			}
		}
		
	private:
		Source_ *source;
		Container::Collection<internal::HandlerBase<Source_, Params_...>> handlers;
	};
	
	
}