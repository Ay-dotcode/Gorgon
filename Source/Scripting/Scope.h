#pragma once

#include <string>
#include <vector>
#include "../String.h"
#include "../Scripting.h"
#include "Instruction.h"
#include "Input.h"
#include "Compilers.h"
#include "Runtime.h"

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
		
	/// This class uniquely represents a source code line. uintptr_t is used for source
	/// to reduce dependency
	class SourceMarker {
		friend class ScopeInstance;
	public:
		SourceMarker() { }
		SourceMarker(const SourceMarker &)=default;
		
		SourceMarker &operator=(const SourceMarker &)=default;

		bool operator <(const SourceMarker &other) {
			return (source == other.source ? line<other.line : source<other.source);
		}
		
		bool IsValid() const { return source!=0; }
		
		uintptr_t GetSource() const { return source; }
		
		uintptr_t GetLine() const { return line; }
		
	private:
		SourceMarker(unsigned long line, uintptr_t source) : line(line), source(source) {}
		
		unsigned long line = 0;
		uintptr_t 	  source = 0;
	};
	
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
		
	/// This is an instantiation of a scope
	class ScopeInstance { 
	public:
		
		/// Constructor requires an input source. Execution scopes can share same input source
		ScopeInstance(Scope &parent) : parent(parent), name(parent.GetName()+" #"+String::From(nextid++)) {
		}
		
		~ScopeInstance() {
			Variables.Destroy();
		}
		
		/// Jumps to the given line, line numbers start at zero.
		void Jumpto(unsigned long line) {
			current=line;
		}
		
		/// Returns the current executing logical line number
		unsigned long GetLineNumber() const {
			return current-1;
		}
		
		/// Returns a unique identifier for the next line in source code. This information can be
		/// used to go back across execution scopes. Useful for Try Catch like structures.
		SourceMarker GetMarkerForNext() const {
			return {current, reinterpret_cast<uintptr_t>(this)};
		}
		
		/// Returns a unique identifier for the next line in source code. This information can be
		/// used to go back across execution scopes. Useful for Try Catch like structures.
		SourceMarker GetMarkerForCurrent() const {
			return {current-1, reinterpret_cast<uintptr_t>(this)};
		}
		
		/// Returns the code at the current line and increments the current line
		const Instruction *Get() {
			auto ret=parent.ReadInstruction(current);
			current++;
			
			return ret;
		}
		
		std::string GetName() const {
			return name;
		}
		
		/// Forces the compilation of entire input source
		void Compile() {
			int c=current;
			while(parent.ReadInstruction(c++)) ;
		}			
		
		/// Returns the code at the current line without incrementing it.
		const Instruction *Peek() {
			return parent.ReadInstruction(current);
		}
		
		/// Returns the code at the given line without incrementing current line.
		const Instruction *Peek(unsigned long line) {
			return parent.ReadInstruction(line);
		}
		
		void MoveToEnd() {
			current=parent.ReadyInstructionCount();
		}

		Scope &GetScope() const { return parent; }
		
		/// Variables defined in this scope
		Containers::Hashmap<std::string, class Variable, &Variable::GetName, std::map, String::CaseInsensitiveLess> Variables;
		
	private:
		std::string name;
		
		static int nextid;
		
		Scope &parent;

		/// Current logical line
		unsigned long current = 0;
	};
	
} }