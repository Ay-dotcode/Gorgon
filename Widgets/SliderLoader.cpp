#include "WidgetMain.h"
#include "../Resource/ResourceBase.h"
#include "../Resource/ResourceFile.h"
#include "../Resource/AnimationResource.h"
#include "Line.h"
#include "Rectangle.h"
#include "SliderBP.h"
#include "SliderElement.h"
#include "SliderStyleGroup.h"

using namespace gge::resource;

namespace gge { namespace widgets {

	ResourceBase* LoadSlider(File* file,FILE* gfile,int sz) {
		SliderBP *slider =new SliderBP();
		ResourceBase *obj;

		int gid,size,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				slider->guid.Load(gfile);
				break;
			case GID_SLIDER_GRP:
				obj=file->LoadObject(gfile,gid,size);
				if(obj) {
					slider->Subitems.Add(obj);
					slider->StyleGroups.Add((SliderStyleGroup*)obj);
				}
				break;
			case GID_SLIDER_PROPS:
				fread(&slider->PointerType,1,4,gfile);
				slider->guid_ticksound.Load(gfile);

				EatChunk(gfile,size-20);
				break;
			default:
				obj=file->LoadObject(gfile,gid,size);
				if(obj)
					slider->Subitems.Add(obj);
				break;
			}
		}

		return slider;
	}

	ResourceBase* LoadSliderStyleGroup(File* file,FILE* gfile,int sz) {
		SliderStyleGroup *slider =new SliderStyleGroup();

		int gid,size,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				slider->guid.Load(gfile);
				break;
			case GID_SLIDER_GRP_PROPS:
				Byte byt;

				fread(&slider->Style,4,1,gfile);
				fread(&slider->PreferredWidth,4,1,gfile);
				fread(&slider->PreferredHeight,4,1,gfile);
				fread(&byt,1,1,gfile);
				slider->AllowArrows=byt;
				fread(&slider->ArrowOffset.x,4,1,gfile);
				fread(&slider->ArrowOffset.y,4,1,gfile);
				fread(&slider->UpArrowSize.x,4,1,gfile);
				fread(&slider->UpArrowSize.y,4,1,gfile);
				fread(&slider->DownArrowSize.x,4,1,gfile);
				fread(&slider->DownArrowSize.y,4,1,gfile);

				slider->guid_normalstyle.Load(gfile);
				slider->guid_hoverstyle.Load(gfile);
				slider->guid_pressedstyle.Load(gfile);
				slider->guid_normal2hover.Load(gfile);
				slider->guid_normal2pressed.Load(gfile);
				slider->guid_hover2pressed.Load(gfile);
				slider->guid_hover2normal.Load(gfile);
				slider->guid_pressed2normal.Load(gfile);
				slider->guid_pressed2hover.Load(gfile);

				fread(&byt,1,1,gfile);
				slider->RevHover2NormalState=byt;
				fread(&byt,1,1,gfile);
				slider->RevPressed2HoverState=byt;
				fread(&byt,1,1,gfile);
				slider->RevPressed2NormalState=byt;

				slider->guid_disabledstyle.Load(gfile);

				EatChunk(gfile,size-(9*4 + 4*1 + 10*16));
				break;
			default:
				ResourceBase *obj=file->LoadObject(gfile,gid,size);
				if(obj)
					slider->Subitems.Add(obj);

				break;
			}
		}

		slider->file=file;
		return slider;
	}

	ResourceBase* LoadSliderElement(File* file,FILE* gfile,int sz) {
		SliderElement *slider =new SliderElement();

		int gid,size,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				slider->guid.Load(gfile);
				break;
			case GID_SLIDER_ELEMENT_PROPS:
				slider->font_guid.Load(gfile);
				slider->sound_guid.Load(gfile);

				fread(&slider->ForeColor,4,1,gfile);
				fread(&slider->Duration,4,1,gfile);
				fread(&slider->TextAlign,4,1,gfile);
				fread(&slider->Offset.x,4,1,gfile);
				fread(&slider->Offset.y,4,1,gfile);
				fread(&slider->TextOffset.x,4,1,gfile);
				fread(&slider->TextOffset.y,4,1,gfile);
				fread(&slider->ShadowOffset.x,4,1,gfile);
				fread(&slider->ShadowOffset.y,4,1,gfile);
				fread(&slider->ShadowColor,4,1,gfile);
				fread(&slider->MinTickDistance,4,1,gfile);
				fread(&slider->SymbolOffset.x,4,1,gfile);
				fread(&slider->SymbolOffset.y,4,1,gfile);
				fread(&slider->TickOffset.x,4,1,gfile);
				fread(&slider->TickOffset.y,4,1,gfile);
				fread(&slider->RuleOffset,4,1,gfile);
				fread(&slider->StartMargin,4,1,gfile);
				fread(&slider->EndMargin,4,1,gfile);
				fread(&slider->OverlayOffset.x,4,1,gfile);
				fread(&slider->OverlayOffset.y,4,1,gfile);
				fread(&slider->OverlaySizeMod,4,1,gfile);
				

				EatChunk(gfile,size-28*4);
				break;
			default:
				ResourceBase *obj=file->LoadObject(gfile,gid,size);
				if(obj)
					slider->Subitems.Add(obj);

				break;
			}
		}

		slider->file=file;
		return slider;
	}


} }
