/// @file Event.h contains event distribution mechanism
#pragma once

#include <type_traits>
#include <vector>
#include <functional>
#include <mutex>

#include <assert.h>
#include <atomic>

#include "Types.h"
#include "Containers/Collection.h"
#include "Utils/Assert.h"


namespace Gorgon {

	/// @cond INTERNAL
	namespace internal  {

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
		
		template<int n> struct arg {};
		template<> struct arg<1>{ static decltype(std::placeholders::_1) value() {return std::placeholders::_1;} };
		template<> struct arg<2>{ static decltype(std::placeholders::_2) value() {return std::placeholders::_2;} };
		template<> struct arg<3>{ static decltype(std::placeholders::_3) value() {return std::placeholders::_3;} };
		template<> struct arg<4>{ static decltype(std::placeholders::_4) value() {return std::placeholders::_4;} };
		template<> struct arg<5>{ static decltype(std::placeholders::_5) value() {return std::placeholders::_5;} };
		template<> struct arg<6>{ static decltype(std::placeholders::_6) value() {return std::placeholders::_6;} };
		template<> struct arg<7>{ static decltype(std::placeholders::_7) value() {return std::placeholders::_7;} };
		template<> struct arg<8>{ static decltype(std::placeholders::_8) value() {return std::placeholders::_8;} };
		template<> struct arg<9>{ static decltype(std::placeholders::_9) value() {return std::placeholders::_9;} };
		

		template<typename... Types>
		struct make_indexes : make_indexes_impl<0, int_tuple<>, Types...> { };

		template< class T, class... Args, int... Indexes >
		std::function< void(Args...) > make_func_helper(void (T::* pm) (Args...),
			T * that, int_tuple< Indexes... >) {
			return std::bind(pm, that, arg<Indexes+1>::value()...);
		}

		template< class T, class... Args >
		std::function< void(Args...) > make_func(void (T::* pm) (Args...), T *
			that) {
			return make_func_helper(pm, that, typename
				make_indexes<Args...>::type());
		}

		template <typename U>
		struct has_parop {
			typedef char yes;
			struct no { char _[2]; };
			template<typename T, typename L=decltype(&T::operator())>
			static yes impl(T*) { return yes(); }
			static no  impl(...) { return no(); }

			enum { value = sizeof(impl(static_cast<U*>(0))) == sizeof(yes) };
		};

		template <typename T>
		struct argscount : public argscount<decltype(&T::operator())> { };

		template <typename ClassType, typename ReturnType, typename... Args>
		struct argscount<ReturnType(ClassType::*)(Args...) const> {
			enum { value = sizeof...(Args) };
		};

		namespace event {
			template<class Source_, typename... Params_>
			struct HandlerBase {
				virtual void Fire(std::mutex &locker, Source_ *, Params_...) = 0;
				
				virtual ~HandlerBase() {}
			};

			template<class Source_, typename... Params_>
			struct EmptyHandlerFn : public HandlerBase<Source_, Params_...> {
				EmptyHandlerFn(std::function<void()> fn) : fn(fn) { }

				virtual void Fire(std::mutex &locker, Source_ *, Params_...) {
					auto f=fn;
					locker.unlock();
					f();
				}

				std::function<void()> fn;
			};

			template<class Source_, typename... Params_>
			struct ArgsHandlerFn : public HandlerBase<Source_, Params_...> {
				ArgsHandlerFn(std::function<void(Params_...)> fn) : fn(fn) { }

				virtual void Fire(std::mutex &locker, Source_ *, Params_... args) {
					auto f=fn;
					locker.unlock();
					f(args...);
				}

				std::function<void(Params_...)> fn;
			};

			template<class Source_, typename... Params_>
			struct FullHandlerFn : public HandlerBase<Source_, Params_...> {
				static_assert(!std::is_same<Source_, void>::value, "No source class exists for this event (void cannot be passed around)");
				
				FullHandlerFn(std::function<void(Source_&, Params_...)> fn) : fn(fn) { }

				virtual void Fire(std::mutex &locker, Source_ *source, Params_... args) {
					auto f=fn;
					locker.unlock();
					f(*source, args...);
				}

				std::function<void(Source_&, Params_...)> fn;
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

			template<class F_, class Source_, int N, class... Params_>
			static typename std::enable_if<argscount<F_>::value==0, HandlerBase<Source_, Params_...>&>::type 
			createhandler(F_ fn) {
				return *new EmptyHandlerFn<Source_, Params_...>(fn);
			}

			template<class F_, class Source_, int N, typename... Params_>
			static typename std::enable_if<argscount<F_>::value!=0 && argscount<F_>::value==N, HandlerBase<Source_, Params_...>&>::type 
			createhandler(F_ fn) {
				return *new ArgsHandlerFn<Source_, Params_...>(fn);
			}

			template<class F_, class Source_, int N, class... Params_>
			static typename std::enable_if<argscount<F_>::value==N+1, HandlerBase<Source_, Params_...>&>::type 
			createhandler(F_ fn) {
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
		}

	}

	/// @endcond
	
	/// This class provides event mechanism. Different function signatures are allowed to
	/// as event handlers. These are:
	///
	/// * <b>`void fn()`</b> neither event source nor event parameters are supplied. 
	/// * <b>`void fn(Params_... params)`</b> parameters will be passed
	/// * <b>`void fn(Source_ &source, Params_... params)`</b> the source and parameters
	///   will be passed
	/// 
	/// Class members or lambda functions can also be used as event handlers. An
	/// event handler can be registered using Register function.
	template<class Source_=void, typename... Params_>
	class Event {
	public:	

		/// Data type for tokens
		typedef intptr_t Token;
		
		/// Constructor for empty source
		Event() : source(nullptr) {
			fire.clear();
			static_assert( std::is_same<Source_, void>::value , "Empty constructor cannot be used." );
		}
		
		/// Constructor for class specific source
		template <class S_ = Source_>
		explicit Event(typename std::enable_if<!std::is_same<S_, void>::value, S_>::type &source) : source(&source) {
			fire.clear();
			static_assert(!std::is_same<Source_, void>::value, "Filling constructor is not required, use the default.");
		}
		
		/// Constructor for class specific source
		template <class S_ = Source_>
		explicit Event(typename std::enable_if<!std::is_same<S_, void>::value, S_>::type *source) : source(source) {
			ASSERT(source, "Source cannot be nullptr");
			fire.clear();
			static_assert(!std::is_same<Source_, void>::value, "Filling constructor is not required, use the default.");
		}
		
		/// Move constructor
		Event(Event &&event) : source(nullptr) {
			Swap(event);
		}
		

		/// Destructor
		~Event() {
			assert(("An event cannot be destroyed while its being fired.", !fire.test_and_set()));
			std::lock_guard<std::mutex> g(access);
			
			handlers.Destroy();
		}
		
		/// Copy constructor is disabled
		Event(const Event &) = delete;
		
		/// Copy assignment is disabled
		Event &operator =(const Event &) = delete;

		/// Move assignment, should be called synchronized
		Event &operator =(const Event &&other) {
			if(&other==this) return *this;

			std::lock_guard<std::mutex> g(access);

			assert(("An event cannot be moved into while its being fired.", !fire.test_and_set()));

			handlers.Destroy();

			source=nullptr;
			Swap(other);

			fire.clear();
		}

		/// Swaps two Events, used for move semantics
		void Swap(Event &other) {
			if(&other==this) return;

			using std::swap;

			assert(("An event cannot be swapped while its being fired.", !fire.test_and_set() && !other.fire.test_and_set()));

			swap(source, other.source);
			swap(handlers, other.handlers);

			fire.clear();
			other.fire.clear();
		}

		/// Registers a new function to be called when this event is triggered. This function can
		/// be called from event handler of the same event. The registered event handler will be
		/// called immediately in this case.
		template<class F_>
		Token Register(F_ fn) {
			auto &handler=internal::event::create_handler<F_, Source_, Params_...>(fn);

			std::lock_guard<std::mutex> g(access);
			handlers.Add(handler);

			return reinterpret_cast<intptr_t>(&handler);
		}

		/// Registers a new function to be called when this event is triggered. This variant is 
		/// designed to be used with member functions. **Example:**
		/// @code
		/// A a;
		/// b.ClickEvent.Register(a, A::f);
		/// @endcode
		///
		/// This function can be called from event handler of the same event. The registered 
		/// event handler will be called immediately in this case.
		template<class C_, typename... A_>
		Token Register(C_ &c, void(C_::*fn)(A_...)) {
			std::function<void(A_...)> f=internal::make_func(fn, &c);

			return Register(f);
		}

		/// Unregisters the given marked with the given token. This function performs no
		/// operation if the token is not contained within this event. A handler can be
		/// unregistered safely while the event is being fired. In this case, if the event that
		/// is being deleted is different from the current event handler, the deleted event
		/// handler might have already been fired. If not, it will not be fired.
		void Unregister(Token token) {
			std::lock_guard<std::mutex> g(access);

			auto item=reinterpret_cast<internal::event::HandlerBase<Source_, Params_...>*>(token);
			
			auto l=handlers.FindLocation(item);
			if(l==-1) return;
			
			if(iterator.IsValid() && iterator.CurrentPtr()==item) {
				handlers.Delete(l);
				
				//Collection iterator can point element -1
				iterator.Previous(); 
			}
			else {
				handlers.Delete(l);
			}
		}

		/// Fire this event. This function will never allow recursive firing, i.e. an event handler
		/// cannot cause this event to be fired again.
		void operator()(Params_... args) {
			//prevent recursion
			if(fire.test_and_set()) return;

			try {
				for(iterator=handlers.begin(); iterator.IsValid(); iterator.Next()) {
					access.lock();
					// fire method will unlock access after it creates a local copy of the function
					// this allows the fired object to be safely deleted.
					iterator->Fire(access, source, args...);
				}
			}
			catch(...) {
				//unlock everything if something goes bad
				
				//just in case
				access.unlock();
				
				fire.clear();
				
				throw;
			}
			
			fire.clear();
		}
		
		/// value for an empty token
		static const Token EmptyToken = 0;
		
	private:

		std::mutex access;
		std::atomic_flag fire;
		Source_ *source;
		Containers::Collection<internal::event::HandlerBase<Source_, Params_...>> handlers;
		typename Containers::Collection<internal::event::HandlerBase<Source_, Params_...>>::Iterator iterator;
	};
	
	/// Swaps two events
	template<class Source_, class... Args_>
	void swap(Event<Source_, Args_...> &l, Event<Source_, Args_...> &r) {
		l.Swap(r);
	}
}
