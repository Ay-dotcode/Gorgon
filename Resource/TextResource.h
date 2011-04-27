#pragma once

#include "GRE.h"
#include "ResourceBase.h"

namespace gge { namespace resource {
	class File;
	
	////This function loads a text resource from the given file
	ResourceBase *LoadTextResource(File* File, FILE* Data, int Size);

	////This is basic text resource, it holds a simple string. This resource is mostly useless
	/// because of data array resource. Still it can be used to hold a long text which is not
	/// related with other data.
	class TextResource : public ResourceBase {
		friend ResourceBase *LoadTextResource(File* File, FILE* Data, int Size);
	public:
		////The text contained within this resource
		string Text;
		
		////02010000h (Basic, Text)
		virtual int getGID() { return GID_TEXT; }
		////Currently does nothing
		virtual bool Save(File *File, FILE *Data) { return false; }
	};
} }