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
		Event::Target	*PressedObject	= NULL;

		utils::Point	PressedPoint	= utils::Point(0,0);
		utils::Point	CurrentPoint	= utils::Point(0,0);

		const int DragDistance			= 5;//px

		bool EventProvider::PropagateMouseEvent(Event::Type event, utils::Point location, int amount) {
			if(Event::isClick(event)) {
				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.First();i.isValid();i.Next()) {
					if((!PressedObject || &(*i)==PressedObject) && i->Bounds.isInside(location)) {
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
			else if(event==Event::Over) {
				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.Last();i.isValid();i.Previous()) {
					bool isover=false;
					if(i->Bounds.isInside(location)) {
						if(i->EventMask & Event::OverCheck) {
							if(i->Fire(Event::OverCheck, location-i->Bounds.TopLeft(), 0))
								isover=true;
						}
						else
							isover=true;
					}

					if(!i->IsOver && isover) {
						bool ret=i->Fire(Event::Over, location-i->Bounds.TopLeft(), amount);

						i->IsOver=ret;

						if(ret)
							return true;
					}
				}

				return false;
			}
			else if(event==Event::Out) {
				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.First();i.isValid();i.Next()) {
					bool isover=false;
					if(i->Bounds.isInside(location)) {
						if(i->EventMask & Event::OverCheck) {
							if(i->Fire(Event::OverCheck, location-i->Bounds.TopLeft(), 0))
								isover=true;
						}
						else
							isover=true;
					}

					if(amount==0)
						isover=false;

					if(i->IsOver && !isover) {
						i->Fire(Event::Out, location-i->Bounds.TopLeft(), amount);
						i->IsOver=false;
					}
				}

				return false;
			}
			else { //Scrolls, double click, mouse move
				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.First();i.isValid();i.Next()) {
					if(i->Bounds.isInside(location)) {
						if(i->Fire(event, location-i->Bounds.TopLeft(), amount))
							return true;
					}
				}

				return false;
			}
		}

		EventProvider::~EventProvider() {
			for(utils::SortedCollection<EventChain::Object>::Iterator it=MouseEvents.Events.First();it.isValid();it.Next())
				if(&(*it)==PressedObject)
					PressedObject=NULL;

			MouseEvents.Events.Destroy();
		}

		bool CallbackProvider::PropagateMouseEvent(Event::Type event, utils::Point location, int amount) {
			if(!MouseCallback) return false;

			if(Event::isClick(event)) {
				if(MouseCallback.object==PressedObject) {
					if(MouseCallback.object->Fire(event, location, amount))
						return true;
				}

				return false;
			}
			else if(Event::isDown(event)) {
				if(MouseCallback.object->Fire(event, location, amount)) {
					mouse::PressedObject=MouseCallback.object;
					return true;
				}

				return false;
			}
			else if(event==Event::Over) {
				bool isover=true;
				if(MouseCallback.object->EventMask & Event::OverCheck) {
					if(!MouseCallback.object->Fire(Event::OverCheck, location, 0))
						isover=false;
				}

				if(!MouseCallback.object->IsOver && isover) {
					bool ret=MouseCallback.object->Fire(Event::Over, location, amount);

					MouseCallback.object->IsOver=ret;

					if(ret)
						return true;
				}

				return false;
			}
			else if(event==Event::Out) {
				bool isover=true;
				if(MouseCallback.object->EventMask & Event::OverCheck) {
					if(!MouseCallback.object->Fire(Event::OverCheck, location, 0))
						isover=false;
				}

				if(amount==0)
					isover=false;

				if(MouseCallback.object->IsOver && !isover) {
					MouseCallback.object->Fire(Event::Out, location, amount);
					MouseCallback.object->IsOver=false;
				}

				return false;
			}
			else { //Scrolls, double click, mouse move
				if(MouseCallback.object->Fire(event, location, amount))
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

		void EventChain::Unregister( Object &obj ) {
			obj.Remove();
		}

		void EventChain::Unregister( Object *obj ) {
			obj->Remove();
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


		void EventChain::Object::Remove() {
			if(PressedObject==this)
				PressedObject=NULL;

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
			Main.PropagateMouseEvent(mouse::Event::Over, mouse::CurrentPoint, 0);
			Main.PropagateMouseEvent(mouse::Event::Move, mouse::CurrentPoint, 0);
			Main.PropagateMouseEvent(mouse::Event::Out , mouse::CurrentPoint, 1);

			//if(!hoverfound && mouse::HoveredObject && !mouse::PressedObject) {
			//	mouse::HoveredObject->Fire(mouse::Event::Out, mouse::CurrentPoint, 0);
			//	mouse::HoveredObject=NULL;
			//}
		}

		void ProcessMouseClick(mouse::Event::Type button,int x,int y) {
			if(mouse::PressedPoint.Distance(utils::Point(x,y))<mouse::DragDistance)
				Main.PropagateMouseEvent(mouse::Event::Click | button, utils::Point(x,y), 0);

			if(mouse::PressedButtons==mouse::Event::None)
				mouse::PressedObject=NULL;
		}

		void ProcessMouseDown(mouse::Event::Type button,int x,int y) {
			mouse::PressedButtons = mouse::PressedButtons | button;
			mouse::PressedPoint   = utils::Point(x,y);

			if(mouse::PressedObject)
				mouse::PressedObject->Fire(mouse::Event::Down | button, utils::Point(x,y), 0);
			else
				Main.PropagateMouseEvent(mouse::Event::Down | button, utils::Point(x,y), 0);
		}

		void ProcessMouseUp(mouse::Event::Type button,int x,int y){
			if(button==mouse::Event::Left)
				button=button;
			mouse::PressedButtons = mouse::PressedButtons & ~button;
			if(mouse::PressedObject)
				mouse::PressedObject->Fire(mouse::Event::Up | button, utils::Point(x,y), 0);

			//Main.PropagateMouseEvent(mouse::Event::Up | button, utils::Point(x,y), 0);
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
