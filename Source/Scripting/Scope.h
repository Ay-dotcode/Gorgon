#pragma once

#include <string>
#include <vector>
#include "../String.h"
#include "../Scripting.h"
#include "Instruction.h"
#include "Input.h"
#include "Compilers.h"

namespace Gorgon { namespace Scripting {
	
	/// @cond INTERNAL
	/// This class represents a logical line
	class Line {
	public:
		Instruction instruction;
		
		unsigned long physical;
	};
	/// @endcond
	
	class CompilerBase;
	
	/** 
	 * A new scope is created automatically when a new input source or a function like construct
	 * is created. Scopes can be linked to each other. There are two methods to supply codes to
	 * a scope: from an InputProvider and directly registering instructions to it.
	 */
	class Scope {
	public:
		
		/// Constructor requires an input provider and a name to define this input source
		Scope(InputProvider &provider, const std::string &name);
		
		/// This constructor allows a scope without an input provider. This allows function
		/// like constructs to have their own scopes with designated source code supplied 
		/// from external sources
		Scope(Scope &parent, const std::string &name);
		
		/// Reads the instruction in the given line
		const Instruction *ReadInstruction(unsigned long line);
		
		/// Current physical line
		unsigned long GetPhysicalLine() const {
			return pline;
		}
		
		unsigned ReadyInstructionCount() const {
			return lines.size();
		}
		
		
		std::string GetName() const { return name; }
		
		void SaveInstruction(Instruction inst, unsigned long pline) {
			lines.push_back({inst, pline});
		}
		
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
		
		Compilers::Base *parser;
		
		/// Every logical line up until current execution point. They are kept so that
		/// it is possible to jump back. Logical lines do not contain comments 
		std::vector<Line> lines;
	};
	
} }