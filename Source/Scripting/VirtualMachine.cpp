#include "VirtualMachine.h"
#include "Exceptions.h"
#include "../Scripting.h"



namespace Gorgon {
	
	namespace Scripting {		
		
		Containers::Hashmap<std::thread::id, VirtualMachine> VirtualMachine::activevms;
		
		VirtualMachine::VirtualMachine(bool automaticreset, std::ostream &out, std::istream &in) : 
		Libraries(libraries), output(&out), input(&in), 
		defoutput(&out), definput(&in), automaticreset(automaticreset)
		{ 
			variablescopes.AddNew("[main]", VariableScope::DefaultLocal);
			libraries.Add(Integrals);
			libraries.Add(Keywords);
		}
		
		void VirtualMachine::AddLibrary(const Library &library) { 
			libraries.Add(library);
			if(alllibnames!="") alllibnames+=", ";
			alllibnames+=library.GetName();
		}
		
		void VirtualMachine::RemoveLibrary(const Library &library) {
			libraries.Remove(library.GetName());
			alllibnames="";
			for(const auto &lib : libraries) {
				if(alllibnames!="") alllibnames+=", ";
				alllibnames+=lib.first;
			}
		}
		
		void VirtualMachine::SetOutput(std::ostream &out) {
			output=&out;
		}
		
		void VirtualMachine::SetInput(std::istream &in) {
			input=&in;
		}
		
		const Type &VirtualMachine::FindType(std::string name) {
			using std::swap;
			
			std::string namespc=String::Extract(name, ':');
			if(name=="") swap(namespc, name);
			
			if(namespc!="") {
				auto lib=libraries.Find(namespc);
				if(!lib.IsValid()) {
					throw SymbolNotFoundException(
						namespc, SymbolType::Library, 
						"Cannot find "+namespc+" library while looking for type "+name
					);
				}
				
				auto element=lib.Current().second.Types.Find(name);
				
				if(!element.IsValid()) {
					throw SymbolNotFoundException(
						namespc+":"+name, SymbolType::Type, 
						"Cannot find "+name+" type in library "+namespc
					);
				}
				
				return element.Current().second;
			}
			else { //search all
				const Type *foundelement;
				std::string foundlibnames;
				int found=0;
				
				for(const auto &lib : libraries) {
					auto element=lib.second.Types.Find(name);
					if(element.IsValid()) {
						found++;
						foundelement=&element.Current().second;
						if(foundlibnames!="") foundlibnames+=", ";
						foundlibnames+=lib.first;
					}
				}
				
				if(found==0) {
					throw SymbolNotFoundException(
						name, SymbolType::Type, 
						"Cannot find "+name+" type in all loaded libraries: "+alllibnames
					);
				}
				else if(found>1) {
					throw AmbiguousSymbolException(
						name, SymbolType::Type,
						name+" type found in following libraries: "+foundlibnames
					);
				}
				
				return *foundelement;
			}
		}
		
		const Function &VirtualMachine::FindFunction(std::string name) {
			using std::swap;
			
			std::string namespc=String::Extract(name, ':');
			if(name=="") swap(namespc, name);
			
			if(namespc=="") { //search all
				const Function *foundelement;
				std::string foundlibnames;
				int found=0;
				
				for(const auto &lib : libraries) {
					auto element=lib.second.Functions.Find(name);
					if(element.IsValid()) {
						found++;
						foundelement=&element.Current().second;
						if(foundlibnames!="") foundlibnames+=", ";
						foundlibnames+=lib.first;
					}
				}
				
				if(found==0) {
					throw SymbolNotFoundException(
						name, SymbolType::Function, 
						"Cannot find "+name+" function in all loaded libraries: "+alllibnames
					);
				}
				else if(found>1) {
					throw AmbiguousSymbolException(
						name, SymbolType::Function,
						name+" function found in following libraries: "+foundlibnames
					);
				}
				
				return *foundelement;
			}
			else if(name.find_first_of(':')!=name.npos) { // both namespace and type is given
				std::string typname = String::Extract(name, ':');
				
				auto lib=libraries.Find(namespc);
				if(!lib.IsValid()) {
					throw SymbolNotFoundException(
						namespc, SymbolType::Library, 
						"Cannot find "+namespc+" library while looking for function "+name
					);
				}
				
				auto type=lib.Current().second.Types.Find(typname);
				if(!type.IsValid()) {
					throw SymbolNotFoundException(
						namespc+":"+typname, SymbolType::Type, 
						"Cannot find "+typname+" type while looking for "+name+" function in library "+namespc
					);
				}
				
				auto element=type.Current().second.Functions.Find(name);				
				
				if(!element.IsValid()) {
					throw SymbolNotFoundException(
						namespc+":"+name, SymbolType::Function, 
						"Cannot find "+name+" function in type "+namespc+":"+typname
					);
				}
				
				return element.Current().second;
			}
			else { // namespc can either be type or library
				auto lib=libraries.Find(namespc);
				if(!lib.IsValid()) {
					const Type *type=nullptr;
					
					try {
						type=&FindType(namespc);
					}
					catch(const SymbolNotFoundException &) {
					}
					catch(...) { 
						throw;
					}
					
					throw SymbolNotFoundException(
						namespc, SymbolType::Namespace, 
						"Cannot find "+namespc+" namespace while looking for function "+name
					);
					
					auto element=type->Functions.Find(name);
					
					if(!element.IsValid()) {
						throw SymbolNotFoundException(
							namespc+":"+name, SymbolType::Function, 
							"Cannot find "+name+" function in library "+namespc
						);
					}
					
					return element.Current().second;
				}
				else {				
					auto element=lib.Current().second.Functions.Find(name);
					
					if(!element.IsValid()) {
						throw SymbolNotFoundException(
							namespc+":"+name, SymbolType::Function, 
							"Cannot find "+name+" function in library "+namespc
						);
					}
					return element.Current().second;
				}
				
			}
		}
		
		
		const Constant &VirtualMachine::FindConstant(std::string name) {
			using std::swap;
			
			std::string namespc=String::Extract(name, ':');
			if(name=="") swap(namespc, name);
			
			if(namespc=="") { //search all
				const Constant *foundelement;
				std::string foundlibnames;
				int found=0;
				
				for(const auto &lib : libraries) {
					auto element=lib.second.Constants.Find(name);
					if(element.IsValid()) {
						found++;
						foundelement=&element.Current().second;
						if(foundlibnames!="") foundlibnames+=", ";
						foundlibnames+=lib.first;
					}
				}
				
				if(found==0) {
					throw SymbolNotFoundException(
						name, SymbolType::Constant, 
						"Cannot find "+name+" constant in all loaded libraries: "+alllibnames
					);
				}
				else if(found>1) {
					throw AmbiguousSymbolException(
						name, SymbolType::Constant,
						name+" contstant found in following libraries: "+foundlibnames
					);
				}
				
				return *foundelement;
			}
			else if(name.find_first_of(':')!=name.npos) { // both namespace and type is given
				std::string typname = String::Extract(name, ':');
				
				auto lib=libraries.Find(namespc);
				if(!lib.IsValid()) {
					throw SymbolNotFoundException(
						namespc, SymbolType::Library, 
						"Cannot find "+namespc+" library while looking for constant "+name
					);
				}
				
				auto type=lib.Current().second.Types.Find(typname);
				if(!type.IsValid()) {
					throw SymbolNotFoundException(
						namespc+":"+typname, SymbolType::Type, 
						"Cannot find "+typname+" type while looking for "+name+" constant in library "+namespc
					);
				}
				
				auto element=type.Current().second.Constants.Find(name);				
				
				if(!element.IsValid()) {
					throw SymbolNotFoundException(
						namespc+":"+name, SymbolType::Constant, 
						"Cannot find "+name+" constant in type "+namespc+":"+typname
					);
				}
				
				return element.Current().second;
			}
			else { // namespc can either be type or library
				auto lib=libraries.Find(namespc);
				if(!lib.IsValid()) {
					const Type *type=nullptr;
					
					try {
						type=&FindType(namespc);
					}
					catch(const SymbolNotFoundException &) {
					}
					catch(...) { 
						throw;
					}
					
					throw SymbolNotFoundException(
						namespc, SymbolType::Namespace, 
						"Cannot find "+namespc+" namespace while looking for constant "+name
					);
					
					auto element=type->Constants.Find(name);
					
					if(!element.IsValid()) {
						throw SymbolNotFoundException(
							namespc+":"+name, SymbolType::Constant, 
							"Cannot find "+name+" constant in library "+namespc
						);
					}
					
					return element.Current().second;
				}
				else {				
					auto element=lib.Current().second.Constants.Find(name);
					
					if(!element.IsValid()) {
						throw SymbolNotFoundException(
							namespc+":"+name, SymbolType::Constant, 
							"Cannot find "+name+" constant in library "+namespc
						);
					}
					return element.Current().second;
				}
				
			}
		}		
		
		
	}
}