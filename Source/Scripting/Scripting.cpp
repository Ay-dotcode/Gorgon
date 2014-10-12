#include "../Scripting.h"
#include "Reflection.h"
#include "VirtualMachine.h"
#include "Embedding.h"

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
	}

	Data::Data(const Type *type, const Any &data) : type(type), data(data) {
		check();
		ASSERT((type!=nullptr), "Data type cannot be nullptr", 1, 2);

		if(type->IsReferenceType() && data.Pointer()) {
			VirtualMachine::Get().References.Increase(*this);
		}
	}

	void Data::check() {
		ASSERT(type->GetDefaultValue().IsSameType(data), "Given data type does not match with: "+type->GetName(), 2, 2);
	}

	Data &Data::operator =(Data other) {
		type=other.type;
		data=other.data;

		other.data=Any();
		other.type=nullptr;

		return *this;
	}

	Data::~Data() {
		if(type && type->IsReferenceType() && data.Pointer()) {
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
	
	MappedValueType<Data, String::From<Data>, GetVariableValue> Variant = {"Variant", 
		"This type can contain any type.",
		Data::Invalid()
	};
	
	int VariableScope::nextid=0;
} }
