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
		
		long physical;
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
	
	class ScopeInstance;
	
	/** 
	 * A new scope is created automatically when a new input source or a function like construct
	 * is created. Scopes can be linked to each other. There are two methods to supply codes to
	 * a scope: from an InputProvider and directly registering instructions to it.
	 */
	class Scope {
		friend class ScopeInstance;
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
		long GetPhysicalLine(unsigned long line) {
			if(ReadInstruction(line)) {
				return lines[line].physical;
			}
			else {
				return -1;
			}
		}
		
		unsigned ReadyInstructionCount() const {
			return lines.size();
		}
		
		std::string GetName() const { return name; }
		
		void SaveInstruction(Instruction inst, long pline) {
			lines.push_back({inst, pline});
		}
		
		ScopeInstance &Instantiate();
		
		ScopeInstance &Instantiate(ScopeInstance &current);
		
		bool HasInstance() const {
			return instances.GetCount()!=0;
		}
		
		ScopeInstance &LastInstance() const {
			ASSERT(instances.GetCount(), "There are no instances available");
			
			return *instances.Last();
		}
		
		bool HasParent() const {
			return parent!=nullptr;
		}
		
		Scope &GetParent() const {
			return *parent;
		}
		
		Variable *GetVariable(const std::string &name) {
			auto var=variables.find(name);
			
			if( var !=variables.end() )
				return &var->second;
			else
				return nullptr;
		}
		
		void SetVariable(const std::string &name, const Data &data) {
			variables[name]={name, data};
		}
		
		bool UnsetVariable(const std::string &name) {
			auto var=variables.find(name);
			
			if( var !=variables.end() ) {
				variables.erase(var);
				return true;
			}
			else
				return false;
		}
		
		/// Unloads an input source by erasing all current data. Unload should only be
		/// called when no more callbacks can be performed and no more lines are left.
		/// Additionally, no keyword scope should be active, otherwise a potential jump 
		/// back might cause undefined behavior.
		void Unload() {
			using std::swap;
			
			std::vector<Line> temp;	
			swap(temp, lines);
			
			provider->Reset();
		}
		
	private:
		std::string name;
		
		int nextid=1;
		
		long pline=0;

		InputProvider *provider = nullptr;
		
		Compilers::Base *parser =nullptr;
		
		Scope *parent=nullptr;
		
		Containers::Collection<ScopeInstance> instances;
		
		//-unordered map
		std::map<std::string, Variable, String::CaseInsensitiveLess> variables;
		
		/// Every logical line at least up until current execution point. They are kept so that
		/// it is possible to jump back. Logical lines do not contain comments.
		std::vector<Line> lines;
	};
	
	/// This is an instantiation of a scope
	class ScopeInstance { 
		friend class Scope;
	public:
		
		~ScopeInstance() {
			scope.instances.Remove(this);
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
			auto ret=scope.ReadInstruction(current);
			current++;
			
			return ret;
		}
		
		std::string GetName() const {
			return name;
		}
		
		Variable *GetVariable(const std::string &name) {
			auto varit=variables.find(name);
			if( varit !=variables.end() )
				return &varit->second;
			
			auto var=scope.GetVariable(name);
			if(var)
				return var;
			
			if(scope.HasParent() && scope.GetParent().HasInstance())
				var=scope.GetParent().LastInstance().GetVariable(name);
			
			return var;
		}
		
		void SetVariable(const std::string &name, const Data &data) {
			variables[name]={name, data};
		}
		
		bool UnsetVariable(const std::string &name) {
			auto var=variables.find(name);
			
			if( var !=variables.end() ) {
				variables.erase(var);
				return true;
			}
			
			auto done=scope.UnsetVariable(name);
			if(done)
				return true;
			
			if(scope.HasParent() && scope.GetParent().HasInstance())
				done=scope.GetParent().LastInstance().UnsetVariable(name);
			
			return done;
		}
		
		/// Forces the compilation of entire scope
		void Compile() {
			int c=current;
			while(scope.ReadInstruction(c++)) ;
		}			
		
		/// Returns the code at the current line without incrementing it.
		const Instruction *Peek() {
			return scope.ReadInstruction(current);
		}
		
		/// Returns the code at the given line without changing current line.
		const Instruction *Peek(unsigned long line) {
			return scope.ReadInstruction(line);
		}
		
		void MoveToEnd() {
			current=scope.ReadyInstructionCount();
		}
		
		long GetPhysicalLine() {
			return scope.GetPhysicalLine(current-1);
		}

		Scope &GetScope() const { return scope; }
		
	private:
		
		/// Constructor requires an input source. Execution scopes can share same input source
		ScopeInstance(Scope &scope, ScopeInstance *parent) : scope(scope), parent(parent) {
			name=scope.GetName()+" #"+String::From(scope.nextid++);
		}
		
		//-unordered map
		std::map<std::string, Variable, String::CaseInsensitiveLess> variables;

		ScopeInstance *parent = nullptr;
		
		std::string name;
		
		Scope &scope;

		/// Current logical line
		unsigned long current = 0;
	};
	
} }