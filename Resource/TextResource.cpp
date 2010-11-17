#include "TextResource.h"
#include "ResourceFile.h"

namespace gre {
	ResourceBase *LoadTextResource(ResourceFile* File, FILE* Data, int Size) {
		char *tmpstr=new char[Size+1];

		fread(tmpstr,1,Size,Data);
		TextResource* txt=new TextResource();
		tmpstr[Size]=0;
		txt->Text=tmpstr;

		delete tmpstr;

		return txt;
	}
}
