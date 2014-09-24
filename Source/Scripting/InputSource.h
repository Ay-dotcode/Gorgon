#pragma once

#include <string>
#include <vector>

namespace Gorgon {
	
	namespace Scripting {
		
		class InputProvider {
		public:
			
			/// This method should read a single physical line from the source. Logical line separation
			/// is handled by InputSource. Return of false means no input is fetched as it is finished.
			/// If there is a read error, rather than returning false, this function should throw.
			virtual bool ReadLine(std::string &) = 0;
			
		};

		/// Reads lines from the console
		class ConsoleInput : public InputProvider {
		};
		
		/// Reads lines from a stream
		class StreamInput : public InputProvider {
		};
		
		/// This class represents a logical line
		class Line {
		public:
			std::string Data;
			unsigned long FromStart;
		};
		
		/// Base class for input sources. This system allows different input sources to supply 
		/// code to virtual machine. Each input source has its own line numbering, a name and
		/// an index to identify it. For instance if the same file is read twice, first one will
		/// be named as file.gs#1 and the other will be file.gs#2. This module also handles
		/// logical line/physical line separation.
		class InputSource {
		public:
			
			/// Constructor requires a input provider
			InputSource(InputProvider &provider, const std::string &name);
			
			
			
		private:
			InputProvider &provider;
			
			/// Logical line number
			unsigned long line;
			
			/// Physical line starting characters
			std::vector<unsigned long> plinestarts;
			
			/// Logical line starting characters
			std::vector<unsigned long> llinestarts;
			
			/// Every logical line up until current execution point. They are kept so that
			/// it is possible to jump back
			std::vector<Line> lines;
		};
		
	}
}
