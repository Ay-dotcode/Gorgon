#pragma once

#include <iostream>
#include "OS.h"

namespace Gorgon {
	
	/// Console manipulation functions. Not thread safe.
	namespace Console {
		
		/// The colors that can be used for console coloring. This is a safe list
		enum Color {
			Default,
			Black,
			White,
			Red,
			Cyan,
			Blue,
			Yellow,
			Magenta,
			Green
		};
		
		/// Returns if color is supported in this terminal
		bool IsColorSupported();
		
		/// Returns if color is supported in this terminal
		bool IsStylesSupported();
		
		/// Sets the color to the given value, avoid, black and white as console can have its
		/// background color reversed. Use Default to set it to default color.
		void SetColor(Color color, std::ostream &stream=std::cout);
		
		/// Sets the background color to the given value. Use Default to set it to default color.
		void SetBackground(Color color, std::ostream &stream=std::cout);
		
		/// Resets terminal attributes
		void Reset(std::ostream &stream=std::cout);
		
		/// Sets terminal font to bold or normal
		void SetBold(bool bold=true, std::ostream &stream=std::cout);
		
		/// Enable/disable blink (note: annoying)
		void SetBlink(bool blink=true, std::ostream &stream=std::cout);
		
		/// Enable/disable underline
		void SetUnderline(bool underline=true, std::ostream &stream=std::cout);
		
		/// Enable/disable italic
		void SetItalic(bool italic=true, std::ostream &stream=std::cout);
		
		/// Background/foreground is switched
		void SetNegative(bool negative=true, std::ostream &stream=std::cout);		
		
		
#ifdef LINUX
		inline bool IsColorSupported() {			
			struct support {
				support() {
					std::string term=OS::GetEnvVar("TERM");
					s=(term=="LINUX" || term.substr(0,5)=="xterm" || term=="gnome-terminal");
				}
				
				operator bool() const { return s; }
				
				bool s;
			};
			
			static support s;
			
			return (bool)s;
		}
		
		inline bool IsStylesSupported() {			
			struct support {
				support() {
					std::string term=OS::GetEnvVar("TERM");
					s=(term=="LINUX" || term.substr(0,5)=="xterm" || term=="gnome-terminal");
				}
				
				operator bool() const { return s; }
				
				bool s;
			};
			
			static support s;
			
			return (bool)s;
		}
		
		inline void SetColor(Color color, std::ostream &stream) {
			if(!IsColorSupported()) return;
			
			int c=29;
			switch(color) {
				case Default:
					c+=2;
				case White:
					c++;
				case Cyan:
					c++;
				case Magenta:
					c++;
				case Blue:
					c++;
				case Yellow:
					c++;
				case Green:
					c++;
				case Red:
					c++;
				case Black:
					c++;
			}
			
			stream<<"\033["<<c<<"m";
		}
		
		inline void SetBackground(Color color, std::ostream &stream) {
			if(!IsColorSupported()) return;
			
			int c=39;
			switch(color) {
				case Default:
					c+=2;
				case White:
					c++;
				case Cyan:
					c++;
				case Magenta:
					c++;
				case Blue:
					c++;
				case Yellow:
					c++;
				case Green:
					c++;
				case Red:
					c++;
				case Black:
					c++;
			}
			
			stream<<"\033["<<c<<"m";
		}
		
		inline void Reset(std::ostream &stream) {
			if(!IsColorSupported() && !IsStylesSupported()) return;
			
			stream<<"\033[0m";
			stream.flush();
		}
		
		inline void SetBold(bool bold, std::ostream &stream) {
			if(!IsStylesSupported()) return;
			
			if(bold)
				stream<<"\033[1m";
			else
				stream<<"\033[22m";
		}
		
		inline void SetBlink(bool blink, std::ostream &stream) {
			if(!IsStylesSupported()) return;
			
			if(blink)
				stream<<"\033[5m";
			else
				stream<<"\033[25m";
		}
		
		inline void SetUnderline(bool underline, std::ostream &stream) {
			if(!IsStylesSupported()) return;
			
			if(underline)
				stream<<"\033[4m";
			else
				stream<<"\033[24m";
		}
		
		inline void SetItalic(bool italic, std::ostream &stream) {
			if(!IsStylesSupported()) return;
			
			if(italic)
				stream<<"\033[3m";
			else
				stream<<"\033[23m";
		}
		
		inline void SetNegative(bool negative, std::ostream &stream) {
			if(!IsStylesSupported()) return;
			
			if(negative)
				stream<<"\033[7m";
			else
				stream<<"\033[27m";
		}
#else
		bool IsColorSupported() { return false; }
		
		bool IsStylesSupported() { return false; }
		
		void SetColor(Color color, std::ostream &stream) { }

		void SetBackground(Color color, std::ostream &stream) { }

		void Reset(std::ostream &stream) { }

		void SetBold(bool bold=true, std::ostream &stream) { }

		void SetBlink(bool blink=true, std::ostream &stream) { }

		void SetUnderline(bool underline=true, std::ostream &stream) { }

		void SetItalic(bool italic=true, std::ostream &stream) { }

		void SetNegative(bool negative=true, std::ostream &stream) { }
#endif
		
	}
}
