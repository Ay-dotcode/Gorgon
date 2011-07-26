#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/BitmapFontResource.h"
#include "../Engine/Sound.h"
#include "../Resource/ResourceFile.h"
#include "../Engine/GGEMain.h"
#include "../Resource/AnimationResource.h"
#include "../Resource/ResizableObject.h"
#include <string>

namespace gge { namespace widgets {

#define GID_TEXTBOX_ELEMENT			0x5230000
#define GID_TEXTBOX_ELEMENT_PROPS	0x5230101

	class Textbox;

	class TextboxElement : public resource::ResourceBase
	{
		friend resource::ResourceBase *LoadTextboxElement(resource::File*,FILE*,int);
		friend class TextboxStyleGroup;
		friend class Textbox;
	public:
		TextboxElement(void);

		virtual int getGID() { return GID_TEXTBOX_ELEMENT; }
		virtual void Prepare(GGEMain &main);
		virtual bool Save(resource::File *File, FILE *Data) { return false; }

		TextboxElement &Draw(WidgetLayer &layer,graphics::Colorizable2DLayer &textlayer,string Caption);
		TextboxElement &ReadyAnimation(bool Backwards);
		TextboxElement &Reverse();

		resource::BitmapFontResource *Font;
		resource::SoundResource *Sound;
		graphics::RGBint ForeColor,ShadowColor,SelectionColor,SelectionHighlight,SelectionShadow;
		Alignment TextAlign;
		int Duration;
		Bounds TextMargins;
		Point Offset;
		Point ShadowOffset;
		int detectChar(int x);
		Textbox *Parent;

		resource::ImageAnimation *Caret;


	protected:
		utils::SGuid font_guid;
		utils::SGuid sound_guid;
		resource::File *file;
		int lx;
		TextAlignment calign;

		resource::ResizableObject *Visual;
		resource::ResizableObject *SelectionRect;
		Bounds SelectionOffset;
	};
} }
