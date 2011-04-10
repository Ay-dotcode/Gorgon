#pragma once

#include "../Utils/LinkedList.h"
#include "../Utils/Collection.h"
#include "../Utils/GGE.h"
#include "../Utils/Utils.h"

#include "OS.h"
#include "../Utils/ConsumableEvent.h"

namespace gge { namespace input {
	class BasicPointerTarget;

	class KeyboardModifier {
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
		KeyboardModifier();
	};

	////Bitwise OR operation on KeyboardModifier enumeration
	inline KeyboardModifier::Type operator | (KeyboardModifier::Type m1, KeyboardModifier::Type m2) {
		return KeyboardModifier::Type( m1|m2 );
	}

	////Bitwise AND operation on KeyboardModifier enumeration
	inline KeyboardModifier::Type operator & (KeyboardModifier::Type m1, KeyboardModifier::Type m2) {
		return KeyboardModifier::Type( m1&m2 );
	}

	////Bitwise EQUAL OR operation on KeyboardModifier enumeration
	inline KeyboardModifier::Type operator |= (KeyboardModifier::Type m1, KeyboardModifier::Type m2) {
		return KeyboardModifier::Type( m1|m2 );
	}

	////Bitwise EQUAL AND operation on KeyboardModifier enumeration
	inline KeyboardModifier::Type operator &= (KeyboardModifier::Type m1, KeyboardModifier::Type m2) {
		return KeyboardModifier::Type( m1&m2 );
	}

	class KeyCodes {
	public:
		static const int Shift;
		static const int Control;
		static const int Alt;
		static const int Super;

		//!TODO: list all keycode except ASCII ones
/*		static const int Home;
		static const int End;
		static const int PageUp;
		static const int PageDown;
		static const int Left;
		static const int Top;
		static const int Right;
		static const int Bottom;
	*/
	};



	////Types of mouse event
	enum MouseEventType {
		MOUSE_EVENT_NONE=0,
		MOUSE_EVENT_LEFT=1,
		MOUSE_EVENT_RIGHT=2,
		MOUSE_EVENT_MIDDLE=4,

		MOUSE_EVENT_CLICK=0,
		////Left click
		MOUSE_EVENT_LCLICK=1,
		////Right click
		MOUSE_EVENT_RCLICK=2,
		////Middle click
		MOUSE_EVENT_MCLICK=4,

		MOUSE_EVENT_DOWN=8,
		////Left button down
		MOUSE_EVENT_LDOWN=9,
		////Right button down
		MOUSE_EVENT_RDOWN=10,
		////Middle button down
		MOUSE_EVENT_MDOWN=12,

		MOUSE_EVENT_UP=16,
		////Left button up
		MOUSE_EVENT_LUP=17,
		////Right button up
		MOUSE_EVENT_RUP=18,
		////Middle button up
		MOUSE_EVENT_MUP=20,

		MOUSE_EVENT_DBLCLICK=32,
		////Left button double click
		MOUSE_EVENT_LDBLCLICK=33,
		////Right button double click
		MOUSE_EVENT_RDBLCLICK=34,
		////Middle button double click
		MOUSE_EVENT_MDBLCLICK=36,

		////Mouse is over the target
		MOUSE_EVENT_OVER,
		////Mouse is out of the target
		MOUSE_EVENT_OUT,
		////Mouse moves within the target
		MOUSE_EVENT_MOVE,
		////Check if the mouse is over the target
		/// application should not perform any mouse over
		/// operation if the event is received
		MOUSE_EVENT_OVER_CHECK,

		////Vertical mouse scrolling is used
		MOUSE_EVENT_VSCROLLL,
		////Horizontal mouse scrolling is used
		MOUSE_EVENT_HSCROLLL,
		
		
		MOUSE_EVENT_X1=64,
		MOUSE_EVENT_X1CLICK=MOUSE_EVENT_X1 | MOUSE_EVENT_CLICK,
		MOUSE_EVENT_X1DOWN=MOUSE_EVENT_X1 | MOUSE_EVENT_DOWN,
		MOUSE_EVENT_X1UP=MOUSE_EVENT_X1 | MOUSE_EVENT_DOWN,
		MOUSE_EVENT_X1DBLCLICK=MOUSE_EVENT_X1 | MOUSE_EVENT_DBLCLICK,

		MOUSE_EVENT_X2=128,
		MOUSE_EVENT_X2CLICK=MOUSE_EVENT_X2 | MOUSE_EVENT_CLICK,
		MOUSE_EVENT_X2DOWN=MOUSE_EVENT_X2 | MOUSE_EVENT_DOWN,
		MOUSE_EVENT_X2UP=MOUSE_EVENT_X2 | MOUSE_EVENT_DOWN,
		MOUSE_EVENT_X2DBLCLICK=MOUSE_EVENT_X2 | MOUSE_EVENT_DBLCLICK,
		
	};

	////Types of keyboard events
	class KeyboardEvent {
	public:
		enum Type {
			////A character is typed (O/S controlled, i.e. repeating keys)
			Char,
			////A key is pressed
			Down,
			////A key is released
			Up
		} event;

		int keycode;
	};

	struct MouseEventObject;
	////Token given by registering a mouse event
	typedef	utils::LinkedListItem<MouseEventObject> (*MouseEventToken);
	////Defines how a mouse event handing function should be
	typedef bool (*MouseEvent)(MouseEventType event, int x, int y, void *data);
	////Defines how a mouse scroll event handing function should be
	typedef bool (*MouseScrollEvent)(int amount, MouseEventType event, int x, int y, void *data);

	////Mouse event object is used internally to keep track of mouse event handlers
	struct MouseEventObject {
		////Bounds of the event area, this variable is relative to the enclosing layer
		Bounds bounds;
		////Mouse over event handler
		MouseEvent over;
		////Mouse over event check handler
		MouseEvent checkover;
		////Mouse out event handler
		MouseEvent out;
		////Mouse click event handler
		MouseEvent click;
		////Mouse move event handler
		MouseEvent move;
		////Mouse down event handler
		MouseEvent down;
		////Mouse up event handler
		MouseEvent up;
		////Mouse double click event handler
		MouseEvent dblclick;
		////Vertical scroll event
		MouseScrollEvent vscroll;
		////Horizontal scroll event (don't rely on it most mice do not have horizontal scroll)
		MouseScrollEvent hscroll;
		////The token given by register mouse event
		MouseEventToken *token;
		BasicPointerTarget *parent;
		////Any data that is left to be passed to event handlers
		void *data;
		////Whether mouse is reported to be over this region
		bool isover;
		////Whether this mouse event is enabled
		bool Enabled;
	};
	extern MouseEventObject *pressedObject;


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
		///@button	: button number 1 for left, 2 for right and 4 for middle
		void ProcessMouseClick(int button, int x, int y);
		////Processes given mouse button as pressed
		///@button	: button number 1 for left, 2 for right and 4 for middle
		void ProcessMouseDown(int button,int x,int y);
		////Processes given mouse button as released
		///@button	: button number 1 for left, 2 for right and 4 for middle
		void ProcessMouseUp(int button,int x,int y);
		////Processes double click action
		///@button	: button number 1 for left, 2 for right and 4 for middle
		void ProcessMouseDblClick(int button,int x,int y);
		////Processes vertical scroll
		void ProcessVScroll(int amount,int x,int y);
		////Processes horizontal scroll
		void ProcessHScroll(int amount,int x,int y);
	}

	////This interface defines a class that can be used
	/// as a common target of mouse events
	class BasicPointerTarget {
	public:
		////The array of mouse events
		utils::LinkedList<MouseEventObject> mouseevents;

		////This function propagates the mouse event to mouse events
		/// registered for this target if any of one of the targets is
		/// the receiver of the event function will return true and 
		/// propagation will terminate, this function is responsible
		/// to determine who will receive the event
		///@event	: the type of the mouse event
		///@x		: Position of the event	
		///@y		: Position of the event	
		///@data	: Always null, used to preserve compatibility
		virtual bool PropagateMouseEvent(MouseEventType event, int x, int y, void *data);

		////This function propagates the mouse scroll event to mouse events
		/// registered for this target if any of one of the targets is
		/// the receiver of the event function will return true and 
		/// propagation will terminate, this function is responsible
		/// to determine who will receive the event
		///@amount	: amount of scrolling
		///@event	: the type of the mouse event
		///@x		: Position of the event	
		///@y		: Position of the event	
		///@data	: Always null, used to preserve compatibility
		virtual bool PropagateMouseScrollEvent(int amount, MouseEventType event, int x, int y, void *data);
		////This function registers a mouse event with the given coordinates,
		/// data and event handlers. This function returns mouse event token that
		/// can be used to modify event properties, events are created in enabled state
		MouseEventToken RegisterMouseEvent(int x, int y, int w, int h, void *data, 
			MouseEvent click	, MouseEvent over=NULL, MouseEvent out=NULL,
			MouseEvent move=NULL, MouseEvent down=NULL, MouseEvent up=NULL, MouseEvent doubleclick=NULL) {
				Bounds b(x,y,x+w,y+h);
				return RegisterMouseEvent(b,data,click,over,out,move,down,up,doubleclick);
		}
		////This function registers a mouse event with the given coordinates,
		/// data and event handlers. This function returns mouse event token that
		/// can be used to modify event properties, events are created in enabled state
		MouseEventToken RegisterMouseEvent(Point position, int w, int h, void *data, 
			MouseEvent click	, MouseEvent over=NULL, MouseEvent out=NULL,
			MouseEvent move=NULL, MouseEvent down=NULL, MouseEvent up=NULL, MouseEvent doubleclick=NULL) {
				Bounds b(position,w,h);
				return RegisterMouseEvent(b,data,click,over,out,move,down,up,doubleclick);
		}
		////This function registers a mouse event with the given coordinates,
		/// data and event handlers. This function returns mouse event token that
		/// can be used to modify event properties, events are created in enabled state
		MouseEventToken RegisterMouseEvent(Bounds bounds, void *data, 
			MouseEvent click	, MouseEvent over=NULL, MouseEvent out=NULL,
			MouseEvent move=NULL, MouseEvent down=NULL, MouseEvent up=NULL, MouseEvent doubleclick=NULL);
		////This function disables a mouse event
		void DisableMouseEvent(MouseEventToken token);
		////This function enables a mouse event
		void EnableMouseEvent(MouseEventToken token);
		////This function removes the given mouse event
		void RemoveMouseEvent(MouseEventToken token);

	protected:
		////This internal function propagates click events
		virtual bool PropagateMouseClickEvent(MouseEventType event, int x, int y, void *data);
		////This internal function propagates down events
		virtual bool PropagateMouseDownEvent(MouseEventType event, int x, int y, void *data);
		////This internal function propagates up events
		virtual bool PropagateMouseUpEvent(MouseEventType event, int x, int y, void *data);
		////This internal function propagates double click events
		virtual bool PropagateMouseDblClickEvent(MouseEventType event, int x, int y, void *data);
		////This internal function propagates move event
		virtual bool PropagateMouseMoveEvent(MouseEventType event, int x, int y, void *data);
		////This internal function propagates over event
		virtual bool PropagateMouseOverEvent(MouseEventType event, int x, int y, void *data);
		////This internal function propagates out event
		virtual bool PropagateMouseOutEvent(MouseEventType event, int x, int y, void *data);
		////This internal function propagates vertical scroll event
		virtual bool PropagateMouseVScrollEvent(int amount, MouseEventType event, int x, int y, void *data);
		////This internal function propagates horizontal scroll event
		virtual bool PropagateMouseHScrollEvent(int amount, MouseEventType event, int x, int y, void *data);

	};

	extern utils::ConsumableEvent<utils::Empty, KeyboardEvent> KeyboardEvents;
	extern MouseEventType MouseButtons;
	////This is the object that is hovered, if mouse moves out of it
	/// it should receive mouse out event
	extern MouseEventObject *hoveredObject;

	////This function registers a top level pointer target.
	/// Lower level targets should be registered to their attached
	/// targets, these pointer targets receive event coordinates
	/// unaltered
	utils::LinkedListItem<BasicPointerTarget> * AddPointerTarget(BasicPointerTarget *target, int order);
	////Removes a previously registered pointer target
	void RemovePointerTarget(utils::LinkedListItem<BasicPointerTarget> *target);
	////Initializes Input system
	void Initialize();
} }
