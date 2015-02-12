#include "../Scripting.h"
#include "Reflection.h"
#include "VirtualMachine.h"

namespace Gorgon { namespace Scripting {	
	
	bool Function::CallEnd(Data data) const { 
		assert( false && "End call on a non-scoped function"); 
		
		return false;
	}
	
	void Function::CallRedirect(Data,std::string &) const { 
		assert( false && "Redirect call on a non-redirecting function");
	}
	
	void Function::init() {
		if(keyword) {
			KeywordNames.insert(name);
		}
	}
	
	Type *TypeType();
	
	Library::Library(const std::string &name, const std::string &help,
	TypeList types, FunctionList functions, ConstantList constants) :
	name(name), help(help), Types(this->types), Functions(this->functions), Constants(this->constants)
	{
		using std::swap;
		
		swap(types, this->types);
		swap(functions, this->functions);
		swap(constants, this->constants);
		
		for(const auto &type : this->types) {
			this->constants.Add(
				new Constant(type.first, type.second.GetHelp(), {TypeType(), &type.second})
			);
		}
	}
	
	void Library::AddTypes(const std::vector<Type*> &list) {
		for(auto &type : list) {
			ASSERT(!SymbolExists(type->GetName()), "Symbol "+type->GetName()+" already exists", 1, 2);

			types.Add(type);

			this->constants.Add(
				new Constant(type->GetName(), type->GetHelp(), {TypeType(), type})
			);
		}
	}
	
	Data Type::Construct(const std::vector<Data> &parameters) const {
		std::multimap<int, const Function *> rankedlist;
		
		for(const auto &fn : constructors) {
			int status=0;
			
			auto pin = parameters.begin();
			for(const auto &pdef : fn.Parameters) {
				if(pin==parameters.end()) {
					if(pdef.IsOptional()) { 
						continue; 
					}
					else {
						status=-1;
						break;
					}
				}
				
				if(pdef.GetType()==pin->GetType()) {
					// perfect match
				}
				else if(pdef.GetType().GetTypeCasting(pin->GetType())) {
					// good match
					status++; 
				}
				else {
					// bad bad match
					status+=2;
				}
				
				++pin;
			}
			if(status==-1) break;
			
			if(pin!=parameters.end()) {
				if(fn.parameters.GetCount() && fn.RepeatLast()) {
					int worst=0;
					const auto &pdef=*fn.parameters.Last();
					while(pin!=parameters.end()) {
						if(pdef.GetType()==pin->GetType()) {
							// perfect match
						}
						else if(pdef.GetType().GetTypeCasting(pin->GetType())) {
							// good match
							if(worst<1) worst=1;
						}
						else {
							// bad bad match
							worst=2;
						}
						
						++pin;
					}
					
					status+=worst;
				}
				else {
					status=-1;
				}
			}
			
			if(status!=-1) {
				rankedlist.insert(std::make_pair(status, &fn));
			}
		}
		
		if(rankedlist.size()==0) {
			std::string pnames;
			for(const auto &param : parameters) {
				if(pnames!="") pnames += ", ";
				pnames+=param.GetType().GetName();
			}
			throw SymbolNotFoundException("Constructor", SymbolType::Function, 
										  "Constructor for "+name+" with parameters: "+pnames+" not found");
		}
		else {
			return rankedlist.begin()->second->Call(false, parameters);
		}
	}
	
} }