#include "TextResource.h"
#include "ResourceFile.h"

using namespace std;

namespace gge { namespace resource {
	ResourceBase *LoadTextResource(File &File, istream &Data, int Size) {
		char *tmpstr=new char[Size+1];

		Data.read(tmpstr,Size);
		TextResource* txt=new TextResource();
		tmpstr[Size]=0;
		txt->Text=tmpstr;

		delete tmpstr;

		return txt;
	}
} }
