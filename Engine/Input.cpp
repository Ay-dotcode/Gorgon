#include "Input.h"
#include "GGEMain.h"

using namespace gge;
using namespace utils;

namespace gge { namespace input {

	namespace keyboard {
		bool Modifier::isAlternate=false;
		Modifier::Type Modifier::Current=Modifier::None;
		utils::ConsumableEvent<utils::Empty, Event> Events;
	}


	namespace mouse {
		Event::Type		PressedButtons	= Event::None;
		Event::Target	*HoveredObject	= NULL;
		Event::Target	*PressedObject	= NULL;

		utils::Point	PressedPoint	= utils::Point(0,0);
		utils::Point	CurrentPoint	= utils::Point(0,0);

		const int DragDistance			= 5;//px

		bool EventProvider::PropagateMouseEvent(Event::Type event, utils::Point location, int amount) {
			if(Event::isClick(event)) {
				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.First();i.isValid();i.Next()) {
					if(&(*i)==PressedObject && i->Bounds.isInside(location)) {
						if(i->Fire(event, location-i->Bounds.TopLeft(), amount))
							return true;
					}
				}

				return false;
			}
			else if(Event::isDown(event)) {
				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.First();i.isValid();i.Next()) {
					if(i->Bounds.isInside(location)) {
						if(i->Fire(event, location-i->Bounds.TopLeft(), amount)) {
							mouse::PressedObject=&(*i);
							return true;
						}
					}
				}

				return false;
			}
			else if(event==Event::Move) {
				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.First();i.isValid();i.Next()) {
					if(i->Bounds.isInside(location)) {
						if(i->EventMask & Event::OverCheck)
							if(!i->Fire(Event::OverCheck, location-i->Bounds.TopLeft(), 0))
								continue;

						bool ret=false;

						if(i->EventMask & Event::Over && &(*i)!=HoveredObject && !PressedObject) {
							ret = i->Fire(Event::Over, location-i->Bounds.TopLeft(), amount);

							if(ret) {
								if(HoveredObject)
									HoveredObject->Fire(Event::Out, utils::Point(0,0), 0);

								HoveredObject = &(*i);

								i->Fire(Event::Move, location-i->Bounds.TopLeft(), amount);
							}
						}
						else if(!PressedObject || PressedObject==&(*i)) {
							ret = i->Fire(Event::Over, location-i->Bounds.TopLeft(), amount);
						}

						if(&(*i)==HoveredObject)
							system::hoverfound=true;

						if(ret || &(*i)==HoveredObject)
							return true;
					}
				}

				return false;
			}
			else { //Scrolls, double click
				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.First();i.isValid();i.Next()) {
					if(i->Bounds.isInside(location)) {
						if(i->Fire(event, location-i->Bounds.TopLeft(), amount))
							return true;
					}
				}

				return false;
			}
		}

		bool CallbackProvider::PropagateMouseEvent(Event::Type event, utils::Point location, int amount) {
			if(!Callback) return false;

			if(Event::isClick(event)) {
				if(Callback.object==PressedObject) {
					if(Callback.object->Fire(event, location, amount))
						return true;
				}

				return false;
			}
			else if(Event::isDown(event)) {
				if(Callback.object->Fire(event, location, amount)) {
					mouse::PressedObject=Callback.object;
					return true;
				}

				return false;
			}
			else if(event==Event::Move) {
				if(Callback.object->EventMask & Event::OverCheck)
					if(!Callback.object->Fire(Event::OverCheck, location, 0))
						return false;

				bool ret=false;

				if(Callback.object->EventMask & Event::Over && Callback.object!=HoveredObject && !PressedObject) {
					ret = Callback.object->Fire(Event::Over, location, amount);

					if(ret) {
						if(HoveredObject)
							HoveredObject->Fire(Event::Out, utils::Point(0,0), 0);

						HoveredObject = Callback.object;

						Callback.object->Fire(Event::Move, location, amount);
					}
				}
				else if(!PressedObject || PressedObject==Callback.object) {
					ret = Callback.object->Fire(Event::Over, location, amount);
				}

				if(Callback.object==HoveredObject)
					system::hoverfound=true;

				return (ret || Callback.object==HoveredObject);
			}
			else { //Scrolls, double click
				if(Callback.object->Fire(event, location, amount))
					return true;

				return false;
			}
		}

		EventChain::Object & EventChain::Register( bool (*fn)(Event::Type event, utils::Point location, int amount, utils::Any data), utils::Bounds bounds, utils::Any data, Event::Type eventmask/*=AllUsed*/ )
		{
			return Register(
				new FullFunctionHandler(fn,data),
				bounds,
				eventmask
				);
		}

		EventChain::Object & EventChain::Register( bool (*fn)(Event::Type event, utils::Point location, int amount), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return Register(
				new FunctionHandler(fn),
				bounds,
				eventmask
				);
		}

		EventChain::Object & EventChain::Register( bool (*fn)(Event::Type event, utils::Point location), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return Register(
				new NoAmountFunctionHandler(fn),
				bounds,
				eventmask
				);
		}

		EventChain::Object & EventChain::Register( bool (*fn)(utils::Point location), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return Register(
				new LocationOnlyFunctionHandler(fn),
				bounds,
				eventmask
				);
		}

		EventChain::Object & EventChain::Register( bool (*fn)(), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return Register(
				new EmptyFunctionHandler(fn),
				bounds,
				eventmask
				);
		}

		template <class R_>
		EventChain::Object & EventChain::Register( R_ &object, bool (R_::*fn)(), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return Register(
				new EmptyClassHandler(object,fn),
				bounds,
				eventmask
				);
		}

		template <class R_>
		EventChain::Object & EventChain::Register( R_ &object, bool (R_::*fn)(utils::Point location), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return Register(
				new LocationOnlyClassHandler(object,fn),
				bounds,
				eventmask
				);
		}

		template <class R_>
		EventChain::Object & EventChain::Register( R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return Register(
				new NoAmountClassHandler(object,fn),
				bounds,
				eventmask
				);
		}

		template <class R_>
		EventChain::Object & EventChain::Register( R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return Register(
				new ClassHandler(object,fn),
				bounds,
				eventmask
				);
		}

		template <class R_>
		EventChain::Object & EventChain::Register( R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount, utils::Any data), utils::Bounds bounds, utils::Any data, Event::Type eventmask/*=AllUsed*/ )
		{
			return Register(
				new FullClassHandler(object,fn,data),
				bounds,
				eventmask
				);
		}


		EventCallback::Object & EventCallback::Set( bool (*fn)(Event::Type event, utils::Point location, int amount, utils::Any data), utils::Any data, Event::Type eventmask/*=AllUsed*/ )
		{
			return Set(
				new FullFunctionHandler(fn,data),
				
				eventmask
				);
		}

		EventCallback::Object & EventCallback::Set( bool (*fn)(Event::Type event, utils::Point location, int amount), Event::Type eventmask/*=AllUsed*/ )
		{
			return Set(
				new FunctionHandler(fn),
				
				eventmask
				);
		}

		EventCallback::Object & EventCallback::Set( bool (*fn)(Event::Type event, utils::Point location), Event::Type eventmask/*=AllUsed*/ )
		{
			return Set(
				new NoAmountFunctionHandler(fn),
				
				eventmask
				);
		}

		EventCallback::Object & EventCallback::Set( bool (*fn)(utils::Point location), Event::Type eventmask/*=AllUsed*/ )
		{
			return Set(
				new LocationOnlyFunctionHandler(fn),
				
				eventmask
				);
		}

		EventCallback::Object & EventCallback::Set( bool (*fn)(), Event::Type eventmask/*=AllUsed*/ )
		{
			return Set(
				new EmptyFunctionHandler(fn),
				
				eventmask
				);
		}

		template <class R_>
		EventCallback::Object & EventCallback::Set( R_ &object, bool (R_::*fn)(), Event::Type eventmask/*=AllUsed*/ )
		{
			return Set(
				new EmptyClassHandler(object,fn),
				
				eventmask
				);
		}

		template <class R_>
		EventCallback::Object & EventCallback::Set( R_ &object, bool (R_::*fn)(utils::Point location), Event::Type eventmask/*=AllUsed*/ )
		{
			return Set(
				new LocationOnlyClassHandler(object,fn),
				
				eventmask
				);
		}

		template <class R_>
		EventCallback::Object & EventCallback::Set( R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location), Event::Type eventmask/*=AllUsed*/ )
		{
			return Set(
				new NoAmountClassHandler(object,fn),
				
				eventmask
				);
		}

		template <class R_>
		EventCallback::Object & EventCallback::Set( R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount), Event::Type eventmask/*=AllUsed*/ )
		{
			return Set(
				new ClassHandler(object,fn),
				
				eventmask
				);
		}

		template <class R_>
		EventCallback::Object & EventCallback::Set( R_ &object, bool (R_::*fn)(Event::Type event, utils::Point location, int amount, utils::Any data), utils::Any data, Event::Type eventmask/*=AllUsed*/ )
		{
			return Set(
				new FullClassHandler(object,fn,data),
				
				eventmask
				);
		}


		void EventChain::Object::Remove() {
			if(PressedObject==this)
				PressedObject=NULL;

			if(HoveredObject==this)
				HoveredObject=NULL;

			wrapper->Delete();
		}

		bool Event::Target::Fire(Event::Type event, utils::Point location, int amount )	{
			if(!handler) 
				return false;

			if((EventMask&event) != event)
				return false;

			return handler->Fire(event, location, amount);
		}

		Event::Target::~Target() {
			delete handler;
		}

		EventChain::Object::~Object() {
		}

		EventCallback::Object::~Object() {
		}

	}


	void Initialize() {

	}
	
	namespace system {
		void ProcessChar(int Char) {
			keyboard::Event params;
			params.keycode=Char;
			params.event=keyboard::Event::Char;

			keyboard::Events(params);
		}

		void ProcessKeyDown(int Key) {
			keyboard::Event params;
			params.keycode=Key;
			params.event=keyboard::Event::Down;

			keyboard::Events.TokenList[params]=keyboard::Events(params);
			
		}

		void ProcessKeyUp(int Key) {
			keyboard::Event params;
			params.keycode=Key;
			params.event=keyboard::Event::Down;

			keyboard::Events.Fire(keyboard::Events.TokenList[params], keyboard::Event(keyboard::Event::Up, Key));
			keyboard::Events.TokenList[params]=keyboard::Events.NullToken;
		}




		bool hoverfound=false;

		void ProcessMousePosition(os::WindowHandle Window) {
			mouse::CurrentPoint=os::input::getMousePosition(Window);

			hoverfound=false;
			Main.PropagateMouseEvent(mouse::Event::Move, mouse::CurrentPoint, 0);

			if(!hoverfound && mouse::HoveredObject && !mouse::PressedObject) {
				mouse::HoveredObject->Fire(mouse::Event::Out, mouse::CurrentPoint, 0);
				mouse::HoveredObject=NULL;
			}
		}

		void ProcessMouseClick(mouse::Event::Type button,int x,int y) {
			if(mouse::PressedPoint.Distance(utils::Point(x,y))<mouse::DragDistance)
				Main.PropagateMouseEvent(mouse::Event::Click | button, utils::Point(x,y), 0);

			mouse::PressedObject=NULL;
		}

		void ProcessMouseDown(mouse::Event::Type button,int x,int y) {
			mouse::PressedButtons = mouse::PressedButtons | button;
			mouse::PressedPoint   = utils::Point(x,y);

			Main.PropagateMouseEvent(mouse::Event::Down | button, utils::Point(x,y), 0);
		}

		void ProcessMouseUp(mouse::Event::Type button,int x,int y){
			mouse::PressedButtons = mouse::PressedButtons & ~button;
			Main.PropagateMouseEvent(mouse::Event::Up | button, utils::Point(x,y), 0);
		}

		void ProcessMouseDblClick(mouse::Event::Type button,int x,int y){
			Main.PropagateMouseEvent(mouse::Event::DoubleClick | button, utils::Point(x,y), 0);
		}

		void ProcessVScroll(int amount,int x,int y){
			Main.PropagateMouseEvent(mouse::Event::VScroll, utils::Point(x,y), amount);
		}

		void ProcessHScroll(int amount,int x,int y){
			Main.PropagateMouseEvent(mouse::Event::HScroll, utils::Point(x,y), amount);
		}
	}

} }
