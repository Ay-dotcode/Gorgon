#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "Data.h"
#include "../Types.h"
#include "Execution.h"

namespace Gorgon {
	namespace Scripting {

		class ParserBase {
		public:
			
			virtual unsigned Parse(const std::string &input) = 0;
			virtual void Finalize() = 0;
			
			std::vector<Instruction> List;
		};

		class IntermediateParser : public ParserBase {
		public:
			
			virtual unsigned Parse(const std::string &input) override;
			virtual void Finalize() override { }
			
		private:
			void storedfn(const std::string &input, int &ch);
			
			void fncall(const std::string &input, int &ch, bool allowmethod=true);
			
			void varassign(const std::string &input, int &ch);
			
			Value parsevalue(const std::string &input, int &ch);
			
			unsigned long parsetemporary(const std::string &input, int &ch);
			
			void eatwhite(const std::string &input, int &ch);
		};


		class ProgrammingParser: public ParserBase {
		public:

			virtual unsigned Parse(const std::string &input) override;
			
			virtual void Finalize() override;
			
			void parseexpr(const std::string &input);

		private:
			std::string left;
			std::vector<unsigned> linestarts;
		};
		
		/// Disassembles the given instruction
		std::string Disassemble(const Instruction *);
		
		class InputSource;
		
		/// Disassembles entire input source to the given stream
		void Disassemble(InputSource &source, std::ostream &out);
	}
}
