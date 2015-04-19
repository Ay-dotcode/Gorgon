#include "VirtualMachine.h"
#include "Exceptions.h"
#include "RuntimeFunction.h"
#include "../Scripting.h"



namespace Gorgon {
	
	namespace Scripting {		
		
		Containers::Hashmap<std::thread::id, VirtualMachine, &VirtualMachine::getthread> VirtualMachine::activevms;


		extern Type &Variant;

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

			toplevel=scopes.Last()->Instantiate();
			scopeinstances.push_back(toplevel);

			Run();
		}
	
		void VirtualMachine::Begin(InputProvider &input) {
			Activate();
			scopes.Add(new Scope(input, input.GetName()));

			toplevel=scopes.Last()->Instantiate();
			scopeinstances.push_back(toplevel);
		}

		void VirtualMachine::Run() {
			int target=scopeinstances.size()-1;
			if(target<0) {
				throw std::runtime_error("No scope to execute.");
			}

			Run(target);
		}

		void VirtualMachine::Run(std::shared_ptr<ScopeInstance> scope) {
			int target=scopeinstances.size();

			scopeinstances.push_back(scope);

			Run(target);
		}

		void VirtualMachine::Run(unsigned executiontarget) {
			returnimmediately=false;
			returnvalue=Data::Invalid();

			//until execution target is reached
			while(scopeinstances.size()>(long)executiontarget) {
				if(returnimmediately) {
					if(toplevel && scopeinstances.back().get()==toplevel.get()) {
						returnvalue=scopeinstances.back()->ReturnValue;
					}
					scopeinstances.pop_back();
					continue;
				}
				
				auto inst=scopeinstances.back()->Get();
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
						scopeinstances.pop_back();
					} 
					else {
						execute(inst);
					}
				}
 				catch(Exception &ex) {
					if(scopeinstances.size()) {
						scopeinstances.back()->MoveToEnd();
						
						if(ex.GetLine()<=0) {
							ex.SetLine(-ex.GetLine()+scopeinstances.back()->GetPhysicalLine());
						}
					}
 
 					throw ex;
 				}
			}
		}
		
		Variable &VirtualMachine::GetVariable(const std::string &name) {
			ASSERT(scopeinstances.size(), "No scope instance is active");
			
			auto var=scopeinstances.back()->GetVariable(name);

			//if found
			if(var) {
				//return
				return *var;
			}
			
/*			//search globals
			auto varit=globalvariables.find(String::ToLower(name));

			//if found
			if(varit!=globalvariables.end()) {
				//return
				return &varit->second;
			}
			//if not
			else */{
				//nothing more to try
				throw SymbolNotFoundException(name, SymbolType::Variable);
			}
		}
		
		void VirtualMachine::SetVariable(const std::string &name, Data data) {
			ASSERT(scopeinstances.size(), "No scope instance is active");
			
			//check if it exists
			auto var=scopeinstances.back()->GetVariable(name);
			
			//if found
			if(var) {
				//assign and return 
				var->Set(data);
				
				return;
			}
			
			//create a new one
			scopeinstances.back()->SetVariable(name, data);
		}
		
		void VirtualMachine::UnsetVariable(const std::string &name) {
			ASSERT(scopeinstances.size(), "No scope instance is active");
			
			scopeinstances.back()->UnsetVariable(name);
		}
		
		bool VirtualMachine::IsVariableSet(const std::string &name) {
			ASSERT(scopeinstances.size(), "No scope instance is active");
			
			auto var=scopeinstances.back()->GetVariable(name);

			//if found
			if(var) {
				//return
				return true;
			}
			
			return false;
		}
		
		void VirtualMachine::CompileCurrent() {
			if(scopeinstances.size()==0) {
				throw FlowException("No active execution scope");
			}
			
			scopeinstances.back()->Compile();
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
					
					if(reference) {
						if(!data.IsReference()) { 
							//temporaries that are not already a reference, cannot be converted to
							//a reference
							throw CastException("non reference temporary", "reference");
						}
							
						return data;
					}
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
								throw SymbolNotFoundException(name, SymbolType::Identifier);
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
									throw SymbolNotFoundException(libname, SymbolType::Identifier);
								}
								else if(range.first!=--range.second) {
									throw AmbiguousSymbolException(libname, SymbolType::Identifier);
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
										throw SymbolNotFoundException(val.Name, SymbolType::Identifier);
									}
								}
							}
						}
						else {
							auto range=symbols.equal_range(val.Name);
							if(range.first==range.second) {
								throw SymbolNotFoundException(val.Name, SymbolType::Identifier);
							}
							else if(range.first!=--range.second) {
								throw AmbiguousSymbolException(val.Name, SymbolType::Identifier);
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
									throw SymbolNotFoundException(val.Name, SymbolType::Identifier, "An unsupported symbol is found");
								}
							}
						}
					}
					default:
						Utils::ASSERT_FALSE("Invalid value type.");
			}
		}
		
		void fixparameter(Data &param, const Type &pdef, bool ref, const std::string &error) {
			if(param.GetType()==pdef) {
				//no worries
			}
			//to variant
			else if(pdef==Variant) {
				param={Variant, param};
			}
			//to string
			else if(pdef==Integrals.Types["string"]) {
				param={Integrals.Types["string"], param.GetType().ToString(param)};
			}
// 			//from string 
// 			else if(param.GetType()==Integrals.Types["string"]) {
// 				param={pdef, pdef.Parse(param.GetValue<std::string>())};
// 			}
			//from variant
			else if(param.GetType()==Variant && param.GetValue<Data>().GetType()==pdef) {
				param={pdef, param.GetValue<Data>()};
			}
			else {
				param=param.GetType().MorphTo(pdef, param, !ref);
			}
		}
		
		
		/// Calls the given function with the given values.
		Data VirtualMachine::callfunction(const Function *fn, bool method, const std::vector<Value> &incomingparams) {
			const Function::Overload *var=nullptr;
			
			int count=fn->Overloads.GetCount()+method*fn->Methods.GetCount();
			ASSERT(count, "This function has no registered body.");
			
			// easy way out, only a single variant exists
			if(count==1) {
				if(fn->Overloads.GetCount()==0) {
					ASSERT(method, "This function has no registered body.");
					return callvariant(fn, &fn->Methods[0], method, incomingparams);
				}
				else {
					return callvariant(fn,& fn->Overloads[0], method, incomingparams);
				}
			}
			
			
			//find correct variant
			std::multimap<int, const Function::Overload*> variantlist;
			
			auto checkparam=[this](const Parameter &param, const Value &cval) {
				int c=0;
				//target is reference
				if(param.IsReference() || param.GetType().IsReferenceType()) {
					Data d=Data::Invalid();
					try {
						d=getvalue(cval, !param.IsConstant());
					} 
					catch(const CastException &) { 
						return -1;
					}
					
					if(param.IsConstant()) {
						if(cval.Type==ValueType::Literal) c+=1;
						
						if(!d.IsConstant()) c+=1;
					}
					else { //non const ref
						if(d.IsConstant()) {
							return -1;
						}
					}
					
					//check polycast
					//if const check implicit cast
					if(d.GetType()==param.GetType() || param.GetType()==Variant) {
						return c;
					}
					else if(d.GetType().Parents.count(param.GetType())) {
						//polycast
						return c+2;
					}
					else if(param.GetType().Parents.count(d.GetType())) {
						//polycast downcast
						return c+3;
					}
					else if(param.IsConstant() && param.GetType().GetTypeCastingFrom(d.GetType())) {
						//implicit constructor cast
						return c+10;
					}
					else {
						return -1;
					}
					
				}
				else { //target is non reference
					Data d=getvalue(cval);
					
					if(d.IsConstant()) {
						if(d.IsReference()) {
							if(!param.IsConstant()) {
								return -1;
							}
						}
						else {
							c+=!param.IsConstant();
						}
					}
					else {
						if(param.IsConstant()) c+=1;
					}
					
					//check implicit cast
					//if source is ref check polycast
					if(d.GetType()==param.GetType() || param.GetType()==Variant) {
						return c;
					}
					else if(param.GetType().GetTypeCastingFrom(d.GetType())) {
						//implicit constructor cast
						return c+10;
					}
					else if(d.GetType().Parents.count(param.GetType())) { //poly upcasting
						//polycast
						return c+2;
					}
					else { 
						//last chance, polymorphic down casting
						try {
							//requires a reference
							d=getvalue(cval, true);
						} catch(...) { return -1; }
						
						//reference and constant
						if(d.IsConstant() && !param.IsConstant()) {
							return -1;
						}
						
						if(param.GetType().Parents.count(d.GetType())) {
							return c+3;
						}
						
						return -1;
					}
				}
			};
			
			auto list=[&](const Containers::Collection<Function::Overload> &variants, int start) {
				for(const auto &var : variants) {
					auto pin=incomingparams.begin();
					int current=0;
					
					if(fn->IsMember() && !fn->IsStatic()) {
						if(pin==incomingparams.end()) {
							throw MissingParameterException(
								"this", 1, fn->GetOwner().GetName(), 
								"Missing this pointer for member function call"
							);
						}

						auto p=Scripting::Parameter("","",fn->GetOwner(), ReferenceTag, (var.IsConstant() ? ConstTag : ReferenceTag));
						
						current=checkparam(p, *pin);
						if(current==-1) continue;
						
						++pin;
					}
					
					for(const Parameter &param : var.Parameters) {
						if(pin==incomingparams.end()) {
							if(!param.IsOptional()) {
								current=-1;
								break;
							}
							else continue;
						}

						const Value &cval=*pin;
						
						int c=checkparam(param, cval);
						if(c==-1) {
							current=-1;
							break;
						}
						else {
							current+=c;
						}
						
						++pin;
					}
					if(current==-1) continue;
					
					//check extra parameters
					if(pin!=incomingparams.end()) {
						if(var.RepeatLast() && var.Parameters.size()) {
							int maxp=0;
							while(pin!=incomingparams.end()) {
								int c=checkparam(var.Parameters.back(), *pin);
								if(c==-1) {
									//parameter does not fit repeating parameter
									continue;
								}
								else {
									if(c>maxp) maxp=c;
								}
								
								++pin;
								current+=5;
							}
							
							current+=maxp;
						}
						else {
							//extra params
							continue;
						}
					}
					
					if(var.GetParent().IsOperator() && var.Parameters[1].GetType()==var.GetParent().GetOwner() && current>0) {
						current-=1;
					}
					
					variantlist.insert(std::make_pair(current,&var));
				}
			};
			
			if(method) {
				list(fn->Methods, 0);
			}
			list(fn->Overloads, method ? 1000 : 0);
			
			if(variantlist.size()) {
				if(variantlist.size()>1 && variantlist.count(variantlist.begin()->first)>1) {
					throw AmbiguousSymbolException(fn->GetName(), SymbolType::Function, "For these arguments.");
				}

				var=variantlist.begin()->second;
			}
			else {
				///... better error reporting
				throw SymbolNotFoundException(fn->GetName(), SymbolType::Function, "For these arguments.");
			}
			
			return callvariant(fn, var, method, incomingparams);
		}
		
		Data VirtualMachine::callvariant(const Function *fn, const Function::Overload *variant, bool method, const std::vector<Value> &incomingparams) {
			auto pin=incomingparams.begin();
			
			std::vector<Data> params;
			
			if(fn->IsMember() && !fn->IsStatic()) {
				if(pin!=incomingparams.end()) {
					// guarantees that the param is a reference
					Data param;
					
					//if data is literal and variant is constant, then a copy of
					//the value can be passed
					if(variant->IsConstant() && (pin->Type==ValueType::Literal || 
						(pin->Type==ValueType::Temp && !temporaries[pin->Result].IsReference())))
						param=getvalue(*pin);
					else
						param=getvalue(*pin, true);
					
					// check for nullness
					if(fn->GetOwner().IsReferenceType()) {
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
					if(!variant->IsConstant() && param.IsConstant()) {
						throw ConstantException(pin->Name, "While calling member function: "+fn->GetName());
					}
					//else ok
					
					if(param.GetType()==fn->GetOwner()) {
						//no worries
					}
					else {
						param=param.GetType().MorphTo(fn->GetOwner(), param);
					}
					
					params.push_back(param);
				}
				else {
					throw MissingParameterException("this", 0, fn->GetOwner().GetName());
				}
				pin++;
			}
			//else nothing else is needed
			
			int ind=1;
			for(const auto &pdef : variant->Parameters) {
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
					else if(pdef.IsReference() && !pdef.IsConstant()) {
						Data param=getvalue(*pin, true);
						
						if(pdef.GetType()!=param.GetType()) {
							param=param.GetType().MorphTo(pdef.GetType(), param, pdef.IsConstant());
						}
						
						params.push_back(param);
					}
					else {
						Data param;
						if(pdef.IsReference()) {
							if((pin->Type==ValueType::Literal || (pin->Type==ValueType::Temp && !temporaries[pin->Result].IsReference())))
								param=getvalue(*pin);
							else
								param=getvalue(*pin, true);
						}
						else {
							param=getvalue(*pin);
						}
						
						fixparameter(param, pdef.GetType(), pdef.IsReference(),
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
			if(variant->RepeatLast()) {
				//add remaining parameters to the parameter list
				while(pin!=incomingparams.end()) {
					Data param=getvalue(*pin);
					fixparameter(param, variant->Parameters.back().GetType(), variant->Parameters.back().IsReference(),
								 "Cannot cast while trying to call "+fn->GetName());
					params.push_back(param);
					++pin;
				}
			}
			// if not and there are extra parameters
			else if(pin!=incomingparams.end()) {
				throw TooManyParametersException(incomingparams.size(), variant->Parameters.size(), 
												 "Too many parameters supplied for "+fn->GetName()
				);
			}
			//else no additional parameters at the end
			
			return variant->Call(method, params);
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
			else if((inst->Name.Type==ValueType::Identifier || inst->Name.Type==ValueType::Variable) && IsVariableSet(inst->Name.Name)) {
				auto var=GetVariable(inst->Name.Name);
				
				Type *FunctionType();				
				if(var.GetType()!=FunctionType()) {
					throw CastException(var.GetType().GetName(), "Function", inst->Name.Name+
						" is a variable that do not contain a function");
				}
				
				if(var.IsConstant())
					fn=var.GetValue<const Function *>();
				else
					fn=var.GetValue<Function *>();
			}
			else if(inst->Name.Type==ValueType::Identifier) {
				functionname=inst->Name.Name;
			}
			else {
				throw SymbolNotFoundException(inst->Name.Name, SymbolType::Function);
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
				
				Data data;
				//search in the type of the first parameter, this ensures the data is a reference
				if(inst->Parameters[0].Type==ValueType::Literal) {
					data=getvalue(inst->Parameters[0]);
				}
				else {
					data=getvalue(inst->Parameters[0], true);
				}

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
							if(data.IsConstant() || !data.IsReference())
								temporaries[inst->Store].MakeConstant();
						}
					}
					//data mutate
					else if(inst->Parameters.size()==2) {
						if(data.IsConstant() || !data.IsReference()) {
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
				if(ret.IsValid()) {
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
				scopeinstances.back()->Jumpto(scopeinstances.back()->GetLineNumber()+inst->JumpOffset);
			}
			else if(inst->Type==InstructionType::JumpTrue) {
				auto dat=getvalue(inst->RHS);
				fixparameter(dat, Types::Bool(), false, "While executing jump. The given value should be convertable to bool");
				if(dat.GetValue<bool>()) {
					scopeinstances.back()->Jumpto(scopeinstances.back()->GetLineNumber()+inst->JumpOffset);
				}
			}
			else if(inst->Type==InstructionType::JumpFalse) {
				auto dat=getvalue(inst->RHS);
				fixparameter(dat, Types::Bool(), false, "While executing jump. The given value should be convertable to bool");
				if(!dat.GetValue<bool>()) {
					scopeinstances.back()->Jumpto(scopeinstances.back()->GetLineNumber()+inst->JumpOffset);
				}
			}
			
			//this instruction declares a new overload
			else if(inst->Type==InstructionType::DeclOverload) {
				ASSERT(inst->Parameters.size()>=3, "Overload declaration requires iskeyword, return type, and instructionlist");
				Function *fn;
				if(IsVariableSet(inst->Name.Name)) {
					fn=GetVariable(inst->Name.Name).ReferenceValue<Function *>();
				}
				else {
					fn=new Function(inst->Name.Name, "", nullptr, inst->Parameters[0].Literal.GetValue<bool>(), false, false);
					References.Register(fn);
					Type *FunctionType();
					SetVariable(inst->Name.Name, {FunctionType(), fn});
				}
				
				auto overld=new RuntimeOverload(CurrentScopeInstance().GetScope(), nullptr, 
											{}, false, false, false, false, false, false, false);
				
				overld->SaveInstructions(*inst->Parameters[2].Literal.ReferenceValue<std::vector<Instruction>*>());
				fn->AddOverload(overld);
			}
			
			else {
				Utils::ASSERT_FALSE("Unknown instruction type.", 0, 8);
			}
		}
		
		void VirtualMachine::Jump(SourceMarker marker) {
			
			if(reinterpret_cast<ScopeInstance*>(marker.GetSource())!=scopeinstances.back().get()) {
				throw FlowException("Jump destination is not valid", "While performing a short jump, a different "
					"execution scope is requested."
				);
			}
			
			scopeinstances.back()->Jumpto(marker.GetLine());
		}


	}
}