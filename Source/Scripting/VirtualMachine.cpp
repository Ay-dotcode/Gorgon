#include "VirtualMachine.h"
#include "Exceptions.h"



namespace Gorgon {
	
	namespace Scripting {		
		
		Containers::Hashmap<std::thread::id, VirtualMachine> VirtualMachine::activevms;
		
		
		const Type &VirtualMachine::FindType(const std::string &name, const std::string &namespc) {
			if(namespc!="") {
				auto lib=libraries.Find(namespc);
				if(!lib.IsValid()) {
					throw SymbolNotFoundException(
						namespc, SymbolType::Namespace, 
						"Cannot find "+namespc+" namespace while looking for type "+name
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
		
		const Function &VirtualMachine::FindFunction(const std::string &name, const std::string &namespc) {
			if(namespc!="") {
				auto lib=libraries.Find(namespc);
				if(!lib.IsValid()) {
					throw SymbolNotFoundException(
						namespc, SymbolType::Namespace, 
						"Cannot find "+namespc+" namespace while looking for function "+name
					);
				}
				
				auto element=lib.Current().second.Functions.Find(name);
				
				if(!element.IsValid()) {
					throw SymbolNotFoundException(
						namespc+":"+name, SymbolType::Function, 
						"Cannot find "+name+" function in library "+namespc
					);
				}
				
				return element.Current().second;
			}
			else { //search all
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
		}
		
		
		const Constant &VirtualMachine::FindConstant(const std::string &name, const std::string &namespc) {
			if(namespc!="") {
				auto lib=libraries.Find(namespc);
				if(!lib.IsValid()) {
					throw SymbolNotFoundException(
						namespc, SymbolType::Namespace, 
						"Cannot find "+namespc+" namespace while looking for cosntant "+name
					);
				}
				
				auto element=lib.Current().second.Constants.Find(name);
				
				if(!element.IsValid()) {
					throw SymbolNotFoundException(
						namespc+":"+name, SymbolType::Constant, 
						"Cannot find "+name+" constant in library "+namespc
					);
				}
				
				return element.Current().second;
			}
			else { //search all
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
						name+" constant found in following libraries: "+foundlibnames
					);
				}
				
				return *foundelement;
			}
		}
		
		
		
	}
}