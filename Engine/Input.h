#pragma once

#include "../Utils/LinkedList.h"
#include "../Utils/Collection.h"
#include "../Utils/GGE.h"
#include "OS.h"
#include "../Utils/Utils.h"

namespace gge {
	class BasicPointerTarget;
	////Types of keyboard modifiers
	enum KeyboardModifier {
		////No modifier
		KEYB_MOD_NONE=0,
		////Shift key is pressed
		KEYB_MOD_SHIFT=1,
		////Control key is pressed
		KEYB_MOD_CTRL=2,
		////Shift and control keys are presses
		KEYB_MOD_SHIFT_CTRL=3,
		////Alt key is pressed
		KEYB_MOD_ALT=4,
		////Shift and alt keys are pressed
		KEYB_MOD_SHIFT_ALT=5,
		////Control and alt keys are pressed
		KEYB_MOD_CTRL_ALT=6,
		////Shift control and alt keys are pressed
		KEYB_MOD_SHIFT_CTRL_ALT=7,
		////Windows / Super key is pressed
		KEYB_MOD_WIN=8,
		////Menu key is pressed
		KEYB_MOD_MENU=16,
		////This flag is set when alternate version of
		/// the key is pressed, alternative keys should
		/// not be relied since not all keyboards have them
		/// this is no longer used, check isAlternateKey
		/// variable
		KEYB_MOD_ALTERNATIVE=32,
		//KEYB_MOD_RIGHTSHIFT=33 ...
	};

	////Whether pressed key is an alternate key (e.g. numpad or right shift)
	extern bool isAlternateKey;

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
		/// operation if the event is recieved
		MOUSE_EVENT_OVER_CHECK,

		////Verticle mouse scrolling is used
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
	enum KeyboardEventType {
		////A character is typed (O/S controlled, i.e. repeating keys)
		KEYB_EVENT_CHR,
		////A key is pressed
		KEYB_EVENT_DOWN,
		////A key is released
		KEYB_EVENT_UP
	};

	struct MouseEventObject;
	////Token given by registering a mouse event
	typedef	LinkedListItem<MouseEventObject> (*MouseEventToken);
	////Defines how a mouse event handing function should be
	typedef bool (*MouseEvent)(MouseEventType event, int x, int y, void *data);
	////Defines how a mouse scroll event handing function should be
	typedef bool (*MouseScrollEvent)(int amount, MouseEventType event, int x, int y, void *data);
	////Defines how a keyboard event handling functoin should be
	typedef bool (*KeyboardEvent)(KeyboardEventType event, int keycode, KeyboardModifier modifier, void *data);

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
		////Verticle scroll event
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

	////Keyboard event objects are internally used to track keyboard event handlers
	struct KeyboardEventObject {
		////This is the event handler that is fired when a char is typed
		/// either by pressing a key or pressing a key and holding it to repeat
		KeyboardEvent chr;
		////This event handler is called when a key is pressed
		KeyboardEvent down;
		////This event handler is called when a key is released
		KeyboardEvent up;
		////ID of the keyboard event, can be used to enable/disable/cancel this
		/// event set
		int id;
		////Any data that is left to be passed to event handlers
		void *data;
		////Whether these events accept modified keys (i.e. when a modifier is set)
		bool Modified;
		////Whether this 
		bool Enabled;
	};
	
	////Registers a keyboard event handler object, event handler function should be in this format:
	/// bool (KeyboardEventType event, int keycode, KeyboardModifier modifier, void *data)
	/// and should return true if the key is processed or blocked and false if the key should be passed
	/// to next event object
	///@data	: the data to be passed to handlers
	///@Char	: This is the event handler that is fired when a char is typed
	/// either by pressing a key or pressing a key and holding it to repeat
	///@Down	: This event handler is called when a key is pressed
	///@Up		: This event handler is called when a key is released
	int	  RegisterKeyboardEvent(void *data,KeyboardEvent Char,KeyboardEvent Down=NULL,KeyboardEvent Up=NULL,bool AllowModified=false);
	////Unregisters a keyboard event object
	///@id		: the identifier returned by RegisterKeyboardEvent function
	void  UnregisterKeyboardEvent(int id);
	////Disables a keyboard event, unlike unregister this state is not permanent
	void  DisableKeyboardEvent(int Index);
	////Enables a keyboard event
	void  EnableKeyboardEvent (int Index);


	////Processes a given char, this function intended to be called from OS
	/// sub-system, however a knowledgable attempt to call from elsewhere is
	/// also valid
	///@Char	: Character code (8bit ASCII or equivalent (ISO-8859))
	void ProcessChar(int Char);
	////Processes a given key as pressed, this function intended to be called from OS
	/// sub-system, however a knowledgable attempt to call from elsewhere is
	/// also valid
	///@Key		: The keyboard code of the pressed key
	void ProcessKeyDown(int Key);
	////Processes a given key as released, this function intended to be called from OS
	/// sub-system, however a knowledgable attempt to call from elsewhere is
	/// also valid
	///@Key		: The keyboard code of the pressed key
	void ProcessKeyUp(int Key);

	////Processes the current mouse position this information is taken from OS subsystem
	void ProcessMousePosition(WindowHandle Window);
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
	////Processes verticle scroll
	void ProcessVScroll(int amount,int x,int y);
	////Processes horizontal scroll
	void ProcessHScroll(int amount,int x,int y);

	////This interface defines a class that can be used
	/// as a common target of mouse events
	class BasicPointerTarget {
	public:
		////The array of mouse events
		LinkedList<MouseEventObject> mouseevents;

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
		////This internal function propagates verticle scroll event
		virtual bool PropagateMouseVScrollEvent(int amount, MouseEventType event, int x, int y, void *data);
		////This internal function propagates horizontal scroll event
		virtual bool PropagateMouseHScrollEvent(int amount, MouseEventType event, int x, int y, void *data);

	};

	////Bitwise OR operation on KeyboardModifier enumeration
	inline KeyboardModifier operator | (KeyboardModifier m1, KeyboardModifier m2) {
		return (KeyboardModifier)((int)m1|(int)m2);
	}

	////Bitwise AND operation on KeyboardModifier enumeration
	inline KeyboardModifier operator & (KeyboardModifier m1, KeyboardModifier m2) {
		return (KeyboardModifier)((int)m1&(int)m2);
	}

	////Bitwise EQUAL OR operation on KeyboardModifier enumeration
	inline KeyboardModifier operator |= (KeyboardModifier m1, KeyboardModifier m2) {
		return (KeyboardModifier)((int)m1|(int)m2);
	}

	////Bitwise EQUAL AND operation on KeyboardModifier enumeration
	inline KeyboardModifier operator &= (KeyboardModifier m1, KeyboardModifier m2) {
		return (KeyboardModifier)((int)m1&(int)m2);
	}


	////Checks the the current state of the keyboard if there are any real modifiers
	/// in effect (Namely control, alt, and windows keys)
	inline bool CheckModifier(KeyboardModifier m1) {
		return m1&KEYB_MOD_CTRL || m1&KEYB_MOD_ALT || m1&KEYB_MOD_WIN;
	}


	////This function can be used internally to remove the second keyboard modifier
	/// from the first one
	inline void RemoveModifier(KeyboardModifier &m1,KeyboardModifier m2) {
		m1=m1&(KeyboardModifier)(~m2);
	}
	////This function add a keyboard modifier to another one
	inline void AddModifier(KeyboardModifier &m1,KeyboardModifier m2) {
		m1=m1|m2;
	}

	extern KeyboardModifier KeyboardModifiers;
	extern MouseEventType MouseButtons;
	extern Collection<KeyboardEventObject> KeyboardEventObjects;
	////This is the object that is hovered, if mouse moves out of it
	/// it should receive mouse out event
	extern MouseEventObject *hoveredObject;

	////This function registers a top level pointer target.
	/// Lower level targets should be registered to their attached
	/// targets, these pointer targets receive event coordinates
	/// unaltered
	LinkedListItem<BasicPointerTarget> * AddPointerTarget(BasicPointerTarget *target, int order);
	////Removes a previously registered pointer target
	void RemovePointerTarget(LinkedListItem<BasicPointerTarget> *target);
	////Initializes Input system
	void InitializeInput();
}
