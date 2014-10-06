#include "VirtualMachine.h"
#include "Exceptions.h"
#include "../Scripting.h"
#include "Embedding.h"



namespace Gorgon {
	
	namespace Scripting {		
		
		Containers::Hashmap<std::thread::id, VirtualMachine> VirtualMachine::activevms;
		
		VirtualMachine::VirtualMachine(bool automaticreset, std::ostream &out, std::istream &in) : 
		Libraries(libraries), output(&out), input(&in), 
		defoutput(&out), definput(&in), automaticreset(automaticreset), temporaries(256, Data::Invalid())
		{ 
			variablescopes.AddNew("[main]", VariableScope::DefaultLocal);
			libraries.Add(Integrals);
			libraries.Add(Keywords);
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
						name+" contstant found in following libraries: "+foundlibnames
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
		
		void VirtualMachine::Start(InputSource &source) {
			Activate();
			inputsources.Add(source);

			executionscopes.Add(new ExecutionScope(source));

			Run();
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

			while(executionscopes.GetCount()>executiontarget) {
				auto inst=executionscopes.Last()->Get();

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
						else {
							ASSERT(false, "Unknown instruction type.", 0, 8);
						}
					}
				}
				catch(Exception &ex) {
					ex.SetLine(executionscopes.Last()->GetSource().GetPhysicalLine());

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
				default:
					ASSERT(false, "Invalid value type.");
			}

		}

		Data VirtualMachine::callfunction(const Function *fn, bool method, const std::vector<Value> &incomingparams) {
			auto pin=incomingparams.begin();

			std::vector<Data> params;

			if(fn->HasParent()) {
				if(pin!=incomingparams.end()) {
					Data param=getvalue(*pin);

					if(param.GetType()==fn->GetParent()) {
						//no worries
					}
					else {
						ASSERT(false, "Not implemented", 0, 8);
					}

					params.push_back(param);
				}
				else {
					throw MissingParameterException("this", 0, fn->GetParent().GetName());
				}
				pin++;
			}

			for(const auto &pdef : fn->Parameters) {
				if(pdef.IsReference()) {

				}
				else if(pin!=incomingparams.end()) {
					Data param=getvalue(*pin);

					if(param.GetType()==pdef.GetType()) {
						//no worries
					}
					//tostring
					else if(pdef.GetType()==Integrals.Types["String"]) {
						param={Integrals.Types["String"], param.GetType().ToString(param)};
					} 
					else if(pdef.GetType()==Variant) {
						param={Variant, param};
					}
					else {
						ASSERT(false, "Not implemented", 0,8);
					}

					params.push_back(param);
				}
				else {
					if(!pdef.IsOptional()) {
						throw std::runtime_error("Parameter: "+pdef.GetName()+" is not optional.");
					}

					break;
				}
				++pin;
			}
			if(pin!=incomingparams.end()) {
				ASSERT(false, "Not implemented", 0, 8);
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
			} else {
				ASSERT(false, "Not implemented", 2);
			}

			// find requested function
			if(!memberonly) {
				//try library functions
				try {
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

			// call it
			Data ret=callfunction(fn, method, inst->Parameters);

			if(fn->IsScoped()) {
				auto scope=new KeywordScope{*fn, ret.GetValue<Data>(), executionscopes.Last()->GetSource().GetPhysicalLine()};
				keywordscopes.Add(scope);
			}

			if(markednoskip && fn==markedkeyword) {
				markednoskip=false;
			}

			//if requested
			if(inst->Store) {
				if(!fn->IsScoped() && fn->HasReturnType()) {
					//store the result
					temporaries[inst->Store]=ret;
				} 
				else {
					throw NoReturnException(functionname);
				}
			}
		}



	}
}