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
					new Scripting::Function("Name",
						"Returns the name of the function", fn,
						{
							MapFunction(
								[](const Function *o) {
									return o->GetName();
								}, Types::String(),
								{ }
							)
						}
					),
					
					new Scripting::Function("Help",
						"Returns help for the function", fn,
						{
							MapFunction(
								[](const Function *o) {
									return o->GetHelp(); 
								}, Types::String(), 
								{ }
							)
						}
					),
				});
			}
			
			return fn;
		}
	}
}
