#include "../../Any.h"
#include "../Reflection.h"
#include "../Embedding.h"
#include "Array.h"


namespace Gorgon {
	namespace Scripting {
		
		std::string FunctionToStr(const Function &fn) {
			return fn.GetName();
		}
		
		static Type *fn=nullptr;
		
		Type *FunctionType() {
			if(fn==nullptr) {
				fn=new Scripting::MappedReferenceType<const Function, &FunctionToStr>("Function",
					"Contains information about a function, also allows them to be called. Functions are immutable."
				);
				
				fn->AddFunctions({
					new Scripting::MappedFunction{"Name",
						"Returns the name of the function", 
						Types::String(), fn, ParameterList(),
						MappedFunctions([](const Function *o){return o->GetName();}), MappedMethods()
					},
					new Scripting::MappedFunction{"Help",
						"Returns help for the function", 
						Types::String(), fn, ParameterList(),
						MappedFunctions([](const Function *o) {return o->GetHelp(); }), MappedMethods()
					},
				});
			}
			
			return fn;
		}
	}
}
