#include "WidgetMain.h"
#include "CheckboxBP.h"
#include "CheckboxStyleGroup.h"
#include "CheckboxElement.h"
#include "Line.h"
#include "Rectangle.h"

namespace gorgonwidgets {

	ResourceBase* LoadCheckbox(ResourceFile* file,FILE* gfile,int sz) {
		CheckboxBP *chk =new CheckboxBP();
		ResourceBase *obj;

		int gid,size,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				chk->guid=new Guid(gfile);
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

	ResourceBase* LoadCheckboxStyleGroup(ResourceFile* file,FILE* gfile,int sz) {
		CheckboxStyleGroup *chk =new CheckboxStyleGroup();

		int gid,size,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				chk->guid=new Guid(gfile);
				break;
			case GID_CHECKBOX_GRP_PROPS:
				fread(&chk->CheckboxType,4,1,gfile);
				fread(&chk->PreferredWidth,4,1,gfile);
				fread(&chk->PreferredHeight,4,1,gfile);

				chk->guid_normalstyle=new Guid(gfile);
				chk->guid_hoverstyle=new Guid(gfile);
				chk->guid_pressedstyle=new Guid(gfile);
				chk->guid_normal2hover=new Guid(gfile);
				chk->guid_normal2pressed=new Guid(gfile);
				chk->guid_hover2pressed=new Guid(gfile);
				chk->guid_hover2normal=new Guid(gfile);
				chk->guid_pressed2normal=new Guid(gfile);
				chk->guid_pressed2hover=new Guid(gfile);
				chk->guid_tonextstyle=new Guid(gfile);

				BYTE byt;
				fread(&byt,1,1,gfile);
				chk->RevHover2NormalState=byt;
				fread(&byt,1,1,gfile);
				chk->RevPressed2HoverState=byt;
				fread(&byt,1,1,gfile);
				chk->RevPressed2NormalState=byt;

				chk->guid_disabledstyle=new Guid(gfile);

				chk->guid_cnormalstyle=new Guid(gfile);
				chk->guid_choverstyle=new Guid(gfile);
				chk->guid_cpressedstyle=new Guid(gfile);
				chk->guid_cnormal2hover=new Guid(gfile);
				chk->guid_cnormal2pressed=new Guid(gfile);
				chk->guid_chover2pressed=new Guid(gfile);
				chk->guid_chover2normal=new Guid(gfile);
				chk->guid_cpressed2normal=new Guid(gfile);
				chk->guid_cpressed2hover=new Guid(gfile);
				chk->guid_ctonextstyle=new Guid(gfile);

				fread(&byt,1,1,gfile);
				chk->RevHover2NormalState=byt;
				fread(&byt,1,1,gfile);
				chk->RevPressed2HoverState=byt;
				fread(&byt,1,1,gfile);
				chk->RevPressed2NormalState=byt;
				fread(&byt,1,1,gfile);
				chk->cRevToNextStyle=byt;

				chk->guid_cdisabledstyle=new Guid(gfile);

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

	ResourceBase* LoadCheckboxElement(ResourceFile* file,FILE* gfile,int sz) {
		CheckboxElement *chk =new CheckboxElement();

		int gid,size,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				chk->guid=new Guid(gfile);
				break;
			case GID_CHECKBOX_ELEMENT_PROPS:
				chk->font_guid=new Guid(gfile);
				chk->sound_guid=new Guid(gfile);

				fread(&chk->ForeColor,4,1,gfile);
				fread(&chk->Duration,4,1,gfile);
				fread(&chk->TextAlign,4,1,gfile);
				fread(&chk->IconAlign,4,1,gfile);
				fread(&chk->TextMargins,4,4,gfile);
				fread(&chk->Offset.x,4,1,gfile);
				fread(&chk->Offset.y,4,1,gfile);
				fread(&chk->ShadowOffset.x,4,1,gfile);
				fread(&chk->ShadowOffset.y,4,1,gfile);
				fread(&chk->ShadowColor,4,1,gfile);
				fread(&chk->IconMargins,4,4,gfile);
				fread(&chk->lines,4,1,gfile);

				EatChunk(gfile,size-(4*6 + 8*2 + 16*2 + 16*2));
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


}