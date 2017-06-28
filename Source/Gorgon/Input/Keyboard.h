#pragma once

#include <map>
#include "../Input.h"

namespace Gorgon { namespace Input {
    
/// Keyboard related functions and enumerations @nosubgrouping
namespace Keyboard {
		
	/// A key on the keyboard
	typedef Gorgon::Input::Key  Key;

    /**
	 * @name Keycodes
     * Contains the codes for keys. These should be used instead of ASCII 
     * codes as these codes are different than ASCII codes apart from letters 
     * and numbers. Letters are lower case. This is more or less a safe list, 
     * many keys such as dot or comma depends on keyboard locale and should 
     * not be trusted. Similarly Numpad, right modifier keys, PrintScreen, 
     * Pause, menu, numlock, insert keys might be missing in some keyboards.
     * This list of keycodes will always be transformed.
     */
	/// @{
	constexpr Key Shift         = 0b100000001;
	constexpr Key Control       = 0b100000010;
	constexpr Key Alt           = 0b100000100;
	constexpr Key Meta          = 0b100001000;

	constexpr Key RShift        = 0b110000001;
	constexpr Key RControl      = 0b110000010;
	constexpr Key RAlt          = 0b110000100;
	constexpr Key RMeta         = 0b110001000;

	constexpr Key Home          = 0b100010001;
	constexpr Key End           = 0b100010010;
	constexpr Key Insert        = 0b100010011;
	constexpr Key Delete        = 0b100010100;
	constexpr Key PageUp        = 0b100010101;
	constexpr Key PageDown      = 0b100010110;

	constexpr Key PrintScreen   = 0b100010111;
	constexpr Key Pause         = 0b100011000;

	constexpr Key Menu          = 0b100011001;

	constexpr Key CapsLock      = 0b100011010;
	constexpr Key Numlock       = 0b100011011;
	constexpr Key ScrollLock    = 0b100011100;

	constexpr Key Enter         = 0x0D;
	constexpr Key Tab           = 0x0B;
	constexpr Key Backspace     = 0x08;
	constexpr Key Space         = 0x20;
	constexpr Key Escape        = 0x1B;

	constexpr Key Left          = 0b100100000;
	constexpr Key Up            = 0b100100001;
	constexpr Key Right         = 0b100100010;
	constexpr Key Down          = 0b100100011;

	constexpr Key F1            = 0b101000000 + 1;
	constexpr Key F2            = 0b101000000 + 2;
	constexpr Key F3            = 0b101000000 + 3;
	constexpr Key F4            = 0b101000000 + 4;
	constexpr Key F5            = 0b101000000 + 5;
	constexpr Key F6            = 0b101000000 + 6;
	constexpr Key F7            = 0b101000000 + 7;
	constexpr Key F8            = 0b101000000 + 8;
	constexpr Key F9            = 0b101000000 + 9;
	constexpr Key F10           = 0b101000000 + 10;
	constexpr Key F11           = 0b101000000 + 11;
	constexpr Key F12           = 0b101000000 + 12;

	constexpr Key A             = 'a';
	constexpr Key B             = 'b';
	constexpr Key C             = 'c';
	constexpr Key D             = 'd';
	constexpr Key E             = 'e';
	constexpr Key F             = 'f';
	constexpr Key G             = 'g';
	constexpr Key H             = 'h';
	constexpr Key I             = 'i';
	constexpr Key J             = 'j';
	constexpr Key K             = 'k';
	constexpr Key L             = 'l';
	constexpr Key M             = 'm';
	constexpr Key N             = 'n';
	constexpr Key O             = 'o';
	constexpr Key P             = 'p';
	constexpr Key Q             = 'q';
	constexpr Key R             = 'r';
	constexpr Key S             = 's';
	constexpr Key T             = 't';
	constexpr Key U             = 'u';
	constexpr Key V             = 'v';
	constexpr Key W             = 'w';
	constexpr Key X             = 'x';
	constexpr Key Y             = 'y';
	constexpr Key Z             = 'z';

	constexpr Key Number_1      = '1';
	constexpr Key Number_2      = '2';
	constexpr Key Number_3      = '3';
	constexpr Key Number_4      = '4';
	constexpr Key Number_5      = '5';
	constexpr Key Number_6      = '6';
	constexpr Key Number_7      = '7';
	constexpr Key Number_8      = '8';
	constexpr Key Number_9      = '9';
	constexpr Key Number_0      = '0';

	constexpr Key Numpad_0      = 0b101100000 + 0;
	constexpr Key Numpad_1      = 0b101100000 + 1;
	constexpr Key Numpad_2      = 0b101100000 + 2;
	constexpr Key Numpad_3      = 0b101100000 + 3;
	constexpr Key Numpad_4      = 0b101100000 + 4;
	constexpr Key Numpad_5      = 0b101100000 + 5;
	constexpr Key Numpad_6      = 0b101100000 + 6;
	constexpr Key Numpad_7      = 0b101100000 + 7;
	constexpr Key Numpad_8      = 0b101100000 + 8;
	constexpr Key Numpad_9      = 0b101100000 + 9;
	constexpr Key Numpad_Decimal= 0b101101010;
	constexpr Key Numpad_Div    = 0b101101011;
	constexpr Key Numpad_Mult   = 0b101101100;
	constexpr Key Numpad_Enter  = 0b101101101;
	constexpr Key Numpad_Plus   = 0b101101110;
	constexpr Key Numpad_Minus  = 0b101101111;
    
    /// Keycodes that are transported from OS.
	constexpr Key OSTransport   = 0b1000000000;
	/// @}

	/// A character, future compatibility for Unicode
	typedef char 		Char; 
		
	/// MOVETO -> Window?
	extern std::map<Key, bool> PressedKeys;
		
	/// This class represents a modifier key. These keys can be 
	class Modifier {
	public:
		/// A type to represent modifier keys
		enum Type {
			/// No modifier is pressed
			None		= 0,
			/// Only shift modifier is pressed
			Shift		= Keyboard::Shift,
			/// Only control modifier is pressed
			Ctrl		= Keyboard::Control,
			/// Only alt modifier is pressed
			Alt			= Keyboard::Alt,
			/// Only meta/super/window modifier is pressed
			Meta		= Keyboard::Meta,

			/// Shift and control
			ShiftCtrl	= Shift | Ctrl ,				
			/// Shift and alt
			ShiftAlt	= Shift | Alt  ,				
			/// Control and alt
			CtrlAlt		= Ctrl  | Alt  ,
				
			/// Shift control alt together
			ShiftCtrlAlt= Shift | Ctrl | Alt ,
		};
			
		/// Constructs a new modifier from the given modifier key
		Modifier(Type key=None) : 
		Key(key)
		{ }			

		/// Assignment operator
		Modifier &operator =(const Modifier &) = default;
			
		/// Checks if this modifier really modifies the key state so
		/// that no printable characters can be formed
		bool IsModified() const {
			return Key>3;
		}
			
		/// Removes the modifier key
		void Remove(Type key) {
			Key = (Type)(Key & ~key);
		}
			
		/// Adds the given modifier key
		void Add(Type key) {
			Key = (Type)(Key | key);
		}
			
		/// Adds the given keyboard key to modifiers
		void Add(Key key) {
			if(key == Keyboard::Shift) {
				Key = Type(Key | Shift);
			}
			else if(key == Keyboard::Control) {
				Key = Type(Key | Ctrl);
			}
			else if(key == Keyboard::Alt) {
				Key = Type(Key | Alt);
			}
			else if(key == Keyboard::Meta) {
				Key = Type(Key | Meta);
			}
		}
			
		/// Removes the given keyboard key from modifiers
		void Remove(Key key) {
			if(key == Keyboard::Shift) {
				Key = Type(Key & ~Shift);
			}
			else if(key == Keyboard::Control) {
				Key = Type(Key & ~Ctrl);
			}
			else if(key == Keyboard::Alt) {
				Key = Type(Key & ~Alt);
			}
			else if(key == Keyboard::Meta) {
				Key = Type(Key & ~Meta);
			}
		}
		
		/// Or assignment
		Modifier &operator |=(const Modifier &r) {
			Key = (Type)(Key | r.Key);
				
			return *this;
		}
			
		/// And assignment
		Modifier &operator &=(const Modifier &r) {
			Key = (Type)(Key & r.Key);
				
			return *this;
		}
		
		/// Or operator
		Modifier operator |(const Modifier &r) const {
			return {Type(Key | r.Key)};
		}
			
		/// And operator
		Modifier operator &(const Modifier &r) const {
			return {Type(Key & r.Key)};
		}
			
		/// The modifier key
		Type Key;
	};
		
	/// Current keyboard modifier, this is a global value.
	extern Modifier CurrentModifier;
} } }
