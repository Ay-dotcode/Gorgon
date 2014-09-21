#include "../Scripting.h"
#include "Reflection.h"

namespace Gorgon { namespace Scripting {
	
	Containers::Hashmap<std::thread::id, VirtualMachine> VirtualMachine::activevms;
	
	Data::Data(Type& type) : type(&type) {
		data = type.GetDefaultValue();
	}
	
	bool Function::CallEnd(Data data) const { 
		assert( false && "End call on a non-scoped function"); 

		return false;
	}
	
	void Function::CallRedirect(Data,std::string &) const { 
		assert( false && "Redirect call on a non-redirecting function");
	}
	
} }
