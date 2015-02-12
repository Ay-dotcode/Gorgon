#include <map>

#include "Data.h"
#include "Exceptions.h"
#include "VirtualMachine.h"

namespace Gorgon { namespace Scripting {
	
	Data::Data(const Data &other) {
		data=other.data;
		type=other.type;
		
		
		if(type && type->IsReferenceType() && data.Pointer() && VirtualMachine::Exists()) {
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
		
		if(type.IsReferenceType() && data.Pointer() && VirtualMachine::Exists()) {
			VirtualMachine::Get().References.Increase(*this);
		}
	}
	
	Data::Data(const Type *type, const Any &data) : type(type), data(data) {
		check();
		ASSERT((type!=nullptr), "Data type cannot be nullptr", 1, 2);
		
		if(type->IsReferenceType() && data.Pointer() && VirtualMachine::Exists()) {
			VirtualMachine::Get().References.Increase(*this);
		}
	}
	
	void Data::check() {
		ASSERT(type->GetDefaultValue().IsSameType(data), "Given data type ("+data.GetTypeName()+
		") does not match with: "+type->GetName()+" ("+type->GetDefaultValue().GetTypeName()+")"
		, 2, 2
		);
	}
	
	std::string Data::ToString() const {
		ASSERT(type, "Type is not set", 1, 2);
		
		return type->ToString(*this);
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
	
	bool Data::IsNull() const {
		ASSERT(type, "Type is not set", 1, 2);
		
		if(type->IsReferenceType()) {
			return data.Pointer()==nullptr;
		}
		else
			return false;
	}
	
	Data::~Data() {
		if(type && type->IsReferenceType() && data.IsSet() && data.Pointer() && VirtualMachine::Exists()) {
			VirtualMachine::Get().References.Decrease(*this);
		}
	}
	
	Data GetVariableValue(const std::string &varname) { throw 0; }
	
} }
