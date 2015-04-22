#include "Runtime.h"
#include "Embedding.h"

namespace Gorgon {
	
	namespace Scripting {
				
		Type *ParameterTemplateType() {
			static Type *par=new MappedValueType<ParameterTemplate, ToEmptyString<ParameterTemplate>, ParseThrow<ParameterTemplate>>(
				"#parametertemplate", "");
			
			return par;
		}

	}
}