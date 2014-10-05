#include "../Scripting.h"
#include "Reflection.h"
#include "VirtualMachine.h"
#include "Embedding.h"

namespace Gorgon { namespace Scripting {
	
	Data::Data(const Type& type) : type(&type) {
		data = type.GetDefaultValue();
	}
	
	void Data::check() {
		ASSERT(type->GetDefaultValue().IsSameType(data), "Given data type does not match with: "+type->GetName(), 2, 2);
	}
	
	bool Function::CallEnd(Data data) const { 
		assert( false && "End call on a non-scoped function"); 

		return false;
	}
	
	void Function::CallRedirect(Data,std::string &) const { 
		assert( false && "Redirect call on a non-redirecting function");
	}
	
	Data GetVariableValue(const std::string &varname) { throw 0; }
	
	MappedValueType<Data, String::From<Data>, GetVariableValue> Variant = {"Variant", 
		"This type can contain any type.",
		Data::Invalid()
	};
	
	int VariableScope::nextid=0;
} }
