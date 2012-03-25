#pragma once

#include "GRE.h"
#include "Base.h"

namespace gge { namespace resource {
	class File;
	class Text;
	
	////This function loads a text resource from the given file
	Text *LoadTextResource(File &File, std::istream &Data, int Size);

	////This is basic text resource, it holds a simple string. This resource is mostly useless
	/// because of data array resource. Also it does not employ any GUID mechanism.
	/// Still it can be used to hold a long text which is not related with other data.
	class Text : public Base {
		friend Text *LoadTextResource(File &File, std::istream &Data, int Size);
	public:
		////The text contained within this resource
		string Value;
		
		////02010000h (Basic, Text)
		virtual GID::Type GetGID() const { return GID::Text; }
		////Currently does nothing
		virtual bool Save(File &File, std::ostream &Data) { return false; }
	};
} }