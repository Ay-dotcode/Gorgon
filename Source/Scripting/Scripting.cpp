#include "../Scripting.h"
#include "Reflection.h"
#include "VirtualMachine.h"
#include "Embedding.h"
#include <map>

namespace Gorgon { namespace Scripting {

	Data::Data(const Data &other) {
		data=other.data;
		type=other.type;


		if(type && type->IsReferenceType() && data.Pointer()) {
			VirtualMachine::Get().References.Increase(*this);
		}
	}

	Data::Data(Data &&other) {
		data=other.data;
		type=other.type;

		other.data=Any();
		other.type=nullptr;
	}
	
	Data::Data(const Type& type) : type(&type) {
		data = type.GetDefaultValue();
		
		if(type.IsReferenceType() && data.Pointer()) {
			VirtualMachine::Get().References.Increase(*this);
		}
	}

	Data::Data(const Type *type, const Any &data) : type(type), data(data) {
		check();
		ASSERT((type!=nullptr), "Data type cannot be nullptr", 1, 2);

		if(type->IsReferenceType() && data.Pointer()) {
			VirtualMachine::Get().References.Increase(*this);
		}
	}

	void Data::check() {
		ASSERT(type->GetDefaultValue().IsSameType(data), "Given data type ("+data.GetTypeName()+
			   ") does not match with: "+type->GetName()+" ("+type->GetDefaultValue().GetTypeName()+")"
			   , 2, 2
		);
	}

	Data &Data::operator =(Data other) {
		if(type && type->IsReferenceType() && data.IsSet() && data.Pointer()) {
			VirtualMachine::Get().References.Decrease(*this);
		}
		
		type=other.type;
		data=other.data;
		
		other.data=Any();
		other.type=nullptr;
		
		return *this;
	}

	Data::~Data() {
		if(type && type->IsReferenceType() && data.IsSet() && data.Pointer()) {
			VirtualMachine::Get().References.Decrease(*this);
		}
	}
	
	bool Function::CallEnd(Data data) const { 
		assert( false && "End call on a non-scoped function"); 

		return false;
	}
	
	void Function::CallRedirect(Data,std::string &) const { 
		assert( false && "Redirect call on a non-redirecting function");
	}
	
	Data GetVariableValue(const std::string &varname) { throw 0; }

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
	
	MappedValueType<Data, String::From<Data>, GetVariableValue> Variant = {"Variant", 
		"This type can contain any type.",
		Data::Invalid()
	};
	
	int VariableScope::nextid=0;
} }
