#include "FontTheme.h"
#include "ResourceFile.h"

namespace gge { namespace resource {
	ResourceBase *LoadFontTheme(File* File, FILE* Data, int Size) {
		FontTheme *ft=new FontTheme;

		ft->file=File;
		
		int tpos=ftell(Data)+Size;
		while(ftell(Data)<tpos) {
			int gid,size;
			fread(&gid,1,4,Data);
			fread(&size,1,4,Data);

			if(gid==GID_GUID) {
				ft->guid.Load(Data);
			}
			if(gid==GID_FONTTHEME_PROPS) {
				ft->guid_normal	.Load(Data);
				ft->guid_bold	.Load(Data);
				ft->guid_italic	.Load(Data);
				ft->guid_small	.Load(Data);
				ft->guid_h1		.Load(Data);
				ft->guid_h2		.Load(Data);
				ft->guid_h3		.Load(Data);


				if(size!=(7*16))
					fseek(Data,size-(7*16),SEEK_CUR);
			} 

		}

		return ft;
	}

	void FontTheme::Prepare(gge::GGEMain &main, gge::resource::File &file) {
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
