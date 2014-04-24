#pragma once

#include <map>
#include "Input.h"

namespace Gorgon {
		
	/// Keyboard related functions and enumerations @nosubgrouping
	namespace Keyboard {
		
		/// A key on the keyboard
		typedef Input::Key  Key;
		typedef char 		Char; //future compatibility for unicode
		
		//MOVETO -> Window?
		extern std::map<Key, bool> PressedKeys;

		/// @name Keycodes
		///       Contains the codes for keys. These should be used
		///       instead of ASCII codes as these codes might differ.
		///       Alternative locations of keys, such as right shift
		///       is reported as normal key. 
		/// @{
		extern const Key Shift;
		extern const Key Control;
		extern const Key Alt;
		extern const Key AltGr;
		extern const Key Meta;

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
		
		extern const Key Menu;

		extern const Key CapsLock;

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
		
		extern const Key A;
		extern const Key B;
		extern const Key C;
		extern const Key D;
		extern const Key E;
		extern const Key F;
		extern const Key G;
		extern const Key H;
		extern const Key I;
		extern const Key J;
		extern const Key K;
		extern const Key L;
		extern const Key M;
		extern const Key N;
		extern const Key O;
		extern const Key P;
		extern const Key Q;
		extern const Key R;
		extern const Key S;
		extern const Key T;
		extern const Key U;
		extern const Key V;
		extern const Key W;
		extern const Key X;
		extern const Key Y;
		extern const Key Z;
		
		extern const Key Number_1;
		extern const Key Number_2;
		extern const Key Number_3;
		extern const Key Number_4;
		extern const Key Number_5;
		extern const Key Number_6;
		extern const Key Number_7;
		extern const Key Number_8;
		extern const Key Number_9;
		extern const Key Number_0;
		
		extern const Key Numpad_1;
		extern const Key Numpad_2;
		extern const Key Numpad_3;
		extern const Key Numpad_4;
		extern const Key Numpad_5;
		extern const Key Numpad_6;
		extern const Key Numpad_7;
		extern const Key Numpad_8;
		extern const Key Numpad_9;
		extern const Key Numpad_Enter;
		extern const Key Numpad_Plus;
		extern const Key Numpad_Minus;
		extern const Key Numpad_Mult;
		extern const Key Numpad_Div;
		extern const Key Numlock;
		/// @}
		
		/// This class represents a modifier key. These keys can be 
		class Modifier {
		public:
			enum Type {
				None		= 0,
				Shift		= 1,
				AltGr		= 2,
				Ctrl		= 4,
				Alt			= 8,
				Meta		=16,

				ShiftCtrl	= Shift | Ctrl ,
				ShiftAlt	= Shift | Alt  ,
				CtrlAlt		= Ctrl  | Alt  ,
				ShiftCtrlAlt= Shift | Ctrl | Alt ,
			};
			
			Modifier(Type key=None) : 
			Key(key)
			{ }
			
			explicit Modifier(int key) : 
			Key(Type(key))
			{ }
			
			Modifier &operator =(const Modifier &) = default;
			
			bool IsModified() const {
				return Key>3;
			}
			
			void Remove(Type key) {
				Key = (Type)(Key & ~key);
			}
			
			void Add(Type key) {
				Key = (Type)(Key | key);
			}
			
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
				else if(key == Keyboard::AltGr) {
					Key = Type(Key | AltGr);
				}
			}
			
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
				else if(key == Keyboard::AltGr) {
					Key = Type(Key & ~AltGr);
				}
			}
		
			Modifier &operator |=(const Modifier &r) {
				Key = (Type)(Key | r.Key);
			}
			
			Modifier &operator &=(const Modifier &r) {
				Key = (Type)(Key & r.Key);
			}
		
			Modifier operator |(const Modifier &r) const {
				return {Type(Key | r.Key)};
			}
			
			Modifier operator &(const Modifier &r) const {
				return {Type(Key & r.Key)};
			}
			
			Type Key;
		};
		
		extern Modifier CurrentModifier;
	}
}
