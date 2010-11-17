#pragma once

#include "collection.h"
#include <string>

using namespace std;

namespace gge {
#ifndef ANY_EXISTS
	typedef void* Any;
#endif

	////An empty event parameter structure
	struct empty_event_params {
	};

	////Event handler types, used to determine how to dispatch
	/// an event
	enum EventHandlerTypes { Function, Class };

	template<class O_, class P_> class EventChain;

	////Base of an event handler
	template <class P_, class O_>
	struct EventHandler {
		Any data;

		EventHandler(Any data) : data(data) {}

		virtual void Fire(P_ params, O_ &caller, string eventname)=0;
	};

	////This is private event object that is used to
	/// store function event handlers
	template<class P_, class O_>
	struct EventHandlerFunction : EventHandler<P_, O_> {
		void(*handler)(P_, O_ &, Any Data, string);

		EventHandlerFunction(void(*handler)(P_, O_ &, Any Data, string), Any data) : EventHandler(data), handler(handler) {}

		virtual void Fire(P_ params, O_ &caller, string eventname) {
			(*handler)(params, caller, data, eventname);
		}
	};

	////This is private event object that is used to
	/// store function event handlers
	template<class R_, class P_, class O_>
	struct EventHandlerClass : EventHandler<P_,O_> {
		void(__thiscall R_::*handler)(P_, O_ &, Any, string);

		R_ *object;

		EventHandlerClass(R_ *object, void(__thiscall R_::*handler)(P_, O_ &, Any, string), Any data) : EventHandler(data), object(object), handler(handler) {}

		virtual void Fire(P_ params, O_ &caller, string eventname) {
			(object->*handler)(params, caller, data, eventname);
		}
	};

	class Empty {
	};

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
		////Constructor
		///@Name	: Name of the event
		///@Object	: Source of the event
		EventChain(string Name,O_ *Object) : 
		  eventname(Name), object(Object), refcount(new int(1)), events(new Collection<EventHandler<P_,O_>>)
		{
			cout<<"Event Chain Created \n";
		}

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
			events=(Collection<EventHandler<P_,O_>>*)(event.events);
			
			(*refcount)++;
		}

		////Sets two event chains to work together
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
		/// void Handler(Parameters params, CallerObject* object, any data, string eventname)
		///@handler	: handler function
		///@data	: data to be passed to handler
		int Register(void (*handler)(P_, O_ &, Any, string), Any data=NULL) {
			EventHandlerFunction<P_, O_> *object=new EventHandlerFunction<P_, O_>(handler,data);

			events->Add(object);
#pragma warning(push)
#pragma warning(disable:4311)
			return (int)object;
#pragma warning(pop)
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
		/// handlers. Handler function template is 
		/// void Handler(Parameters params, CallerObject* object, any data, string eventname)
		/// EventParams parameters)
		///@receiver: handler object
		///@handler	: handler function
		///@data	: data to be passed to handler
		template<class R_>
		int Register(R_ *receiver, void (__thiscall R_::*handler)(P_, O_ &, Any, string), Any data=NULL) {
			EventHandlerClass<R_, P_, O_> *object=new EventHandlerClass<R_, P_, O_>(receiver, handler,data);

			events->Add(object);
#pragma warning(push)
#pragma warning(disable: 4311)
			return (int)object;
#pragma warning(pop)
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
		/// handlers. Handler function template is 
		/// void Handler(Parameters params, CallerObject* object, any data, string eventname)
		/// EventParams parameters)
		///@receiver: handler object
		///@handler	: handler function
		///@data	: data to be passed to handler
		template<class R_>
		int RegisterClass(R_ *receiver, void (__thiscall R_::*handler)(P_, O_ &, Any, string), Any data=NULL) {

			return Register(receiver, handler, data);
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
			events->Remove((EventHandler<P_,O_>*)token);
		}

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

		const string &GetName() const { return eventname; }

		~EventChain() {
			Destroy();
		}

	protected:
		////Name of the event
		string eventname;
		////Source of the events
		O_ *object;
		////Collection of event handlers
		Collection<EventHandler<P_,O_>> *events;
		int *refcount;

		void Destroy() {
			(*refcount)--;
			if(!*refcount) {
				delete events;
				delete refcount;
				cout<<"Event Chain Destroyed\n";
			}
		}
	};
}