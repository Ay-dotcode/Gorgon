#include "WidgetMain.h"
#include "CheckboxBP.h"
#include "CheckboxStyleGroup.h"
#include "CheckboxElement.h"
#include "Line.h"
#include "Rectangle.h"

using namespace gge::resource;

namespace gge { namespace widgets {

	ResourceBase* LoadCheckbox(File* file,FILE* gfile,int sz) {
		CheckboxBP *chk =new CheckboxBP();
		ResourceBase *obj;

		int gid,size,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				chk->guid.Load(gfile);
				break;
			case GID_CHECKBOX_GRP:
				obj=file->LoadObject(gfile,gid,size);
				if(obj) {
					chk->Subitems.Add(obj);
					chk->StyleGroups.Add((CheckboxStyleGroup*)obj);
				}
				break;
			case GID_CHECKBOX_PROPS:
				fread(&chk->PointerType,1,4,gfile);
				break;
			default:
				obj=file->LoadObject(gfile,gid,size);
				if(obj)
					chk->Subitems.Add(obj);
				break;
			}
		}

		return chk;
	}

	ResourceBase* LoadCheckboxStyleGroup(File* file,FILE* gfile,int sz) {
		CheckboxStyleGroup *chk =new CheckboxStyleGroup();

		int gid,size,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				chk->guid.Load(gfile);
				break;
			case GID_CHECKBOX_GRP_PROPS:
				fread(&chk->CheckboxType,4,1,gfile);
				fread(&chk->PreferredWidth,4,1,gfile);
				fread(&chk->PreferredHeight,4,1,gfile);

				chk->guid_normalstyle.Load(gfile);
				chk->guid_hoverstyle.Load(gfile);
				chk->guid_pressedstyle.Load(gfile);
				chk->guid_normal2hover.Load(gfile);
				chk->guid_normal2pressed.Load(gfile);
				chk->guid_hover2pressed.Load(gfile);
				chk->guid_hover2normal.Load(gfile);
				chk->guid_pressed2normal.Load(gfile);
				chk->guid_pressed2hover.Load(gfile);
				chk->guid_tonextstyle.Load(gfile);

				Byte byt;
				fread(&byt,1,1,gfile);
				chk->RevHover2NormalState=byt;
				fread(&byt,1,1,gfile);
				chk->RevPressed2HoverState=byt;
				fread(&byt,1,1,gfile);
				chk->RevPressed2NormalState=byt;

				chk->guid_disabledstyle.Load(gfile);

				chk->guid_cnormalstyle.Load(gfile);
				chk->guid_choverstyle.Load(gfile);
				chk->guid_cpressedstyle.Load(gfile);
				chk->guid_cnormal2hover.Load(gfile);
				chk->guid_cnormal2pressed.Load(gfile);
				chk->guid_chover2pressed.Load(gfile);
				chk->guid_chover2normal.Load(gfile);
				chk->guid_cpressed2normal.Load(gfile);
				chk->guid_cpressed2hover.Load(gfile);
				chk->guid_ctonextstyle.Load(gfile);

				fread(&byt,1,1,gfile);
				chk->RevHover2NormalState=byt;
				fread(&byt,1,1,gfile);
				chk->RevPressed2HoverState=byt;
				fread(&byt,1,1,gfile);
				chk->RevPressed2NormalState=byt;
				fread(&byt,1,1,gfile);
				chk->cRevToNextStyle=byt;

				chk->guid_cdisabledstyle.Load(gfile);

				EatChunk(gfile,size-371);
				break;
			default:
				ResourceBase *obj=file->LoadObject(gfile,gid,size);
				if(obj)
					chk->Subitems.Add(obj);

				break;
			}
		}

		chk->file=file;
		return chk;
	}

	ResourceBase* LoadCheckboxElement(File* file,FILE* gfile,int sz) {
		CheckboxElement *chk =new CheckboxElement();

		int gid,size,cnt=0, size2, gid2;
		int temp;
		int pos;

		int tpos=ftell(gfile)+sz;
		Point Offset;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				chk->guid.Load(gfile);
				break;
			case GID_CHECKBOX_ELEMENT_PROPS:

				/*chk->font_guid=*/utils::SGuid().Load(gfile);
				chk->sound_guid.Load(gfile);


				fread(&chk->ForeColor,4,1,gfile);
				fread(&chk->Duration,4,1,gfile);
				fread(&chk->TextAlign,4,1,gfile);
				fread(&chk->IconAlign,4,1,gfile);
				fread(&chk->TextMargin,4,4,gfile);
				fread(&Offset.x,4,1,gfile);
				fread(&Offset.y,4,1,gfile);
				chk->ContentMargin.Left=Offset.x;
				chk->ContentMargin.Top=Offset.y;
				fread(&chk->ShadowOffset.x,4,1,gfile);
				fread(&chk->ShadowOffset.y,4,1,gfile);
				fread(&chk->ShadowColor,4,1,gfile);

				EatChunk(gfile,size-(4*5 + 8*2 + 16*1 + 16*2));
				break;
			case GID_CHECKBOX_ELEMENT_PROPS2:
				pos=ftell(gfile);

				fread(&gid2,  4,1, gfile);
				fread(&size2, 4,1, gfile);

				chk->temp_font=Font::Load(file, gfile, size2);
				chk->sound_guid.Load(gfile);

				fread(&chk->Duration,4,1,gfile);

				fread(&chk->Lines, 4,1, gfile);
				fread(&temp, 4,1, gfile);
				chk->SymbolIconOrder=(CheckboxElement::SymbolIconOrderConstants)temp;
				fread(&temp, 4,1, gfile);
				chk->SymbolAlign=(Alignment)temp;
				fread(&temp, 4,1, gfile);
				chk->IconAlign=(Alignment)temp;
				fread(&temp, 4,1, gfile);
				chk->TextAlign=(Alignment)temp;

				fread(&chk->BorderMargin, 4,4, gfile);
				fread(&chk->BorderWidth, 4,4, gfile);
				fread(&temp, 4,1, gfile);
				chk->AutoBorderWidth=(bool)temp;
				fread(&chk->ContentMargin, 4,4, gfile);
				fread(&chk->SymbolMargin, 4,4, gfile);
				fread(&chk->IconMargin, 4,4, gfile);
				fread(&chk->TextMargin, 4,4, gfile);


				fseek(gfile, pos+size, SEEK_SET);
				break;
			default:
				ResourceBase *obj=file->LoadObject(gfile,gid,size);
				if(obj)
					chk->Subitems.Add(obj);

				break;
			}
		}

		chk->file=file;
		return chk;
	}


} }
