#include "WidgetMain.h"
#include "TextboxBP.h"
#include "TextboxStyleGroup.h"
#include "TextboxElement.h"
#include "Line.h"

namespace gorgonwidgets {
	void eatchunk(FILE* f,int size);

	ResourceBase* LoadTextbox(ResourceFile* file,FILE* gfile,int sz) {
		TextboxBP *txt =new TextboxBP();
		ResourceBase *obj;
		int animcount=0;

		int gid,size,i,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_TEXTBOX_PROPS:
				txt->guid_typingsound=new Guid(gfile);
				fread(&txt->PointerType,1,4,gfile);

				break;
			case GID_GUID:
				txt->guid=new Guid(gfile);
				break;
			case GID_TEXTBOX_GRP:
				obj=file->LoadObject(gfile,gid,size);
				if(obj) {
					txt->Subitems.Add(obj);
					txt->StyleGroups.Add((TextboxStyleGroup*)obj);
				}
				break;
			case GID_ANIMATION:
				obj=file->LoadObject(gfile,gid,size);
				if(obj)
					txt->Subitems.Add(obj);

				if(animcount++==0) {
					txt->Caret=(AnimationResource*)obj;
				}
				break;
			default:
				obj=file->LoadObject(gfile,gid,size);
				if(obj)
					txt->Subitems.Add(obj);
				break;
			}
		}

		txt->file=file;
		return txt;
	}

	ResourceBase* LoadTextboxStyleGroup(ResourceFile* file,FILE* gfile,int sz) {
		TextboxStyleGroup *txt =new TextboxStyleGroup();

		int gid,size,i,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				txt->guid=new Guid(gfile);
				break;
			case GID_TEXTBOX_GRP_PROPS:
				fread(&txt->SizeMode,4,1,gfile);
				fread(&txt->PreferredWidth,4,1,gfile);
				fread(&txt->PreferredHeight,4,1,gfile);

				txt->guid_normalstyle=new Guid(gfile);
				txt->guid_hoverstyle=new Guid(gfile);
				txt->guid_activestyle=new Guid(gfile);
				txt->guid_normal2hover=new Guid(gfile);
				txt->guid_normal2active=new Guid(gfile);
				txt->guid_hover2active=new Guid(gfile);
				txt->guid_hover2normal=new Guid(gfile);
				txt->guid_active2normal=new Guid(gfile);
				txt->guid_active2hover=new Guid(gfile);

				Byte byt;
				fread(&byt,1,1,gfile);
				txt->RevHover2NormalState=byt;
				fread(&byt,1,1,gfile);
				txt->RevActive2HoverState=byt;
				fread(&byt,1,1,gfile);
				txt->RevActive2NormalState=byt;

				txt->guid_disabledstyle=new Guid(gfile);

				EatChunk(gfile,size-175);
				break;
			default:
				ResourceBase *obj=file->LoadObject(gfile,gid,size);
				if(obj)
					txt->Subitems.Add(obj);

				break;
			}
		}

		txt->file=file;
		return txt;
	}

	ResourceBase* LoadTextboxElement(ResourceFile* file,FILE* gfile,int sz) {
		TextboxElement *txt =new TextboxElement();

		int gid,size,i,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				txt->guid=new Guid(gfile);
				break;
			case GID_TEXTBOX_ELEMENT_PROPS:
				txt->font_guid=new Guid(gfile);
				txt->sound_guid=new Guid(gfile);

				fread(&txt->ForeColor,4,1,gfile);
				fread(&txt->Duration,4,1,gfile);
				fread(&txt->TextAlign,4,1,gfile);
				fread(&txt->TextMargins.Left,4,1,gfile);
				fread(&txt->TextMargins.Right,4,1,gfile);
				fread(&txt->TextMargins.Top,4,1,gfile);
				fread(&txt->TextMargins.Bottom,4,1,gfile);
				fread(&txt->Offset.x,4,1,gfile);
				fread(&txt->Offset.y,4,1,gfile);
				fread(&txt->ShadowOffset.x,4,1,gfile);
				fread(&txt->ShadowOffset.y,4,1,gfile);
				fread(&txt->ShadowColor,4,1,gfile);

				fread(&txt->SelectionColor,4,1,gfile);
				fread(&txt->SelectionHighlight,4,1,gfile);
				fread(&txt->SelectionShadow,4,1,gfile);

				if(size>92) {
					fread(&txt->SelectionOffset, 4,4, gfile);
				
					EatChunk(gfile,size-108);
				}

				break;
			default:
				ResourceBase *obj=file->LoadObject(gfile,gid,size);
				if(obj)
					txt->Subitems.Add(obj);

				break;
			}
		}

		txt->file=file;
		return txt;
	}


}