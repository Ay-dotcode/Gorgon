//DESCRIPTION
//	This file allows exposing event objects. Supports multiple events
//	to be registered, unregister and different function signatures
//	Event handlers are ordered and this order can be changed moreover
//	handlers can stop chaining of the event. This system is suitable
//	for events like mouse and keyboard events
//	If C++0x feature lambda functionality is causing problems, define
//	NOLAMBDA 

//REQUIRES:
//	gge::utils::SortedCollection
//	gge::utils::Any
//	std::unordered_map

//LICENSE
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the Lesser GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//	Lesser GNU General Public License for more details.
//
//	You should have received a copy of the Lesser GNU General Public License
//	along with this program. If not, see < http://www.gnu.org/licenses/ >.

//COPYRIGHT
//	Cem Kalyoncu, DarkGaze.Org (cemkalyoncu[at]gmail[dot]com)

#pragma once


#define CONSUMABLEEVENT_EXIST



#include <string>

#ifndef NOLAMBDA
#	include <functional>
#endif


#include "UtilsBase.h"
#include "Any.h"
#include "SortedCollection.h"
#include "EventChain.h"
#include <unordered_map>



namespace gge { namespace utils {




	template<class O_, class P_> class ConsumableEvent;


	namespace prvt { namespace consumableevent {

		////Base of an event handler
		template <class P_, class O_>
		struct EventHandler {
			Any data;

			bool enabled;

			EventHandler(Any data) : data(data), enabled(true) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname)=0;
		};

		//These template functions provide pseudo inheritance for these functions
		//every handler type must implement their of specialization
		template <class F_,class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler(F_ function, Any data) {
			return NULL;
		}
		template <class F_, class R_, class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler(R_ *object, F_ function, Any data) {
			return NULL;
		}

		template <class F_,class P_, class O_>
		bool Compare(EventHandler<P_, O_> *obj, F_ function) {
			return false;
		}
		template <class F_, class R_, class P_, class O_>
		bool Compare(EventHandler<P_,O_> *obj, R_ *object, F_ function) {
			return false;
		}
		template <class F_, class R_, class P_, class O_>
		bool Compare(EventHandler<P_,O_> *obj, R_ &object, F_ function) {
			return Compare(obj, &object, function);
		}



		////This is private event object that is used to
		/// store function event handlers
		template<class P_, class O_>
		struct EventHandlerFunction : public EventHandler<P_, O_> {
			bool(*handler)(P_, O_ &, Any Data, std::string);

			EventHandlerFunction(bool(*handler)(P_, O_ &, Any Data, std::string), Any data) : EventHandler<P_, O_>(data), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (*handler)(params, caller, EventHandler<P_, O_>::data, eventname);
			}

		};
		template <class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler( bool(*function)(P_, O_ &, Any Data, std::string), Any data) {
			return new EventHandlerFunction<P_, O_>(function, data);
		}

		template <class P_, class O_>
		bool Compare(EventHandler<P_, O_> *obj, bool(*function)(P_, O_ &, Any Data, std::string)) {
			if(!dynamic_cast<EventHandlerFunction<P_,O_>*>(obj))
				return false;

			return function==dynamic_cast<EventHandlerFunction<P_,O_>*>(obj)->handler;
		}
		////This is private event object that is used to
		/// store function event handlers
		template<class P_, class O_>
		struct EventHandlerFunctionPlain : public EventHandler<P_, O_> {
			bool(*handler)(P_, O_ &);

			EventHandlerFunctionPlain(bool(*handler)(P_, O_ &)) : EventHandler<P_, O_>(NULL), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (*handler)(params, caller);
			}

			bool Compare(bool(*function)(P_, O_ &), void*) {
				if(handler==function)
					return true;
			}
		};

		template <class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler( bool(*function)(P_, O_ &), Any data) {
			return new EventHandlerFunctionPlain<P_, O_>(function);
		}

		template <class P_, class O_>
		bool Compare(EventHandler<P_, O_> *obj, bool(*function)(P_, O_ &)) {
			if(!dynamic_cast<EventHandlerFunctionPlain<P_,O_>*>(obj))
				return false;

			return function==dynamic_cast<EventHandlerFunctionPlain<P_,O_>*>(obj)->handler;
		}

		////This is private event object that is used to
		/// store function event handlers
		template<class P_, class O_>
		struct EventHandlerFunctionEmpty : public EventHandler<P_, O_> {
			bool(*handler)();

			EventHandlerFunctionEmpty(bool(*handler)()) : EventHandler<P_, O_>(Any()), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (*handler)();
			}

			bool Compare(bool(*function)(), void*) {
				if(handler==function)
					return true;
			}
		};


		template <class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler( bool(*function)(), Any data) {
			return new EventHandlerFunctionEmpty<P_, O_>(function);
		}
		template <class P_, class O_>
		bool Compare(EventHandler<P_, O_> *obj, bool(*function)()) {
			if(!dynamic_cast<EventHandlerFunctionEmpty<P_,O_>*>(obj))
				return false;

			return function==dynamic_cast<EventHandlerFunctionEmpty<P_,O_>*>(obj)->handler;
		}
		////This is private event object that is used to
		/// store function event handlers
		template<class P_, class O_>
		struct EventHandlerFunctionObjectOnly : public EventHandler<P_, O_> {
			bool(*handler)(O_ &);

			EventHandlerFunctionObjectOnly(bool(*handler)(O_ &)) : EventHandler<P_, O_>(Any()), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (*handler)(caller);
			}

			bool Compare(bool(*function)(O_ &), void*) {
				if(handler==function)
					return true;
			}
		};


		template <class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler( bool(*function)(O_ &), Any data) {
			return new EventHandlerFunctionObjectOnly<P_, O_>(function);
		}
		template <class P_, class O_>
		bool Compare(EventHandler<P_, O_> *obj, bool(*function)(O_ &)) {
			if(!dynamic_cast<EventHandlerFunctionObjectOnly<P_,O_>*>(obj))
				return false;

			return function==dynamic_cast<EventHandlerFunctionObjectOnly<P_,O_>*>(obj)->handler;
		}
		////This is private event object that is used to
		/// store function event handlers
		template<class P_, class O_>
		struct EventHandlerFunctionParamOnly : public EventHandler<P_, O_> {
			bool(*handler)(P_);

			EventHandlerFunctionParamOnly(bool(*handler)(P_)) : EventHandler<P_, O_>(Any()), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (*handler)(params);
			}

			bool Compare(bool(*function)(P_), void*) {
				if(handler==function)
					return true;
			}
		};
		template <class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler( bool(*function)(P_), Any data) {
			return new EventHandlerFunctionParamOnly<P_, O_>(function);
		}

		template <class P_, class O_>
		bool Compare(EventHandler<P_, O_> *obj, bool(*function)(P_)) {
			if(!dynamic_cast<EventHandlerFunctionParamOnly<P_,O_>*>(obj))
				return false;

			return function==dynamic_cast<EventHandlerFunctionParamOnly<P_,O_>*>(obj)->handler;
		}
		////This is private event object that is used to
		/// store function event handlers
		template<class P_, class O_>
		struct EventHandlerFunctionDataOnly : public EventHandler<P_, O_> {
			bool(*handler)(Any);

			EventHandlerFunctionDataOnly(bool(*handler)(Any), Any data) : EventHandler<P_, O_>(data), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (*handler)(EventHandler<P_, O_>::data);
			}

		};
		template <class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler( bool(*function)(Any Data), Any data) {
			return new EventHandlerFunctionDataOnly<P_, O_>(function, data);
		}

		template <class P_, class O_>
		bool Compare(EventHandler<P_, O_> *obj, bool(*function)(Any Data)) {
			if(!dynamic_cast<EventHandlerFunctionDataOnly<P_,O_>*>(obj))
				return false;

			return function==dynamic_cast<EventHandlerFunctionDataOnly<P_,O_>*>(obj)->handler;
		}

		////This is private event object that is used to
		/// store function event handlers
		template<class P_, class O_>
		struct EventHandlerFunctionObjectAndData : EventHandler<P_, O_> {
			bool(*handler)(O_ &, Any);

			EventHandlerFunctionObjectAndData(bool(*handler)(O_ &, Any), Any data) : EventHandler<P_, O_>(data), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (*handler)(caller,EventHandler<P_, O_>::data);
			}

			bool Compare(bool(*function)(O_ &, Any), void*) {
				if(handler==function)
					return true;
			}
		};

		template <class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler( bool(*function)(O_ &, Any), Any data) {
			return new EventHandlerFunctionObjectAndData<P_, O_>(function,data);
		}
		template <class P_, class O_>
		bool Compare(EventHandler<P_, O_> *obj, bool(*function)(O_ &, Any)) {
			if(!dynamic_cast<EventHandlerFunctionObjectAndData<P_,O_>*>(obj))
				return false;

			return function==dynamic_cast<EventHandlerFunctionObjectAndData<P_,O_>*>(obj)->handler;
		}

		////This is private event object that is used to
		/// store function event handlers
		template<class R_, class P_, class O_>
		struct EventHandlerClass : public EventHandler<P_,O_> {
			bool(R_::*handler)(P_, O_ &, Any, std::string);

			R_ *object;

			EventHandlerClass(R_ *object, bool(R_::*handler)(P_, O_ &, Any, std::string), Any data) : EventHandler<P_, O_>(data), object(object), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (object->*handler)(params, caller, EventHandler<P_,O_>::data, eventname);
			}

		};

		template <class R_, class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler(R_ *object, bool(R_::*function)(P_, O_ &, Any, std::string), Any data) {
			return new EventHandlerClass<R_, P_, O_>(object, function, data);
		}
		template <class R_, class P_, class O_>
		bool Compare(EventHandler<P_,O_> *obj, R_ *object, bool(R_::*function)(P_, O_ &, Any, std::string)) {
			if(!dynamic_cast<EventHandlerClass<R_, P_,O_>*>(obj))
				return false;

			return object==dynamic_cast<EventHandlerClass<R_, P_,O_>*>(obj)->object && function==dynamic_cast<EventHandlerClass<R_, P_,O_>*>(obj)->handler;
		}
		////This is private event object that is used to
		/// store function event handlers
		template<class R_, class P_, class O_>
		struct EventHandlerClassPlain : public EventHandler<P_,O_> {
			bool(R_::*handler)(P_, O_ &);

			R_ *object;

			EventHandlerClassPlain(R_ *object, bool(R_::*handler)(P_, O_ &)) : EventHandler<P_, O_>(Any()), object(object), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (object->*handler)(params, caller);
			}

			bool Compare(bool(R_::*function)(P_, O_ &), void* object) {
				if((void*)handler==function && this->object==object)
					return true;
			}
		};



		template <class R_, class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler(R_ *object, bool(R_::*function)(P_, O_ &), Any data) {
			return new EventHandlerClassPlain<R_, P_, O_>(object, function);
		}
		template <class R_, class P_, class O_>
		bool Compare(EventHandler<P_,O_> *obj, R_ *object, bool(R_::*function)(P_, O_ &)) {
			if(!dynamic_cast<EventHandlerClassPlain<R_, P_,O_>*>(obj))
				return false;

			return object==dynamic_cast<EventHandlerClassPlain<R_, P_,O_>*>(obj)->object && function==dynamic_cast<EventHandlerClassPlain<R_, P_,O_>*>(obj)->handler;
		}
		////This is private event object that is used to
		/// store function event handlers
		template<class R_, class P_, class O_>
		struct EventHandlerClassEmpty : public EventHandler<P_,O_> {
			bool(R_::*handler)();

			R_ *object;

			EventHandlerClassEmpty(R_ *object, bool(R_::*handler)()) : EventHandler<P_, O_>(Any()), object(object), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (object->*handler)();
			}

			bool Compare(bool(R_::*function)(), void* object) {
				if((void*)handler==function && this->object==object)
					return true;
			}
		};

		template <class R_, class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler(R_ *object, bool(R_::*function)(), Any data) {
			return new EventHandlerClassEmpty<R_, P_, O_>(object, function);
		}
		template <class R_, class P_, class O_>
		bool Compare(EventHandler<P_,O_> *obj, R_ *object, bool(R_::*function)()) {
			if(!dynamic_cast<EventHandlerClassEmpty<R_, P_,O_>*>(obj))
				return false;

			return object==dynamic_cast<EventHandlerClassEmpty<R_, P_,O_>*>(obj)->object && function==dynamic_cast<EventHandlerClassEmpty<R_, P_,O_>*>(obj)->handler;
		}
		////This is private event object that is used to
		/// store function event handlers
		template<class R_, class P_, class O_>
		struct EventHandlerClassObjectOnly : public EventHandler<P_,O_> {
			bool(R_::*handler)(O_ &);

			R_ *object;

			EventHandlerClassObjectOnly(R_ *object, bool(R_::*handler)(O_ &)) : EventHandler<P_, O_>(Any()), object(object), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (object->*handler)(caller);
			}

			bool Compare(bool(R_::*function)(O_ &), void* object) {
				if((void*)handler==function && this->object==object)
					return true;
			}
		};

		template <class R_, class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler(R_ *object, bool(R_::*function)(O_ &), Any data) {
			return new EventHandlerClassObjectOnly<R_, P_, O_>(object, function);
		}

		template <class R_, class P_, class O_>
		bool Compare(EventHandler<P_,O_> *obj, R_ *object, bool(R_::*function)(O_ &)) {
			if(!dynamic_cast<EventHandlerClassObjectOnly<R_, P_,O_>*>(obj))
				return false;

			return object==dynamic_cast<EventHandlerClassObjectOnly<R_, P_,O_>*>(obj)->object && function==dynamic_cast<EventHandlerClassObjectOnly<R_, P_,O_>*>(obj)->handler;
		}
		////This is private event object that is used to
		/// store function event handlers
		template<class R_, class P_, class O_>
		struct EventHandlerClassParamOnly : public EventHandler<P_,O_> {
			bool(R_::*handler)(P_);

			R_ *object;

			EventHandlerClassParamOnly(R_ *object, bool(R_::*handler)(P_ )) : EventHandler<P_, O_>(Any()), object(object), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (object->*handler)(params);
			}

			bool Compare(bool(R_::*function)(P_), void* object) {
				if((void*)handler==function && this->object==object)
					return true;
			}
		};


		template <class R_, class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler(R_ *object, bool(R_::*function)(P_), Any data) {
			return new EventHandlerClassParamOnly<R_, P_, O_>(object, function);
		}
		template <class R_, class P_, class O_>
		bool Compare(EventHandler<P_,O_> *obj, R_ *object, bool(R_::*function)(P_)) {
			if(!dynamic_cast<EventHandlerClassParamOnly<R_, P_,O_>*>(obj))
				return false;

			return object==dynamic_cast<EventHandlerClassParamOnly<R_, P_,O_>*>(obj)->object && function==dynamic_cast<EventHandlerClassParamOnly<R_, P_,O_>*>(obj)->handler;
		}
		////This is private event object that is used to
		/// store function event handlers
		template<class R_, class P_, class O_>
		struct EventHandlerClassDataOnly : public EventHandler<P_,O_> {
			bool(R_::*handler)(Any);

			R_ *object;

			EventHandlerClassDataOnly(R_ *object, bool(R_::*handler)(Any), Any data) : EventHandler<P_, O_>(data), object(object), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (object->*handler)(EventHandler<P_, O_>::data);
			}

			bool Compare(bool(R_::*function)(Any), void* object) {
				if((void*)handler==function && this->object==object)
					return true;
			}
		};


		template <class R_, class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler(R_ *object, bool(R_::*function)(Any), Any data) {
			return new EventHandlerClassDataOnly<R_, P_, O_>(object, function, data);
		}
		template <class R_, class P_, class O_>
		bool Compare(EventHandler<P_,O_> *obj, R_ *object, bool(R_::*function)(Any)) {
			if(!dynamic_cast<EventHandlerClassDataOnly<R_, P_,O_>*>(obj))
				return false;

			return object==dynamic_cast<EventHandlerClassDataOnly<R_, P_,O_>*>(obj)->object && function==dynamic_cast<EventHandlerClassDataOnly<R_, P_,O_>*>(obj)->handler;
		}


#ifndef NOLAMBDA
		////This is private event object that is used to
		/// store function event handlers
		template<class P_, class O_>
		struct EventHandlerLambdaEmpty : public EventHandler<P_, O_> {
			std::function<bool()> handler;

			EventHandlerLambdaEmpty(std::function<bool()> handler) : EventHandler<P_, O_>(Any()), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (handler)();
			}

			bool Compare(std::function<bool()> function, void*) {
				return false;
			}
		};


		template <class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler( std::function<bool()> function, Any data) {
			return new EventHandlerLambdaEmpty<P_, O_>(function);
		}
		template <class P_, class O_>
		bool Compare(EventHandler<P_, O_> *obj, std::function<bool()> function) {
			return false;
		}

		////This is private event object that is used to
		/// store function event handlers
		template<class P_, class O_>
		struct EventHandlerLambdaParamOnly : public EventHandler<P_, O_> {
			std::function<bool(P_)> handler;

			EventHandlerLambdaParamOnly(std::function<bool(P_)> handler) : EventHandler<P_, O_>(Any()), handler(handler) {}

			virtual bool Fire(P_ params, O_ &caller, std::string eventname) {
				return (handler)(params);
			}

			bool Compare(std::function<bool(P_)> function, void*) {
				return false;
			}
		};


		template <class P_, class O_>
		EventHandler<P_, O_> *CreateEventHandler( std::function<bool(P_)> function, Any data) {
			return new EventHandlerLambdaParamOnly<P_, O_>(function);
		}
		template <class P_, class O_>
		bool Compare(EventHandler<P_, O_> *obj, std::function<bool(P_)> function) {
			return false;
		}
#endif

	} }


	////ConsumableEvent class is used to create event objects
	/// that can be multi-handled. It is a template
	/// so an event definer can define parameters
	/// that can be used. Every event has an object
	/// that is the source of the event and a name
	/// for the event. This information is given
	/// in constructor.
	template<class O_=Empty, class P_=empty_event_params>
	class ConsumableEvent {
		template<class O2_, class P2_> friend class ConsumableEvent;
	public:

		typedef int Token;
		static const Token NullToken = 0;

		//To be used by owner
		std::unordered_map<P_, Token> TokenList;

		////Constructor
		///@Name	: Name of the event
		///@Object	: Source of the event
		ConsumableEvent(std::string Name,O_ *Object=NULL) : 
		eventname(Name), object(Object)
		{ }

		////Constructor
		///@Name	: Name of the event
		///@Object	: Source of the event
		ConsumableEvent(O_ *Object=NULL) : 
		eventname(""), object(Object)
		{ }

		////Constructor
		///@Name	: Name of the event
		///@Object	: Source of the event
		ConsumableEvent(std::string Name,O_ &Object) : 
		eventname(Name), object(&Object)
		{ }

		////Constructor
		///@Name	: Name of the event
		///@Object	: Source of the event
		ConsumableEvent(O_ &Object) : 
		eventname(""), object(&Object)
		{ }

		////Registers an event handler. Every event handler
		/// can specify EventHandler<P_, O_>::data to be passed to handler
		/// that can be used to identify who is registered
		/// the event. This function returns event token
		/// that can be used to remove this handler. The
		/// token is not an id or sequential number
		/// therefore, it should not be altered to find other
		/// handlers. Handler function template is 
		/// void Handler(Parameters params, CallerObject* object, any EventHandler<P_, O_>::data, std::string eventname)
		///@handler	: handler function
		///@data	: data to be passed to handler
		template<class F_>
		Token Register(F_ *handler, Any data=Any()) {
			return AddHandler(
				prvt::consumableevent::CreateEventHandler<P_, O_>(handler, data)
			);
		}

		////Registers an event handler. Every event handler
		/// can specify EventHandler<P_, O_>::data to be passed to handler
		/// that can be used to identify who is registered
		/// the event. This function returns event token
		/// that can be used to remove this handler. The
		/// token is not an id or sequential number
		/// therefore, it should not be altered to find other
		/// handlers. Handler function template is 
		/// void Handler(Parameters params, CallerObject* object, any EventHandler<P_, O_>::data, std::string eventname)
		///@handler	: handler function
		///@data	: data to be passed to handler
		template<class F_>
		Token Register(F_ *handler, int order, Any data=Any()) {
			return AddHandler(
				prvt::consumableevent::CreateEventHandler<P_, O_>(handler, data),
				order
			);
		}


		////Registers a class event handler. This handler
		/// should be a non static function of
		/// the given object. Event handler
		/// can specify EventHandler<P_, O_>::data to be passed to handler
		/// that can be used to identify who is registered
		/// the event. This function returns event token
		/// that can be used to remove this handler. The
		/// token is not an id or sequential number
		/// therefore, it should not be altered to find other
		/// handlers. Handler function full template is 
		/// void Handler(Parameters params, CallerObject* object, any EventHandler<P_, O_>::data, std::string eventname)
		/// EventParams parameters)
		///@receiver: handler object
		///@handler	: handler function
		///@data	: data to be passed to handler
		template<class R_, class F_>
		Token Register(R_ *receiver, F_ handler, Any data=Any()) {
			return AddHandler(
				prvt::consumableevent::CreateEventHandler<R_, P_, O_>(receiver, handler, data)
			);
		}
		template<class R_, class F_>
		Token Register(R_ *receiver, F_ handler, int order, Any data=Any()) {
			return AddHandler(
				prvt::consumableevent::CreateEventHandler<R_, P_, O_>(receiver, handler, data),
				order
			);
		}

		////Registers a class event handler. This handler
		/// should be a non static function of
		/// the given object. Event handler
		/// can specify data to be passed to handler
		/// that can be used to identify who is registered
		/// the event. This function returns event token
		/// that can be used to remove this handler. The
		/// token is not an id or sequential number
		/// therefore, it should not be altered to find other
		/// handlers. Handler function full template is 
		/// void Handler(Parameters params, CallerObject* object, any data, std::string eventname)
		/// EventParams parameters)
		///@receiver: handler object
		///@handler	: handler function
		///@data	: EventHandler<P_, O_>::data to be passed to handler
		template<class R_, class F_>
		Token Register(R_ &receiver, F_ handler, Any data=Any()) {
			return Register(&receiver, handler, data);
		}
		template<class R_, class F_>
		Token Register(R_ &receiver, F_ handler, int order, Any data=Any()) {
			return Register(&receiver, handler, order, data);
		}

		template<class R_, class F_>
		Token RegisterClass(R_ *receiver, F_ handler, Any data=Any()) {
			return AddHandler(
				prvt::consumableevent::CreateEventHandler<R_, P_, O_>(receiver, handler, data)
			);
		}

		template<class R_, class F_>
		Token RegisterClass(R_ *receiver, F_ handler, int order, Any data=Any()) {
			return AddHandler(
				prvt::consumableevent::CreateEventHandler<R_, P_, O_>(receiver, handler, data),
				order
			);
		}

		template<class R_, class F_>
		Token RegisterClass(R_ &receiver, F_ handler, Any data=Any()) {
			return RegisterClass(receiver, handler, data);
		}

		template<class R_, class F_>
		Token RegisterClass(R_ &receiver, F_ handler, int order, Any data=Any()) {
			return RegisterClass(receiver, handler, order, data);
		}

#ifndef NOLAMBDA
		template <class F_>
		typename count_arg<F_, 0, Token>::type RegisterLambda(F_ handler, int order) {
			return AddHandler(
				prvt::consumableevent::CreateEventHandler<P_, O_>(std::function<bool()>(handler), Any()),
				order
			);
		}
		template <class F_>
		typename count_arg<F_, 1, Token>::type RegisterLambda(F_ handler, int order) {
			return AddHandler(
				prvt::consumableevent::CreateEventHandler<P_, O_>(std::function<bool(P_)>(handler), Any()),
				order
			);
		}
		template <class F_>
		typename count_arg<F_, 0, Token>::type RegisterLambda(F_ handler) {
			return AddHandler(
				prvt::consumableevent::CreateEventHandler<P_, O_>(std::function<bool()>(handler), Any())
			);
		}
		template <class F_>
		typename count_arg<F_, 1, Token>::type RegisterLambda(F_ handler) {
			return AddHandler(
				prvt::consumableevent::CreateEventHandler<P_, O_>(std::function<bool(P_)>(handler), Any())
			);
		}
#endif

		template<class R_>
		Token LinkTo(ConsumableEvent<R_, P_> &target) {
			return AddHandler(
				prvt::consumableevent::CreateEventHandler<ConsumableEvent<R_, P_>, P_, O_>(
					&target, 
					(bool(ConsumableEvent<R_,P_>::*)(P_))&ConsumableEvent<R_, P_>::Fire,
					NULL
				)
			);
		}

		template<class R_>
		Token LinkTo(ConsumableEvent<R_, P_> &target, int order) {
			return AddHandler(
				prvt::consumableevent::CreateEventHandler<ConsumableEvent<R_, P_>, P_, O_>(
					&target, 
					(bool(ConsumableEvent<R_,P_>::*)(P_))&ConsumableEvent<R_, P_>::Fire,
					NULL
				),
				order
			);
		}

		template<class R_>
		Token DoubleLink(ConsumableEvent<R_, P_> &target) {
			typedef bool(ConsumableEvent<O_,P_>::*MyFire	)(P_) ;
			typedef bool(ConsumableEvent<R_,P_>::*TargetFire)(P_);

			target.RegisterClass< ConsumableEvent<O_, P_>, MyFire >(
				this, 
				(MyFire) &ConsumableEvent<R_, P_>::linkedfire,
				NULL
			);

			return AddHandler(
				prvt::consumableevent::CreateEventHandler<ConsumableEvent<R_, P_>, P_, O_>(
					&target, 
					(TargetFire) &ConsumableEvent<R_, P_>::linkedfire,
					NULL							  
				)
			);
		}

		template<class R_>
		
		Token DoubleLink(ConsumableEvent<R_, P_> &target, int order) {
			typedef bool(ConsumableEvent<O_,P_>::*MyFire	)(P_) ;
			typedef bool(ConsumableEvent<R_,P_>::*TargetFire)(P_);

			target.RegisterClass< ConsumableEvent<O_, P_>, MyFire >(
				this, 
				(MyFire) &ConsumableEvent<R_, P_>::linkedfire,
				order,
				Any()
			);

			return AddHandler(
				prvt::consumableevent::CreateEventHandler<ConsumableEvent<R_, P_>, P_, O_>(
					&target, 
					(TargetFire) &ConsumableEvent<R_, P_>::linkedfire,
					Any()
				), order
			);
		}

		////Unregisters the given event handler using handler function
		template<class F_>
		Token Find(F_ handler) {
			for(SortedCollection<HANDLER_,int>::Iterator it=events.First(); it.IsValid(); it.Next()) {
				if(Compare(&(*it), handler)) {

					return reinterpret_cast<Token> (object);
				}
			}
		}

		////Unregisters the given handler referenced by the object and function
		template<class R_, class F_>
		Token Find(R_ *obj, F_ handler) {
			for(SortedCollection<HANDLER_,int>::Iterator it=events.First(); it.IsValid(); it.Next()) {
				if(Compare(&(*it), obj, handler)) {

					return reinterpret_cast<Token> (object);
				}
			}
		}

		////Unregisters the given event handler using handler function
		template<class F_>
		void Unregister(F_ handler) {
			for(SortedCollection<HANDLER_,int>::Iterator it=events.First(); it.IsValid(); it.Next()) {
				if(Compare(&(*it), handler)) {
					Unregister(reinterpret_cast<Token>(&it.GetWrapper()));
					return;
				}
			}
		}

		////Unregisters the given handler referenced by the object and function
		template<class R_, class F_>
		void Unregister(R_ *obj, F_ handler) {
			for(SortedCollection<HANDLER_,int>::Iterator it=events.First(); it.IsValid(); it.Next()) {
				if(Compare(&(*it),obj, handler)) {
					Unregister(reinterpret_cast<Token>(&it.GetWrapper()));
					return;
				}
			}
		}

		template<class R_, class F_>
		void UnregisterClass(R_ &obj, F_ handler) {
			Unregister(&obj, handler);
		}

		////Unregisters the given handler referenced by the object and function
		template<class R_, class F_>
		void UnregisterClass(R_ *obj, F_ handler) {
			for(SortedCollection<HANDLER_,int>::Iterator it=events.First(); it.IsValid(); it.Next()) {
				if(Compare(&(*it), obj, handler)) {
					Unregister(reinterpret_cast<Token>(&it.GetWrapper()));
					return;
				}
			}
		}

		////Unregisters the given handler referenced by the object and function
		template<class R_, class F_>
		void Unregister(R_ &obj, F_ handler) {
			Unregister(&obj, handler);
		}

		////Unregisters a given handler token
		void Unregister(Token token) {
			for(std::unordered_map<P_, Token>::iterator i=TokenList.begin();i!=TokenList.end();++i) {
				if(i->second==token)
					i->second=NullToken;
			}

			reinterpret_cast< ITEMTYPE_* >(token)->Delete();
		}

		void MakeFirst(Token token) {
			ITEMTYPE_ *item=reinterpret_cast<ITEMTYPE_*>(token);

			item->setOrder(events.LowestOrder()-1);
		}

		void MakeLast(Token token) {
			ITEMTYPE_ *item=reinterpret_cast<ITEMTYPE_*>(token);

			item->setOrder(events.HighestOrder()-1);
		}

		int GetOrder(Token token) {
			ITEMTYPE_ *item=reinterpret_cast<ITEMTYPE_*>(token);
			
			return item->getOrder();
		}

		bool IsFirst(Token token) {
			ITEMTYPE_ *item=reinterpret_cast<ITEMTYPE_*>(token);

			return item==events.getOrderedFirst();
		}

		bool IsLast(Token token) {
			ITEMTYPE_ *item=reinterpret_cast<ITEMTYPE_*>(token);

			return item==events.getOrderedLast();
		}

		void SetOrder(Token token, int order) {
			ITEMTYPE_ *item=reinterpret_cast<ITEMTYPE_*>(token);

			item->Reorder(order);
		}

		void Enable(Token token) {
			ITEMTYPE_ *item=reinterpret_cast<ITEMTYPE_*>(token);

			item->Get().enabled=true;
		}

		void Disable(Token token) {
			ITEMTYPE_ *item=reinterpret_cast<ITEMTYPE_*>(token);

			item->Get().enabled=false;
		}

		bool IsEnabled(Token token) {
			ITEMTYPE_ *item=reinterpret_cast<ITEMTYPE_*>(token);

			return item->Get().enabled;
		}

		bool Fire(Token token, P_ params) {
			if(token==NullToken)
				return false;

			return reinterpret_cast< ITEMTYPE_* >(token)->Get().Fire(params, *this->object, eventname);
		}

		bool Fire(Token token) {
			Fire(token, P_());
		}

		////This function triggers the event causing all 
		/// handlers to be called
		Token operator()(P_ params) {
			return Fire(params);
		}

		////This function triggers the event causing all 
		/// handlers to be called
		Token operator()() {
			return Fire();
		}

		////This function triggers the event causing all 
		/// handlers to be called
		Token Fire(P_ params) {
			for(SortedCollection<HANDLER_, int>::Iterator it=events.First();it.IsValid();it.Next()) {
				if(it->enabled)
					if(it->Fire(params, *this->object, eventname))
						return reinterpret_cast<Token>(&it.GetWrapper());
			}

			return 0;
		}

		////This function triggers the event causing all 
		/// handlers to be called
		Token Fire() {
			return Fire(P_());
		}

		const std::string &GetName() const { return eventname; }

		void SetName(const std::string &name) {
			eventname=name;
		}

		~ConsumableEvent() {
		}

	protected:
		typedef typename SortedCollection<prvt::consumableevent::EventHandler<P_, O_>, int>::Wrapper ITEMTYPE_ ;
		typedef prvt::consumableevent::EventHandler<P_, O_> HANDLER_;
		////Name of the event
		std::string eventname;
		////Source of the events
		O_ *object;
		////Collection of event handlers
		SortedCollection<HANDLER_,int> events;

		////Unregisters a given handler token
		void RemoveHandler(ITEMTYPE_ *object) {
			object->Delete();
		}
		Token AddHandler(HANDLER_ *object) {
			ITEMTYPE_ *item = &events.Add(object);

			return reinterpret_cast<Token>(item);
		}
		Token AddHandler(HANDLER_ *object, int order) {
			ITEMTYPE_ *item = &events.Add(object, order);

			return reinterpret_cast<Token>(item);
		}


	private:
		//copy constructor is disabled
		ConsumableEvent(const ConsumableEvent<O_, P_> &);

		//assignment operator is disabled
		ConsumableEvent<O_,P_> operator =(const ConsumableEvent<O_, P_> &);

		bool checklinkedfire(HANDLER_ *object) {
			prvt::consumableevent::EventHandlerClassParamOnly<ConsumableEvent, P_, O_> *obj=dynamic_cast<prvt::consumableevent::EventHandlerClassParamOnly<ConsumableEvent, P_, O_> *>(object);

			if(obj!=NULL) {
				ConsumableEvent *ec;
				ec=dynamic_cast<ConsumableEvent*>(obj->object);
				if(ec!=NULL && obj->handler == &ConsumableEvent::linkedfire)
					return true;
			}

			return false;
		}

		bool linkedfire(P_ params) {
			foreach(HANDLER_ , object, events) {
				if(object->enabled && !checklinkedfire(object))
					if(object->Fire(params, *this->object, eventname))
						return true;
			}

			return false;
		}

	};
} }
