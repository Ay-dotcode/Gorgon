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
					new Function("[]", 
						"Returns the element at the given index.", array,
						{
							MapFunction(
								[](const Array *a, unsigned ind) {
									return a->GetItemData(ind);
								}, Variant,
								{
									Parameter( "Index",
										"The index of the element",
										Types::Unsigned()
									)
								},
								ConstTag
							),
							MapFunction(
								[](Array *a, unsigned ind) {
									return a->GetItemData(ind);
								}, Variant,
								{
									Parameter( "Index",
										"The index of the element",
										Types::Unsigned()
									)
								},
								ReferenceTag
							)
				  
						}
					),
					
					
					new Function("[]=", 
						"Changes the element at the given index.", array,
						{
							MapFunction(
								[](Array &a, unsigned ind, Data d) {
									return a.SetItemData(ind, d);
								}, nullptr,
								{
									Parameter( "Index",
										"The index of the element",
										Types::Unsigned()
									),
									Parameter( "Value",
										"Value to be assigned",
										Variant
									)
								}
							)
				
						}
					),
					
					new Function("Push", 
						"Pushes a new element at the end of the array.", array,
						{
							MapFunction(
								&Array::PushData, nullptr,
								{
									Parameter { "Element",
										"Element to be added.",
										Types::Variant()
									}
								}
							)
						}
					),
				  
					new Function("Size", 
						"Returns the size of the array.", array,
						{
							MapFunction(
								[](Array *a) { 
									return a->GetSize(); 
								}, Types::Unsigned(),
								{ }
							)
						}
					),
					
					new Function("Resize", 
						"Changes the size of the array. New elements will have their default value.", array, 
						{
							MapFunction(
								&Array::Resize, nullptr,
								{
									Parameter("Size", "The new size.", Types::Unsigned())
								}
							)
						}
					)
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
		
		std::vector<Function*> ArrayFunctions() {
			return {
				new Function("Range",
					"Creates a range array between two numbers", nullptr,
					{
						MapFunction(
							&Range3, ArrayType(),
							{
								Parameter { "Start",
									"Starting value for the range. This value is included in the array.",
									Types::Int()
								},
								Parameter { "End",
									"Ending value for the range. This value is not included in the array.",
									Types::Int()
								},
								Parameter { "Step",
									"Stepping amount for the range. If not given its determined to be either 1 or -1.",
									Types::Int()
								}
							}
						),
						
						MapFunction(
							&Range2, ArrayType(),
							{
								Parameter { "Start",
									"Starting value for the range. This value is included in the array.",
									Types::Int()
								},
								Parameter { "End",
									"Ending value for the range. This value is not included in the array.",
									Types::Int()
								}
							}
						),
					}
				),
			};
		}
	}
}
