#pragma once

#include "GRE.h"
#include "ResourceBase.h"

namespace gge { namespace resource {
	class File;
	
	////This function loads a text resource from the given file
	ResourceBase *LoadTextResource(File &File, std::istream &Data, int Size);

	////This is basic text resource, it holds a simple string. This resource is mostly useless
	/// because of data array resource. Also it does not employ any GUID mechanism.
	/// Still it can be used to hold a long text which is not related with other data.
	class TextResource : public ResourceBase {
		friend ResourceBase *LoadTextResource(File &File, std::istream &Data, int Size);
	public:
		////The text contained within this resource
		string Text;
		
		////02010000h (Basic, Text)
		virtual GID::Type getGID() const { return GID::Text; }
		////Currently does nothing
		virtual bool Save(File &File, std::ostream &Data) { return false; }
	};
} }