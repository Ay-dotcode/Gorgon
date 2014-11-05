#include "../Embedding.h"

#include "Array.h"

namespace Gorgon {
	namespace Scripting {

		// to avoid name clashes
		namespace {
			std::string ArrayToStr(const Array &array) {
				std::string ret;
				for(auto a : array.Elements) {
					if(ret!="") ret+=", ";
					else ret=array.GetType().GetName()+"[";
					
					ret+=array.GetType().ToString({array.GetType(), a});
				}
				if(ret=="") {
					ret=array.GetType().GetName()+"[";
				}
				ret+="]";
				
				return ret;
			}

			Array *Range2(int start, int end) {
				Array *ret=new Array(*Types::Int());

				if(start<end) {
					for(int i=start; i<end; i++) {
						ret->PushWithoutCheck(i);
					}
				}
				else {
					for(int i=start; i<end; i--) {
						ret->PushWithoutCheck(i);
					}
				}

				VirtualMachine::Get().References.Register(ret);

				return ret;
			}

			Array *Range3(int start, int end, int step) {
				Array *ret=new Array(*Types::Int());

				for(int i=start; i<end; i++) {
					ret->PushWithoutCheck(i);
				}

				return ret;
			}
		}
		
		Type *ArrayType() {
			static Type *array = nullptr;
			
			if(!array) {
				array=new MappedReferenceType<Array, ArrayToStr>("Array", "This is an array", (Array*)nullptr);
				
				array->AddFunctions({
					new MappedFunction{"[]", "Returns the element at the given index.",
						Variant, array, ParameterList{
							new Parameter { "Index",
								"The index of the element",
								Types::Unsigned()
							}
						},
						MappedFunctions([](Array *a, unsigned ind){return a->GetItemData(ind);}), MappedMethods()	
					},
					new MappedFunction{"Push", "Pushes a new element at the end of the array.",
						nullptr, array, ParameterList{
							new Parameter { "Element",
								"Element to be added.",
								Types::Variant()
							}
						},
						MappedFunctions(&Array::PushData), MappedMethods()	
					},
					new MappedFunction{"Resize", "Changes the size of the array. New elements will have their default value.",
						nullptr, array, ParameterList{
							new Parameter { "Size",
								"The new size.",
								Types::Unsigned()
							}
						},
						MappedFunctions(&Array::Resize), MappedMethods()	
					},
				});
			}
			
			return array;
		}

		Array *BuildArray(const Type *type, std::vector<Data> datav) {
			if(type==nullptr) return nullptr;

			Array *ret=new Array(*type);

			for(auto &data : datav) {
				ret->PushData(data);
			}


			return ret;
		}
		
		std::initializer_list<Function*> ArrayFunctions() {
			return {
				new MappedFunction{"Range",
					"Creates a range array between two numbers",
					ArrayType(), nullptr, ParameterList {
						new Parameter { "Start",
							"Starting value for the range. This value is included in the array.",
							Types::Int()
						},
						new Parameter { "End",
							"Ending value for the range. This value is not included in the array.",
							Types::Int()
						},
						new Parameter { "Step",
							"Stepping amount for the range. If not given its determined to be either 1 or -1.",
							Types::Int(),
							OptionalTag
						}
					},
					MappedFunctions(Range3, Range2), MappedMethods()
				}
			};
		}
	}
}
