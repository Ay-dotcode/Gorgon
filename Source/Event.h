/// @file Event.h contains event distribution mechanism
#pragma once

#include <type_traits>
#include <vector>
#include <functional>

#include "Types.h"
#include "Container/Collection.h"


namespace Gorgon {
	
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
		struct HandlerBase {
			virtual void Fire(Source_ &, Params_...) = 0;
		};
		
		struct EmptyHandlerFn : public HandlerBase {
			EmptyHandlerFn(std::function<void()> fn) : fn(fn) {}
			
			virtual void Fire(Source_ &, Params_...) {
				fn();
			}
			
			std::function<void()> fn;
		};
		
	public:		
		
		Event() : source(nullptr) {
			static_assert( std::is_same<Source_, Empty>::value , "Empty constructor cannot be used." );
		}
		
		Event(Source_ &source) : source(&source) {
			static_assert( !std::is_same<Source_, Empty>::value, "Filling constructor is not required, use the default." );
		}
		
		Event(const Event &) = delete;
		
		Event &operator =(const Event &) = delete;
		
		//void Register
		
	private:
		Source_ *source;
		Container::Collection<HandlerBase> handlers;
	};
	
	
}