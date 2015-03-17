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
			AddLibrary(Integrals);
			AddLibrary(Keywords);
			AddLibrary(Reflection);
		}
		
		void VirtualMachine::AddLibrary(const Library &library) { 
			libraries.Add(library);
			if(alllibnames!="") alllibnames+=", ";
			alllibnames+=library.GetName();
			
			for(const auto &elm : library.Types) {
				types.insert(std::make_pair(elm.first, &elm.second));
			}
			
			for(const auto &elm : library.Constants) {
				symbols.insert(std::make_pair(elm.first, Symbol{library.GetName(), SymbolType::Constant, &elm.second}));
			}
			
			for(const auto &elm : library.Functions) {
				symbols.insert(std::make_pair(elm.first, Symbol{library.GetName(), SymbolType::Function, &elm.second}));
			}
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
			else { //use types table
				const Type *foundelement=nullptr;
				std::string foundlibnames;
				
				auto range=types.equal_range(name);
				int found=0;
				
				for(auto it=range.first; it!=range.second; ++it) {
					if(foundlibnames!="") foundlibnames+=", ";
					foundlibnames+=it->first;
					if(!foundelement || Integrals.Functions.Exists(name)) {
						foundelement=it->second;
						
						found++;
						
						if(Integrals.Functions.Exists(name)) { //Integral library takes precedence
							found=1;
							break;
						}
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
			
			if(namespc=="") { //look in the symbol table
				const Function *foundelement=nullptr;
				std::string foundlibnames;
				
				auto range=symbols.equal_range(name);
				int found=0;
				
				for(auto it=range.first; it!=range.second; ++it) {
					if(it->second.type==SymbolType::Function) {
						if(foundlibnames!="") foundlibnames+=", ";
						foundlibnames+=it->first;
						found++;

						if(!foundelement || it->second.namespc=="Integral") {
							foundelement=it->second.object.Get<const Function*>();
							
							
							if(it->second.namespc=="Integral") { //Integral library takes precedence
								found=1;
								break;
							}
						}
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
					
					//not in this type
					if(!element.IsValid()) {
						//search parents
						for(auto t : type->Parents) {
							if(t.first->Functions.Find(name)) {
								
							}
						}
						
						//not found anywhere
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
			
			if(namespc=="") { //use symbol table
				const Constant *foundelement=nullptr;
				std::string foundlibnames;
				
				auto range=symbols.equal_range(name);
				int found=0;
				
				for(auto it=range.first; it!=range.second; ++it) {
					if(it->second.type==SymbolType::Constant) {
						if(foundlibnames!="") foundlibnames+=", ";
						foundlibnames+=it->first;
						found++;

						if(!foundelement || it->second.namespc=="Integral") {
							foundelement=it->second.object.Get<const Constant*>();
							
							
							if(it->second.namespc=="Integral") { //Integral library takes precedence
								found=1;
								break;
							}
						}
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
						/*for(auto &t : type->Parents) {
							
						}*/
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
		
		void VirtualMachine::Start(InputProvider &input) {
			Activate();
			scopes.Add(new Scope(input, input.GetName()));

			executionscopes.Add(new ExecutionScope(*scopes.Last()));

			Run();
		}
	
		void VirtualMachine::Begin(InputProvider &input) {
			Activate();
			scopes.Add(new Scope(input, input.GetName()));

			executionscopes.Add(new ExecutionScope(*scopes.Last()));
		}

		void VirtualMachine::Run() {
			int target=executionscopes.GetCount()-1;
			if(target<0) {
				throw std::runtime_error("No scope to execute.");
			}

			Run(target);
		}

		void VirtualMachine::Run(unsigned executiontarget) {
			returnimmediately=false;
			returnvalue=Data::Invalid();

			//until execution target is reached
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
						execute(inst);
					}
				}
 				catch(Exception &ex) {
					if(executionscopes.GetCount()) {
						executionscopes.Last()->MoveToEnd();
						
						if(ex.GetLine()<=0) {
							ex.SetLine(-ex.GetLine()+executionscopes.Last()->GetScope().GetPhysicalLine());
						}
					}
 
 					throw ex;
 				}
			}
		}
		
		void VirtualMachine::SetVariable(const std::string &name, Data data) {
			//check if it exists
			auto &vars=executionscopes.Last()->Variables;
			auto var=vars.Find(String::ToLower(name));
			
			//if not found in locals
			if(!var.IsValid()) {
				//search in globals
				var=globalvariables.Find(String::ToLower(name));
			}
			//else ok
			
			//if found
			if(var.IsValid()) {
				if(var.Current().second.IsConstant()) {
					throw ConstantException(name, "Variable is constant.");
				}
				
				//change existing variable
				var.Current().second.Set(data.GetType(), data.GetData());
			}
			else {
				//as local
				vars.Add(new Variable(name, data.GetType(), data.GetData()));
			}
		}
		
		void VirtualMachine::UnsetVariable(const std::string &name) {
			//check if it exists
			auto &vars=executionscopes.Last()->Variables;
			auto var=vars.Find(String::ToLower(name));
			
			//if not found in locals
			if(!var.IsValid()) {
				//search in globals
				var=globalvariables.Find(String::ToLower(name));
				if(var.IsValid()) {
					var.Delete();
				}
			}
			else {
				var.Delete();
			}
		}
		
		bool VirtualMachine::IsVariableSet(const std::string &name) {
			auto &vars=executionscopes.Last()->Variables;
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
			auto &vars=executionscopes.Last()->Variables;
			auto var=vars.Find(String::ToLower(name));

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
		
		Data VirtualMachine::getvalue(const Value &val, bool reference) {
			switch(val.Type) {
				case ValueType::Literal:
					if(reference) { //literals cannot be converted to reference
						throw CastException("literal", "reference");
					}
					return val.Literal;
					
				case ValueType::Constant:
					if(reference) { //constant refrence
						auto data=FindConstant(val.Name).GetData().GetReference();
						data.MakeConstant(); //should be constant
						return data;
					}
					else {
						return FindConstant(val.Name).GetData();
					}
					
				case ValueType::Temp: {
					auto &data=temporaries[val.Result];
					
					if(!data.IsValid()) {
						throw std::runtime_error("Invalid temporary.");
					}
					
					if(reference)
						return data.GetReference();
					else
						return data;
				}
				
				case ValueType::Variable: {
					auto &var=GetVariable(val.Name);
					
					if(reference)
						return var.GetReference();
					else
						return var;
				}
				case ValueType::Identifier:		
					//if it is a variable
					if(IsVariableSet(val.Name)) {
						auto &var=GetVariable(val.Name);
						
						if(reference)
							return var.GetReference();
						else
							return var;
					}
					else {
						int namespcs=std::count(val.Name.begin(), val.Name.end(), ':');
						
						if(namespcs==2) { //function or constant in a type
							auto name=val.Name;
							auto lib=String::Extract(name, ':');
							auto typ=String::Extract(name, ':');
							
							//-use iterators
							if(!Libraries.Exists(lib)) {
								throw SymbolNotFoundException(lib, SymbolType::Library);
							}
							if(!Libraries[lib].Types.Exists(typ)) {
								throw SymbolNotFoundException(typ, SymbolType::Type);
							}
							auto &type=Libraries[lib].Types[typ];
							if(type.Constants.Exists(name)) {
								if(reference) {
									auto data=type.Constants[name].GetData().GetReference();
									data.MakeConstant();
									
									return data;
								}
								else {
									return type.Constants[name].GetData();
								}
							}
							else if(type.Functions.Exists(name)) {
								return {Types::Function(), &type.Functions[name]};
							}
							else {
								throw SymbolNotFoundException(name, SymbolType::Unknown);
							}
						}
						else if(namespcs==1) { //function or constant in a library
							auto name=val.Name;
							auto libname=String::Extract(name, ':');
							
							//...function or constant in a type
							
							//-use iterators
							
							const Library *lib=nullptr;
							if(Libraries.Exists(libname))
								lib=&Libraries[libname];
							
							if(lib && lib->Constants.Exists(name)) {
								if(reference) {
									auto data=lib->Constants[name].GetData().GetReference();
									data.MakeConstant();
									
									return data;
								}
								else {
									return lib->Constants[name].GetData();
								}
							}
							else if(lib && lib->Functions.Exists(name)) {
								return {Types::Function(), &lib->Functions[name]};
							}
							else { //could be a function or constant in a type which is in an unknown library
								auto range=types.equal_range(libname);
								if(range.first==range.second) {
									throw SymbolNotFoundException(libname, SymbolType::Unknown);
								}
								else if(range.first!=--range.second) {
									throw AmbiguousSymbolException(libname, SymbolType::Unknown);
								}
								else {
									if(range.first->second->Constants.Exists(name)) {
										if(reference) {
											auto data=range.first->second->Constants[name].GetData().GetReference();
											data.MakeConstant();
											
											return data;
										}
										else {
											return range.first->second->Constants[name].GetData();
										}
									}
									else if(range.first->second->Functions.Exists(name)) {
										return {Types::Function(), &range.first->second->Functions[name]};
									}
									else {
										throw SymbolNotFoundException(val.Name, SymbolType::Unknown);
									}
								}
							}
						}
						else {
							auto range=symbols.equal_range(val.Name);
							if(range.first==range.second) {
								throw SymbolNotFoundException(val.Name, SymbolType::Unknown);
							}
							else if(range.first!=--range.second) {
								throw AmbiguousSymbolException(val.Name, SymbolType::Unknown);
							}
							else {
								if(range.first->second.type==SymbolType::Constant) {
									if(reference) {
										auto data=range.first->second.object.Get<const Constant*>()->GetData().GetReference();
										data.MakeConstant();
										
										return data;
									}
									else {
										return range.first->second.object.Get<const Constant*>()->GetData();
									}
								}
								else if(range.first->second.type==SymbolType::Function) {
									return {Types::Function(), range.first->second.object.Get<const Function*>()};
								}
								else {
									throw SymbolNotFoundException(val.Name, SymbolType::Unknown, "An unsupported symbol is found");
								}
							}
						}
					}
					default:
						Utils::ASSERT_FALSE("Invalid value type.");
			}
		}
		
		void fixparameter(Data &param, const Type &pdef, const std::string &error) {
			if(param.GetType()==pdef) {
				//no worries
			}
			//to string
			else if(pdef==Integrals.Types["string"]) {
				param={Integrals.Types["string"], param.GetType().ToString(param)};
			}
			//from string 
			else if(param.GetType()==Integrals.Types["string"]) {
				param={pdef, pdef.Parse(param.GetValue<std::string>())};
			}
			//to variant
			else if(pdef==Variant) {
				param={Variant, param};
			}
			//from variant
			else if(param.GetType()==Variant && param.GetValue<Data>().GetType()==pdef) {
				param={pdef, param.GetValue<Data>()};
			}
			else {
				const Function *ctor=pdef.GetTypeCasting(param.GetType());
				
				if(ctor) {
					param=ctor->Call(false, {param});
				}
				else {
					throw CastException(param.GetType().GetName(), pdef.GetName(), error);
				}
			}
		}
		
		/// Calls the given function with the given values.
		Data VirtualMachine::callfunction(const Function *fn, bool method, const std::vector<Value> &incomingparams) {
			auto pin=incomingparams.begin();
			
			std::vector<Data> params;
			
			if(fn->HasParent() && !fn->IsStatic()) {
				if(pin!=incomingparams.end()) {
					// guarantees that the param is a reference
					Data param=getvalue(*pin, true);
					
					// check for nullness
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
					//else ok
					
					// check constantness
					if(!fn->IsConstant() && param.IsConstant()) {
						throw ConstantException(pin->Name, "While calling member function: "+fn->GetName());
					}
					//else ok
					
					if(param.GetType()==fn->GetParent()) {
						//no worries
					}
					else {
						param=param.GetType().MorphTo(fn->GetParent(), param);
					}
					
					params.push_back(param);
				}
				else {
					throw MissingParameterException("this", 0, fn->GetParent().GetName());
				}
				pin++;
			}
			//else nothing else is needed
			
			int ind=1;
			for(const auto &pdef : fn->Parameters) {
				if(pin!=incomingparams.end()) {
					if(pdef.IsVariable()) {
						if(pin->Type==ValueType::Variable || pin->Type==ValueType::Identifier) {
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
					else if(pdef.IsReference()) {
						Data param=getvalue(*pin, true);
						
						if(pdef.GetType()!=param.GetType()) {							
							param=param.GetType().MorphTo(pdef.GetType(),param);
						}
						
						params.push_back(param);
					}
					else {
						Data param=getvalue(*pin);
						
						fixparameter(param, pdef.GetType(), 
									"Cannot cast while trying to call "+fn->GetName());
						
						params.push_back(param);
					}
					
					++pin;
				}
				else { // no value is given for the parameter
					if(!pdef.IsOptional()) {
						throw MissingParameterException(pdef.GetName(), ind, pdef.GetType().GetName(),
														"Parameter "+pdef.GetName()+" is not optional."
						);
					}
					//else ok, it was optional
					
					break;
				}
				++ind;
			}
			
			// if last parameter can be repeated
			if(fn->RepeatLast()) {
				//add remaining parameters to the parameter list
				while(pin!=incomingparams.end()) {
					Data param=getvalue(*pin);
					fixparameter(param, fn->Parameters.Last().Current().GetType(),  
								 "Cannot cast while trying to call "+fn->GetName());
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
			//else no additional parameters at the end
			
			return fn->Call(method, params);
		}
		
		// !Careful there are early terminations
		/// Handles function/method call instructions
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
				fn=GetVariable(inst->Name.Name).GetValue<const Function *>();
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
					throw std::runtime_error("Invalid instruction, missing this parameter");
				}
				
				//search in the type of the first parameter, this ensures the data is a reference
				Data data=getvalue(inst->Parameters[0], true);

				if(functionname=="{}") { //constructor
					if(data.GetType()==Types::String()) {
						data={Reflection.Types["Type"], (const Type*)(&FindType(data.GetValue<std::string>()))};
					}
					else if(data.GetType()!=Reflection.Types["Type"]) {
						throw std::runtime_error("Invalid intermediate instruction, type parameter is not a type");
					}
					//else Reflection:Type typed variable/temporary
					
					std::vector<Data> params;
					int i=0;
					for(auto &pin : inst->Parameters) {
						if(i++!=0) {
							params.push_back(getvalue(pin));
						}
						//else ignore the first
					}
					
					//call the constructor
					Data ret=data.GetValue<const Type*>()->Construct(params);
					
					if(inst->Store) {
						temporaries[inst->Store]=ret;
					}
					//else do not store the created object, maybe a warning is necessary
					
					return;
				}				
				else if(functionname[0]=='.') { //data member
					//data access
					if(inst->Parameters.size()==1) {
						Data ret=Data::Invalid();
						if(!data.GetType().DataMembers.Exists(functionname.substr(1))) {
							//check parent symbols
							auto it=data.GetType().InheritedSymbols.Find(functionname.substr(1));
							
							//if found
							if(it.IsValid()) {
								//cast current data to its parent and perform the data retrieval
								ret=it.Current().second.DataMembers[functionname.substr(1)].Get(data.GetType().MorphTo(it.Current().second, data));
							}
							else {							
								throw SymbolNotFoundException(functionname.substr(1), SymbolType::Member);
							}
						}
						else {
							ret=data.GetType().DataMembers[functionname.substr(1)].Get(data);
						}
						
						//store the result
						if(inst->Store) {
							//variants should be casted
							if(ret.GetType()==Types::Variant()) {
								ret=ret.GetValue<Data>();
							}
							//else ok

							temporaries[inst->Store]=ret;
							
							//if the source is constant, then the result should be a constant too
							if(data.IsConstant())
								temporaries[inst->Store].MakeConstant();
						}
					}
					//data mutate
					else if(inst->Parameters.size()==2) {
						if(data.IsConstant()) {
							throw ConstantException(inst->Parameters[0].Name, "While setting member: "+functionname.substr(1));
						}
						//else ok
						
						//cannot store the result of assignment
						if(inst->Store) {
							throw NoReturnException("Data member: "+data.GetType().DataMembers[functionname.substr(1)].GetName());
						}
						//else ok
						
						if(!data.GetType().DataMembers.Exists(functionname.substr(1))) {
							//check parent symbols
							auto it=data.GetType().InheritedSymbols.Find(functionname.substr(1));
							
							//if found
							if(it.IsValid()) {
								//cast current data to its parent and let the rest do the work
								data=data.GetType().MorphTo(it.Current().second, data); //:data is reference
							}
							else {							
								throw SymbolNotFoundException(functionname.substr(1), SymbolType::Member);
							}
						}
						//else ok
						
						data.GetType().DataMembers[functionname.substr(1)].Set(data, getvalue(inst->Parameters[1]));
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
					} 
					//cannot find it
					else {
						
						//check inherited symbols for it
						auto it=data.GetType().InheritedSymbols.Find(functionname);
						//if found
						if(it.IsValid()) {
							//use that function instead
							fn=&it.Current().second.Functions[functionname];
						}
						else {						
							throw SymbolNotFoundException(functionname, SymbolType::Function, 
								"Cannot find the member function "+data.GetType().GetName()+":"+functionname);
						}
					}
				}
			}

			// call it
			Data ret=callfunction(fn, method, inst->Parameters);

			//if requested
			if(inst->Store) {
				if(fn->HasReturnType()) {
					//fix variants
					if(ret.GetType()==Types::Variant()) {
						ret=ret.GetValue<Data>();
					}

					//store the result
					temporaries[inst->Store]=ret;
				} 
				else {
					//requested but function does not return a value
					throw NoReturnException(functionname);
				}
			}
		}
		
		void VirtualMachine::execute(const Instruction* inst) {
			// assignment ...
			if(inst->Type==InstructionType::Assignment) {
				if(inst->Name.Type!=ValueType::Variable && inst->Name.Type!=ValueType::Identifier)
					throw std::runtime_error("Variables can only be represented with variables.");
				
				SetVariable(inst->Name.Name, getvalue(inst->RHS));
			}
			
			//function calls
			else if(inst->Type==InstructionType::FunctionCall) {
				functioncall(inst, false, false);
			} 
			else if(inst->Type==InstructionType::MemberFunctionCall) {
				functioncall(inst, true, false);
			} 
			else if(inst->Type==InstructionType::MethodCall) {
				functioncall(inst, false, true);
			}
			else if(inst->Type==InstructionType::MemberMethodCall) {
				functioncall(inst, true, true);
			}
			else if(inst->Type==InstructionType::RemoveTemp) {
				temporaries[inst->Store]=Data::Invalid();
			}
			
			//jumps
			else if(inst->Type==InstructionType::Jump) {
				executionscopes.Last()->Jumpto(executionscopes.Last()->GetLineNumber()+inst->JumpOffset);
			}
			else if(inst->Type==InstructionType::JumpTrue) {
				auto dat=getvalue(inst->RHS);
				fixparameter(dat, Types::Bool(), "While executing jump. The given value should be convertable to bool");
				if(dat.GetValue<bool>()) {
					executionscopes.Last()->Jumpto(executionscopes.Last()->GetLineNumber()+inst->JumpOffset);
				}
			}
			else if(inst->Type==InstructionType::JumpFalse) {
				auto dat=getvalue(inst->RHS);
				fixparameter(dat, Types::Bool(), "While executing jump. The given value should be convertable to bool");
				if(!dat.GetValue<bool>()) {
					executionscopes.Last()->Jumpto(executionscopes.Last()->GetLineNumber()+inst->JumpOffset);
				}
			}
			
			else {
				Utils::ASSERT_FALSE("Unknown instruction type.", 0, 8);
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