#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include "../String.h"
#include "../Scripting.h"
#include "Execution.h"

namespace Gorgon {
	
	namespace Scripting {
		
		class InputProvider {
		public:
			enum Dialect {
				Console,
				Programming,
				Intermediate,
				Binary
			};
			
			InputProvider(Dialect dialect) : dialect(dialect) {}
			
			Dialect GetDialect() const {
				return dialect;
			}
			
			void SetDialect(Dialect dialect) {
				this->dialect=dialect;
				checkdialect();
			}
			
			
			/// This method should read a single physical line from the source. Logical line separation
			/// is handled by InputSource. Return of false means no input is fetched as it is finished.
			/// If there is a read error, rather than returning false, this function should throw.
			/// newline parameter denotes that this line is a new line, not continuation of another.
			virtual bool ReadLine(std::string &, bool newline) = 0;
			
			//virtual int ReadBinary(std::vector<Byte> &buffer) = 0;
			
			virtual void Reset() = 0;
			
		protected:
			virtual void checkdialect() { }
			
			Dialect dialect;
		};
		
		/// Reads lines from the console
		class ConsoleInput : public InputProvider {
		public:
			
			/// Initializes console input. line number will be appended at the start of the prompt
			ConsoleInput(Dialect dialect=InputProvider::Console, const std::string &prompt="> ") : InputProvider(dialect),
			prompt(prompt) { }
			
			void SetPrompt(const std::string &prompt) {
				this->prompt=prompt;
			}
			
			virtual bool ReadLine(std::string &input, bool newline) override final {
				line++;
				std::cout<<std::setw(3)<<line<<prompt;

				return bool(std::getline(std::cin, input));
			}
			
			virtual void Reset() override {
				line=0;
			}
			
		protected:
			virtual void checkdialect() override { 
				if(dialect==InputProvider::Binary) {
					SetDialect(InputProvider::Console);
					throw std::runtime_error("Cannot accept binary code from the console");
				}
			}
			
		private:
			std::string prompt;
			int line=0;
		};
		
		/// Reads lines from a stream
		class StreamInput : public InputProvider {
		public:
			StreamInput(std::istream &stream, Dialect dialect) : InputProvider(dialect), stream(stream) {
			}
			
			virtual bool ReadLine(std::string &input, bool) override final {
				return bool(std::getline(stream,input));
			}
			
			virtual void Reset() override {
				stream.seekg(0);
			}
			
		private:
			std::istream &stream;
		};
		
		/// Reads lines from a file
		class FileInput : public StreamInput {
		public:
			FileInput(const std::string &filename) : StreamInput(file, InputProvider::Programming) {
				auto loc=filename.find_last_of('.');
				std::string ext="";
				if(loc!=filename.npos)
					ext=filename.substr(loc);
				
				if(ext.length()>=3 && ext.substr(0,3)=="gsb") {
					dialect=InputProvider::Binary;
				}
				else if(ext.length()>=3 && ext.substr(0,3)=="gsc") {
					dialect=InputProvider::Console;
				}
				else if(ext.length()>=3 && ext.substr(0,3)=="gsi") {
					dialect=InputProvider::Intermediate;
				}
				else { // generally *.gs*
					dialect=InputProvider::Programming;
				}
				
				file.open(filename, dialect==InputProvider::Binary ? std::ios::out | std::ios::binary : std::ios::out);
				if(!file.is_open()) {
					throw std::runtime_error("Cannot open file");
				}
			}
			
		private:
			std::ifstream file;
		};
		
		/// @cond INTERNAL
		/// This class represents a logical line
		class Line {
		public:
			Instruction instruction;
			
			unsigned long physical;
		};
		/// @endcond

		class ParserBase;
		
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
			
			const Instruction *ReadInstruction(unsigned long line);

			unsigned long GetPhysicalLine() const {
				return pline;
			}

			std::string GetName() const { return name; }
			
			/// Unloads an input source by erasing all current data. Unload should only be
			/// called when no more callbacks can be performed and no more lines are left.
			/// Additionally, no keyword scope should be active, otherwise a potential jump 
			/// back might cause undefined behavior.
			void Unload() {
				using std::swap;
				
				std::vector<Line> temp;	
				swap(temp, lines);
				pline=0;
				
				provider.Reset();
			}
			
		private:
			InputProvider &provider;
			
			unsigned long pline = 0;
			std::string name;

			ParserBase *parser;
			
			/// Every logical line up until current execution point. They are kept so that
			/// it is possible to jump back. Logical lines do not contain comments 
			std::vector<Line> lines;
		};
		
	}
}
