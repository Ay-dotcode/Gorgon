#pragma once

#include "../Utils/SortedCollection.h"
#include "../Utils/Collection.h"
#include "GGE.h"

#include "OS.h"
#include "../Utils/ConsumableEvent.h"
#include "../Utils/Bounds2D.h"
#include <functional>

namespace gge { namespace input {
	class BasicPointerTarget;


	namespace keyboard { 
		typedef int Key;

		class Modifier {
		public:

			enum Type {
				None		= 0,
				Shift		= 1,
				Ctrl		= 2,
				Alt			= 4,
				Super		= 8,

				ShiftCtrl	= Shift | Ctrl ,
				ShiftAlt	= Shift | Alt  ,
				CtrlAlt		= Ctrl  | Alt  ,
				ShiftCtrlAlt= Shift | Ctrl | Alt ,
			};

			static bool isAlternate;
			static Type Current;

			////Checks the the current state of the keyboard if there are any real modifiers
			/// in effect (Namely control, alt, and windows keys)
			static bool Check(Type m1) {
				return m1&Ctrl || m1&Alt || m1&Super;
			}

			static bool Check() {
				return Current&Ctrl || Current&Alt || Current&Super;
			}

			static Type Remove(Type m1, Type m2) {
				return Type(m1 & ~m2);
			}

			static Type Add(Type m1, Type m2) {
				return Type(m1 | m2);
			}

			static void Remove(Type m2) {
				Current=Type(Current & ~m2);
			}

			static void Add(Type m2) {
				Current=Type(Current | m2);
			}

			static bool IsModified() {
				return Check();
			}


		private:
			Modifier();
		};

		////Bitwise OR operation on KeyboardModifier enumeration
		inline Modifier::Type operator | (Modifier::Type m1, Modifier::Type m2) {
			return Modifier::Type( m1|m2 );
		}

		////Bitwise AND operation on KeyboardModifier enumeration
		inline Modifier::Type operator & (Modifier::Type m1, Modifier::Type m2) {
			return Modifier::Type( m1&m2 );
		}

		////Bitwise EQUAL OR operation on KeyboardModifier enumeration
		inline Modifier::Type operator |= (Modifier::Type m1, Modifier::Type m2) {
			return Modifier::Type( m1|m2 );
		}

		////Bitwise EQUAL AND operation on KeyboardModifier enumeration
		inline Modifier::Type operator &= (Modifier::Type m1, Modifier::Type m2) {
			return Modifier::Type( m1&m2 );
		}

		class KeyCodes {
		public:
			static const Key Shift;
			static const Key Control;
			static const Key Alt;
			static const Key Super;

			static const Key Home;
			static const Key End;
			static const Key Insert;
			static const Key Delete;
			static const Key PageUp;
			static const Key PageDown;

			static const Key Left;
			static const Key Up;
			static const Key Right;
			static const Key Down;

			static const Key PrintScreen;
			static const Key Pause;

			static const Key CapsLock;
			static const Key NumLock;

			static const Key Enter;
			static const Key Tab;
			static const Key Backspace;
			static const Key Space;
			static const Key Escape;

			static const Key F1;
			static const Key F2;
			static const Key F3;
			static const Key F4;
			static const Key F5;
			static const Key F6;
			static const Key F7;
			static const Key F8;
			static const Key F9;
			static const Key F10;
			static const Key F11;
			static const Key F12;
		};


		////Types of keyboard events
		class Event {
		public:
			enum Type {
				////A character is typed (O/S controlled, i.e. repeating keys)
				Char,
				////A key is pressed
				Down,
				////A key is released
				Up
			} event;

			Key keycode;

			Event() { }
			Event(Type event, int keycode) : event(event), keycode(keycode) { }

			bool operator ==(const Event &r) const {
				return (event==r.event)&&(keycode==r.keycode);
			}
		};
	}
} }

namespace std { //for consumable event's token list
	template<>
	class hash<gge::input::keyboard::Event>
		: public unary_function<gge::input::keyboard::Event, size_t>
	{	// hash F_
	public:
		typedef gge::input::keyboard::Event _Kty;

		size_t operator()(const _Kty& _Keyval) const {	
			return _Keyval.event<<16 ^ _Keyval.keycode;
		}
	};
}

namespace gge { namespace input {
	namespace keyboard {
		extern utils::ConsumableEvent<utils::Empty, Event> Events;
	}

	namespace mouse {

		class HandlerBase;
		class Event;

		class MouseTarget {

		};

		class Event {
		public:
			enum Type {
				None		= 0,

				Left		= 1<<0,
				Right		= 1<<1,
				Middle		= 1<<2,
				X1			= 1<<3,
				X2			= 1<<4,

				Click		= 1<<5,
				Down		= 1<<6,
				Up			= 1<<7,
				DoubleClick	= 1<<8,

				Over		= 1<<9,
				Out			= 1<<10,
				Move		= 1<<11,
				VScroll		= 1<<12,
				HScroll		= 1<<13,

				OverCheck	= 1<<14,


				Left_Click	= Left	| Click,
				Right_Click	= Right	| Click,
				Middle_Click= Middle| Click,
				X1_Click	= X1	| Click,
				X2_Click	= X2	| Click,

				Left_Down	= Left	| Down,
				Right_Down	= Right	| Down,
				Middle_Down	= Middle| Down,
				X1_Down		= X1	| Down,
				X2_Down		= X2	| Down,

				Left_Up		= Left	| Up,
				Right_Up	= Right	| Up,
				Middle_Up	= Middle| Up,
				X1_Up		= X1	| Up,
				X2_Up		= X2	| Up,

				Left_DoubleClick	= Left	| DoubleClick,
				Right_DoubleClick	= Right	| DoubleClick,
				Middle_DoubleClick	= Middle| DoubleClick,
				X1_DoubleClick		= X1	| DoubleClick,
				X2_DoubleClick		= X2	| DoubleClick,

				ButtonMask			= B8 (00001111),
				AllButOverCheck		= B16(00011111,11111111),
			};

			static bool isClick(Type t) {
				return (t&ButtonMask) && (t&Click);
			}

			static bool isScroll(Type t) {
				return t==VScroll || t==HScroll;
			}

			static bool isDown(Type t) {
				return (t&ButtonMask) && (t&Down);
			}

			static bool isUp(Type t) {
				return (t&ButtonMask) && (t&Up);
			}

			static bool isDoubleClick(Type t) {
				return (t&ButtonMask) && (t&DoubleClick);
			}

			static bool isLeft(Type t) {
				return (t&Left);
			}

			static bool isRight(Type t) {
				return (t&Right)!=0;
			}

			static bool isMiddle(Type t) {
				return (t&Middle)!=0;
			}

			static Type getButton(Type t) {
				return (Type)(t&ButtonMask);
			}

			static bool isX1(Type t) {
				return (t&X1)!=0;
			}

			static bool isX2(Type t) {
				return (t&X2)!=0;
			}

			class Target {
			public:

				Target(HandlerBase *handler, Event::Type eventmask=AllButOverCheck) : EventMask(eventmask) ,handler(handler), IsOver(false)  {

				}

				Event::Type EventMask;
				HandlerBase *handler;

				virtual bool Fire(Event::Type event, utils::Point location, int amount);

				virtual ~Target();

				bool IsOver;

			};

			Event(Type event=None, utils::Point location=utils::Point(0,0), int amount=0) :
				event(event), location(location), amount(amount)
			{ }

			Type event;
			utils::Point location;
			int amount;
		};

		inline Event::Type operator | (Event::Type l, Event::Type r) {
			return (Event::Type)((int)l|r);
		}

		inline Event::Type operator & (Event::Type l, Event::Type r) {
			return (Event::Type)((int)l&r);
		}

		inline Event::Type operator ~ (Event::Type l) {
			return (Event::Type)(~(int)l);
		}

		class EventChain {
			friend class EventProvider;
		private:

		public:
			class Object : public Event::Target {
				friend class EventChain;
			public:
				Object(HandlerBase *handler, utils::Bounds bounds, Event::Type eventmask=Event::AllButOverCheck) : Event::Target(handler, eventmask), Bounds(bounds), enabled(true) {

				}
				virtual ~Object();

				utils::Bounds Bounds;

				void Reorder(int order) {
					wrapper->Reorder(order);
				}

				int GetOrder() const {
					return wrapper->GetKey();
				}

				void Remove();

				void Enable() {
					enabled=true;
				}

				void Disable() {
					enabled=false;
				}

				void ToggleEnabled() {
					enabled=!enabled;
				}

				bool isEnabled() const {
					return enabled;
				}

				virtual bool Fire(Event::Type event, utils::Point location, int amount) {
					if(!enabled)
						return false;

					return Event::Target::Fire(event, location, amount);
				}

			private:
				utils::SortedCollection<Object>::Wrapper * wrapper;

				bool enabled;
			};

			bool Fire(Event::Type event, utils::Point location, int amount) {
				for(utils::SortedCollection<Object>::Iterator i=Events.Last();i.isValid();i.Previous()) {
					i->Fire(event, location, amount);
				}
			}

		protected:
			utils::SortedCollection<Object> Events;


		private:
			Object &registr(HandlerBase *handler, utils::Bounds bounds, Event::Type eventmask) {
				Object *obj=new Object(
					handler,
					bounds,
					eventmask
				);

				obj->wrapper=&Events.Add(obj);

				return *obj;
			}

		public:
			Object &Register(bool (*fn)(Event::Type event, utils::Point location, int amount, utils::Any data), utils::Bounds bounds, utils::Any data, Event::Type eventmask=Event::AllButOverCheck);

			Object &Register(bool (*fn)(Event::Type event, utils::Point location, int amount), utils::Bounds bounds, Event::Type eventmask=Event::AllButOverCheck);

			Object &Register(bool (*fn)(Event::Type event, utils::Point location), utils::Bounds bounds, Event::Type eventmask=Event::AllButOverCheck);

			Object &Register(bool (*fn)(utils::Point location), utils::Bounds bounds, Event::Type eventmask=Event::AllButOverCheck);

			Object &Register(bool (*fn)(), utils::Bounds bounds, Event::Type eventmask=Event::AllButOverCheck);

			template <class R_>
			Object &Register(R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount, utils::Any data), utils::Bounds bounds, utils::Any data, Event::Type eventmask=Event::AllButOverCheck);

			template <class R_>
			Object &Register(R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount), utils::Bounds bounds, Event::Type eventmask=Event::AllButOverCheck);

			template <class R_>
			Object &Register(R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location), utils::Bounds bounds, Event::Type eventmask=Event::AllButOverCheck);

			template <class R_>
			Object &Register(R_ &object, bool (R_::*fn)(utils::Point location), utils::Bounds bounds, Event::Type eventmask=Event::AllButOverCheck);

			template <class R_>
			Object &Register(R_ &object, bool (R_::*fn)(), utils::Bounds bounds, Event::Type eventmask=Event::AllButOverCheck);

			template<class F_>
			typename utils::count_arg<F_, 0, Object &>::type 
				RegisterLambda(F_ f, utils::Bounds bounds, Event::Type eventmask);

			template<class F_>
			typename utils::count_arg<F_, 1, Object &>::type 
				RegisterLambda(F_ f, utils::Bounds bounds, Event::Type eventmask);

			template<class F_>
			typename utils::count_arg<F_, 2, Object &>::type 
				RegisterLambda(F_ f, utils::Bounds bounds, Event::Type eventmask);

			template<class F_>
			typename utils::count_arg<F_, 3, Object &>::type 
				RegisterLambda(F_ f, utils::Bounds bounds, Event::Type eventmask);

			void Unregister(Object &obj);

			void Unregister(Object *obj);
		};

		////This is a code base to help with mouse events
		class EventProvider {
		public:

			virtual bool PropagateMouseEvent(Event::Type event, utils::Point location, int amount);

			EventChain MouseEvents;

			virtual ~EventProvider();

		protected:

		};


		class EventCallback {
			friend class CallbackProvider;
		private:

		public:
			EventCallback() : object(NULL) 
			{ }

			class Object : public Event::Target {
				friend class EventCallback;
			public:
				Object(HandlerBase *handler, Event::Type eventmask=Event::AllButOverCheck) : Event::Target(handler, eventmask), enabled(true) {

				}
				virtual ~Object();

				void Enable() {
					enabled=true;
				}

				void Disable() {
					enabled=false;
				}

				void ToggleEnabled() {
					enabled=!enabled;
				}

				bool isEnabled() const {
					return enabled;
				}

				virtual bool Fire(Event::Type event, utils::Point location, int amount) {
					if(!enabled)
						return false;

					return Event::Target::Fire(event, location, amount);
				}

			private:
				utils::SortedCollection<Object>::Wrapper * wrapper;

				bool enabled;
			};

			operator bool() {
				if(!object)
					return false;
				if(!object->isEnabled())
					return false;

				return true;
			}

			bool Fire(Event::Type event, utils::Point location, int amount) {
				if(object)
					object->Fire(event, location, amount);
				else
					return false;
			}

			Object *object;


		private:
			Object &set(HandlerBase *handler, Event::Type eventmask) {
				if(object)
					delete object;

				object=new Object(
					handler,
					eventmask
				);

				return *object;
			}

		public:
			Object &Set(bool (*fn)(Event::Type event, utils::Point location, int amount, utils::Any data), utils::Any data, Event::Type eventmask=Event::AllButOverCheck);

			Object &Set(bool (*fn)(Event::Type event, utils::Point location, int amount), Event::Type eventmask=Event::AllButOverCheck);

			Object &Set(bool (*fn)(Event::Type event, utils::Point location), Event::Type eventmask=Event::AllButOverCheck);

			Object &Set(bool (*fn)(utils::Point location), Event::Type eventmask=Event::AllButOverCheck);

			Object &Set(bool (*fn)(), Event::Type eventmask=Event::AllButOverCheck);

			template <class R_>
			Object &Set(R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount, utils::Any data), utils::Any data, Event::Type eventmask=Event::AllButOverCheck);

			template <class R_>
			Object &Set(R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount), Event::Type eventmask=Event::AllButOverCheck);

			template <class R_>
			Object &Set(R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location), Event::Type eventmask=Event::AllButOverCheck);

			template <class R_>
			Object &Set(R_ &object, bool (R_::*fn)(utils::Point location), Event::Type eventmask=Event::AllButOverCheck);

			template <class R_>
			Object &Set(R_ &object, bool (R_::*fn)(), Event::Type eventmask=Event::AllButOverCheck);

			template<class F_>
			typename utils::count_arg<F_, 0, Object &>::type SetLambda(F_ f, Event::Type eventmask=Event::AllButOverCheck);

			template<class F_>
			typename utils::count_arg<F_, 1, Object &>::type SetLambda(F_ f, Event::Type eventmask=Event::AllButOverCheck);

			template<class F_>
			typename utils::count_arg<F_, 2, Object &>::type SetLambda(F_ f, Event::Type eventmask=Event::AllButOverCheck);

			template<class F_>
			typename utils::count_arg<F_, 3, Object &>::type SetLambda(F_ f, Event::Type eventmask=Event::AllButOverCheck);
			
			void Reset() {
				if(object)
					delete object;

				object=NULL;
			}
		};

		////This is a code base to help with mouse events
		class CallbackProvider {
		public:

			virtual bool PropagateMouseEvent(Event::Type event, utils::Point location, int amount);

			EventCallback MouseCallback;

		protected:

		};

		class HandlerBase{
		public:
			utils::Any data;

			HandlerBase(utils::Any data) : data(data) {

			}

			virtual bool Fire(Event::Type event, utils::Point location, int amount) = 0;
		};
		class FullFunctionHandler : public HandlerBase {
		public:
			typedef bool (*Handler)(Event::Type event, utils::Point location, int amount, utils::Any data);
			Handler fn;

			FullFunctionHandler(Handler fn, utils::Any data) : HandlerBase(data), fn(fn) {

			}

			virtual bool Fire(Event::Type event, utils::Point location, int amount) {
				return fn(event, location, amount, data);
			}
		};

		class FunctionHandler : public HandlerBase {
		public:
			typedef bool (*Handler)(Event::Type event, utils::Point location, int amount);
			Handler fn;

			FunctionHandler(Handler fn) : HandlerBase(utils::Any()), fn(fn) {

			}

			virtual bool Fire(Event::Type event, utils::Point location, int amount) {
				return fn(event, location, amount);
			}
		};

		class NoAmountFunctionHandler : public HandlerBase {
		public:
			typedef bool (*Handler)(Event::Type event, utils::Point location);
			Handler fn;

			NoAmountFunctionHandler(Handler fn) : HandlerBase(utils::Any()), fn(fn) {

			}

			virtual bool Fire(Event::Type event, utils::Point location, int amount) {
				return fn(event, location);
			}
		};

		class LocationOnlyFunctionHandler : public HandlerBase {
		public:
			typedef bool (*Handler)(utils::Point location);
			Handler fn;

			LocationOnlyFunctionHandler(Handler fn) : HandlerBase(utils::Any()), fn(fn) {

			}

			virtual bool Fire(Event::Type event, utils::Point location, int amount) {
				return fn(location);
			}
		};

		class EmptyFunctionHandler : public HandlerBase {
		public:
			typedef bool (*Handler)();
			Handler fn;

			EmptyFunctionHandler(Handler fn) : HandlerBase(utils::Any()), fn(fn) {

			}

			virtual bool Fire(Event::Type event, utils::Point location, int amount) {
				return fn();
			}
		};

		template <class R_>
		class FullClassHandler : public HandlerBase {
		public:
			typedef bool (R_::*Handler)(Event::Type event, utils::Point location, int amount, utils::Any data);
			Handler fn;
			R_ *object;

			FullClassHandler(R_ *object, Handler fn, utils::Any data) : HandlerBase(data), object(object), fn(fn) {

			}

			virtual bool Fire(Event::Type event, utils::Point location, int amount) {
				return (object->*fn)(event, location, amount, data);
			}
		};

		template <class R_>
		class ClassHandler : public HandlerBase {
		public:
			typedef bool (R_::*Handler)(Event::Type event, utils::Point location, int amount);
			Handler fn;
			R_ *object;

			ClassHandler(R_ *object, Handler fn) : HandlerBase(utils::Any()), object(object), fn(fn) {

			}

			virtual bool Fire(Event::Type event, utils::Point location, int amount) {
				return (object->*fn)(event, location, amount);
			}
		};

		template <class R_>
		class NoAmountClassHandler : public HandlerBase {
		public:
			typedef bool (R_::*Handler)(Event::Type event, utils::Point location);
			Handler fn;
			R_ *object;

			NoAmountClassHandler(R_ *object, Handler fn) : HandlerBase(utils::Any()), object(object), fn(fn) {

			}

			virtual bool Fire(Event::Type event, utils::Point location, int amount) {
				return (object->*fn)(event, location);
			}
		};

		template <class R_>
		class LocationOnlyClassHandler : public HandlerBase {
		public:
			typedef bool (R_::*Handler)(utils::Point location);
			Handler fn;
			R_ *object;

			LocationOnlyClassHandler(R_ *object, Handler fn) : HandlerBase(utils::Any()), object(object), fn(fn) {

			}

			virtual bool Fire(Event::Type event, utils::Point location, int amount) {
				return (object->*fn)(location);
			}
		};

		template <class R_>
		class EmptyClassHandler : public HandlerBase {
		public:
			typedef bool (R_::*Handler)();
			Handler fn;
			R_ *object;

			EmptyClassHandler(R_ *object, Handler fn) : HandlerBase(utils::Any()), object(object), fn(fn) {

			}

			virtual bool Fire(Event::Type event, utils::Point location, int amount) {
				return (object->*fn)();
			}
		};
		class FullLambdaHandler : public HandlerBase {
		public:
			typedef std::function<bool(Event::Type, utils::Point, int)> Handler;
			Handler fn;

			FullLambdaHandler(Handler fn) : HandlerBase(utils::Any()), fn(fn) {}

			virtual	bool Fire(Event::Type event, utils::Point location, int amount) {
				return fn(event, location, amount);
			}
		};
		class NoAmountLambdaHandler : public HandlerBase {
		public:
			typedef std::function<bool(Event::Type, utils::Point)> Handler;
			Handler fn;

			NoAmountLambdaHandler(Handler fn) : HandlerBase(utils::Any()), fn(fn) {}

			virtual	bool Fire(Event::Type event, utils::Point location, int amount) {
				return fn(event, location);
			}
		};
		class LocationOnlyLambdaHandler : public HandlerBase {
		public:
			typedef std::function<bool(utils::Point)> Handler;
			Handler fn;

			LocationOnlyLambdaHandler(Handler fn) : HandlerBase(utils::Any()), fn(fn) {}

			virtual	bool Fire(Event::Type event, utils::Point location, int amount) {
				return fn(location);
			}
		};
		class EmptyLambdaHandler : public HandlerBase {
		public:
			typedef std::function<bool()> Handler;
			Handler fn;

			EmptyLambdaHandler(Handler fn) : HandlerBase(utils::Any()), fn(fn) {}

			virtual	bool Fire(Event::Type event, utils::Point location, int amount) {
				return fn();
			}
		};

		
		extern Event::Type		PressedButtons;

		extern Event::Target	*PressedObject;

		extern utils::Point	PressedPoint;
		extern utils::Point	CurrentPoint;




		template <class R_>
		EventChain::Object & EventChain::Register( R_ &object, bool (R_::*fn)(), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new EmptyClassHandler<R_>(&object,fn),
				bounds,
				eventmask
				);
		}

		template <class R_>
		EventChain::Object & EventChain::Register( R_ &object, bool (R_::*fn)(utils::Point location), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new LocationOnlyClassHandler<R_>(&object,fn),
				bounds,
				eventmask
				);
		}

		template <class R_>
		EventChain::Object & EventChain::Register( R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new NoAmountClassHandler<R_>(&object,fn),
				bounds,
				eventmask
				);
		}

		template <class R_>
		EventChain::Object & EventChain::Register( R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new ClassHandler<R_>(&object,fn),
				bounds,
				eventmask
				);
		}

		template <class R_>
		EventChain::Object & EventChain::Register( R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount, utils::Any data), utils::Bounds bounds, utils::Any data, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new FullClassHandler<R_>(&object,fn,data),
				bounds,
				eventmask
				);
		}

		template <class F_>
		typename utils::count_arg<F_, 0, EventChain::Object &>::type EventChain::RegisterLambda( F_ fn, utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new EmptyLambdaHandler(fn,data),
				bounds,
				eventmask
				);
		}

		template <class F_>
		typename utils::count_arg<F_, 1, EventChain::Object &>::type EventChain::RegisterLambda( F_ fn, utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new LocationOnlyLambdaHandler(fn,data),
				bounds,
				eventmask
				);
		}

		template <class F_>
		typename utils::count_arg<F_, 2, EventChain::Object &>::type EventChain::RegisterLambda( F_ fn, utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new NoAmountLambdaHandler(fn,data),
				bounds,
				eventmask
				);
		}

		template <class F_>
		typename utils::count_arg<F_, 3, EventChain::Object &>::type EventChain::RegisterLambda( F_ fn, utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new FullLambdaHandler(fn,data),
				bounds,
				eventmask
				);
		}

		template <class R_>
		EventCallback::Object & EventCallback::Set( R_ &object, bool (R_::*fn)(), Event::Type eventmask/*=AllUsed*/ )
		{
			return set(
				new EmptyClassHandler<R_>(&object,fn),

				eventmask
				);
		}

		template <class R_>
		EventCallback::Object & EventCallback::Set( R_ &object, bool (R_::*fn)(utils::Point location), Event::Type eventmask/*=AllUsed*/ )
		{
			return set(
				new LocationOnlyClassHandler<R_>(&object,fn),

				eventmask
				);
		}

		template <class R_>
		EventCallback::Object & EventCallback::Set( R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location), Event::Type eventmask/*=AllUsed*/ )
		{
			return set(
				new NoAmountClassHandler<R_>(&object,fn),

				eventmask
				);
		}

		template <class R_>
		EventCallback::Object & EventCallback::Set( R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount), Event::Type eventmask/*=AllUsed*/ )
		{
			return set(
				new ClassHandler<R_>(&object,fn),

				eventmask
				);
		}

		template <class R_>
		EventCallback::Object & EventCallback::Set( R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount, utils::Any data), utils::Any data, Event::Type eventmask/*=AllUsed*/ )
		{
			return set(
				new FullClassHandler<R_>(&object,fn,data),

				eventmask
				);
		}

		template<class F_>
		typename utils::count_arg<F_, 0, EventCallback::Object &>::type EventCallback::SetLambda(F_ f, Event::Type eventmask) {
			return set(
				new EmptyLambdaHandler(f),

				eventmask
				);
		}

		template<class F_>
		typename utils::count_arg<F_, 1, EventCallback::Object &>::type EventCallback::SetLambda(F_ f, Event::Type eventmask) {
			return set(
				new LocationOnlyLambdaHandler(f),

				eventmask
				);
		}

		template<class F_>
		typename utils::count_arg<F_, 2, EventCallback::Object &>::type EventCallback::SetLambda(F_ f, Event::Type eventmask) {
			return set(
				new NoAmountLambdaHandler(f),

				eventmask
				);
		}

		template<class F_>
		typename utils::count_arg<F_, 3, EventCallback::Object &>::type EventCallback::SetLambda(F_ f, Event::Type eventmask) {
			return set(
				new FullLambdaHandler(f),

				eventmask
				);
		}
	}

	namespace system {
		////Processes a given char, this function intended to be called from OS
		/// sub-system, however a knowledgeable attempt to call from elsewhere is
		/// also valid
		///@Char	: Character code (8bit ASCII or equivalent (ISO-8859))
		void ProcessChar(int Char);
		////Processes a given key as pressed, this function intended to be called from OS
		/// sub-system, however a knowledgeable attempt to call from elsewhere is
		/// also valid
		///@Key		: The keyboard code of the pressed key
		void ProcessKeyDown(int Key);
		////Processes a given key as released, this function intended to be called from OS
		/// sub-system, however a knowledgeable attempt to call from elsewhere is
		/// also valid
		///@Key		: The keyboard code of the pressed key
		void ProcessKeyUp(int Key);

		////Processes the current mouse position this information is taken from OS subsystem
		void ProcessMousePosition(os::WindowHandle Window);
		////Processes click of mouse button
		///@button	: button number 1 for left, 2 for right  and 4 for middle
		void ProcessMouseClick(mouse::Event::Type button, int x, int y);
		////Processes given mouse button as pressed
		///@button	: button number 1 for left, 2 for right and 4 for middle
		void ProcessMouseDown(mouse::Event::Type button,int x,int y);
		////Processes given mouse button as released
		///@button	: button number 1 for left, 2 for right and 4 for middle
		void ProcessMouseUp(mouse::Event::Type button,int x,int y);
		////Processes double click action
		///@button	: button number 1 for left, 2 for right and 4 for middle
		void ProcessMouseDblClick(mouse::Event::Type button,int x,int y);
		////Processes vertical scroll
		void ProcessVScroll(int amount,int x,int y);
		////Processes horizontal scroll
		void ProcessHScroll(int amount,int x,int y);

		extern bool hoverfound;
	}




	////Initializes Input system
	void Initialize();
} }
