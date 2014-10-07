#pragma once

#include <iostream>
#include <string>
#include <thread>

#include "../Types.h"
#include "../Event.h"
#include "../Containers/Collection.h"
#include "../Containers/Hashmap.h"

#include "Reflection.h"
#include "Runtime.h"
#include "InputSource.h"
#include "Data.h"
#include "Exceptions.h"



namespace Gorgon {
	
	namespace Scripting {
		
		
		/// This class defines a virtual environment for scripts to run. It determines
		/// which additional functions the script can access, working directory, and safety
		/// level.
		class VirtualMachine {
		public:

			/// Default constructor
			VirtualMachine(bool automaticreset=true, std::ostream &out=std::cout, std::istream &in=std::cin);

			/// Executes a single statement in this virtual machine. This operation will create a new
			/// input scope and will not affect current scope
			bool ExecuteStatement(const std::string &code);

			/// This method starts the virtual machine
			void Run();

			/// This method starts the virtual machine
			void Run(unsigned executiontarget);


			/// This method starts the virtual machine with the given input source
			void Start(InputSource &source);

			/// Includes a new library to be used in this virtual machine
			void AddLibrary(const Library &library);

			/// Removes a library
			void RemoveLibrary(const Library &library);

			/// Finds the given type by name. Namespace information will be extracted if exists. 
			/// This function may throw symbol not found / ambiguous symbol.
			const Type &FindType(std::string name);

			/// Finds the given type by name. Namespace information will be extracted if exists. 
			/// This function may throw symbol not found / ambiguous symbol.
			const Function &FindFunction(std::string name);

			/// Finds the given type by name. Namespace information will be extracted if exists. 
			/// This function may throw symbol not found / ambiguous symbol.
			const Constant &FindConstant(std::string name);

			/// Sets the input source to read code lines from. Does not change active execution context.
			/// only allows 
			void AddInputSource(InputSource &source);

			/// Changes the current executing line.
			void Jump(unsigned long line);

			/// Changes the current executing line. This function checks if the marker is in this execution
			/// scope. If not, it throws.
			void Jump(SourceMarker marker);

			/// Changes current executing line. This function can jump to a previous execution point. Useful
			/// for keywords like try/catch
			void LongJump(SourceMarker marker);


			/// Returns the current VM for this thread.
			static VirtualMachine &Get() {
				if(!activevms.Exists(std::this_thread::get_id())) {
					throw std::runtime_error("No active VMs for this thread.");
				}

				return activevms[std::this_thread::get_id()];
			}

			Variable &GetVariable(const std::string &name);

			void SetVariable(const std::string &name, Data data);

			/// Creates a new InputSource using a console input provider. Also creates an activates
			/// a new execution scope using this input source.
			void AttachCommandConsole();

			/// If there is an attached command console, this function detaches that console, 
			/// stops execution and returns true. Otherwise, it returns false.
			bool DetachCommandConsole();

			/// Redirects the output stream to the given stream
			void SetOutput(std::ostream &out);

			/// Redirects input stream to the given stream
			void SetInput(std::istream &in);

			/// Returns the output stream
			std::ostream &GetOutput() const {
				return *output;
			}

			/// Returns the input stream.
			std::istream &GetInput() const {
				return *input;
			}

			/// Resets the output stream to default stream that is given in the constructor
			void ResetOutput() {
				output=defoutput;
			}

			/// Resets the input stream to default stream that is given in the constructor
			void ResetInput() {
				input=definput;
			}

			/// Activate this VM for this thread. This VM will automatically activate when Start
			/// is issued, therefore, this is mostly used for debugging
			void Activate() {
				activevms.Add(std::this_thread::get_id(), this);
			}

			/// Puts the VM into skipping mode. In skipping mode, VM will parse the commands but
			/// will not execute them unless they are marked as no skip. VM can be placed into
			/// skipping mode by any function, not necessarily by a scoped keyword.
			void StartSkipping() {
				if(skipping) {
					throw FlowException("Already skipping.", "The VM is already placed in skipping mode "
						"by another keyword. Quiting to evade further problems.");
				}
				skipping=true;
			}

			/// VM will stop skipping. 
			void StopSkipping() {
				if(skippingdepth) {
					throw FlowException("Cannot stop skipping, code execution should be resumed by the"
						"scope that started it.", "The VM is placed in skipping mode by another scope."
						"Quiting to evade further problems.");
				}
				skipping=false;
			}

			/// Returns whether the VM is in skipping mode
			bool IsSkipping() const {
				return skipping;
			}

			/// This function returns the depth of skipping. Depth of skipping is the number of silent
			/// (skipped) scoped keywords that still need to be terminated. For instance, if there are
			/// two if scopes inside each other and both have else statements and if exterior if statement
			/// is false, the interior else may want to resume code execution even if it is not the else
			/// statement that should resume code execution. However, at the point interior else is reached,
			/// skipping depth would be 1, denoting the code skipping is now inside another scope and should
			/// not be stopped.
			int SkippingDepth() const {
				return skippingdepth;
			}

			/// Reduces the skipping count
			void ReduceSkipping() {
				if(skippingdepth==0) {
					throw FlowException("Cannot reduce skipping, It is already been reduced to 0.");
				}
				skippingdepth--;
			}

			/// Returns if there is an active keyword scope
			bool HasKeywordScope() const {
				return keywordscopes.GetCount()!=0;
			}

			/// Returns current keyword scope
			KeywordScope &GetKeywordScope() {
				if(keywordscopes.GetCount()==0) {
					throw FlowException("No active keyword scope.");
				}
				return *keywordscopes.Last();
			}

			/// Removes the last keyword scope, removing skipping mode if it is set
			void PopKeywordScope() {
				if(keywordscopes.GetCount()==0) {
					throw FlowException("No active keyword scope.");
				}
				keywordscopes.Last().Delete();
				skipping=false;
			}

			/// Returns the name of the current variable scope. This could be a function name or
			/// [main] if no function is yet called. Embedded functions do not have their own
			/// variable scopes, therefore, this function will return the encompassing scope.
			std::string GetVariableScopeName() const {
				if(variablescopes.GetCount()==0) {
					throw FlowException("No active variable scope.", "This shows that VM is currently in "
						"an invalid state.");
				}
				return variablescopes.Last()->GetName();
			}

			/// Returns the number of active execution scopes. If this number is 0, VM cannot be started without
			/// providing additional code source.
			unsigned GetExecutionScopeCount() const {
				return executionscopes.GetCount();
			}

			/// Resets any runtime information that this VM has. This includes all scopes and global
			/// variables
			void Reset();

			//TODO: events

			/// Allows read-only access to libraries
			const Containers::Hashmap<std::string, const Library, &Library::GetLowercaseName> &Libraries;

		private:
			Data callfunction(const Function *fn, bool method, const std::vector<Value> &params);
			Data getvalue(const Value &val);
			void functioncall(const Instruction *inst, bool memberonly, bool method);

			/// All libraries that are available globally. 
			Containers::Hashmap<std::string, const Library, &Library::GetLowercaseName> libraries;

			std::string alllibnames;


			/// If set, VM will reset itself as soon as the execution is stopped
			bool automaticreset;


			Containers::Collection<KeywordScope> 	keywordscopes;
			Containers::Collection<ExecutionScope> 	executionscopes;
			Containers::Collection<VariableScope> 	variablescopes;

			Containers::Collection<InputSource>		inputsources;

			Library runtime;

			Containers::Hashmap<std::string, Variable, &Variable::GetLowercaseName>	globalvariables;
			std::map<Function*, Containers::Hashmap<std::string, Variable>> staticvariables;

			std::ostream *output;
			std::istream *input;

			std::ostream *defoutput;
			std::istream *definput;

			bool skipping = false;
			int  skippingdepth = 0;
			bool markednoskip = false;
			const Function *markedkeyword=nullptr;

			std::vector<Data> temporaries;

			
			/// List of active VMs. A VM can be active on more than one thread. But it cannot
			/// execute two different contexts.
			static Containers::Hashmap<std::thread::id, VirtualMachine> activevms;
		};
		
		
	}
}
