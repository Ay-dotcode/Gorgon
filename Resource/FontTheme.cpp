#include "FontTheme.h"
#include "ResourceFile.h"

using namespace gge::utils;
using namespace std;

namespace gge { namespace resource {
	ResourceBase *LoadFontTheme(File &File, std::istream &Data, int Size) {
		FontTheme *ft=new FontTheme;

		ft->file=&File;
		
		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;

			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			if(gid==GID::Guid) {
				ft->guid.LoadLong(Data);
			}
			else if(gid==GID::SGuid) {
				ft->guid.Load(Data);
			} 
			else if(gid==GID::FontTheme_Props) {
				if(size==7*16) {
					ft->guid_normal	.LoadLong(Data);
					ft->guid_bold	.LoadLong(Data);
					ft->guid_italic	.LoadLong(Data);
					ft->guid_small	.LoadLong(Data);
					ft->guid_h1		.LoadLong(Data);
					ft->guid_h2		.LoadLong(Data);
					ft->guid_h3		.LoadLong(Data);
				}
				else {
					ft->guid_normal	.Load(Data);
					ft->guid_bold	.Load(Data);
					ft->guid_italic	.Load(Data);
					ft->guid_small	.Load(Data);
					ft->guid_h1		.Load(Data);
					ft->guid_h2		.Load(Data);
					ft->guid_h3		.Load(Data);

					if(size!=(7*8))
						Data.seekg(size-(7*8),ios::cur);
				}

			} 

		}

		return ft;
	}

	void FontTheme::Prepare(gge::GGEMain &main) {
		ResourceBase::Prepare(main);

		NormalFont	= dynamic_cast<FontRenderer*>(file->FindObject(guid_normal));
		BoldFont	= dynamic_cast<FontRenderer*>(file->FindObject(guid_bold));
		ItalicFont	= dynamic_cast<FontRenderer*>(file->FindObject(guid_italic));
		SmallFont	= dynamic_cast<FontRenderer*>(file->FindObject(guid_small));
		H1Font		= dynamic_cast<FontRenderer*>(file->FindObject(guid_h1));
		H2Font		= dynamic_cast<FontRenderer*>(file->FindObject(guid_h2));
		H3Font		= dynamic_cast<FontRenderer*>(file->FindObject(guid_h3));
	}
} }
