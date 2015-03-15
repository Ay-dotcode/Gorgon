#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include "../String.h"
#include "../Scripting.h"

namespace Gorgon { namespace Scripting {
		
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
			
			//determine dialect from the extension
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
		
} }
