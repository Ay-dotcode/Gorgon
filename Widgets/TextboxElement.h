#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/BitmapFontResource.h"
#include "../Engine/Sound.h"
#include "../Resource/ResourceFile.h"
#include "../Engine/GGEMain.h"
#include "../Resource/AnimationResource.h"
#include "../Resource/ResizableObject.h"
#include <string>

using namespace gre;
using namespace gge;
using namespace std;

namespace gorgonwidgets {

#define GID_TEXTBOX_ELEMENT			0x5230000
#define GID_TEXTBOX_ELEMENT_PROPS	0x5230101

	class Textbox;

	class TextboxElement : public ResourceBase
	{
		friend ResourceBase *LoadTextboxElement(ResourceFile*,FILE*,int);
		friend class TextboxStyleGroup;
		friend class Textbox;
	public:
		TextboxElement(void);

		virtual int getGID() { return GID_TEXTBOX_ELEMENT; }
		virtual void Prepare(gge::GGEMain *main);
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }

		TextboxElement &Draw(WidgetLayer &layer,Colorizable2DLayer &textlayer,string Caption);
		TextboxElement &ReadyAnimation(bool Backwards);
		TextboxElement &Reverse();

		BitmapFontResource *Font;
		SoundResource *Sound;
		RGBint ForeColor,ShadowColor,SelectionColor,SelectionHighlight,SelectionShadow;
		Alignment TextAlign;
		int Duration;
		Bounds TextMargins;
		Point Offset;
		Point ShadowOffset;
		int detectChar(int x);
		Textbox *Parent;

		ImageAnimation *Caret;


	protected:
		Guid *font_guid;
		Guid *sound_guid;
		ResourceFile *file;
		int lx;
		TextAlignment calign;

		ResizableObject *Visual;
		ResizableObject *SelectionRect;
		Bounds SelectionOffset;
	};
}