#include "../../Any.h"
#include "../Reflection.h"
#include "../Embedding.h"
#include "Array.h"


namespace Gorgon {
	namespace Scripting {
		
		std::string TypeToStr(const Type &type) {
			return type.GetName();
		}
		
		static Type *type=nullptr;
		
		Type *TypeType() {
			if(type==nullptr) {
				type=new Scripting::MappedReferenceType<const Type, &TypeToStr>("Type",
					"Contains information about a type. Also contains "
					"functions for various purposes. Types are immutable."
				);
			}
			
			return type;
		}

		Array *BuildArray(const Type *type, std::vector<Data> data);
		Type *ArrayType();
		
		void InitTypeType() {
			if(type->Functions.GetCount()==0) {				
				type->AddFunctions({
					new Scripting::MappedFunction{"Name",
						"Returns the name of the type", 
						Types::String(), type, ParameterList(),
						MappedFunctions([](const Type *o){return o->GetName();}), MappedMethods()
					},
					new Scripting::MappedFunction{"Help",
						"Returns help for the type", 
						Types::String(), type, ParameterList(),
						MappedFunctions([](const Type *o) {return o->GetHelp(); }), MappedMethods()
					},
					new Scripting::MappedFunction{"[]",
						"Creates a new array for this type.",
						ArrayType(), type, ParameterList(
							new Parameter{"Elements",
								"The newly constructed array will be filled with these elements",
								Types::Variant(), OptionalTag
							}
						),
						MappedFunctions(&BuildArray, [](const Type *type){ return BuildArray(type, {}); }), MappedMethods(),
						RepeatTag
					},
				});
			}
		}
	}
}
