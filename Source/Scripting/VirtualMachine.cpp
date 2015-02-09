#include "VirtualMachine.h"
#include "Exceptions.h"
#include "../Scripting.h"
#include "Embedding.h"



namespace Gorgon {
	
	namespace Scripting {		
		
		Containers::Hashmap<std::thread::id, VirtualMachine, &VirtualMachine::getthread> VirtualMachine::activevms;
		
		VirtualMachine::VirtualMachine(bool automaticreset, std::ostream &out, std::istream &in) : 
		Libraries(libraries), output(&out), input(&in), 
		defoutput(&out), definput(&in), automaticreset(automaticreset), temporaries(256, Data::Invalid())
		{
			Activate();
			init_builtin();
			libraries.Add(Integrals);
			libraries.Add(Keywords);
			libraries.Add(Reflection);
			
			variablescopes.AddNew("[main]", VariableScope::DefaultLocal);
		}
		
		void VirtualMachine::AddLibrary(const Library &library) { 
			libraries.Add(library);
			if(alllibnames!="") alllibnames+=", ";
			alllibnames+=library.GetName();
		}
		
		void VirtualMachine::RemoveLibrary(const Library &library) {
			libraries.Remove(library.GetName());
			alllibnames="";
			for(const auto &lib : libraries) {
				if(alllibnames!="") alllibnames+=", ";
				alllibnames+=lib.first;
			}
		}
		
		void VirtualMachine::SetOutput(std::ostream &out) {
			output=&out;
		}
		
		void VirtualMachine::SetInput(std::istream &in) {
			input=&in;
		}
		
		const Type &VirtualMachine::FindType(std::string name) {
			using std::swap;

			name=String::ToLower(name);
			
			std::string namespc=String::Extract(name, ':');
			if(name=="") swap(namespc, name);
			
			if(namespc!="") {
				auto lib=libraries.Find(namespc);
				if(!lib.IsValid()) {
					throw SymbolNotFoundException(
						namespc, SymbolType::Library, 
						"Cannot find "+namespc+" library while looking for type "+name
					);
				}
				
				auto element=lib.Current().second.Types.Find(name);
				
				if(!element.IsValid()) {
					throw SymbolNotFoundException(
						namespc+":"+name, SymbolType::Type, 
						"Cannot find "+name+" type in library "+namespc
					);
				}
				
				return element.Current().second;
			}
			else { //search all
				const Type *foundelement;
				std::string foundlibnames;
				int found=0;
				
				for(const auto &lib : libraries) {
					auto element=lib.second.Types.Find(name);
					if(element.IsValid()) {
						found++;
						foundelement=&element.Current().second;
						if(foundlibnames!="") foundlibnames+=", ";
						foundlibnames+=lib.first;
					}
				}
				
				if(found==0) {
					throw SymbolNotFoundException(
						name, SymbolType::Type, 
						"Cannot find "+name+" type in all loaded libraries: "+alllibnames
					);
				}
				else if(found>1) {
					throw AmbiguousSymbolException(
						name, SymbolType::Type,
						name+" type found in following libraries: "+foundlibnames
					);
				}
				
				return *foundelement;
			}
		}
		
		const Function &VirtualMachine::FindFunction(std::string name) {
			using std::swap;

			name=String::ToLower(name);

			std::string namespc=String::Extract(name, ':');
			if(name=="") swap(namespc, name);
			
			if(namespc=="") { //search all
				const Function *foundelement;
				std::string foundlibnames;
				int found=0;
				
				for(const auto &lib : libraries) {
					auto element=lib.second.Functions.Find(name);
					if(element.IsValid()) {
						found++;
						foundelement=&element.Current().second;
						if(foundlibnames!="") foundlibnames+=", ";
						foundlibnames+=lib.first;
					}
				}
				
				if(found==0) {
					throw SymbolNotFoundException(
						name, SymbolType::Function, 
						"Cannot find "+name+" function in all loaded libraries: "+alllibnames
					);
				}
				else if(found>1) {
					throw AmbiguousSymbolException(
						name, SymbolType::Function,
						name+" function found in following libraries: "+foundlibnames
					);
				}
				
				return *foundelement;
			}
			else if(name.find_first_of(':')!=name.npos) { // both namespace and type is given
				std::string typname = String::Extract(name, ':');
				
				auto lib=libraries.Find(namespc);
				if(!lib.IsValid()) {
					throw SymbolNotFoundException(
						namespc, SymbolType::Library,
						"Cannot find "+namespc+" library while looking for function "+name
					);
				}
				
				auto type=lib.Current().second.Types.Find(typname);
				if(!type.IsValid()) {
					throw SymbolNotFoundException(
						namespc+":"+typname, SymbolType::Type, 
						"Cannot find "+typname+" type while looking for "+name+" function in library "+namespc
					);
				}
				
				auto element=type.Current().second.Functions.Find(name);				
				
				if(!element.IsValid()) {
					throw SymbolNotFoundException(
						namespc+":"+name, SymbolType::Function, 
						"Cannot find "+name+" function in type "+namespc+":"+typname
					);
				}
				
				return element.Current().second;
			}
			else { // namespc can either be type or library
				auto lib=libraries.Find(namespc);
				if(!lib.IsValid()) {
					const Type *type=nullptr;
					
					try {
						type=&FindType(namespc);
					}
					catch(const SymbolNotFoundException &) {
					}
					catch(...) { 
						throw;
					}
					
					throw SymbolNotFoundException(
						namespc, SymbolType::Namespace, 
						"Cannot find "+namespc+" namespace while looking for function "+name
					);
					
					auto element=type->Functions.Find(name);
					
					if(!element.IsValid()) {
						throw SymbolNotFoundException(
							namespc+":"+name, SymbolType::Function, 
							"Cannot find "+name+" function in library "+namespc
						);
					}
					
					return element.Current().second;
				}
				else {				
					auto element=lib.Current().second.Functions.Find(name);
					
					if(!element.IsValid()) {
						throw SymbolNotFoundException(
							namespc+":"+name, SymbolType::Function, 
							"Cannot find "+name+" function in library "+namespc
						);
					}
					return element.Current().second;
				}
				
			}
		}
		
		const Constant &VirtualMachine::FindConstant(std::string name) {
			using std::swap;

			name=String::ToLower(name);

			std::string namespc=String::Extract(name, ':');
			if(name=="") swap(namespc, name);
			
			if(namespc=="") { //search all
				const Constant *foundelement;
				std::string foundlibnames;
				int found=0;
				
				for(const auto &lib : libraries) {
					auto element=lib.second.Constants.Find(name);
					if(element.IsValid()) {
						found++;
						foundelement=&element.Current().second;
						if(foundlibnames!="") foundlibnames+=", ";
						foundlibnames+=lib.first;
					}
				}
				
				if(found==0) {
					throw SymbolNotFoundException(
						name, SymbolType::Constant, 
						"Cannot find "+name+" constant in all loaded libraries: "+alllibnames
					);
				}
				else if(found>1) {
					throw AmbiguousSymbolException(
						name, SymbolType::Constant,
						name+" constant found in following libraries: "+foundlibnames
					);
				}
				
				return *foundelement;
			}
			else if(name.find_first_of(':')!=name.npos) { // both namespace and type is given
				std::string typname = String::Extract(name, ':');
				
				auto lib=libraries.Find(namespc);
				if(!lib.IsValid()) {
					throw SymbolNotFoundException(
						namespc, SymbolType::Library, 
						"Cannot find "+namespc+" library while looking for constant "+name
					);
				}
				
				auto type=lib.Current().second.Types.Find(typname);
				if(!type.IsValid()) {
					throw SymbolNotFoundException(
						namespc+":"+typname, SymbolType::Type, 
						"Cannot find "+typname+" type while looking for "+name+" constant in library "+namespc
					);
				}
				
				auto element=type.Current().second.Constants.Find(name);				
				
				if(!element.IsValid()) {
					throw SymbolNotFoundException(
						namespc+":"+name, SymbolType::Constant, 
						"Cannot find "+name+" constant in type "+namespc+":"+typname
					);
				}
				
				return element.Current().second;
			}
			else { // namespc can either be type or library
				auto lib=libraries.Find(namespc);
				if(!lib.IsValid()) {
					const Type *type=nullptr;
					
					try {
						type=&FindType(namespc);
					}
					catch(const SymbolNotFoundException &) {
					}
					catch(...) { 
						throw;
					}
					
					throw SymbolNotFoundException(
						namespc, SymbolType::Namespace, 
						"Cannot find "+namespc+" namespace while looking for constant "+name
					);
					
					auto element=type->Constants.Find(name);
					
					if(!element.IsValid()) {
						throw SymbolNotFoundException(
							namespc+":"+name, SymbolType::Constant, 
							"Cannot find "+name+" constant in library "+namespc
						);
					}
					
					return element.Current().second;
				}
				else {				
					auto element=lib.Current().second.Constants.Find(name);
					
					if(!element.IsValid()) {
						throw SymbolNotFoundException(
							namespc+":"+name, SymbolType::Constant, 
							"Cannot find "+name+" constant in library "+namespc
						);
					}
					return element.Current().second;
				}
				
			}
		}
		
		const SymbolType FindSymbol(std::string name, SymbolType hint, Any &result) {
			using std::swap;
			
			name=String::ToLower(name);
			
			std::string namespc=String::Extract(name, ':');
			if(name=="") swap(namespc, name);
			
			if(namespc!="") {
				//cannot be variable
			}
			else {
			}
			
			
			//!TODO
			
			throw SymbolNotFoundException(name, SymbolType::Unknown);
		}
		
		void VirtualMachine::Start(InputSource &source) {
			Activate();
			inputsources.Add(source);

			executionscopes.Add(new ExecutionScope(source));

			Run();
		}
	
		void VirtualMachine::Begin(InputSource &source) {
			Activate();
			inputsources.Add(source);

			executionscopes.Add(new ExecutionScope(source));
		}

		void VirtualMachine::Run() {
			int target=executionscopes.GetCount()-1;
			if(target<0) {
				throw std::runtime_error("No scope to execute.");
			}

			Run(target);
		}

		void VirtualMachine::Run(unsigned executiontarget) {
			int keywordtarget=keywordscopes.GetCount();
			int variabletarget=variablescopes.GetCount();
			
			returnimmediately=false;
			returnvalue=Data::Invalid();

			while(executionscopes.GetCount()>(long)executiontarget) {
				if(returnimmediately) {
					executionscopes.DeleteAll();
					return;
				}
				
				auto inst=executionscopes.Last()->Get();
#ifdef TESTVM
				Console::SetColor(Console::Black);
				Console::SetBold();
				if(inst) {
					std::cout<<"Executing: "<<executionscopes.Last()->GetSource().GetPhysicalLine()<<std::endl;
				}
				else {
					std::cout<<"Scope finished"<<std::endl;
				}
				Console::Reset();
#endif

				try {
					// execution scope is done, cull it
					if(inst==nullptr) {
						executionscopes.Last().Delete();
					} 
					else {
						if(inst->Type==InstructionType::Mark) {
							auto &fn=FindFunction(inst->Name.Name);
							if(fn.NeverSkip()) {
								markednoskip=true;
							}
							markedkeyword=&fn;
							markedline=executionscopes.Last()->GetMarkerForCurrent();
						}
						// assignment ...
						else if(inst->Type==InstructionType::Assignment) {
							if(inst->Name.Type!=ValueType::Variable)
								throw std::runtime_error("Variables can only be represented with variables.");

							if(!skipping || markednoskip) {
								SetVariable(inst->Name.Name, getvalue(inst->RHS));
							}
						}
						//function call
						else if(inst->Type==InstructionType::FunctionCall) {
							if(!skipping || markednoskip) {
								functioncall(inst, false, false);
							}
						} 
						else if(inst->Type==InstructionType::MemberFunctionCall) {
							if(!skipping) {
								functioncall(inst, true, false);
							}
						} 
						else if(inst->Type==InstructionType::MethodCall) {
							if(!skipping || markednoskip) {
								functioncall(inst, false, true);
							}
						}
						else if(inst->Type==InstructionType::MemberMethodCall) {
							if(!skipping) {
								functioncall(inst, true, true);
							}
						}
						else if(inst->Type==InstructionType::RemoveTemp) {
							temporaries[inst->Store]=Data::Invalid();
						}
						else {
							ASSERT(false, "Unknown instruction type.", 0, 8);
						}
					}
				}
 				catch(Exception &ex) {
					if(ex.GetLine()<=0) {
						ex.SetLine(-ex.GetLine()+executionscopes.Last()->GetSource().GetPhysicalLine());
					}
 
 					throw ex;
 				}
			}

			if(executionscopes.GetCount()==executiontarget) {
				if(keywordtarget<keywordscopes.GetCount()) {
					throw FlowException("Missing end statement",
						"While searching the end for "+keywordscopes.Last()->GetFunction().GetName(),
						keywordscopes.Last()->GetPhysicalLine()
					);
				}
				else if(keywordtarget>keywordscopes.GetCount()) {
					throw FlowException("Extra end statement");
				}

				ASSERT(variabletarget==variablescopes.GetCount(), "Variable scope error",0,8);
			}
		}

		Data VirtualMachine::getvalue(const Value &val) {
			switch(val.Type) {
				case ValueType::Literal:
					return val.Literal;
				case ValueType::Constant:
					return FindConstant(val.Name).GetData();
				case ValueType::Temp: {
					auto &data=temporaries[val.Result];
					if(!data.IsValid()) {
						throw std::runtime_error("Invalid temporary.");
					}
					return temporaries[val.Result];
				}
				case ValueType::Variable:
					return GetVariable(val.Name);
				case ValueType::Identifier:
					if(IsVariableSet(val.Name))
						return GetVariable(val.Name);
					else//!constants
						return {Types::String(), val.Name};
				default:
					Utils::ASSERT_FALSE("Invalid value type.");
			}

		}

		Data VirtualMachine::callfunction(const Function *fn, bool method, const std::vector<Value> &incomingparams) {
			auto pin=incomingparams.begin();

			std::vector<Data> params;

			if(fn->HasParent()) {
				if(pin!=incomingparams.end()) {
					Data param=getvalue(*pin);
					
					if(fn->GetParent().IsReferenceType()) {
						if(param.IsNull()) {
							if(pin->Type==ValueType::Variable) {
								throw NullValueException("$"+pin->Name);
							}
							else {
								throw NullValueException("parent parameter");
							}
						}
					}

					if(param.GetType()==fn->GetParent()) {
						//no worries
					}
					else {
						Utils::NotImplemented("Polymorphism");
					}

					params.push_back(param);
				}
				else {
					throw MissingParameterException("this", 0, fn->GetParent().GetName());
				}
				pin++;
			}
			
			auto fixparam = [&] (Data &param, const Parameter &pdef) {
				if(param.GetType()==pdef.GetType()) {
					//no worries
				}
				//to string
				else if(pdef.GetType()==Integrals.Types["string"]) {
					param={Integrals.Types["string"], param.GetType().ToString(param)};
				}
				//from string 
				else if(param.GetType()==Integrals.Types["string"]) {
					param={pdef.GetType(), pdef.GetType().Parse(param.GetValue<std::string>())};
				}
				//to variant
				else if(pdef.GetType()==Variant) {
					param={Variant, param};
				}
				//from variant
				else if(param.GetType()==Variant && param.GetValue<Data>().GetType()==pdef.GetType()) {
					param={pdef.GetType(), param.GetValue<Data>()};
				}
				else {
					const Function *ctor=pdef.GetType().GetTypeCasting(param.GetType());

					if(ctor) {
						param=ctor->Call(false, {param});
					}
					else {
						throw CastException(param.GetType().GetName(), pdef.GetType().GetName(), 
											"Cannot cast while trying to call "+fn->GetName());
					}
				}
			};

			int ind=1;
			for(const auto &pdef : fn->Parameters) {
				if(pdef.IsReference()) {
					if(pin->Type==ValueType::Variable) {
						if(pdef.IsInput()) {
							GetVariable(pin->Name);
						}
						
						Data param={Types::String(), pin->Name};
						
						params.push_back(param);
					}
					else  if(pin->Type==ValueType::Literal) {
						if(pin->Literal.GetType()!=Types::String()) {
							throw InstructionException("Reference type can only be represented string literals and variables");
						}
						
						params.push_back(pin->Literal);
					}
					else {
						throw InstructionException("Reference type can only be represented string literals and variables");
					}
				}
				else if(pin!=incomingparams.end()) {
					Data param=getvalue(*pin);

					fixparam(param, pdef);

					params.push_back(param);
				}
				else {
					if(!pdef.IsOptional()) {
						throw MissingParameterException(pdef.GetName(), ind, pdef.GetType().GetName(),
							"Parameter "+pdef.GetName()+" is not optional."
						);
					}

					break;
				}
				++pin;
				++ind;
			}
			
			// if last parameter can be repeated
			if(fn->RepeatLast()) {
				//add remaining parameters to the parameter list
				while(pin!=incomingparams.end()) {
					Data param=getvalue(*pin);
					fixparam(param, fn->Parameters.Last().Current());
					params.push_back(param);
					++pin;
				}
			}
			// if not and there are extra parameters
			else if(pin!=incomingparams.end()) {
				throw TooManyParametersException(incomingparams.size(), fn->Parameters.GetCount(), 
					"Too many parameters supplied for "+fn->GetName()
				);
			}

			return fn->Call(method, params);
		}

		void VirtualMachine::SetVariable(const std::string &name, Data data) {
			//check if it exists
			auto &vars=variablescopes.Last()->Variables;
			auto var=vars.Find(String::ToLower(name));

			//if not found in locals
			if(!var.IsValid()) {
				//search in globals
				var=globalvariables.Find(String::ToLower(name));

				//global should have defined in the same InputSource as dictated by the scope
				if(variablescopes.Last()->GetScopingMode()==VariableScope::LimitGlobals) {
					//if not
					if(!var.Current().second.IsDefinedIn(executionscopes.Last()->GetSource())) {
						//this scope cannot use this global and should define a new local variable
						var=globalvariables.end();
					}
				}
			}

			//if found
			if(var.IsValid()) {
				//change existing variable
				var.Current().second.Set(data.GetType(), data.GetData());
			}
			else {
				//add a new one
				if(variablescopes.Last()->GetScopingMode()==VariableScope::DefaultGlobal) {
					//as global
					globalvariables.Add(new Variable(name, data.GetType(), data.GetData()));
				}
				else {
					//as local
					vars.Add(new Variable(name, data.GetType(), data.GetData()));
				}
			}
		}
		
		bool VirtualMachine::IsVariableSet(const std::string &name) {
			auto &vars=variablescopes.Last()->Variables;
			auto var=vars.Find(String::ToLower(name));
			
			if(var.IsValid()) return true;
			
			var=globalvariables.Find(String::ToLower(name));

			//if found
			if(var.IsValid()) return true;
			
			return false;
		}
		
		void VirtualMachine::CompileCurrent() {
			if(executionscopes.GetCount()==0) {
				throw FlowException("No active execution scope");
			}
			
			executionscopes.Last()->Compile();
		}
		
		

		
		Variable &VirtualMachine::GetVariable(const std::string &name) {
			//check variable scopes first
			auto &vars=variablescopes.Last()->Variables;
			auto var=vars.Find(String::ToLower(name));

			//TODO static variables? may be they can be handled by function function

			//if found
			if(var.IsValid()) {
				//return
				return var.Current().second;
			}
			//if not
			else {
				//search globals
				auto var=globalvariables.Find(String::ToLower(name));

				//if found
				if(var.IsValid()) {
					//global should have defined in the same InputSource as dictated by the scope
					if(variablescopes.Last()->GetScopingMode()==VariableScope::LimitGlobals) {
						//if not
						if(!var.Current().second.IsDefinedIn(executionscopes.Last()->GetSource())) {
							//this scope cannot use this global
							throw SymbolNotFoundException(name, SymbolType::Variable);
						}
					}

					//return
					return var.Current().second;
				}
				//if not
				else {
					//nothing more to try
					throw SymbolNotFoundException(name, SymbolType::Variable);
				}
			}
		}

		// !Careful there are early terminations
		void VirtualMachine::functioncall(const Instruction *inst, bool memberonly, bool method) {
			const Function *fn=nullptr;
			std::string functionname;

			//function call from a literal, should be a string.
			if(inst->Name.Type==ValueType::Literal) {
				try {
					functionname=inst->Name.Literal.GetValue<std::string>();
				} catch(...) {
					throw std::runtime_error("Invalid instruction. Function names should be string literals.");
				}
			}
			else if(inst->Name.Type==ValueType::Identifier && IsVariableSet(inst->Name.Name)) {
				fn=GetVariableValue(inst->Name.Name).GetValue<const Function *>();
			}
			else if(inst->Name.Type==ValueType::Identifier) {
				functionname=inst->Name.Name;
			}
			else {
				ASSERT(false, "Not implemented", 2);
			}

			// find requested function
			if(!memberonly) {
				//try library functions
				try {
					if(!fn)
						fn=&FindFunction(functionname);
				}
				//if not found, try member functions
				catch(const SymbolNotFoundException &) {
					//should have parameter for resolving
					if(inst->Parameters.size()==0) {
						throw;
					}

					//search in the type of the first parameter
					Data data=getvalue(inst->Parameters[0]);
					auto fnit=data.GetType().Functions.Find(functionname);

					//if found
					if(fnit.IsValid()) {
						fn=&fnit.Current().second;
					} else {
						//cannot find anywhere
						throw;
					}
				} catch(...) {
					//some other error
					throw;
				}
			}
			else {
				if(inst->Parameters.size()==0) {
					throw std::runtime_error("Invalid intermediate instruction, missing this parameter");
				}
				
				//search in the type of the first parameter
				Data data=getvalue(inst->Parameters[0]);

				if(functionname=="{}") { //constructor
					if(data.GetType()==Types::String()) {
						data={Reflection.Types["Type"], (const Type*)(&FindType(data.GetValue<std::string>()))};
					}
					else if(data.GetType()!=Reflection.Types["Type"]) {
						throw std::runtime_error("Invalid intermediate instruction, type parameter is not a type");
					}
					
					std::vector<Data> params;
					int i=0;
					for(auto &pin : inst->Parameters) {
						if(i++!=0) {
							params.push_back(getvalue(pin));
						}
					}
					
					Data ret=data.GetValue<const Type*>()->Construct(params);
					if(inst->Store) {
						temporaries[inst->Store]=ret;
					}
					
					return;
				}				
				else if(functionname[0]=='.') {
					if(inst->Parameters.size()==1) {
						Data ret=data.GetType().DataMembers[functionname.substr(1)].Get(data);
						
						if(inst->Store) {
							//store the result
							if(ret.GetType()==Types::Variant()) {
								ret=ret.GetValue<Data>();
							}
							temporaries[inst->Store]=ret;
						}
					}
					else if(inst->Parameters.size()==2) {
						if(inst->Store) {
							throw NoReturnException("Data member: "+data.GetType().DataMembers[functionname.substr(1)].GetName());
						}
						if(inst->Parameters[0].Type==ValueType::Variable) {
							data.GetType().DataMembers[functionname.substr(1)].
								Set(GetVariable(inst->Parameters[0].Name), getvalue(inst->Parameters[1]));
						}
						else if(inst->Parameters[0].Type==ValueType::Temp) {
							data.GetType().DataMembers[functionname.substr(1)].
								Set(temporaries[inst->Parameters[0].Result], getvalue(inst->Parameters[1]));
						}
						else if(inst->Parameters[0].Type==ValueType::Identifier) {
							ASSERT(IsVariableSet(inst->Parameters[0].Name), "Only variable identifiers can be assigned to");
							
							data.GetType().DataMembers[functionname.substr(1)].
								Set(GetVariable(inst->Parameters[0].Name), getvalue(inst->Parameters[1]));
						}
						else if(inst->Parameters[0].Type==ValueType::Literal || inst->Parameters[0].Type==ValueType::Constant) {
							ASSERT(false, "Cannot assign to literal or constants");
						}						
						else {
						}						
					}
					else {
						throw std::runtime_error("Member access or mutate requires exactly 1 and 2 parameters");
					}
					
					return;
				}
				else {
					auto fnit=data.GetType().Functions.Find(functionname);

					//if found
					if(fnit.IsValid()) {
						fn=&fnit.Current().second;
					} else {
						//cannot find it
						throw SymbolNotFoundException(functionname, SymbolType::Function, 
							"Cannot find the member function "+data.GetType().GetName()+":"+functionname);
					}
				}
			}

			// call it
			Data ret=callfunction(fn, method, inst->Parameters);

			if(fn->IsScoped() && ret.IsValid() && ret.GetValue<Data>().IsValid() ) {
				auto scope=new KeywordScope{*fn, ret.GetValue<Data>(), executionscopes.Last()->GetSource().GetPhysicalLine()};
				keywordscopes.Add(scope);
			}

			if(markednoskip && fn==markedkeyword) {
				markednoskip=false;
				markedkeyword=nullptr;
			}

			//if requested
			if(inst->Store) {
				if(!fn->IsScoped() && fn->HasReturnType()) {
					//store the result
					if(ret.GetType()==Types::Variant()) {
						ret=ret.GetValue<Data>();
					}
					temporaries[inst->Store]=ret;
				} 
				else {
					throw NoReturnException(functionname);
				}
			}
		}
		
		void VirtualMachine::Jump(SourceMarker marker) {
			if(reinterpret_cast<ExecutionScope*>(marker.GetSource())!=executionscopes.Last().CurrentPtr()) {
				throw FlowException("Jump destination is not valid", "While performing a short jump, a different "
					"execution scope is requested."
				);
			}
			
			executionscopes.Last()->Jumpto(marker.GetLine());
		}


	}
}