#pragma once

#include <assert.h>

#include "Event.h"
#include "Containers/Collection.h"

namespace Gorgon { 
	
	/// This namespace contains general input related functionality.
	/// Also check Gorgon::Keyboard and Gorgon::Mouse.
	namespace Input {
		/// A type to represent an input key
		typedef int Key;
		
		/// @cond INTERNAL
		namespace internal {
			
			template<class Source_>
			struct HandlerBase {
				virtual bool Fire(std::mutex &locker, Source_ *object, Key key, float amount) = 0;
				
				bool active=true;
			};
			

			template<class Source_>
			struct EmptyHandlerFn : public HandlerBase<Source_> {
				EmptyHandlerFn(std::function<bool()> fn) : fn(fn) { }

				virtual bool Fire(std::mutex &locker, Source_ *, Key key, float amount) {
					auto f=fn;
					locker.unlock();
					return f();
				}

				std::function<bool()> fn;
			};

			template<class Source_>
			struct ArgsHandlerFn : public HandlerBase<Source_> {
				ArgsHandlerFn(std::function<bool(Key, float)> fn) : fn(fn) { }

				virtual bool Fire(std::mutex &locker, Source_ *, Key key, float amount) {
					auto f=fn;
					locker.unlock();
					return f(key, amount);
				}

				std::function<bool(Key, float)> fn;
			};

			template<class Source_>
			struct FullHandlerFn : public HandlerBase<Source_> {
				static_assert(!std::is_same<Source_, Empty>::value, "No source class exists for this event (Empty should not be passed around)");
				
				FullHandlerFn(std::function<bool(Source_&, Key, float)> fn) : fn(fn) { }

				virtual bool Fire(std::mutex &locker, Source_ *source, Key key, float amount) {
					auto f=fn;
					locker.unlock();
					return f(*source, key, amount);
				}

				std::function<bool(Source_&, Key, float)> fn;
			};
			

			template<class Source_>
			static HandlerBase<Source_>& createhandlerfn(void(*fn)()) {
				return *new EmptyHandlerFn<Source_>(fn);
			}

			template<class Source_>
			static HandlerBase<Source_>& createhandlerfn(void(*fn)(Key, float)) {
				return *new ArgsHandlerFn<Source_>(fn);
			}

			template<class Source_>
			static HandlerBase<Source_>& createhandlerfn(void(*fn)(Source_ &, Key, float)) {
				return *new FullHandlerFn<Source_>(fn);
			}

			template<class F_, class Source_>
			static typename std::enable_if<Gorgon::TMP::argscount<F_>::value==0, HandlerBase<Source_>&>::type 
			createhandler(F_ fn) {
				return *new EmptyHandlerFn<Source_>(fn);
			}

			template<class F_, class Source_>
			static typename std::enable_if<Gorgon::TMP::argscount<F_>::value==2, HandlerBase<Source_>&>::type 
			createhandler(F_ fn) {
				return *new ArgsHandlerFn<Source_>(fn);
			}

			template<class F_, class Source_>
			static typename std::enable_if<Gorgon::TMP::argscount<F_>::value==3, HandlerBase<Source_>&>::type 
			createhandler(F_ fn) {
				return *new FullHandlerFn<Source_>(fn);
			}
		}
		/// @endcond
			
		/// Represents an input event. First parameter is the key that is related
		/// with the event. Second parameter represents the amount of event occurring.
		/// In case of keyboard events, this value could either 1 or 0 representing
		/// whether the key is pressed or released. In case of mouse scroll, this value
		/// represents the amount of scroll. In analog controllers this value is between
		/// 0 and 1, representing how much further controller is pushed. 
		
		/// @warning Analog controllers triggers event only when changing from 0 to a positive value and vice versa.
		template <class Source_=Empty>
		class Event {
		public:
			typedef intptr_t Token;
			
			/// Constructor for empty source
			Event() : source(nullptr) {
				static_assert( std::is_same<Source_, Empty>::value , "Empty constructor cannot be used." );
				fire.clear();
			}
			
			/// Constructor for class specific source
			explicit Event(Source_ &source) : source(&source) {
				static_assert( !std::is_same<Source_, Empty>::value, "Filling constructor is not required, use the default." );
				fire.clear();
			}
			
			/// Move constructor
			Event(Event &&event) : source(nullptr) {
				Swap(event);
			}
			

			/// Destructor
			~Event() {
				assert( ("An event cannot be destroyed while its being fired.", !fire.test_and_set()) );

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
			
			/// Deactivates the given handler token
			void Deactivate(Token token) {
				std::lock_guard<std::mutex> g(access);

				auto item=reinterpret_cast<internal::HandlerBase<Source_>*>(token);
				
				auto l=handlers.FindLocation(item);
				if(l==-1) return;
				
				item->active=false;
			}
			
			/// Activates the given handler token and moves it to the top of the list
			/// so that it will be called first
			void Activate(Token token) {
				std::lock_guard<std::mutex> g(access);

				auto item=reinterpret_cast<internal::HandlerBase<Source_>*>(token);
				
				auto l=handlers.FindLocation(item);
				if(l==-1) return;
				
				item->active=true;
				handlers.MoveBefore(*item, 0);
			}
			
			/// Sends the given handler token to the bottom of the list so that it will
			/// be fired last
			void FireLast(Token token) {
				std::lock_guard<std::mutex> g(access);

				auto item=reinterpret_cast<internal::HandlerBase<Source_>*>(token);
				
				auto l=handlers.FindLocation(item);
				if(l==-1) return;
				
				item->active=true;
				handlers.MoveBefore(item, handlers.GetCount());
			}
			

			/// Registers a new function to be called when this event is triggered. This function can
			/// be called from event handler of the same event. The registered event handler will be
			/// called immediately in this case.
			template<class F_>
			Token Register(F_ fn) {
				auto &handler=internal::createhandler<F_, Source_>(fn);

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
				std::function<void(A_...)> f=Gorgon::TMP::MakeFunctionFromMember(fn, &c);

				return Register(f);
			}			

			/// Unregisters the given marked with the given token. This function performs no
			/// operation if the token is not contained within this event. A handler can be
			/// unregistered safely while the event is being fired. In this case, if the event that
			/// is being deleted is different from the current event handler, the deleted event
			/// handler might have already been fired. If not, it will not be fired.
			void Unregister(Token token) {
				std::lock_guard<std::mutex> g(access);

				auto item=reinterpret_cast<internal::HandlerBase<Source_>*>(token);
				
				auto l=handlers.FindLocation(item);
				if(l==-1) return;
				
				if(iterator.CurrentPtr()==item) {
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
			/// @return Whether this event is handled by a handler.
			Token operator()(Key key, float amount) {
				//prevent recursion
				if(fire.test_and_set()) return EmptyToken; /// add to queue??

				stop=false;

				try {
					for(iterator=handlers.begin(); iterator.IsValid(); iterator.Next()) {
						if(!iterator->active) continue;

						access.lock();
						// fire method will unlock access after it creates a local copy of the function
						// this allows the fired object to be safely deleted.
						bool ret=iterator->Fire(access, source, key, amount);

						if(ret) {
							fire.clear();
							return reinterpret_cast<Token>(iterator.CurrentPtr());
						}

						if(stop) {
							fire.clear();
							return EmptyToken;
						}
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

				return EmptyToken;
			}

			/// Fire this event only for the given handler provided that it still exists in this event. If it doesn't exists
			/// this method will return false otherwise, it will always return true.
			bool operator()(Token token, Key key, float amount) {
				//prevent recursion
				if(fire.test_and_set()) return false; /// add to queue??

				stop=false;

				try {
					auto item=reinterpret_cast<internal::HandlerBase<Source_>*>(token);
					if(handlers.FindLocation(item)==-1) {
						fire.clear();
						return false;
					}

					access.lock();

					item->Fire(access, source, key, amount);
				}
				catch(...) {
					//unlock everything if something goes bad

					//just in case
					access.unlock();

					fire.clear();

					throw;
				}

				fire.clear();
				return true;
			}

			/// Stops event from continuing. Should be called inside an eventhandler to be effective.
			/// Use this instead of returning true to keep unhandled status while canceling rest of
			/// the handlers.
			void Stop() {
				stop=true;
			}
			
			/// value for an empty token
			static const Token EmptyToken = 0;
			

		private:
			std::mutex access;
			std::atomic_flag fire;
			Source_ *source;
			Containers::Collection<internal::HandlerBase<Source_>> handlers;
			typename Containers::Collection<internal::HandlerBase<Source_>>::Iterator iterator;
			bool stop;
		};
	}
}

#ifdef nononee	

	namespace keyboard { 
		extern std::map<Key, bool> PressedKeys;


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

		//You have to use start to receive events
		class EventRepeater {
		public:
			EventRepeater(unsigned timeout, std::function<void(Key key)> handler, std::string keys="") : 
				timeout(timeout), timeleft(0), keytoken(0), repeattoken(0), handler(handler)
			{
				for(auto &c : keys) {
					this->keys.push_back(c);
				}
			}

			void AddKey(Key key) {
				if(std::find(keys.begin(), keys.end(), key)==keys.end()) {
					keys.push_back(key);
				}
			}

			void RemoveKey(Key key) {
				auto it=std::find(keys.begin(), keys.end(), key);

				if(it!=keys.end())
					keys.erase(it);
			}

			void Reset();

			void Set() {
				timeleft=timeout;
			}

			void SetTimeout(unsigned timeout) {
				if(timeleft>0) {
					timeleft+=timeout-this->timeout;
					if(timeleft<0) timeleft=0;
				}
				this->timeout=timeout;
			}

			unsigned GetTimeout() const {
				return timeout;
			}

			void Start();

			void Stop();

			bool ProcessKey(Event event);

			void Tick();

		private:
			std::vector<Key> keys;
			int timeleft;
			int lasttick;
			int timeout;
			std::function<void(Key key)> handler;
			utils::ConsumableEvent<utils::Empty, Event>::Token keytoken;
			utils::EventChain<>::Token repeattoken;
		};
	}

	namespace mouse {

		class HandlerBase;
		class Event;
		
		const int DragDistance			= 5;//px

		class IDragData {
		public:
			virtual ~IDragData() {}
			
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

			~SimpleDragData() {}
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
					return object->Fire(event, location, amount);
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

		class HandlerBase {
		public:
			utils::Any data;

			HandlerBase(utils::Any data) : data(data) {

			}
			virtual ~HandlerBase() {}

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
#endif
