#include "Text.h"
#include "File.h"

using namespace std;

namespace gge { namespace resource {
	Text *LoadTextResource(File &File, istream &Data, int Size) {
		char *tmpstr=new char[Size+1];

		Data.read(tmpstr,Size);
		Text* txt=new Text();
		tmpstr[Size]=0;
		txt->Value=tmpstr;

		delete[] tmpstr;

		return txt;
	}
} }
