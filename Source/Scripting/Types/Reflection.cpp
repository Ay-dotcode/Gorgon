#include "../../Any.h"
#include "../Reflection.h"
#include "../Embedding.h"
#include "Array.h"


namespace Gorgon { namespace Scripting {
	static Type *type=nullptr;	
	Library Reflection("Reflection", "This library contains reflection objects");
	
	Type *FunctionType() {		
		static Type *fn=nullptr;
		if(fn==nullptr) {
			fn=new Scripting::MappedReferenceType<Function, &GetNameOf<Function>>("Function",
				"Contains information about a function, also allows them to be called. Functions are immutable."
			);
			
			fn->AddFunctions({
				new Scripting::Function("Name",
					"Returns the name of the function", fn,
					{
						MapFunction(
							&GetNameOf<Function>, Types::String(),
							{ }, ConstTag
						)
					}
				),
				
				new Scripting::Function("Help",
					"Returns help for the function", fn,
					{
						MapFunction(
							&GetHelpOf<Function>, Types::String(), 
							{ }, ConstTag
						)
					}
				),
			});
		}
		
		return fn;
	}
	
	Type *ConstantType() {		
		static Type *obj=nullptr;
		if(obj==nullptr) {
			obj=new Scripting::MappedReferenceType<Constant, &GetNameOf<Constant>>("Constant",
				"Contains information about a constant."
			);
			
			obj->AddFunctions({
				new Scripting::Function("Name",
					"Returns the name of the constant", obj,
					{
						MapFunction(
							&GetNameOf<Constant>, Types::String(),
							{ }, ConstTag
						)
					}
				),
				
				new Scripting::Function("Help",
					"Returns help for the constant", obj,
					{
						MapFunction(
							&GetHelpOf<Constant>, Types::String(), 
							{ }, ConstTag
						)
					}
				),
				
				new Scripting::Function("Value",
					"Returns the value of the constant", obj,
					{
						MapFunction(
							&Constant::GetData, Types::Variant(), 
							{ }, ConstTag
						)
					}
				),
				
				new Scripting::Function("Type",
					"Returns the type of the constant", obj,
					{
						MapFunction(
							&Constant::GetType, Types::Type(), 
							{ }, ConstTag, ReturnsConstTag, ReferenceTag
						)
					}
				),
			});
		}
		
		return obj;
	}

	Type *ParameterType() {
		static Type *param = nullptr;
		if(param==nullptr) {
			param=new Scripting::MappedValueType<Parameter, &GetNameOf<Parameter>, ParseThrow<Parameter>>(
				"Parameter", "A function parameter.", Parameter("", "", Types::Variant())
			);

			param->AddFunctions({
				new Scripting::Function("Name",
				"Returns the name of the parameter", param,
				{
					MapFunction(
						&GetNameOf<Parameter>, Types::String(),
						{ }, ConstTag
					)
				}
				),

				new Scripting::Function("Help",
				"Returns help for the parameter", param,
				{
					MapFunction(
						&GetHelpOf<Parameter>, Types::String(),
						{ }, ConstTag
					)
				}
				),
			});
		}

		return param;
	}
	
	
	//Initialization should be separate as Type type should be created before any types are added to
	//libraries. Therefore, a bare type, just for the pointer of it, is created when this function
	//is first called. InitTypeType should be called after basic types are added to Integral library
	Type *TypeType() {
		if(type==nullptr) {
			type=new Scripting::MappedReferenceType<Type, &GetNameOf<Type>>("Type",
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
							&GetNameOf<Type>, Types::String(),
							{ }, ConstTag
						)
					}
				},

				new Scripting::Function{"Help",
					"Returns help for the type", type,
					{
						MapFunction(
							&GetHelpOf<Type>, Types::String(),
							{ }, ConstTag
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
									Types::Variant(), OptionalTag
								)
							},
							RepeatTag, ConstTag, ReferenceTag
						)
					}
				},
				
				new Scripting::Function{"Functions", 
					"Returns the functions in this type", type,
					{
						MapFunction(
							[](const Type &type) -> Array* {
								auto arr=new Array(*FunctionType());
								VirtualMachine::Get().References.Register(arr);
								for(auto it=type.Functions.First(); it.IsValid(); it.Next()) {
									arr->PushData(&it.Current().second, true, true);
								}
								
								return arr;
							},ArrayType(),
							{ }, ReferenceTag, ConstTag
						)
					}
				},
				
				new Scripting::Function{"Constants", 
					"Returns the constants in this type", type,
					{
						MapFunction(
							[](const Type &type) -> Array* {
								auto arr=new Array(*ConstantType());
								VirtualMachine::Get().References.Register(arr);
								for(auto it=type.Constants.First(); it.IsValid(); it.Next()) {
									arr->PushData(&it.Current().second, true, true);
								}
								
								return arr;
							},ArrayType(),
							{ }, ReferenceTag, ConstTag
						)
					}
				},
				
				
				new Scripting::Function{"IsReference", 
					"Returns if this type is a reference type", type,
					{
						MapFunction(
							&Type::IsReferenceType,Types::Bool(),
							{ }, ConstTag
						)
					}
				},
				
				new Scripting::Function{"DefaultValue", 
					"Returns the default value of this type. Reference types might return null.", type,
					{
						MapFunction(
							[](const Type *type) {
								auto c=Data(type, type->GetDefaultValue(), type->IsReferenceType());
								c.MakeConstant();
								return c;
							}, Types::Variant(),
							{ }, ConstTag
						)
					}
				},
				
			});
		}
	}
	
	Type *LibraryType() {
		static Type *lib=nullptr;
		if(!lib) {
			lib=new MappedReferenceType<Library, &GetNameOf<Library>>("Library",
				"Contains information about a library. It is possible to list contents "
				"of a library using Types, Functions, and Constants members."				
			);
			
			lib->AddFunctions({
				new Scripting::Function{"Name",
					"Returns the name of the type", lib,
					{
						MapFunction(
							&GetNameOf<Library>, Types::String(),
							{ }, ConstTag
						)
					}
				},

				new Scripting::Function{"Help",
					"Returns help for the type", lib,
					{
						MapFunction(
							&GetHelpOf<Library>, Types::String(),
							{ }, ConstTag
						)
					}
				},
				
				new Scripting::Function{"List", 
					"Returns the list of libraries", lib,
					{
						MapFunction(
							[]() -> Array* {
								auto arr=new Array(*lib);
								VirtualMachine::Get().References.Register(arr);
								for(auto it=VirtualMachine::Get().Libraries.First(); it.IsValid(); it.Next()) {
									arr->PushData(&it.Current().second, true, true);
								}
								
								return arr;
							},ArrayType(),
							{ }, ReferenceTag
						)
					},
					StaticTag
				},
				
				new Scripting::Function{"Functions", 
					"Returns the functions in this library", lib,
					{
						MapFunction(
							[](const Library &lib) -> Array* {
								auto arr=new Array(*FunctionType());
								VirtualMachine::Get().References.Register(arr);
								for(auto it=lib.Functions.First(); it.IsValid(); it.Next()) {
									arr->PushData(&it.Current().second, true, true);
								}
								
								return arr;
							},ArrayType(),
							{ }, ReferenceTag, ConstTag
						)
					}
				},
				
				new Scripting::Function{"Constants", 
					"Returns the constants in this library", lib,
					{
						MapFunction(
							[](const Library &lib) -> Array* {
								auto arr=new Array(*ConstantType());
								VirtualMachine::Get().References.Register(arr);
								for(auto it=lib.Constants.First(); it.IsValid(); it.Next()) {
									arr->PushData(&it.Current().second, true, true);
								}
								
								return arr;
							},ArrayType(),
							{ }, ReferenceTag, ConstTag
						)
					}
				},
				
				new Scripting::Function{"Types", 
					"Returns the types in this library", lib,
					{
						MapFunction(
							[](const Library &lib) -> Array* {
								auto arr=new Array(*TypeType());
								VirtualMachine::Get().References.Register(arr);
								for(auto it=lib.Types.First(); it.IsValid(); it.Next()) {
									arr->PushData(&it.Current().second, true, true);
								}
								
								return arr;
							},ArrayType(),
							{ }, ReferenceTag, ConstTag
						)
					}
				},
			});
		}
		
		return lib;
	}
	
	void InitReflection() {		
		
		Reflection.Member(TypeType());
			
		Reflection.AddMembers({FunctionType(), ParameterType(), LibraryType(), ConstantType()});
		
		Reflection.AddMembers({
				new Function("TypeOf",
					"This function returns the type of the given variable.", nullptr, 
					{
						MapFunction(
							[](std::string variable) -> const Type* {
								return &VirtualMachine::Get().GetVariable(variable).GetType();
							}, TypeType(),
							{
								Parameter("Variable",
									"The variable to determine its type.",
									Types::String(), VariableTag
								)
							}, ReturnsConstTag
						)
						
					}
				),				
				new Function("Locals",
					"This function returns the names of the local variables.", nullptr, 
					{
						MapFunction(
							[]() -> Array* {
								auto vars=VirtualMachine::Get().CurrentScopeInstance().GetLocalVariables();
								auto arr=new Array(Types::String());
								VirtualMachine::Get().References.Register(arr);
								
								for(auto var : vars) {
									arr->PushWithoutCheck(var.first);
								}
								
								return arr;
							}, ArrayType(),
							{ }, ReferenceTag
						)
					},
					{
						MapFunction(
							[] {
								auto &vm=VirtualMachine::Get();
								auto &out=vm.GetOutput();
								auto vars=vm.CurrentScopeInstance().GetLocalVariables();
								
								out<<"Local variables: "<<std::endl;
								
								int maxlen=0;
								for(auto var : vars) {
									if(maxlen<var.first.length()) maxlen=var.first.length();
								}
								
								for(std::pair<std::string, Variable> var : vars) {
									out<<std::setw(maxlen)<<var.first<<": ";
									if(!var.second.IsValid())
										out<<"<Invalid>";
									else
										out<<var.second.GetType().ToString(var.second);
									
									out<<std::endl;
								}
								out<<std::endl;
							}, nullptr,
							{ }
						)
					}
				),
				new Function("VarInfo",
					"This function returns information about a variable.", nullptr, 
					{
						MapFunction(
							[](std::string variable) {
								auto &vm=VirtualMachine::Get();
								auto var=vm.GetVariable(variable);
								vm.GetOutput()<<"Name : "<<var.GetName()<<std::endl;
								if(var.IsValid()) {
									vm.GetOutput()<<"Type : "<<var.GetType().GetName()<<std::endl;
									vm.GetOutput()<<"Ref  : "<<(var.IsReference() ? "yes" : "no")<<std::endl;
									vm.GetOutput()<<"Const: "<<(var.IsConstant()  ? "yes" : "no")<<std::endl;
									vm.GetOutput()<<"Value: "<<var.GetType().ToString(var)<<std::endl;
									if(var.IsReference()) {
										vm.GetOutput()<<"Ptr  : "<<var.GetData().Pointer()<<std::endl;
									}
								}
								else {
									vm.GetOutput()<<"Value: <Invalid>"<<std::endl;
								}
							}, nullptr,
							{
								Parameter("Variable",
									"The variable to determine its type.",
									Types::String(), VariableTag
								)
							}, ReturnsConstTag
						)
						
					}
				)
			}
		);
		
		InitTypeType();
	}
	
	
} }
