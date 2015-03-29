#include <map>

#include "../Scripting.h"
#include "Reflection.h"
#include "VirtualMachine.h"
#include "Embedding.h"

namespace Gorgon { namespace Scripting {

	

	Data GetVariableValue(const std::string &varname);
	Type &Variant = *new MappedValueType<Data, String::From<Data>, GetVariableValue>("Variant",
		"This type can contain any type.",
		Data::Invalid()
	);
	
	
	std::set<std::string, String::CaseInsensitiveLess> KeywordNames;
} }
