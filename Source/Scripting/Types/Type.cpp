#include "../../Any.h"
#include "../Reflection.h"
#include "../Embedding.h"


namespace Gorgon {
	namespace Scripting {
		
		std::string TypeToStr(const Type &type) {
			return type.GetName();
		}
		
		static Type *type=nullptr;
		
		const Type *TypeType() {
			if(type==nullptr) {
				type=new Scripting::MappedReferenceType<const Type, &TypeToStr>("Type",
					"Contains information about a type. Also contains "
					"functions for various purposes. Types are immutable."
				);
			}
			
			return type;
		}
		
		void InitTypeType() {
			if(type->Functions.GetCount()==0) {				
				type->AddFunctions({
					new Scripting::MappedFunction{"Name",
						"Returns the name of the type", 
						Types::String(), type, ParameterList(),
						MappedFunctions(MapConstReferenceMemberFunction(&Type::GetName)), MappedMethods()
					},
					new Scripting::MappedFunction{"Help",
						"Returns help for the type", 
						Types::String(), type, ParameterList(),
						MappedFunctions(MapConstReferenceMemberFunction(&Type::GetHelp)), MappedMethods()
					},
				});
			}
		}
	}
}
