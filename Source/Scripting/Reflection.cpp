#include "../Scripting.h"
#include "Reflection.h"
#include "VirtualMachine.h"

namespace Gorgon { namespace Scripting {	
	
	void Function::init() {
		if(keyword) {
			KeywordNames.insert(name);
		}
	}
	
	Type *TypeType();
	
	Library::Library(const std::string &name, const std::string &help,
	TypeList types, FunctionList functions, ConstantList constants) :
	name(name), help(help), Types(this->types), Functions(this->functions), Constants(this->constants)
	{
		using std::swap;
		
		swap(types, this->types);
		swap(functions, this->functions);
		swap(constants, this->constants);
		
		for(const auto &type : this->types) {
			this->constants.Add(
				new Constant(type.first, type.second.GetHelp(), {TypeType(), &type.second})
			);
		}
	}
	
	void Library::AddTypes(const std::vector<Type*> &list) {
		for(auto &type : list) {
			ASSERT(!SymbolExists(type->GetName()), "Symbol "+type->GetName()+" already exists", 1, 2);

			types.Add(type);

			this->constants.Add(
				new Constant(type->GetName(), type->GetHelp(), {TypeType(), type})
			);
		}
	}
	
	Data Type::Construct(const std::vector<Data> &parameters) const {
		std::multimap<int, const Function::Variant *> rankedlist;
		
		for(const auto &var : constructor.Variants) {
			int status=0;
			
			auto pin = parameters.begin();
			for(const auto &pdef : var.Parameters) {
				if(pin==parameters.end()) {
					if(pdef.IsOptional()) { 
						continue; 
					}
					else {
						status=-1;
						break;
					}
				}
				
				if(pdef.GetType()==pin->GetType()) {
					// perfect match
				}
				else if(pdef.GetType().GetTypeCastingFrom(pin->GetType())) {
					// good match
					status++; 
				}
				else {
					// bad bad match
					status+=2;
				}
				
				++pin;
			}
			if(status==-1) continue;
			
			if(pin!=parameters.end()) {
				if(var.Parameters.size() && var.RepeatLast()) {
					int worst=0;
					const auto &pdef=*var.Parameters.rbegin();
					while(pin!=parameters.end()) {
						if(pdef.GetType()==pin->GetType()) {
							// perfect match
						}
						else if(pdef.GetType().GetTypeCastingFrom(pin->GetType())) {
							// good match
							if(worst<1) worst=1;
						}
						else {
							// bad bad match
							worst=2;
						}
						
						++pin;
					}
					
					status+=worst;
				}
				else {
					status=-1;
				}
			}
			
			if(status!=-1) {
				rankedlist.insert(std::make_pair(status, &var));
			}
		}
		
		if(rankedlist.size()==0) {
			std::string pnames;
			for(const auto &param : parameters) {
				if(pnames!="") pnames += ", ";
				pnames+=param.GetType().GetName();
			}
			throw SymbolNotFoundException("Constructor", SymbolType::Function, 
										  "Constructor for "+name+" with parameters: "+pnames+" not found");
		}
		else {
			return rankedlist.begin()->second->Call(false, parameters);
		}
	}
	
	Type::Type(const std::string& name, const std::string& help, const Any& defaultvalue, Any::TypeInterface* consttype, Any::TypeInterface* ptrtype, Any::TypeInterface* constptrtype, bool isref):
		name(name), help(help), DataMembers(datamembers), Functions(functions), Constructor(constructor),
		Constants(constants), Events(events), InheritsFrom(inheritsfrom), defaultvalue(defaultvalue),
		referencetype(isref), TypeInterface(defaultvalue.GetTypeInterface()), ConstTypeInterface(consttype),
		PtrTypeInterface(ptrtype), ConstPtrTypeInterface(constptrtype), Parents(parents), InheritedSymbols(inheritedsymbols),
		constructor("{}", "Constructs "+name, this, Containers::Collection<Function::Variant>(), StaticTag)
	{
		ASSERT((defaultvalue.GetTypeInterface()->PtrTypeInfo()==PtrTypeInterface->TypeInfo()),
			"The type and its pointer does not match");
	}
	
	Data Type::MorphTo(const Type& type, Data source, bool allowtypecast) const {
		auto inheritance=inheritsfrom.find(type);
		Inheritance::ConversionFunction fn;
		bool downcasting=false;
		
		//not a direct parent
		if(inheritance==inheritsfrom.end()) {
			//relative parent?
			auto relative=parents.find(type);
			
			if(relative!=parents.end()) {
				//it is relative parent, call parent to handle it
				auto data=relative->second->MorphTo(type, inheritsfrom.at(relative->first).to(source));
					
				ASSERT((data.GetType()==type), "Type casting function from "+GetName()+" to "+type.GetName()+
					" does not perform its job. Instead it casts data to "+data.GetType().GetName());
				
				return data;				
			}
			
			//check reverse, may be this is a type that is inherited from this one
			inheritance=type.inheritsfrom.find(this);
			
			//not a direct decendant
			if(inheritance==type.inheritsfrom.end()) {
				auto relative=type.parents.find(this);
				
				if(relative!=type.parents.end()) {
					auto data=inheritsfrom.at(relative->first).from(relative->second->MorphTo(type, source));
					
					ASSERT((data.GetType()==type), "Type casting function from "+GetName()+" to "+type.GetName()+
						" does not perform its job. Instead it casts data to "+data.GetType().GetName());
					
					return data;
				}
				
				//Try casting
				if(allowtypecast && type.GetTypeCastingFrom(*this)!=nullptr) {
					//call the constructor and perform conversion
					auto data=type.GetTypeCastingFrom(*this)->Call(false, {source});
					
					ASSERT((data.GetType()==type), "Type casting function from "+GetName()+" to "+type.GetName()+
						" does not perform its job. Instead it casts data to "+data.GetType().GetName());
					
					return data;				
				}
				
				throw CastException(GetName(), type.GetName(), "Source type is neither derived from destination or is a parent of it.");
			}
			
			//downcasting requires a reference
			if( source.IsReference() || (IsReferenceType() && type.IsReferenceType()) ) {
				fn=inheritance->second.from;
				downcasting=true;
			}
			else {
				throw CastException(GetName(), type.GetName(), "Source type is not derived from destination and the data is not a reference.");
			}
		}
		else {
			//direct parent
			fn=inheritance->second.to;
		}
		
		try {
			auto data=fn(source);
			
			ASSERT((data.GetType()==type), "Type casting function from "+GetName()+" to "+type.GetName()+
				" does not perform its job. Instead it casts data to "+data.GetType().GetName());
			
			return data;
		}
		catch(const std::bad_cast &) {
			if(downcasting) {
				throw CastException(GetName(), type.GetName(), "Source is not instantiated from the target");
			}
			else {
				throw CastException(GetName(), type.GetName());
			}
		}
		
	}
	
	Type::MorphType Type::CanMorphTo(const Type& type) const {
		auto inheritance=inheritsfrom.find(type);
		Inheritance::ConversionFunction fn;
		bool downcasting=false;
		
		//not a direct parent
		if(inheritance==inheritsfrom.end()) {
			//relative parent?
			auto relative=parents.find(type);
			
			if(relative!=parents.end()) {
				//it is relative parent, call parent to handle it
				return relative->second->CanMorphTo(type);
			}
			
			//check reverse, may be this is a type that is inherited from this one
			inheritance=type.inheritsfrom.find(this);
			
			//not a direct decendant
			if(inheritance==type.inheritsfrom.end()) {
				auto relative=type.parents.find(this);
				
				if(relative!=type.parents.end()) {
					return DownCasting;
				}
				
				//Try casting
				if(type.GetTypeCastingFrom(*this)!=nullptr) {
					return TypeCasting;				
				}
				
				return NotPossible;
			}
			
			return DownCasting;
		}
		else {
			return UpCasting;
		}		
	}
	
	void Type::AddInheritance(const Type& type, Type::Inheritance::ConversionFunction from, Type::Inheritance::ConversionFunction to) {
		inheritsfrom.insert(std::make_pair(&type, Inheritance(type, from, to)));

		for(auto t : type.parents) {
			parents.insert(std::make_pair(t.first, &type));
		}

		for(const auto &fn : type.Functions) {
			if(functions.Find(fn.first)==functions.end()) {
				ASSERT(!inheritedsymbols.Find(fn.first).IsValid(), "Symbol: "+fn.first+" is ambiguous");

				inheritedsymbols.Add(fn.first, type);
			}
		}

		for(const auto &c : type.Constants) {
			if(constants.Find(c.first)==constants.end()) {
				ASSERT(!inheritedsymbols.Find(c.first).IsValid(), "Symbol: "+c.first+" is ambiguous");

				inheritedsymbols.Add(c.first, type);
			}
		}

		for(const auto &e : type.Events) {
			if(events.Find(e.first)==events.end()) {
				ASSERT(!inheritedsymbols.Find(e.first).IsValid(), "Symbol: "+e.first+" is ambiguous");

				inheritedsymbols.Add(e.first, type);
			}
		}

		for(const auto &d : type.DataMembers) {
			if(datamembers.Find(d.first)==datamembers.end()) {
				ASSERT(!inheritedsymbols.Find(d.first).IsValid(), "Symbol: "+d.first+" is ambiguous");

				inheritedsymbols.Add(d.first, type);
			}
		}

		for(const auto &s : type.inheritedsymbols) {
			if( functions.Find(s.first)==functions.end() && 
				constants.Find(s.first)!=constants.end() &&
				events.Find(s.first)!=events.end() &&
				datamembers.Find(s.first)!=datamembers.end()
			) {
				ASSERT(!inheritedsymbols.Find(s.first).IsValid(), "Symbol: "+s.first+" is ambiguous");
		
				inheritedsymbols.Add(s.first, s.second);
			}
		}
	}
	
} }