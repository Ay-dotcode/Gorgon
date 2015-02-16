#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "Data.h"
#include "Instruction.h"
#include "Compilers/AST.h"
#include "../Types.h"

namespace Gorgon {	namespace Scripting { 
	
	class InputSource;

/// This namespace contains compliers and any utilities related with it
namespace Compilers {

	/// The base class for compilers
	class Base {
	public:
		
		/// Asks the compiler to compile the given input. Returns the number of instructions
		/// generated from the code. Compiler is free to request additional input by returning
		/// zero. This does not necessarily mean there is no compilable source in the given input
		/// it may simply be an incomplete line. When there is no more input, caller should finish
		/// compilation by calling Finalize
		virtual unsigned Compile(const std::string &input) = 0;
		
		/// Finalizes the compilation. Compiler may throw an error about missing constructs at this
		/// point.
		virtual void Finalize() = 0;
		
		/// The instructions that are compiled
		std::vector<Instruction> List;
	};

	/// Intermediate language complier
	class Intermediate : public Base {
	public:
		
		virtual unsigned Compile(const std::string &input) override;
		virtual void Finalize() override { }
		
	private:
		void storedfn(const std::string &input, int &ch);
		
		void fncall(const std::string &input, int &ch, bool allowmethod=true);
		
		void varassign(const std::string &input, int &ch);
		
		Value parsevalue(const std::string &input, int &ch);
		
		unsigned long parsetemporary(const std::string &input, int &ch);
		
		void eatwhite(const std::string &input, int &ch);
		
		void jinst(std::string input, int &ch);
	};

	/// Programming dialect compiler
	class Programming : public Base {
	public:
		Programming() : compiler(List) { }
		
		virtual unsigned Compile(const std::string &input) override;
		
		virtual void Finalize() override;

	private:
		ASTCompiler compiler;
		std::string left;
		std::vector<unsigned> linestarts;
		int waiting=0;
	};
	
	/// Disassembles the given instruction
	std::string Disassemble(const Instruction *);
	
	/// Disassembles entire input source to the given stream
	void Disassemble(InputSource &source, std::ostream &out);
	
} } }
