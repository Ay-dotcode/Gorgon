#pragma once

#include "collection.h"
#include <string>

namespace gge {
#ifndef ANY_EXISTS
	typedef void* Any;
#endif

	////An empty event parameter structure
	struct empty_event_params {
	};

	
	class Empty {
	};

	
	
	
	template<class O_, class P_> class EventChain;

	
	////Base of an event handler
	template <class P_, class O_>
	struct EventHandler {
		Any data;

		EventHandler(Any data) : data(data) {}

		virtual void Fire(P_ params, O_ &caller, std::string eventname)=0;
	};

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
	struct EventHandlerFunction : EventHandler<P_, O_> {
		void(*handler)(P_, O_ &, Any Data, std::string);

		EventHandlerFunction(void(*handler)(P_, O_ &, Any Data, std::string), Any data) : EventHandler(data), handler(handler) {}

		virtual void Fire(P_ params, O_ &caller, std::string eventname) {
			(*handler)(params, caller, data, eventname);
		}

	};
	template <class P_, class O_>
	EventHandler<P_, O_> *CreateEventHandler( void(*function)(P_, O_ &, Any Data, std::string), Any data) {
		return new EventHandlerFunction<P_, O_>(function, data);
	}

	template <class P_, class O_>
	bool Compare(EventHandler<P_, O_> *obj, void(*function)(P_, O_ &, Any Data, std::string)) {
		if(!dynamic_cast<EventHandlerFunction<P_,O_>*>(obj))
			return false;

		return function==dynamic_cast<EventHandlerFunction<P_,O_>*>(obj)->handler;
	}
	////This is private event object that is used to
	/// store function event handlers
	template<class P_, class O_>
	struct EventHandlerFunctionPlain : EventHandler<P_, O_> {
		void(*handler)(P_, O_ &);

		EventHandlerFunctionPlain(void(*handler)(P_, O_ &)) : EventHandler(NULL), handler(handler) {}

		virtual void Fire(P_ params, O_ &caller, std::string eventname) {
			(*handler)(params, caller);
		}

		bool Compare(void(*function)(P_, O_ &), void*) {
			if(handler==function)
				return true;
		}
	};

	template <class P_, class O_>
	EventHandler<P_, O_> *CreateEventHandler( void(*function)(P_, O_ &), Any data) {
		return new EventHandlerFunctionPlain<P_, O_>(function);
	}

	template <class P_, class O_>
	bool Compare(EventHandler<P_, O_> *obj, void(*function)(P_, O_ &)) {
		if(!dynamic_cast<EventHandlerFunctionPlain<P_,O_>*>(obj))
			return false;

		return function==dynamic_cast<EventHandlerFunctionPlain<P_,O_>*>(obj)->handler;
	}
	////This is private event object that is used to
	/// store function event handlers
	template<class P_, class O_>
	struct EventHandlerFunctionEmpty : EventHandler<P_, O_> {
		void(*handler)();

		EventHandlerFunctionEmpty(void(*handler)()) : EventHandler(NULL), handler(handler) {}

		virtual void Fire(P_ params, O_ &caller, std::string eventname) {
			(*handler)();
		}

		bool Compare(void(*function)(), void*) {
			if(handler==function)
				return true;
		}
	};


	template <class P_, class O_>
	EventHandler<P_, O_> *CreateEventHandler( void(*function)(), Any data) {
		return new EventHandlerFunctionEmpty<P_, O_>(function);
	}
	template <class P_, class O_>
	bool Compare(EventHandler<P_, O_> *obj, void(*function)()) {
		if(!dynamic_cast<EventHandlerFunctionEmpty<P_,O_>*>(obj))
			return false;

		return function==dynamic_cast<EventHandlerFunctionEmpty<P_,O_>*>(obj)->handler;
	}
	////This is private event object that is used to
	/// store function event handlers
	template<class P_, class O_>
	struct EventHandlerFunctionObjectOnly : EventHandler<P_, O_> {
		void(*handler)(O_ &);

		EventHandlerFunctionObjectOnly(void(*handler)(O_ &)) : EventHandler(NULL), handler(handler) {}

		virtual void Fire(P_ params, O_ &caller, std::string eventname) {
			(*handler)(caller);
		}

		bool Compare(void(*function)(O_ &), void*) {
			if(handler==function)
				return true;
		}
	};


	template <class P_, class O_>
	EventHandler<P_, O_> *CreateEventHandler( void(*function)(O_ &), Any data) {
		return new EventHandlerFunctionObjectOnly<P_, O_>(function);
	}
	template <class P_, class O_>
	bool Compare(EventHandler<P_, O_> *obj, void(*function)(O_ &)) {
		if(!dynamic_cast<EventHandlerFunctionObjectOnly<P_,O_>*>(obj))
			return false;

		return function==dynamic_cast<EventHandlerFunctionObjectOnly<P_,O_>*>(obj)->handler;
	}
	////This is private event object that is used to
	/// store function event handlers
	template<class P_, class O_>
	struct EventHandlerFunctionParamOnly : EventHandler<P_, O_> {
		void(*handler)(P_);

		EventHandlerFunctionParamOnly(void(*handler)(P_)) : EventHandler(NULL), handler(handler) {}

		virtual void Fire(P_ params, O_ &caller, std::string eventname) {
			(*handler)(params);
		}

		bool Compare(void(*function)(P_), void*) {
			if(handler==function)
				return true;
		}
	};
	template <class P_, class O_>
	EventHandler<P_, O_> *CreateEventHandler( void(*function)(P_), Any data) {
		return new EventHandlerFunctionParamOnly<P_, O_>(function);
	}

	template <class P_, class O_>
	bool Compare(EventHandler<P_, O_> *obj, void(*function)(P_)) {
		if(!dynamic_cast<EventHandlerFunctionParamOnly<P_,O_>*>(obj))
			return false;

		return function==dynamic_cast<EventHandlerFunctionParamOnly<P_,O_>*>(obj)->handler;
	}
	////This is private event object that is used to
	/// store function event handlers
	template<class P_, class O_>
	struct EventHandlerFunctionDataOnly : EventHandler<P_, O_> {
		void(*handler)(Any);

		EventHandlerFunctionDataOnly(void(*handler)(Any), Any data) : EventHandler(data), handler(handler) {}

		virtual void Fire(P_ params, O_ &caller, std::string eventname) {
			(*handler)(data);
		}

	};
	template <class P_, class O_>
	EventHandler<P_, O_> *CreateEventHandler( void(*function)(Any Data), Any data) {
		return new EventHandlerFunctionDataOnly<P_, O_>(function, data);
	}

	template <class P_, class O_>
	bool Compare(EventHandler<P_, O_> *obj, void(*function)(Any Data)) {
		if(!dynamic_cast<EventHandlerFunctionDataOnly<P_,O_>*>(obj))
			return false;

		return function==dynamic_cast<EventHandlerFunctionDataOnly<P_,O_>*>(obj)->handler;
	}
	
	
	////This is private event object that is used to
	/// store function event handlers
	template<class R_, class P_, class O_>
	struct EventHandlerClass : EventHandler<P_,O_> {
		void(__thiscall R_::*handler)(P_, O_ &, Any, std::string);

		R_ *object;

		EventHandlerClass(R_ *object, void(__thiscall R_::*handler)(P_, O_ &, Any, std::string), Any data) : EventHandler(data), object(object), handler(handler) {}

		virtual void Fire(P_ params, O_ &caller, std::string eventname) {
			(object->*handler)(params, caller, data, eventname);
		}

	};

	template <class R_, class P_, class O_>
	EventHandler<P_, O_> *CreateEventHandler(R_ *object, void(__thiscall R_::*function)(P_, O_ &, Any, std::string), Any data) {
		return new EventHandlerClass<R_, P_, O_>(object, function, data);
	}
	template <class R_, class P_, class O_>
	bool Compare(EventHandler<P_,O_> *obj, R_ *object, void(__thiscall R_::*function)(P_, O_ &, Any, std::string)) {
		if(!dynamic_cast<EventHandlerClass<R_, P_,O_>*>(obj))
			return false;

		return object==dynamic_cast<EventHandlerClass<R_, P_,O_>*>(obj)->object && function==dynamic_cast<EventHandlerClass<R_, P_,O_>*>(obj)->handler;
	}
	////This is private event object that is used to
	/// store function event handlers
	template<class R_, class P_, class O_>
	struct EventHandlerClassPlain : EventHandler<P_,O_> {
		void(__thiscall R_::*handler)(P_, O_ &);

		R_ *object;

		EventHandlerClassPlain(R_ *object, void(__thiscall R_::*handler)(P_, O_ &)) : EventHandler(NULL), object(object), handler(handler) {}

		virtual void Fire(P_ params, O_ &caller, std::string eventname) {
			(object->*handler)(params, caller);
		}

		bool Compare(void(__thiscall R_::*function)(P_, O_ &), void* object) {
			if((void*)handler==function && this->object==object)
				return true;
		}
	};



	template <class R_, class P_, class O_>
	EventHandler<P_, O_> *CreateEventHandler(R_ *object, void(__thiscall R_::*function)(P_, O_ &), Any data) {
		return new EventHandlerClassPlain<R_, P_, O_>(object, function);
	}
	template <class R_, class P_, class O_>
	bool Compare(EventHandler<P_,O_> *obj, R_ *object, void(__thiscall R_::*function)(P_, O_ &)) {
		if(!dynamic_cast<EventHandlerClassPlain<R_, P_,O_>*>(obj))
			return false;

		return object==dynamic_cast<EventHandlerClassPlain<R_, P_,O_>*>(obj)->object && function==dynamic_cast<EventHandlerClassPlain<R_, P_,O_>*>(obj)->handler;
	}
	////This is private event object that is used to
	/// store function event handlers
	template<class R_, class P_, class O_>
	struct EventHandlerClassEmpty : EventHandler<P_,O_> {
		void(__thiscall R_::*handler)();

		R_ *object;

		EventHandlerClassEmpty(R_ *object, void(__thiscall R_::*handler)()) : EventHandler(NULL), object(object), handler(handler) {}

		virtual void Fire(P_ params, O_ &caller, std::string eventname) {
			(object->*handler)();
		}

		bool Compare(void(__thiscall R_::*function)(), void* object) {
			if((void*)handler==function && this->object==object)
				return true;
		}
	};

	template <class R_, class P_, class O_>
	EventHandler<P_, O_> *CreateEventHandler(R_ *object, void(__thiscall R_::*function)(), Any data) {
		return new EventHandlerClassEmpty<R_, P_, O_>(object, function);
	}
	template <class R_, class P_, class O_>
	bool Compare(EventHandler<P_,O_> *obj, R_ *object, void(__thiscall R_::*function)()) {
		if(!dynamic_cast<EventHandlerClassEmpty<R_, P_,O_>*>(obj))
			return false;

		return object==dynamic_cast<EventHandlerClassEmpty<R_, P_,O_>*>(obj)->object && function==dynamic_cast<EventHandlerClassEmpty<R_, P_,O_>*>(obj)->handler;
	}
	////This is private event object that is used to
	/// store function event handlers
	template<class R_, class P_, class O_>
	struct EventHandlerClassObjectOnly : EventHandler<P_,O_> {
		void(__thiscall R_::*handler)(O_ &);

		R_ *object;

		EventHandlerClassObjectOnly(R_ *object, void(__thiscall R_::*handler)(O_ &)) : EventHandler(NULL), object(object), handler(handler) {}

		virtual void Fire(P_ params, O_ &caller, std::string eventname) {
			(object->*handler)(caller);
		}

		bool Compare(void(__thiscall R_::*function)(O_ &), void* object) {
			if((void*)handler==function && this->object==object)
				return true;
		}
	};

	template <class R_, class P_, class O_>
	EventHandler<P_, O_> *CreateEventHandler(R_ *object, void(__thiscall R_::*function)(O_ &), Any data) {
		return new EventHandlerClassObjectOnly<R_, P_, O_>(object, function);
	}

	template <class R_, class P_, class O_>
	bool Compare(EventHandler<P_,O_> *obj, R_ *object, void(__thiscall R_::*function)(O_ &)) {
		if(!dynamic_cast<EventHandlerClassObjectOnly<R_, P_,O_>*>(obj))
			return false;

		return object==dynamic_cast<EventHandlerClassObjectOnly<R_, P_,O_>*>(obj)->object && function==dynamic_cast<EventHandlerClassObjectOnly<R_, P_,O_>*>(obj)->handler;
	}
	////This is private event object that is used to
	/// store function event handlers
	template<class R_, class P_, class O_>
	struct EventHandlerClassParamOnly : EventHandler<P_,O_> {
		void(__thiscall R_::*handler)(P_);

		R_ *object;

		EventHandlerClassParamOnly(R_ *object, void(__thiscall R_::*handler)(P_ )) : EventHandler(NULL), object(object), handler(handler) {}

		virtual void Fire(P_ params, O_ &caller, std::string eventname) {
			(object->*handler)(caller);
		}

		bool Compare(void(__thiscall R_::*function)(P_), void* object) {
			if((void*)handler==function && this->object==object)
				return true;
		}
	};


	template <class R_, class P_, class O_>
	EventHandler<P_, O_> *CreateEventHandler(R_ *object, void(__thiscall R_::*function)(P_), Any data) {
		return new EventHandlerClassParamOnly<R_, P_, O_>(object, function);
	}
	template <class R_, class P_, class O_>
	bool Compare(EventHandler<P_,O_> *obj, R_ *object, void(__thiscall R_::*function)(P_)) {
		if(!dynamic_cast<EventHandlerClassParamOnly<R_, P_,O_>*>(obj))
			return false;

		return object==dynamic_cast<EventHandlerClassParamOnly<R_, P_,O_>*>(obj)->object && function==dynamic_cast<EventHandlerClassParamOnly<R_, P_,O_>*>(obj)->handler;
	}
	////This is private event object that is used to
	/// store function event handlers
	template<class R_, class P_, class O_>
	struct EventHandlerClassDataOnly : EventHandler<P_,O_> {
		void(__thiscall R_::*handler)(Any);

		R_ *object;

		EventHandlerClassDataOnly(R_ *object, void(__thiscall R_::*handler)(Any), Any Data) : EventHandler(Data), object(object), handler(handler) {}

		virtual void Fire(P_ params, O_ &caller, std::string eventname) {
			(object->*handler)(data);
		}

		bool Compare(void(__thiscall R_::*function)(Any), void* object) {
			if((void*)handler==function && this->object==object)
				return true;
		}
	};


	template <class R_, class P_, class O_>
	EventHandler<P_, O_> *CreateEventHandler(R_ *object, void(__thiscall R_::*function)(Any), Any data) {
		return new EventHandlerClassDataOnly<R_, P_, O_>(object, function, data);
	}
	template <class R_, class P_, class O_>
	bool Compare(EventHandler<P_,O_> *obj, R_ *object, void(__thiscall R_::*function)(Any)) {
		if(!dynamic_cast<EventHandlerClassDataOnly<R_, P_,O_>*>(obj))
			return false;

		return object==dynamic_cast<EventHandlerClassDataOnly<R_, P_,O_>*>(obj)->object && function==dynamic_cast<EventHandlerClassDataOnly<R_, P_,O_>*>(obj)->handler;
	}
	
	
	////EventChain class is used to create event objects
	/// that can be multi-handled. It is a template
	/// so an event definer can define parameters
	/// that can be used. Every event has an object
	/// that is the source of the event and a name
	/// for the event. This information is given
	/// in constructor.
	template<class O_=Empty, class P_=empty_event_params> class
		EventChain {
			template<class O_, class P_> friend class EventChain;
	public:

		typedef EventHandler<P_, O_> * Token;


		////Constructor
		///@Name	: Name of the event
		///@Object	: Source of the event
		EventChain(std::string Name,O_ *Object=NULL) : 
		  eventname(Name), object(Object), refcount(new int(1)), events(new Collection<EventHandler<P_,O_>>)
		{ }

		////Copy constructor, not safe to use, is not tested properly
		///@event   : Event object to be copied from
		template<class N_>
		EventChain(EventChain<N_, P_> &event) :
		   eventname(event.eventname), object(event.object), refcount(event.refcount)
		{
#ifdef _MSC_VER
#pragma message ( "Warning: Copy constructor is not safe to use" )
#else
#warning "Warning: Copy constructor is not safe to use"
#endif
			events=dynamic_cast<Collection<EventHandler<P_,O_>>*>(event.events);
			
			(*refcount)++;
		}

		////Sets two event chains to work together, can be unsafe
		template<class N_>
		EventChain<O_, P_> &operator =(EventChain<N_, P_> &event) {
			events->ResetIteration(true);
			EventHandler<P_, O_> *obj;

			while(obj=events->previous()) {
				event.events->Add((EventHandler<P_, N_>*)obj);
			}

			Destroy();
			eventname=event.eventname;
			object=event.object;
			refcount=event.refcount;

			//this is safe since no template arguments are used as regular variables
			events=(Collection<EventHandler<P_,O_>>*)(event.events);
			
			(*refcount)++;

			return *this;
		}

		////Registers an event handler. Every event handler
		/// can specify data to be passed to handler
		/// that can be used to identify who is registered
		/// the event. This function returns event token
		/// that can be used to remove this handler. The
		/// token is not an id or sequential number
		/// therefore, it should not be altered to find other
		/// handlers. Handler function template is 
		/// void Handler(Parameters params, CallerObject* object, any data, std::string eventname)
		///@handler	: handler function
		///@data	: data to be passed to handler
		template<class F_>
		Token Register(F_ handler) {
			return AddHandler(
				CreateEventHandler<P_, O_>(handler, NULL)
			);
		}

		////Registers an event handler. Every event handler
		/// can specify data to be passed to handler
		/// that can be used to identify who is registered
		/// the event. This function returns event token
		/// that can be used to remove this handler. The
		/// token is not an id or sequential number
		/// therefore, it should not be altered to find other
		/// handlers. Handler function template is 
		/// void Handler(Parameters params, CallerObject* object, any data, std::string eventname)
		///@handler	: handler function
		///@data	: data to be passed to handler
		template<class F_>
		Token Register(F_ *handler, Any data) {
			return AddHandler(
				CreateEventHandler<P_, O_>(handler, data)
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
		///@data	: data to be passed to handler
		template<class R_, class F_>
		Token Register(R_ *receiver, F_ handler, Any data=NULL) {
			return AddHandler(
				CreateEventHandler<R_, P_, O_>(receiver, handler, data)
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
		///@data	: data to be passed to handler
		template<class R_, class F_>
		Token Register(R_ &receiver, F_ handler, Any data=NULL) {
			return Register(&receiver, handler, data);
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
		///@data	: data to be passed to handler
		template<class R_, class F_>
		Token RegisterClass(R_ *receiver, F_ handler, Any data=NULL) {
			return AddHandler(
				CreateEventHandler<R_, P_, O_>(receiver, handler, data)
			);
		}
		
		////Unregisters the given event handler using handler function
		template<class F_>
		void Unregister(F_ handler) {
			events->ResetIteration(true);
			EventHandler<P_, O_> *object;

			while(object=events->previous()) {
				if(Compare(object, handler)) {
					RemoveHandler(object);
					return;
				}
			}
		}

		////Unregisters the given handler referenced by the object and function
		template<class R_, class F_>
		void Unregister(R_ *obj, F_ handler) {
			events->ResetIteration(true);
			EventHandler<P_, O_> *object;

			while(object=events->previous()) {
				if(Compare(object, obj, handler)) {
					RemoveHandler(object);
					return;
				}
			}
		}


		////Unregisters the given handler referenced by the object and function
		template<class R_, class F_>
		void UnregisterClass(R_ &obj, F_ handler) {
			Unregister(&obj, handler);
		}

		////Unregisters the given handler referenced by the object and function
		template<class R_, class F_>
		void UnregisterClass(R_ *obj, F_ handler) {
			events->ResetIteration(true);
			EventHandler<P_, O_> *object;

			while(object=events->previous()) {
				if(Compare(object, obj, handler)) {
					RemoveHandler(object);
					return;
				}
			}
		}


		////Unregisters the given handler referenced by the object and function
		template<class R_, class F_>
		void Unregister(R_ &obj, F_ handler) {
			Unregister(&obj, handler);
		}

		////This function triggers the event causing all 
		/// handlers to be called
		void operator()(P_ params) {
			Fire(params);
		}

		////This function triggers the event causing all 
		/// handlers to be called
		void operator()() {
			Fire();
		}

		////Unregisters a given handler token
		void Unregister(int token) {
			RemoveHandler(reinterpret_cast<EventHandler<P_,O_>*>(token));
		}

		//***************** More Unregister functions ******************

		////This function triggers the event causing all 
		/// handlers to be called
		void Fire(P_ params) {
			events->ResetIteration(true);
			EventHandler<P_, O_> *object;

			while(object=events->previous()) {
				object->Fire(params, *this->object, eventname);
			}
		}

		////This function triggers the event causing all 
		/// handlers to be called
		void Fire() {
			Fire(P_());
		}

		const std::string &GetName() const { return eventname; }

		~EventChain() {
			Destroy();
		}

	protected:
		////Name of the event
		std::string eventname;
		////Source of the events
		O_ *object;
		////Collection of event handlers
		Collection<EventHandler<P_,O_>> *events;
		int *refcount;

		////Unregisters a given handler token
		void RemoveHandler(EventHandler<P_, O_> *object) {
			events->Remove(object);
		}
		Token AddHandler(EventHandler<P_, O_> *object) {

			events->Add(object);

			return (Token)(object);

		}

		void Destroy() {
			(*refcount)--;
			if(!*refcount) {
				events->Destroy();
				delete refcount;
			}
		}
	};
}