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



namespace Gorgon {
	
	namespace Scripting {
		
		
		/// This class defines a virtual environment for scripts to run. It determines
		/// which additional functions the script can access, working directory, and safety
		/// level.
		class VirtualMachine {
		public:
			
			/// Default constructor
			VirtualMachine(bool automaticreset=true, std::ostream &out=std::cout, std::istream &in=std::cin) : 
			Libraries(libraries), output(&out), input(&in), 
			defoutput(&out), definput(&in), automaticreset(automaticreset)
			{ 
				variablescopes.AddNew("[main]", VariableScope::DefaultLocal);
			}
			
			/// Executes a single statement in this virtual machine. This operation will create a new
			/// input scope and will not affect current scope
			bool ExecuteStatement(const std::string &code);
			
			/// This method starts the virtual machine
			void Start();
			
			/// Includes a new library to be used in this virtual machine
			void AddLibrary(const Library &library) { 
				libraries.Add(library);
				if(alllibnames!="") alllibnames+=", ";
				alllibnames+=library.GetName();
			}
			
			/// Removes a library
			void RemoveLibrary(const Library &library) {
				libraries.Remove(library.GetName());
				alllibnames="";
				for(const auto &lib : libraries) {
					if(alllibnames!="") alllibnames+=", ";
					alllibnames+=lib.first;
				}
			}
			
			/// Finds the given type by name. Unless namespc is specified, it will be searched
			/// from all libraries. This function may throw ambiguous symbol.
			const Type &FindType(const std::string &name, const std::string &namespc="");
			
			/// Finds the given function by name. Unless namespc is specified, it will be searched
			/// from all libraries. This function may throw ambiguous symbol.
			const Function &FindFunction(const std::string &name, const std::string &namespc="");
			
			/// Finds the given constant by name. Unless namespc is specified, it will be searched
			/// from all libraries. This function may throw ambiguous symbol.
			const Constant &FindConstant(const std::string &name, const std::string &namespc="");
			
			/// Sets the input source to read code lines from
			void AddInputSource(InputSource &source);
			
			/// Returns the current VM for this thread.
			static VirtualMachine &Get() {
				if(!activevms.Exists(std::this_thread::get_id())) {
					throw std::runtime_error("No active VMs for this thread.");
				}
				
				return activevms[std::this_thread::get_id()];
			}
			
			/// Redirects the output stream to the given stream
			void SetOutput(std::ostream &out) {
				output=&out;
			}
			
			/// Redirects input stream to the given stream
			void SetInput(std::istream &in) {
				input=&in;
			}
			
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
			
			/// Returns the name of the current variable scope. This could be a function name or
			/// [main] if no function is yet called. Embedded functions do not have their own
			/// variable scopes, therefore, this function will return the encompassing scope.
			std::string GetVariableScopeName() const {
				return variablescopes.Last()->GetName();
			}
			
			/// Resets any runtime information that this VM has. This includes all scopes and global
			/// variables
			void Reset();
			
			//TODO: events
			
			/// Allows read-only access to libraries
			const Containers::Hashmap<std::string, const Library, &Library::GetName> &Libraries;
			
		private:
			/// All libraries that are available globally. 
			Containers::Hashmap<std::string, const Library, &Library::GetName> libraries;
			
			std::string alllibnames;
			
			
			/// If set, VM will reset itself as soon as the execution is stopped
			bool automaticreset;

			
			Containers::Collection<KeywordScope> 	keywordscopes;
			Containers::Collection<ExecutionScope> 	executionscopes;
			Containers::Collection<VariableScope> 	variablescopes;
			
			Containers::Collection<InputSource>		inputsources;
			
			std::map<std::string, Variable> 					 globalvariables;
			std::map<Function*, std::map<std::string, Variable>> staticvariables;
			
			std::ostream *output;
			std::istream *input;
			
			std::ostream *defoutput;
			std::istream *definput;
			
			/// List of active VMs. A VM can be active on more than one thread. But it cannot
			/// execute two different contexts.
			static Containers::Hashmap<std::thread::id, VirtualMachine> activevms;
		};
		
		
	}
}