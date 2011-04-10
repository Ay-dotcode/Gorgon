#include "Input.h"

using namespace gge;
using namespace utils;

namespace gge { namespace input {

	bool KeyboardModifier::isAlternate=false;
	KeyboardModifier::Type KeyboardModifier::Current=KeyboardModifier::None;

	LinkedList<BasicPointerTarget> PointerTargets;
	MouseEventObject *hoveredObject=NULL;
	MouseEventObject *pressedObject=NULL;
	MouseEventType MouseButtons=MOUSE_EVENT_NONE;
	utils::ConsumableEvent<utils::Empty, KeyboardEvent> KeyboardEvents;

	bool hoverfound=false;

	void Initialize() {

	}

	LinkedListItem<BasicPointerTarget> *AddPointerTarget(BasicPointerTarget *target, int order) {
		return PointerTargets.AddItem(target,order);
	}

	void RemovePointerTarget(LinkedListItem<BasicPointerTarget> * target) {
		PointerTargets.Remove(target);
	}

	////This internal function propagates an event to all top level targets
	void propagateevent(MouseEventType event, int x, int y) {
		LinkedListOrderedIterator<BasicPointerTarget> it=PointerTargets;
		BasicPointerTarget *target;

		while(target=it) {
			if(target->PropagateMouseEvent(event, x, y, NULL))
				break;
		}
	}

	////This internal function propagates scroll event to all top level targets
	void propagatescrollevent(int amount, MouseEventType event, int x, int y) {
		LinkedListOrderedIterator<BasicPointerTarget> it=PointerTargets;
		BasicPointerTarget *target;

		while(target=it) {
			if(target->PropagateMouseScrollEvent(amount, event, x, y, NULL))
				break;
		}
	}

	namespace system {
		void ProcessMousePosition(os::WindowHandle Window) {
			Point pnt;
			pnt=os::input::getMousePosition(Window);

			int x=pnt.x;
			int y=pnt.y;

			hoverfound=false;
			propagateevent(MOUSE_EVENT_MOVE, x, y);
			if(!hoverfound && hoveredObject) {
				if(hoveredObject->out)
					hoveredObject->out(MOUSE_EVENT_OUT, x, y, hoveredObject->data);
				hoveredObject=NULL;
			}
		}

		void ProcessMouseClick(int button,int x,int y) {
			MouseEventType event;
			if(button==1)
				event=MOUSE_EVENT_LCLICK;
			if(button==2)
				event=MOUSE_EVENT_RCLICK;
			if(button==4)
				event=MOUSE_EVENT_MCLICK;
			if(button==101)
				event=MOUSE_EVENT_X1CLICK;
			if(button==102)
				event=MOUSE_EVENT_X2CLICK;


			propagateevent(event, x, y);
			pressedObject=NULL;
		}

		void ProcessMouseDown(int button,int x,int y) {

			MouseButtons=(MouseEventType)(MouseButtons | button);

			MouseEventType event;
			if(button==1)
				event=MOUSE_EVENT_LDOWN;
			if(button==2)
				event=MOUSE_EVENT_RDOWN;
			if(button==4)
				event=MOUSE_EVENT_MDOWN;
			if(button==101)
				event=MOUSE_EVENT_X1DOWN;
			if(button==102)
				event=MOUSE_EVENT_X2DOWN;

			propagateevent(event, x, y);
		}

		void ProcessMouseUp(int button,int x,int y){

			MouseButtons=(MouseEventType)(MouseButtons & ~button);

			MouseEventType event;
			if(button==1)
				event=MOUSE_EVENT_LUP;
			if(button==2)
				event=MOUSE_EVENT_RUP;
			if(button==4)
				event=MOUSE_EVENT_MUP;
			if(button==101)
				event=MOUSE_EVENT_X1UP;
			if(button==102)
				event=MOUSE_EVENT_X2UP;

			propagateevent(event, x, y);
		}

		void ProcessMouseDblClick(int button,int x,int y){
			MouseEventType event;
			if(button==1)
				event=MOUSE_EVENT_LDBLCLICK;
			if(button==2)
				event=MOUSE_EVENT_RDBLCLICK;
			if(button==4)
				event=MOUSE_EVENT_MDBLCLICK;
			if(button==101)
				event=MOUSE_EVENT_X1DBLCLICK;
			if(button==102)
				event=MOUSE_EVENT_X2DBLCLICK;

			propagateevent(event, x, y);
		}

		void ProcessVScroll(int amount,int x,int y){
			propagatescrollevent(amount, MOUSE_EVENT_VSCROLLL, x, y);
		}

		void ProcessHScroll(int amount,int x,int y){
			propagatescrollevent(amount, MOUSE_EVENT_HSCROLLL, x, y);
		}


		void ProcessChar(int Char) {
			KeyboardEvent params;
			params.keycode=Char;
			params.event=KeyboardEvent::Char;

			KeyboardEvents(params);
		}

		void ProcessKeyDown(int Key) {
			KeyboardEvent params;
			params.keycode=Key;
			params.event=KeyboardEvent::Down;

			KeyboardEvents(params);
		}

		void ProcessKeyUp(int Key) {
			KeyboardEvent params;
			params.keycode=Key;
			params.event=KeyboardEvent::Up;

			KeyboardEvents(params);
		}

	}

	MouseEventToken BasicPointerTarget::RegisterMouseEvent(Bounds bounds, void *data, 
		MouseEvent click, MouseEvent over, MouseEvent out,
		MouseEvent move , MouseEvent down, MouseEvent up , MouseEvent doubleclick) {

			MouseEventObject *meo=new MouseEventObject;

			meo->bounds=bounds;
			meo->data=data;
			meo->Enabled=true;
			meo->click=click;
			meo->over=over;
			meo->out=out;
			meo->move=move;
			meo->down=down;
			meo->up=up;
			meo->dblclick=doubleclick;
			meo->parent=this;
			meo->checkover=NULL;
			meo->vscroll=NULL;
			meo->hscroll=NULL;

			MouseEventToken token=mouseevents.AddItem(meo, 0);

			return token;
	}

	void BasicPointerTarget::DisableMouseEvent(MouseEventToken token) {
		token->Item->Enabled=false;
	}

	void BasicPointerTarget::EnableMouseEvent(MouseEventToken token) {
		token->Item->Enabled=true;
	}

	void BasicPointerTarget::RemoveMouseEvent(MouseEventToken token) {
		delete token->Item;
		token->Item=NULL;

		mouseevents.Delete((MouseEventObject*)token);
	}

	bool BasicPointerTarget::PropagateMouseClickEvent(MouseEventType event, int x, int y, void *data) {
		bool ret=false;

		LinkedListOrderedIterator<MouseEventObject> it=mouseevents;
		MouseEventObject *object;
		while(object=it) {
			if(object->click && isinbounds(x,y,object->bounds) && object->Enabled) {

				MouseEventObject *obj=pressedObject;
				if(pressedObject==NULL || pressedObject==object) {
					if(object->click(event, x, y, object->data))
						ret=true;
				}
			}
		}

		return ret;
	}

	bool BasicPointerTarget::PropagateMouseDownEvent(MouseEventType event, int x, int y, void *data) {
		LinkedListOrderedIterator<MouseEventObject> it=mouseevents;
		MouseEventObject *object;
		while(object=it) {
			if(object->down && isinbounds(x,y,object->bounds) && object->Enabled) {
				if(object->down(event, x, y, object->data)) {
					pressedObject=object;
					return true;				
				}
			}
		}

		return false;
	}

	bool BasicPointerTarget::PropagateMouseUpEvent(MouseEventType event, int x, int y, void *data) {
		LinkedListOrderedIterator<MouseEventObject> it=mouseevents;
		MouseEventObject *object;
		while(object=it) {
			if(object->up && object==pressedObject) {
				if(object->up(event, x, y, object->data))
					return true;		
			}
		}

		return false;
	}

	bool BasicPointerTarget::PropagateMouseDblClickEvent(MouseEventType event, int x, int y, void *data) {
		LinkedListOrderedIterator<MouseEventObject> it=mouseevents;
		MouseEventObject *object;
		while(object=it) {
			if(object->dblclick && isinbounds(x,y,object->bounds) && object->Enabled) {
				if(object->dblclick(event, x, y, object->data))
					return true;				
			}
		}

		return false;
	}

	bool BasicPointerTarget::PropagateMouseHScrollEvent(int amount, MouseEventType event, int x, int y, void *data) {
		LinkedListOrderedIterator<MouseEventObject> it=mouseevents;
		MouseEventObject *object;
		while(object=it) {
			if(object->hscroll && isinbounds(x,y,object->bounds) && object->Enabled) {
				if(object->hscroll(amount, event, x, y, object->data))
					return true;				
			}
		}

		return false;
	}

	bool BasicPointerTarget::PropagateMouseVScrollEvent(int amount, MouseEventType event, int x, int y, void *data) {
		LinkedListOrderedIterator<MouseEventObject> it=mouseevents;
		MouseEventObject *object;
		while(object=it) {
			if(object->vscroll && isinbounds(x,y,object->bounds) && object->Enabled) {
				if(object->vscroll(amount, event, x, y, object->data))
					return true;				
			}
		}

		return false;
	}

	bool BasicPointerTarget::PropagateMouseOverEvent(MouseEventType event, int x, int y, void *data) {
		LinkedListOrderedIterator<MouseEventObject> it=mouseevents;
		MouseEventObject *object;
		while(object=it) {
			if(isinbounds(x,y,object->bounds) && object->Enabled) {
				if(object->over) {
					if(object->over(event, x, y, object->data))
						return true;
					else 
						return false;
				} else {
					return true;
				}
			}
		}

		return false;
	}

	bool BasicPointerTarget::PropagateMouseOutEvent(MouseEventType event, int x, int y, void *data) {
		LinkedListOrderedIterator<MouseEventObject> it=mouseevents;
		MouseEventObject *object;
		while(object=it) {
			if(object->out && isinbounds(x,y,object->bounds) && object->Enabled) {
				if(object->out(event, x, y, object->data))
					return true;				
			}
		}

		return false;
	}

	bool BasicPointerTarget::PropagateMouseMoveEvent(MouseEventType event, int x, int y, void *data) {
		LinkedListOrderedIterator<MouseEventObject> it=mouseevents;
		MouseEventObject *object;
		while(object=it) {
			if(isinbounds(x,y,object->bounds) && object->Enabled && !hoverfound) {
				if(object!=hoveredObject) {
					if(object->over) {
						bool isowned=true;
						if(object->checkover)
							isowned=object->checkover(MOUSE_EVENT_OVER_CHECK, x, y, hoveredObject->data);
						if(isowned) {
							if(hoveredObject)
								if(hoveredObject->out)
									hoveredObject->out(MOUSE_EVENT_OUT, x, y, hoveredObject->data);
							if(object->over(MOUSE_EVENT_OVER, x, y, object->data)) {

								hoveredObject=object;
								hoverfound=true;
								return true;
							}
						}
					}
				} else 
					hoverfound=true;
			}

			if(object->move && isinbounds(x,y,object->bounds) && !pressedObject) {
				if(object->move(event, x, y, object->data))
					return true;				
			}

			if(object->move && pressedObject==object) {
				if(object->move(event, x, y, object->data))
					return true;				
			}
		}

		return false;
	}

	bool BasicPointerTarget::PropagateMouseEvent(MouseEventType event, int x, int y, void *data) {
		if(event == MOUSE_EVENT_MOVE)
			return PropagateMouseMoveEvent(event, x, y, data);
		else if(event & MOUSE_EVENT_UP) 
			return PropagateMouseUpEvent(event, x, y, data);
		else if(event & MOUSE_EVENT_DOWN)
			return PropagateMouseDownEvent(event, x, y, data);
		else if(event & MOUSE_EVENT_DBLCLICK)
			return PropagateMouseDblClickEvent(event, x, y, data);
		else
			return PropagateMouseClickEvent(event, x, y, data);
	}

	bool BasicPointerTarget::PropagateMouseScrollEvent(int amount, MouseEventType event, int x, int y, void *data) {
		if(event == MOUSE_EVENT_VSCROLLL)
			return PropagateMouseVScrollEvent(amount, event, x, y, data);
		else if(event == MOUSE_EVENT_HSCROLLL)
			return PropagateMouseHScrollEvent(amount, event, x, y, data);

		return false;
	}
} }
