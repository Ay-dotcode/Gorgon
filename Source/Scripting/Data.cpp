#include <map>

#include "Data.h"
#include "Exceptions.h"
#include "VirtualMachine.h"

namespace Gorgon { namespace Scripting {
	
	Data::Data(const Data &other) {
		data=other.data;
		type=other.type;
		isreference=other.isreference;
		
		if(isreference)
			isconstant=other.isconstant;
		
		
		if(type && type->IsReferenceType() && data.Pointer() && VirtualMachine::Exists()) {
			VirtualMachine::Get().References.Increase(*this);
		}
	}
	
	Data::Data(Data &&other) {
		data=other.data;
		type=other.type;
		
		isreference=other.isreference;
		isconstant=other.isconstant;
		
		other.data=Any();
		other.type=nullptr;
	}
	
	Data::Data(const Type& type) : type(&type) {
		data = type.GetDefaultValue();
		
		if(type.IsReferenceType() && data.Pointer() && VirtualMachine::Exists()) {
			VirtualMachine::Get().References.Increase(*this);
		}
	}
	
	Data::Data(const Type *type, const Any &data, bool isreference, bool isconstant) : 
		type(type), data(data), 
		isreference(isreference), isconstant(isconstant)
	{
		check();
		ASSERT((type!=nullptr), "Data type cannot be nullptr", 1, 2);
		
		if(type->IsReferenceType() && data.Pointer() && VirtualMachine::Exists()) {
			VirtualMachine::Get().References.Increase(*this);
		}
	}
	
	void Data::check() {
		if(isconstant) {
			if(isreference && !type->IsReferenceType()) {
				ASSERT(data.IsSameConstPtrOfType(type->GetDefaultValue()), "Given data type ("+data.GetTypeName()+
				") does not match with: "+type->GetName()+" ("+type->GetDefaultValue().GetTypeName()+")"
				, 2, 2
				);
			}
			else {
				ASSERT(data.IsSameConstOfType(type->GetDefaultValue()), "Given data type ("+data.GetTypeName()+
				") does not match with: "+type->GetName()+" ("+type->GetDefaultValue().GetTypeName()+")"
				, 2, 2
				);
			}
		}
		else {
			if(isreference && !type->IsReferenceType()) {
				ASSERT(data.IsSamePtrOfType(type->GetDefaultValue()), "Given data type ("+data.GetTypeName()+
					") does not match with: "+type->GetName()+" ("+type->GetDefaultValue().GetTypeName()+")"
					, 2, 2
				);
			}
			else {
				ASSERT(data.IsSameType(type->GetDefaultValue()), "Given data type ("+data.GetTypeName()+
				") does not match with: "+type->GetName()+" ("+type->GetDefaultValue().GetTypeName()+")"
				, 2, 2
				);
			}
		}
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
		isreference=other.isreference;
		
		if(isreference)
			isconstant=other.isconstant;
		
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
	
	Data Data::GetReference() {
		ASSERT(type, "Type is not set", 1, 2);
		
		if(isreference) return *this;
		if(type->IsReferenceType()) return *this;
		
		void *r=data.GetRaw();
		void **p = new void*(r);
		
		if(isconstant) {
			return {type, {p, type->ConstPtrTypeInterface}, true, true};
		}
		else {
			return {type, {p, type->PtrTypeInterface}, true, false};
		}
	}
	
	void Data::MakeConstant() {
		ASSERT(type, "Type is not set", 1, 2);
		
		if(!isconstant) {
			if(isreference && !type->IsReferenceType())
				data.SetType(type->ConstPtrTypeInterface);
			else
				data.SetType(type->ConstTypeInterface);
			
			isconstant=true;
		}
	}
	
	bool Data::IsReference() const {
		return isreference || type->IsReferenceType();
	}
	
} }
