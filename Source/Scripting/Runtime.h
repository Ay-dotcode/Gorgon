/// @file Runtime.h This file contains classes that stores runtime and end programmer defined objects.

#pragma once

#include <ostream>
#include <string>


#include "Reflection.h"
#include "Data.h"
#include "InputSource.h"

namespace Gorgon {
	
	namespace Scripting {
		
		/// This class allows references to be counted and destroyed properly.
		class ReferenceCounter {
		public:
			
			/// Registers a new object of reference counting, this will set reference count to one. This function
			/// ignores register requests for nullptr
			void Register(const Data &data) {
				ASSERT(data.GetData().IsPointer(), "Reference keeping can only be performed for reference types, "
				"offender: "+data.GetType().GetName(), 1, 4);
				
				void *ptr=data.GetData().Pointer();
				
				//ignore register requests to nullptr
				if(ptr==nullptr) return;
				
				if(references[ptr]==0)
					references[ptr]=1;
			}
			
			/// Registers a new object of reference counting, this will set reference count to 0. This function
			/// ignores register requests for nullptr
			void Register(void *ptr) {
				//ignore register requests to nullptr
				if(ptr==nullptr) return;
				
				references[ptr];
			}
			
			/// Increases the reference count of the given object. If it is not registered, this request is ignored
			void Increase(const Data &data) {
				ASSERT(data.GetData().IsPointer(), "Reference keeping can only be performed for reference types, "
					   "offender: "+data.GetType().GetName(), 1, 4);

				void *ptr=data.GetData().Pointer();
				auto f=references.find(ptr);
				if(f==references.end()) return;

				f->second++;
			}
			
			/// Decreases the reference count of the given object. If it is not registered, this request is ignored.
			/// If reference count of the object reaches 0, it is deleted.
			void Decrease(const Data &data) {
				ASSERT(data.GetData().IsPointer(), "Reference keeping can only be performed for reference types, "
					   "offender: "+data.GetType().GetName(), 1, 4);

				void *ptr=data.GetData().Pointer();
				auto f=references.find(ptr);
				if(f==references.end()) return;

				int &v=f->second;
				v--;
				if(v<=0) {
					data.GetType().Delete(data);
					references.erase(f);
				}
			}
			
			/// Resets the reference count to 0
			void Reset(const Data &data) {
				ASSERT(data.GetData().IsPointer(), "Reference keeping can only be performed for reference types, "
					   "offender: "+data.GetType().GetName(), 1, 4);

				void *ptr=data.GetData().Pointer();
				auto f=references.find(ptr);
				if(f==references.end()) return;

				int &v=f->second;
				v=0;
			}
			
			bool IsRegistered(const Data &data) const {
				if(!data.IsValid()) return false;
				
				ASSERT(data.GetData().IsPointer(), "Reference keeping can only be performed for reference types, "
				"offender: "+data.GetType().GetName(), 1, 4);
				
				void *ptr=data.GetData().Pointer();
				auto f=references.find(ptr);
				if(f==references.end()) return false;
				
				return true;
			}
			
			/// Unregisters an object from reference counter
			void Unregister(const Data &data) {
				ASSERT(data.GetData().IsPointer(), "Reference keeping can only be performed for reference types, "
				"offender: "+data.GetType().GetName(), 1, 4);
				
				void *ptr=data.GetData().Pointer();
				references.erase(ptr);
			}
			
			/// Never use without a proper reason. Gets rid of the data without destroying its content. It does
			/// decrease reference count. However, if it hits 0 it will 
			void GetRidOf(Data &data) {
				ASSERT(data.GetData().IsPointer(), "Reference keeping can only be performed for reference types, "
				"offender: "+data.GetType().GetName(), 1, 4);
				
				void *ptr=data.GetData().Pointer();
				auto f=references.find(ptr);
				if(f==references.end()) return;
				
				int &v=f->second;
				v++;
				data=Data::Invalid();
				v--;
			}

		private:
			std::map<void*, int> references;
		};
		
		/// This class represents a variable. It contains the data and the name of the variable.
		class Variable : public Data {
		public:
			
			/// Constructor that sets the name, type and value of the variable. Unless this variable is
			/// declared inside an executing code, definedin should be left nullptr.
			Variable(const std::string &name, const Type &type, const Any &value, const InputSource *definedin=nullptr) : 
			name(name), Data(type, value), definedin(definedin) {
			}
			
			
			/// Constructor that sets the name, and type of the variable. Default value of the specified
			/// type is used as value. Unless this variable is declared inside an executing code, definedin 
			/// should be left nullptr.
			Variable(const std::string &name, const Type &type, const InputSource *definedin=nullptr) :
			name(name), Data(type), definedin(definedin) {
			}
			
			
			/// Sets the data contained in this variable without changing its type
			void Set(Any value) {
				data.Swap(value);
			}
			
			/// Sets the data contained in this variable by modifying its type. Also this function
			/// resets the tags unless they are re-specified
			void Set(const Type &type, Any value) {
				data.Swap(value);
				this->type=&type;
			}

			/// Returns the name of the variable
			std::string GetName() const {
				return name;
			}

			/// Checks if this variable is defined in the given scope. Used in determining automatic global
			/// marks
			bool IsDefinedIn(const InputSource &source) {
				return &source==definedin;
			}
			
		private:
			std::string name;
			const InputSource *definedin;
		};
		
		/**
		 * This class stores information about a scope that is built by a scoped keyword.
		 */
		class KeywordScope { 
		public:
			/// Constructor requires the function that is creating this scope and data associated
			/// with it. Data type checking will not be performed on embedded keywords.
			KeywordScope(const Function &fn, Data data, unsigned long pline) : data(data), fn(&fn), pline(pline) {
			}
			
			/// Returns the data associated with this scope
			Data GetData() const {
				return data;
			}
			
			/// Replaces the current data of this scope with another. Useful for modified value type
			/// scope data
			void SetData(Data data) {
				this->data=data;
			}

			/// Returns the line number 
			unsigned long GetPhysicalLine() const {
				return pline;
			}
			
			/// Signals the end of the scope. If return value is false, this means that the scope is not
			/// yet ended. CallEnd function is responsible to move execution point back inside the keyword
			/// scope.
			bool CallEnd() const {
				return fn->CallEnd(data);
			}
			
			/// Returns the function that created this scope
			const Function &GetFunction() const {
				return *fn;
			}
			
		private:
			const Function *fn;
			Data data;
			unsigned long pline;
		};
		
		/**
		 * This class contains information about a variable scope. Variable scopes are generally created
		 * by functions.
		 */
		class VariableScope { 
		public:
			
			/// Scoping mode dictates what variable scoping mode should be used for auto allocated variables
			enum ScopingMode {
				DefaultGlobal,
				DefaultLocal,
				LimitGlobals
			};
			
			/// Default constructor requires a name and working mode. Name is generally name of the
			/// function that opens the scope, but could also contain additional information like values
			/// of parameters (not necessary)
			VariableScope(const std::string &name, ScopingMode mode=DefaultLocal) : 
			name(name+" #"+String::From(nextid++)), mode(mode) { }
			
			/// Name of the variable scope
			std::string GetName() const {
				return name;
			}
			
			/// Returns the scoping mode for the variables that are auto allocated, i.e $var = val;
			/// instead of local $var = val; or global $var = val;
			ScopingMode GetScopingMode() const {
				return mode;
			}
			
			/// Variables defined in this scope
			Containers::Hashmap<std::string, class Variable, &Variable::GetName, std::map, String::CaseInsensitiveLess> Variables;
			
		private:
			std::string name;

			static int nextid;
			
			ScopingMode mode;
		};
		
		
		/// This class uniquely represents a source code line. uintptr_t is used for source
		/// to reduce dependency
		class SourceMarker {
			friend class ExecutionScope;
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
		
		class ExecutionScope { 
		public:
			
			/// Constructor requires an input source. Execution scopes can share same input source
			ExecutionScope(InputSource &source) : source(&source) {
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
				auto ret=source->ReadInstruction(current);
				current++;
				
				return ret;
			}
			
			/// Forces the compilation of entire input source
			void Compile() {
				int c=current;
				while(source->ReadInstruction(c++)) ;
			}			
			
			/// Returns the code at the current line without incrementing it.
			const Instruction *Peek() {
				return source->ReadInstruction(current);
			}
			
			/// Returns the code at the given line without incrementing current line.
			const Instruction *Peek(unsigned long line) {
				return source->ReadInstruction(line);
			}
			
			void MoveToEnd() {
				current=source->ReadyInstructionCount();
			}

			InputSource &GetSource() const { return *source; }
			
		private:
			/// Current logical line
			unsigned long current = 0;
			
			/// InputSource for the current execution scope
			InputSource *source;
		};
		
		
		///@cond INTERNAL
		class CustomFunction : public Function {
		public:
			
		private:
			InputSource *source;
		};
		///@endcond
		
		
	}
}
