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
	
	
} }