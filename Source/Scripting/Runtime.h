/// @file Runtime.h This file contains classes that stores runtime and end programmer defined objects.

#pragma once

#include <ostream>
#include <string>


#include "Reflection.h"
#include "Data.h"
#include "InputSource.h"

namespace Gorgon {
	
	namespace Scripting {
		
		
		/// This class represents a variable. It contains the data and the name of the variable.
		class Variable : public Data {
		public:
			
			/// Constructor that sets the name, type and value of the variable. Unless this variable is
			/// declared inside an executing code, definedin should be left nullptr.
			Variable(const std::string &name, Type &type, Any value, const InputSource *definedin=nullptr) : 
			name(name), Data(type, value), definedin(definedin) {
			}
			
			
			/// Constructor that sets the name, and type of the variable. Default value of the specified
			/// type is used as value. Unless this variable is declared inside an executing code, definedin 
			/// should be left nullptr.
			Variable(const std::string &name, Type &type, const InputSource *definedin=nullptr) : 
			name(name), Data(type), definedin(definedin) {
			}
			
			
			/// Sets the data contained in this variable without changing its type
			void Set(Any value) {
				data=value;
			}
			
			/// Sets the data contained in this variable by modifying its type. Also this function
			/// resets the tags unless they are re-specified
			void Set(Type &type, Any value) {
				data=value;
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
			KeywordScope(const Function &fn, Data data) : data(data), fn(fn) {
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
			
			/// Signals the end of the scope. If return value is false, this means that the scope is not
			/// yet ended. CallEnd function is responsible to move execution point back inside the keyword
			/// scope.
			bool CallEnd() const {
				return fn.CallEnd(data);
			}
			
		private:
			const Function &fn;
			Data data;
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
				DefaultLocal
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
			Containers::Hashmap<std::string, Variable, &Variable::GetName> Variables;
			
		private:
			std::string name;

			static int nextid;
			
			ScopingMode mode;
		};
		
		
		/// This class uniquely represents a source code line. uintptr_t is used for source
		/// to reduce dependency
		class SourceMarker {
		public:
			unsigned long Line;
			uintptr_t 	  Source;
			
			bool operator <(const SourceMarker &other) {
				return (Source == other.Source ? Line<other.Line : Source<other.Source);
			}
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
				return current;
			}
			
			/// Returns a unique identifier for the current source code line. This information can be
			/// used to go back across execution scopes. Useful for Try Catch like structures.
			SourceMarker GetMarker() const {
				return {current, reinterpret_cast<uintptr_t>(this)};
			}
			
			/// Returns the code at the current line and increments the current line
			bool Get(std::string &data) {
				bool ret=source->GetLine(current, data);
				current++;
				
				return ret;
			}
			
			/// Returns the code at the current line without incrementing it.
			bool Peak(std::string &data) {
				return source->GetLine(current, data);
			}
			
			/// Returns the code at the given line without incrementing current line.
			bool Peak(unsigned long line, std::string &data) {
				return source->GetLine(line, data);
			}
			
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
