#include "../Scripting.h"
#include "Reflection.h"
#include "VirtualMachine.h"

namespace Gorgon { namespace Scripting {	
	
	bool Function::CallEnd(Data data) const { 
		assert( false && "End call on a non-scoped function"); 
		
		return false;
	}
	
	void Function::CallRedirect(Data,std::string &) const { 
		assert( false && "Redirect call on a non-redirecting function");
	}
	
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
		std::multimap<int, const Function *> rankedlist;
		
		for(const auto &fn : constructors) {
			int status=0;
			
			auto pin = parameters.begin();
			for(const auto &pdef : fn.Parameters) {
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
				else if(pdef.GetType().GetTypeCasting(pin->GetType())) {
					// good match
					status++; 
				}
				else {
					// bad bad match
					status+=2;
				}
				
				++pin;
			}
			if(status==-1) break;
			
			if(pin!=parameters.end()) {
				if(fn.parameters.GetCount() && fn.RepeatLast()) {
					int worst=0;
					const auto &pdef=*fn.parameters.Last();
					while(pin!=parameters.end()) {
						if(pdef.GetType()==pin->GetType()) {
							// perfect match
						}
						else if(pdef.GetType().GetTypeCasting(pin->GetType())) {
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
				rankedlist.insert(std::make_pair(status, &fn));
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
		name(name), help(help), DataMembers(datamembers), Functions(functions), Constructors(constructors),
		Constants(constants), Events(events), InheritsFrom(inheritsfrom), defaultvalue(defaultvalue),
		referencetype(isref), TypeInterface(defaultvalue.GetTypeInterface()), ConstTypeInterface(consttype),
		PtrTypeInterface(ptrtype), ConstPtrTypeInterface(constptrtype), Parents(parents), InheritedSymbols(inheritedsymbols)
	{
		ASSERT((defaultvalue.GetTypeInterface()->PtrTypeInfo()==PtrTypeInterface->TypeInfo()),
			"The type and its pointer does not match");
	}
	
	Data Type::MorphTo(const Type& type, Data source) const {
		auto inheritance=inheritsfrom.find(type);
		Inheritance::ConversionFunction fn;
		bool downcasting=false;
		
		//not a direct parent
		if(inheritance==inheritsfrom.end()) {
			//relative parent?
			auto relative=parents.find(type);
			
			if(relative!=parents.end()) {
				//it is relative parent, call parent to handle it
				return relative->second->MorphTo(type, inheritsfrom.at(relative->first).to(source));
			}
			
			//check reverse, may be this is a type that is inherited from this one
			inheritance=type.inheritsfrom.find(this);
			
			//not a direct decendant
			if(inheritance==type.inheritsfrom.end()) {
				auto relative=type.parents.find(this);
				
				if(relative!=type.parents.end()) {
					return inheritsfrom.at(relative->first).from(relative->second->MorphTo(type, source));
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
		catch(const std::bad_cast &cast) {
			if(downcasting) {
				throw CastException(GetName(), type.GetName(), "Source is not instantiated from the target");
			}
			else {
				throw CastException(GetName(), type.GetName());
			}
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