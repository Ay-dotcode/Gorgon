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
				fn=new Scripting::MappedReferenceType<Function, &FunctionToStr>("Function",
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
								{ }, ConstTag
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
								{ }, ConstTag
							)
						}
					),
				});
			}
			
			return fn;
		}

		std::string ParameterToStr(const Parameter &p) {
			return p.GetName();
		}

		Type *ParameterType() {
			static Type *param = nullptr;
			if(param==nullptr) {
				param=new Scripting::MappedValueType<Parameter, ParameterToStr, ParseThrow<Parameter>>(
					"Parameter", "A function parameter.", Parameter("", "", Types::Variant())
				);

				param->AddFunctions({
					new Scripting::Function("Name",
					"Returns the name of the parameter", param,
					{
						MapFunction(
							[](const Parameter &o) {
								return o.GetName();
							}, Types::String(),
							{ }, ConstTag
						)
					}
					),

					new Scripting::Function("Help",
					"Returns help for the parameter", param,
					{
						MapFunction(
							[](const Parameter &o) {
								return o.GetHelp();
							}, Types::String(),
							{ }, ConstTag
						)
					}
					),
				});
			}

			return param;
		}
	}
}
