#include "Embedding.h"
#include "../Scripting.h"

namespace Gorgon { namespace Scripting {
	
	
	Data EmbeddedFunction::Call(bool ismethod, const std::vector<Data> &parameters) const {
		if(ismethod) {
			if(method) {
				method(parameters);
			}
			else {
				auto ret = function(parameters);
				VirtualMachine::Get().GetOutput()<<ret;
			}
			return Data::Invalid();
		}
		else {
			return function(parameters);
		}
	}
	
	
} }