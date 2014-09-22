#include "Console.h"

#include <Windows.h>

namespace Gorgon { namespace Console {

	/// @cond INTERNAL
	struct consoleattributes {
		consoleattributes() {
			stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
			WORD attribs;
			DWORD len;
			COORD coords = { 0, 0 };
			ReadConsoleOutputAttribute(stdhandle, &attribs, 1, coords, &len);

			if(len != 0) {
				fore = defaultfore = attribs & 0x07;
				back = defaultback = attribs & 0x70;
			}
		}
		
		static void set() {
			get().set_();
		}

		void set_() {
			if(negative) {
				SetConsoleTextAttribute(stdhandle, fore>>4 | back<<4 | (bold ? FOREGROUND_INTENSITY : 0));
			}
			else {
				SetConsoleTextAttribute(stdhandle, fore | back | (bold ? FOREGROUND_INTENSITY : 0));
			}
		}

		static consoleattributes &get() { 
			static consoleattributes console;

			return console;
		}

		int fore = 7, back = 0;
		int  defaultfore = 7, defaultback = 0;
		bool bold = false;
		bool negative = false;

		HANDLE stdhandle;
	};
	/// @endcond

	bool IsColorSupported() { return GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE; }

	bool IsStylesSupported() { return GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE; }

	void SetColor(Color color, std::ostream &stream) {
		int c;
		switch (color) {
		case Default:
			c = consoleattributes::get().defaultfore;
			break;
		case Black:
			c = 0;
			break;
		case White:
			c = 7;
			break;
		case Red:
			c = 4;
			break;
		case Green:
			c = 2;
			break;
		case Blue:
			c = 1;
			break;
		case Yellow:
			c = 6;
			break;
		case Cyan:
			c = 3;
			break;
		case Magenta:
			c = 5;
			break;
		}
		consoleattributes::get().fore = c;
		consoleattributes::set();
	}

	void SetBackground(Color color, std::ostream &stream) {
		int c;
		switch (color) {
		case Default:
			c = consoleattributes::get().defaultback<<4;
			break;
		case Black:
			c = 0;
			break;
		case White:
			c = 7;
			break;
		case Red:
			c = 4;
			break;
		case Green:
			c = 2;
			break;
		case Blue:
			c = 1;
			break;
		case Yellow:
			c = 6;
			break;
		case Cyan:
			c = 3;
			break;
		case Magenta:
			c = 5;
			break;
		}
		consoleattributes::get().back = c>>4;
		consoleattributes::set();
	}

	void Reset(std::ostream &stream) {
		consoleattributes::get().bold = false;
		consoleattributes::get().fore = consoleattributes::get().defaultfore;
		consoleattributes::get().back = consoleattributes::get().defaultback;
		consoleattributes::set();
	}

	void SetBold(bool bold, std::ostream &stream) {
		consoleattributes::get().bold = bold;
		consoleattributes::set();
	}
	
	void SetUnderline(bool underline, std::ostream &stream) { }

	void SetItalic(bool italic, std::ostream &stream) { }

	void SetNegative(bool negative, std::ostream &stream) { 
		consoleattributes::get().negative = negative; 
		consoleattributes::set();
	}

} }