#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/BitmapFontResource.h"
#include "../Resource/SoundResource.h"
#include "../Resource/ResourceFile.h"
#include "../Engine/GGEMain.h"
#include "../Resource/AnimationResource.h"
#include "../Engine/ResizableObject.h"
#include "../Utils/Margins.h"
#include <string>

using namespace gre;
using namespace gge;
using namespace std;

namespace gorgonwidgets {

#define GID_CHECKBOX_ELEMENT			0x5250000
#define GID_CHECKBOX_ELEMENT_PROPS		0x5250101

	class CheckboxElement : public ResourceBase {
		friend ResourceBase *LoadCheckboxElement(ResourceFile*,FILE*,int);
		friend class CheckboxStyleGroup;
	public:
		CheckboxElement(void);

		virtual int getGID() { return GID_CHECKBOX_ELEMENT; }
		virtual void Prepare(gge::GGEMain *main);
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }

		CheckboxElement &Draw(WidgetLayer &layer,Colorizable2DLayer &textlayer,string &caption);
		CheckboxElement &ReadyAnimation(bool Backwards);
		CheckboxElement &Reverse();

		BitmapFontResource *Font;
		SoundResource *Sound;
		RGBint ForeColor,ShadowColor;
		Alignment TextAlign,IconAlign;
		int Duration;
		Margins TextMargins;
		Margins IconMargins;
		Point Offset;
		Point ShadowOffset;


	protected:
		Guid *font_guid;
		Guid *sound_guid;
		ResourceFile *file;
		ImageAnimation *symbol;
		ResizableObject *border;
		ResourceBase *bordertemplate;

		int lines;
	};
}