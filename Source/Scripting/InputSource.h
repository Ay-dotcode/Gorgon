#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <iomanip>
#include "../String.h"

namespace Gorgon {
	
	namespace Scripting {
		
		class InputProvider {
		public:
			
			/// This method should read a single physical line from the source. Logical line separation
			/// is handled by InputSource. Return of false means no input is fetched as it is finished.
			/// If there is a read error, rather than returning false, this function should throw.
			/// newline parameter denotes that this line is a new line, not continuation of another.
			virtual bool ReadLine(std::string &, bool newline) = 0;
			
		};
		
		/// Reads lines from the console
		class ConsoleInput : public InputProvider {
		public:
			
			/// Initializes console input. line number will be appended at the start of the prompt
			ConsoleInput(const std::string &prompt="> ") : prompt(prompt) { }
			
			void SetPrompt(const std::string &prompt) {
				this->prompt=prompt;
			}
			
			virtual bool ReadLine(std::string &input, bool newline) override {
				line++;
				std::cout<<std::setw(3)<<line<<prompt;
				
				return (std::cin>>input);
			}
			
		private:
			std::string prompt;
			int line=0;
		};
		
		/// Reads lines from a stream
		class StreamInput : public InputProvider {
		public:
			StreamInput(std::istream &stream) : stream(stream) {
			}
			
			virtual bool ReadLine(std::string &input, bool) override {
				return (stream>>input);
			}
			
		private:
			std::istream &stream;
		};
		
		/// This class represents a logical line
		class Line {
		public:
			std::string Data;
			
			unsigned long Physical;
			unsigned long Logical;
		};

		/** 
		 * Base class for input sources. This system allows different input sources to supply 
		 * code to virtual machine. When source code is loaded into virtual machine, it stays
		 * until that all execution scopes that are operating in that inputscope is finished.
		 * This allows faster processing for loops. Additionally, function and similar keywords
		 * can store their data in their own inputsources
		 */
		class InputSource {
		public:
			
			/// Constructor requires an input provider and a name to define this input source
			InputSource(InputProvider &provider, const std::string &name);
			
			bool GetLine(unsigned long line, std::string &data) {
				bool eof=false;

				while(lines.size()<=line && !eof) {
					std::string newline;
					
					while(true) {
						std::string s;
						if(!provider.ReadLine(s, newline=="")) {
							eof=true;
							break;
						}
						pline++;
						
						//TODO offset
						s=String::TrimStart(s);
						
						// remove comment lines
						if(s[0]=='#') continue;
						
						newline+=s;
						
						//TODO check if newline is a complete line
						if(true)
							break;
					}					
					
					while(newline!="") {
						//TODO parse newline if necessary split to multiple lines
						lines.push_back({newline, pline, lines.size()});
						newline="";
					}
				}
				
				if(lines.size()>line) {
					data=lines[line].Data;
					return true;
				}
				else {
					data="";
					return false;
				}
			}
			
		private:
			InputProvider &provider;
			
			unsigned long pline = 0;
			
			/// Every logical line up until current execution point. They are kept so that
			/// it is possible to jump back. Logical lines do not contain comments 
			std::vector<Line> lines;
		};
		
	}
}
