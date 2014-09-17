#include "../Scripting.h"

namespace Gorgon { namespace Scripting {
	
	Containers::Hashmap<std::thread::id, VirtualMachine> VirtualMachine::activevms;
	
} }