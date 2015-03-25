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
					new Scripting::Function{"Name",
						"Returns the name of the type", type,
					    {
							MapFunction(
								[](const Type *o) { 
									return o->GetName();
								}, Types::String(),
								{ }
							)
						}
					},

					new Scripting::Function{"Help",
						"Returns help for the type", type,
					    {
							MapFunction(
								[](const Type *o) { 
									return o->GetHelp();
								}, Types::String(),
								{ }
							)
						}
					},

					new Scripting::Function{"[]",
						"Creates a new array for this type.", type, 
					    {
							MapFunction(
								&BuildArray, ArrayType(),
								{
									Parameter("Elements",
										"The newly constructed array will be filled with these elements",
										Types::Variant()/*, OptionalTag*///...
									)
								},
								RepeatTag
							)
						}
					},
				});
			}
		}
	}
}
