#pragma once

#include "../Utils/SortedCollection.h"
#include "../Utils/Collection.h"
#include "GGE.h"

#include "OS.h"
#include "../Utils/ConsumableEvent.h"
#include "../Utils/Bounds2D.h"
#include <functional>
#include <map>

namespace gge { 
	class LayerBase;

	namespace input {
	class BasicPointerTarget;


	namespace keyboard { 
		typedef int Key;
		extern std::map<Key, bool> PressedKeys;

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

		namespace KeyCodes {
			extern const Key Shift;
			extern const Key Control;
			extern const Key Alt;
			extern const Key Super;

			extern const Key Home;
			extern const Key End;
			extern const Key Insert;
			extern const Key Delete;
			extern const Key PageUp;
			extern const Key PageDown;

			extern const Key Left;
			extern const Key Up;
			extern const Key Right;
			extern const Key Down;

			extern const Key PrintScreen;
			extern const Key Pause;

			extern const Key CapsLock;
			extern const Key NumLock;

			extern const Key Enter;
			extern const Key Tab;
			extern const Key Backspace;
			extern const Key Space;
			extern const Key Escape;

			extern const Key F1;
			extern const Key F2;
			extern const Key F3;
			extern const Key F4;
			extern const Key F5;
			extern const Key F6;
			extern const Key F7;
			extern const Key F8;
			extern const Key F9;
			extern const Key F10;
			extern const Key F11;
			extern const Key F12;
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
		
		const int DragDistance			= 5;//px

		class IDragData {
		public:
			//Negative typeids are reserved for system, use positive
			virtual int TypeID() const = 0;
		};

		template<class T_, int TID>
		class SimpleDragData : public IDragData {
		public:
			SimpleDragData(const T_ &data=T_()) : data(data) { }

			virtual int TypeID() const { return type; }

			static const int type=TID;

			T_ data;
		};

		typedef SimpleDragData<std::string, -1> TextDragData;
		typedef SimpleDragData<std::string, -2> IndexDragData;


		class FileListDragData : public gge::input::mouse::SimpleDragData<std::vector<std::string>, -3> {
		public:
			enum FileDragAction {
				Move,
				Copy
			};

			virtual void SetAction(FileDragAction action) {
				this->action=action;
			}

			FileDragAction GetAction() const {
				return action;
			}

		protected:
			FileDragAction action;
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
				Up			= 1<<7, //Always supplies absolute position

				Over		= 1<<9,
				Out			= 1<<10,
				Move		= 1<<11,
				VScroll		= 1<<12,
				HScroll		= 1<<13,

				OverCheck	= 1<<14,

				Cancel		= 1<<15,
				Accept		= 1<<16,
				Drop		= 1<<17,

				Drag		= 1<<18,

				DragOver	= Drag | Over,
				DragMove	= Drag | Move,
				DragDrop	= Drag | Drop,
				DragOut		= Drag | Out,
				DragAccepted= Drag | Accept, //Always supplies absolute position
				DragCanceled= Drag | Cancel, //Always supplies absolute position


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

				ButtonMask			= B8 (00001111),
				NoDragNoOverCheck	= B32(00000000,00000011,10111111,11111111),
				AllButOverCheck		= B32(00000000,00000111,10111111,11111111),
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

			//static bool isDoubleClick(Type t) {
			//	return (t&ButtonMask) && (t&DoubleClick);
			//}

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

			static bool isDrag(Type t) {
				return (t&Drag)!=0;
			}

			class Target {
			public:

				Target(HandlerBase *handler, Event::Type eventmask=NoDragNoOverCheck) : EventMask(eventmask) ,handler(handler), IsOver(false)  {

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
		////Begins dragging operation. Does not alter mouse cursor or does not show any preview.
		/// While drag continues, source event will keep receiving dragmove event. Targeted mouse event
		/// will receive dragover event. If it accepts dragover event, target layer will receive
		/// dragmove and dragdrop events. If mouse exits the current target area, dragout event will
		/// fire. If target event handler accepts drop event, source will receive dragaccept event.
		/// If it is not accepted or there is no target, source will receive dragcancel event.
		/// Any object is allowed to cancel drag operation.
		///@object: Data for the dragged object
		///@sourceevent: Source mouse event object. This value is returned from the event register or set 
		///              functions
		void BeginDrag(IDragData &object, mouse::Event::Target &sourceevent);
		void BeginDrag(IDragData &object);

		void CancelDrag();

		void DropDrag();

		bool IsDragging();

		bool HasDragTarget();
		
		extern utils::Point DragLocation;

		extern utils::EventChain<> DragStateChanged;
		extern utils::Point DragLocation;

		//throws runtime error on no object is being dragged
		IDragData &GetDraggedObject();

		class EventChain {
			friend class EventProvider;
		private:

		public:
			class Object : public Event::Target {
				friend class EventChain;
			public:
				Object(HandlerBase *handler, utils::Bounds bounds, Event::Type eventmask=Event::NoDragNoOverCheck) : Event::Target(handler, eventmask), Bounds(bounds), enabled(true) {

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
				for(utils::SortedCollection<Object>::Iterator i=Events.Last();i.IsValid();i.Previous()) {
					if(i->Fire(event, location, amount))
						return true;
				}
				
				return false;
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
			Object &Register(bool (*fn)(Event::Type event, utils::Point location, int amount, utils::Any data), utils::Bounds bounds, utils::Any data, Event::Type eventmask=Event::NoDragNoOverCheck);

			Object &Register(bool (*fn)(Event::Type event, utils::Point location, int amount), utils::Bounds bounds, Event::Type eventmask=Event::NoDragNoOverCheck);

			Object &Register(bool (*fn)(Event::Type event, utils::Point location), utils::Bounds bounds, Event::Type eventmask=Event::NoDragNoOverCheck);

			Object &Register(bool (*fn)(utils::Point location), utils::Bounds bounds, Event::Type eventmask=Event::NoDragNoOverCheck);

			Object &Register(bool (*fn)(), utils::Bounds bounds, Event::Type eventmask=Event::NoDragNoOverCheck);

			template <class R_>
			Object &Register(R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount, utils::Any data), utils::Bounds bounds, utils::Any data, Event::Type eventmask=Event::NoDragNoOverCheck);

			template <class R_>
			Object &Register(R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount), utils::Bounds bounds, Event::Type eventmask=Event::NoDragNoOverCheck);

			template <class R_>
			Object &Register(R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location), utils::Bounds bounds, Event::Type eventmask=Event::NoDragNoOverCheck);

			template <class R_>
			Object &Register(R_ &object, bool (R_::*fn)(utils::Point location), utils::Bounds bounds, Event::Type eventmask=Event::NoDragNoOverCheck);

			template <class R_>
			Object &Register(R_ &object, bool (R_::*fn)(), utils::Bounds bounds, Event::Type eventmask=Event::NoDragNoOverCheck);

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
				Object(HandlerBase *handler, Event::Type eventmask=Event::NoDragNoOverCheck) : Event::Target(handler, eventmask), enabled(true) {

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
			Object &Set(bool (*fn)(Event::Type event, utils::Point location, int amount, utils::Any data), utils::Any data, Event::Type eventmask=Event::NoDragNoOverCheck);

			Object &Set(bool (*fn)(Event::Type event, utils::Point location, int amount), Event::Type eventmask=Event::NoDragNoOverCheck);

			Object &Set(bool (*fn)(Event::Type event, utils::Point location), Event::Type eventmask=Event::NoDragNoOverCheck);

			Object &Set(bool (*fn)(utils::Point location), Event::Type eventmask=Event::NoDragNoOverCheck);

			Object &Set(bool (*fn)(), Event::Type eventmask=Event::NoDragNoOverCheck);

			template <class R_>
			Object &Set(R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount, utils::Any data), utils::Any data, Event::Type eventmask=Event::NoDragNoOverCheck);

			template <class R_>
			Object &Set(R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount), Event::Type eventmask=Event::NoDragNoOverCheck);

			template <class R_>
			Object &Set(R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location), Event::Type eventmask=Event::NoDragNoOverCheck);

			template <class R_>
			Object &Set(R_ &object, bool (R_::*fn)(utils::Point location), Event::Type eventmask=Event::NoDragNoOverCheck);

			template <class R_>
			Object &Set(R_ &object, bool (R_::*fn)(), Event::Type eventmask=Event::NoDragNoOverCheck);

			template<class F_>
			typename utils::count_arg<F_, 0, Object &>::type SetLambda(F_ f, Event::Type eventmask=Event::NoDragNoOverCheck);

			template<class F_>
			typename utils::count_arg<F_, 1, Object &>::type SetLambda(F_ f, Event::Type eventmask=Event::NoDragNoOverCheck);

			template<class F_>
			typename utils::count_arg<F_, 2, Object &>::type SetLambda(F_ f, Event::Type eventmask=Event::NoDragNoOverCheck);

			template<class F_>
			typename utils::count_arg<F_, 3, Object &>::type SetLambda(F_ f, Event::Type eventmask=Event::NoDragNoOverCheck);
			
			void Reset() {
				utils::CheckAndDelete(object);
			}

			~EventCallback() { Reset(); }
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
				new EmptyLambdaHandler(fn),
				bounds,
				eventmask
				);
		}

		template <class F_>
		typename utils::count_arg<F_, 1, EventChain::Object &>::type EventChain::RegisterLambda( F_ fn, utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new LocationOnlyLambdaHandler(fn),
				bounds,
				eventmask
				);
		}

		template <class F_>
		typename utils::count_arg<F_, 2, EventChain::Object &>::type EventChain::RegisterLambda( F_ fn, utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new NoAmountLambdaHandler(fn),
				bounds,
				eventmask
				);
		}

		template <class F_>
		typename utils::count_arg<F_, 3, EventChain::Object &>::type EventChain::RegisterLambda( F_ fn, utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new FullLambdaHandler(fn),
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
		////Processes vertical scroll
		void ProcessVScroll(int amount,int x,int y);
		////Processes horizontal scroll
		void ProcessHScroll(int amount,int x,int y);
		void ReleaseAll();

		extern bool hoverfound;
		extern mouse::Event::Target *dragsource;

		extern mouse::Event::Target *dragtarget;
		extern mouse::IDragData *draggedobject;
		extern bool isdragging;

	}




	////Initializes Input system
	void Initialize();
} }
