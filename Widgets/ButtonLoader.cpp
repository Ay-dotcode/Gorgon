#include "WidgetMain.h"
#include "../Resource/ResourceBase.h"
#include "../Resource/ResourceFile.h"
#include "../Resource/AnimationResource.h"
#include "Line.h"
#include "Rectangle.h"
#include "ButtonBP.h"
#include "ButtonElement.h"
#include "ButtonStyleGroup.h"

using namespace gge::resource;

namespace gge { namespace widgets {
	void eatchunk(FILE* f,int size);

	ResourceBase* LoadButton(File* file,FILE* gfile,int sz) {
		ButtonBP *btn =new ButtonBP();
		ResourceBase *obj;

		int gid,size,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				btn->guid.Load(gfile);
				break;
			case GID_BUTTON_GRP:
				obj=file->LoadObject(gfile,gid,size);
				if(obj) {
					btn->Subitems.Add(obj);
					btn->StyleGroups.Add((ButtonStyleGroup*)obj);
				}
				break;
			case GID_BUTTON_PROPS:
				fread(&btn->PointerType,1,4,gfile);
				break;
			default:
				obj=file->LoadObject(gfile,gid,size);
				if(obj)
					btn->Subitems.Add(obj);
				break;
			}
		}

		return btn;
	}

	ResourceBase* LoadButtonStyleGroup(File* file,FILE* gfile,int sz) {
		ButtonStyleGroup *btn =new ButtonStyleGroup();

		int gid,size,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				btn->guid.Load(gfile);
				break;
			case GID_BUTTON_GRP_PROPS:
				fread(&btn->SizeMode,4,1,gfile);
				fread(&btn->PreferredWidth,4,1,gfile);
				fread(&btn->PreferredHeight,4,1,gfile);

				btn->guid_normalstyle.Load(gfile);
				btn->guid_hoverstyle.Load(gfile);
				btn->guid_pressedstyle.Load(gfile);
				btn->guid_normal2hover.Load(gfile);
				btn->guid_normal2pressed.Load(gfile);
				btn->guid_hover2pressed.Load(gfile);
				btn->guid_hover2normal.Load(gfile);
				btn->guid_pressed2normal.Load(gfile);
				btn->guid_pressed2hover.Load(gfile);

				Byte byt;
				fread(&byt,1,1,gfile);
				btn->RevHover2NormalState=byt;
				fread(&byt,1,1,gfile);
				btn->RevPressed2HoverState=byt;
				fread(&byt,1,1,gfile);
				btn->RevPressed2NormalState=byt;

				btn->guid_disabledstyle.Load(gfile);

				EatChunk(gfile,size-175);
				break;
			default:
				ResourceBase *obj=file->LoadObject(gfile,gid,size);
				if(obj)
					btn->Subitems.Add(obj);

				break;
			}
		}

		btn->file=file;
		return btn;
	}

	ResourceBase* LoadButtonElement(File* file,FILE* gfile,int sz) {
		ButtonElement *btn =new ButtonElement();

		int gid,size,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				btn->guid.Load(gfile);
				break;
			case GID_BUTTON_ELEMENT_PROPS:
				btn->font_guid.Load(gfile);
				btn->sound_guid.Load(gfile);

				fread(&btn->ForeColor,4,1,gfile);
				fread(&btn->Duration,4,1,gfile);
				fread(&btn->TextAlign,4,1,gfile);
				fread(&btn->TextMargins.Left,4,1,gfile);
				fread(&btn->TextMargins.Right,4,1,gfile);
				fread(&btn->TextMargins.Top,4,1,gfile);
				fread(&btn->TextMargins.Bottom,4,1,gfile);
				fread(&btn->Offset.x,4,1,gfile);
				fread(&btn->Offset.y,4,1,gfile);
				fread(&btn->ShadowOffset.x,4,1,gfile);
				fread(&btn->ShadowOffset.y,4,1,gfile);
				fread(&btn->ShadowColor,4,1,gfile);

				EatChunk(gfile,size-80);
				break;
			default:
				ResourceBase *obj=file->LoadObject(gfile,gid,size);
				if(obj)
					btn->Subitems.Add(obj);

				break;
			}
		}

		btn->file=file;
		return btn;
	}


} }
