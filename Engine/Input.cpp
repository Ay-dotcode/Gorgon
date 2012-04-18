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

		bool HasDragTarget() { return system::dragtarget!=NULL;  }
		
		const int DragDistance			= 5;//px


		bool EventProvider::PropagateMouseEvent(Event::Type event, utils::Point location, int amount) {
			if(Event::isClick(event)) {
				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.First();i.IsValid();i.Next()) {
					if((!PressedObject || &(*i)==PressedObject) && i->Bounds.isInside(location)) {
						if(i->Fire(event, location-i->Bounds.TopLeft(), amount))
							return true;
					}
				}

				return false;
			}
			else if(Event::isDown(event)) {
				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.First();i.IsValid();i.Next()) {
					if(i->Bounds.isInside(location)) {
						if(i->Fire(event, location-i->Bounds.TopLeft(), amount)) {
							mouse::PressedObject=&(*i);
							return true;
						}
					}
				}

				return false;
			}
			else if(event==Event::Over || event==Event::DragOver) {
				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.Last();i.IsValid();i.Previous()) {
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
						bool ret=i->Fire(event, location-i->Bounds.TopLeft(), amount);

						i->IsOver=ret;

						if(ret) {
							if(event==Event::DragOver) 
								system::dragtarget=&(*i);

							return true;
						}
					}
				}

				return false;
			}
			else if(event==Event::Out || event==Event::DragOut) {
				bool ret=false;

				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.First();i.IsValid();i.Next()) {
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

					if(isover)
						return true;

					if(i->IsOver && !isover) {
						i->Fire(event, location-i->Bounds.TopLeft(), amount);
						i->IsOver=false;

						system::dragtarget=NULL;
					}
				}

				return false;
			}
			else if(event==Event::Move && PressedObject) {
				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.First();i.IsValid();i.Next()) {
					if(PressedObject==&(*i)) {
						if(i->Fire(event, location-i->Bounds.TopLeft(), amount))
							return true;
					}
				}

				return false;
			}
			else if(event==Event::DragMove) {
				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.First();i.IsValid();i.Next()) {
					if(system::dragtarget==&(*i) || system::dragsource==&(*i)) {
						if(i->Fire(event, location-i->Bounds.TopLeft(), amount)) {
							system::dragtarget=NULL;
						}
					}
				}

				return false;
			}
			else { //Scrolls, double click
				for(utils::SortedCollection<EventChain::Object>::Iterator i = this->MouseEvents.Events.First();i.IsValid();i.Next()) {
					if(i->Bounds.isInside(location)) {
						if(i->Fire(event, location-i->Bounds.TopLeft(), amount))
							return true;
					}
				}

				return false;
			}
		}

		EventProvider::~EventProvider() {
			for(utils::SortedCollection<EventChain::Object>::Iterator it=MouseEvents.Events.First();it.IsValid();it.Next())
				if(&(*it)==PressedObject)
					PressedObject=NULL;

			MouseEvents.Events.Destroy();
		}

		bool CallbackProvider::PropagateMouseEvent(Event::Type event, utils::Point location, int amount) {
			if(!MouseCallback) return false;

			if(Event::isClick(event)) {
				if(MouseCallback.object==PressedObject || !PressedObject) {
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
			else if(event==Event::Over || event==Event::DragOver) {
				bool isover=true;
				if(MouseCallback.object->EventMask & Event::OverCheck) {
					if(!MouseCallback.object->Fire(Event::OverCheck, location, 0))
						isover=false;
				}

				if(MouseCallback.object->IsOver)
					return true;

				if(isover) {
					bool ret=MouseCallback.object->Fire(event, location, amount);

					MouseCallback.object->IsOver=ret;

					if(ret) {
						if(event==Event::DragOver)
							system::dragtarget=MouseCallback.object;

						return true;
					}
				}

				return false;
			}
			else if(event==Event::Out || event==Event::DragOut) {
				bool isover=true;
				if(MouseCallback.object->EventMask & Event::OverCheck) {
					if(!MouseCallback.object->Fire(Event::OverCheck, location, 0))
						isover=false;
				}

				if(amount==0)
					isover=false;

				if(isover)
					return true;

				if(MouseCallback.object->IsOver && !isover) {
					MouseCallback.object->Fire(event, location, amount);
					MouseCallback.object->IsOver=false;
					system::dragtarget=NULL;
				}

				return false;
			}
			else if(event==Event::Move && PressedObject) {
				if(PressedObject==MouseCallback.object) {
					if(MouseCallback.object->Fire(event, location, amount))
						return true;
				}

				return false;
			}
			else if(event==Event::DragMove) {
				if(system::dragtarget==MouseCallback.object || system::dragsource==MouseCallback.object) {
					if(!MouseCallback.object->Fire(event, location, amount)) {
						system::dragtarget=NULL;
					}
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
			return registr(
				new FullFunctionHandler(fn,data),
				bounds,
				eventmask
				);
		}

		EventChain::Object & EventChain::Register( bool (*fn)(Event::Type event, utils::Point location, int amount), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new FunctionHandler(fn),
				bounds,
				eventmask
				);
		}

		EventChain::Object & EventChain::Register( bool (*fn)(Event::Type event, utils::Point location), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new NoAmountFunctionHandler(fn),
				bounds,
				eventmask
				);
		}

		EventChain::Object & EventChain::Register( bool (*fn)(utils::Point location), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
				new LocationOnlyFunctionHandler(fn),
				bounds,
				eventmask
				);
		}

		EventChain::Object & EventChain::Register( bool (*fn)(), utils::Bounds bounds, Event::Type eventmask/*=AllUsed*/ )
		{
			return registr(
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
			return set(
				new FullFunctionHandler(fn,data),
				
				eventmask
				);
		}

		EventCallback::Object & EventCallback::Set( bool (*fn)(Event::Type event, utils::Point location, int amount), Event::Type eventmask/*=AllUsed*/ )
		{
			return set(
				new FunctionHandler(fn),
				
				eventmask
				);
		}

		EventCallback::Object & EventCallback::Set( bool (*fn)(Event::Type event, utils::Point location), Event::Type eventmask/*=AllUsed*/ )
		{
			return set(
				new NoAmountFunctionHandler(fn),
				
				eventmask
				);
		}

		EventCallback::Object & EventCallback::Set( bool (*fn)(utils::Point location), Event::Type eventmask/*=AllUsed*/ )
		{
			return set(
				new LocationOnlyFunctionHandler(fn),
				
				eventmask
				);
		}

		EventCallback::Object & EventCallback::Set( bool (*fn)(), Event::Type eventmask/*=AllUsed*/ )
		{
			return set(
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



		void BeginDrag(IDragData &object, mouse::Event::Target &source) {
			system::draggedobject=&object;
			system::dragsource=&source;
			system::isdragging=true;

			DragStateChanged();
			system::ProcessMousePosition(gge::Main.getWindow());
		}

		void CancelDrag() {
			if(system::dragsource)
				system::dragsource->Fire(mouse::Event::DragCanceled, utils::Point(0,0), 0);

			if(system::dragtarget)
				system::dragtarget->Fire(mouse::Event::DragOut, utils::Point(0,0), 0);

			system::dragsource=NULL;
			system::isdragging=false;
			system::dragtarget=NULL;

			DragStateChanged();
		}

		utils::EventChain<> DragStateChanged;

		IDragData &GetDraggedObject() {
			if(!system::draggedobject)
				throw std::runtime_error("No object is being dragged.");

			return *system::draggedobject;
		}

		bool IsDragging() {
			return system::dragsource!=NULL;
		}

		void DropDrag() {
			if(system::isdragging) {
				bool handled=system::dragtarget->Fire(mouse::Event::DragDrop, utils::Point(0,0), 0);
				if(handled)
					system::dragsource->Fire(mouse::Event::DragAccepted, utils::Point(0,0), 0);
				else
					system::dragsource->Fire(mouse::Event::DragCanceled, utils::Point(0,0), 0);

				mouse::DragStateChanged();

				system::dragsource=NULL;
				system::isdragging=false;
				system::dragtarget=NULL;
			}
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

			if(keyboard::Events.TokenList[params]!=keyboard::Events.NullToken)
				keyboard::Events.Fire(keyboard::Events.TokenList[params], keyboard::Event(keyboard::Event::Up, Key));
			else
				keyboard::Events.Fire(keyboard::Event(keyboard::Event::Up, Key));
			keyboard::Events.TokenList[params]=keyboard::Events.NullToken;
		}




		bool hoverfound=false;
		bool isdragging=false;
		mouse::IDragData *draggedobject=NULL;
		
		mouse::Event::Target *dragsource=NULL;

		mouse::Event::Target *dragtarget=NULL;

		void ProcessMousePosition(os::WindowHandle Window) {
			mouse::CurrentPoint=os::input::getMousePosition(Window);

			if(isdragging) {
				Main.PropagateMouseEvent(mouse::Event::DragOver, mouse::CurrentPoint, 0);
				Main.PropagateMouseEvent(mouse::Event::DragMove, mouse::CurrentPoint, 0);
				Main.PropagateMouseEvent(mouse::Event::DragOut , mouse::CurrentPoint, 1);
			}
			else {
				hoverfound=false;
				Main.PropagateMouseEvent(mouse::Event::Over, mouse::CurrentPoint, 0);
				Main.PropagateMouseEvent(mouse::Event::Move, mouse::CurrentPoint, 0);
				Main.PropagateMouseEvent(mouse::Event::Out , mouse::CurrentPoint, 1);
			}

			//if(!hoverfound && mouse::HoveredObject && !mouse::PressedObject) {
			//	mouse::HoveredObject->Fire(mouse::Event::Out, mouse::CurrentPoint, 0);
			//	mouse::HoveredObject=NULL;
			//}
		}

		void ProcessMouseClick(mouse::Event::Type button,int x,int y) {
			if(mouse::PressedPoint.Distance(utils::Point(x,y))<mouse::DragDistance) {
				Main.PropagateMouseEvent(mouse::Event::Click | button, utils::Point(x,y), 0);
			}
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
			if(isdragging) {
				bool handled=Main.PropagateMouseEvent(mouse::Event::DragDrop, utils::Point(x,y), 0);
				if(handled)
					dragsource->Fire(mouse::Event::DragAccepted, utils::Point(x,y), 0);
				else
					dragsource->Fire(mouse::Event::DragCanceled, utils::Point(x,y), 0);

				mouse::DragStateChanged();

				dragsource=NULL;
				isdragging=false;
				dragtarget=NULL;
			}

			mouse::PressedButtons = mouse::PressedButtons & ~button;
			if(mouse::PressedObject) {
				mouse::PressedObject->Fire(mouse::Event::Up | button, utils::Point(x,y), 0);
				mouse::PressedObject=NULL;
			}

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
