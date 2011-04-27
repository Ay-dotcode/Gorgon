#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/BitmapFontResource.h"
#include "../Resource/SoundResource.h"
#include "../Resource/ResourceFile.h"
#include "../Engine/GGEMain.h"
#include "../Resource/AnimationResource.h"
#include "../Resource/ResizableObject.h"
#include "Line.h"
#include "Rectangle.h"
#include <string>

namespace gge { namespace widgets {

#define GID_BUTTON_ELEMENT			0x5240000
#define GID_BUTTON_ELEMENT_PROPS	0x5240101

	class ButtonElement : public resource::ResourceBase
	{
		friend resource::ResourceBase *LoadButtonElement(resource::File*,FILE*,int);
		friend class ButtonStyleGroup;
	public:
		ButtonElement(void);

		virtual int getGID() { return GID_BUTTON_ELEMENT; }
		virtual bool Save(resource::File *File, FILE *Data) { return false; }
		virtual void Prepare(gge::GGEMain &main, gge::resource::File &file);

		ButtonElement &Draw(WidgetLayer &Target,graphics::Colorizable2DLayer &TextTarget,graphics::Colorizable2DLayer &IconTarget,string Caption,graphics::Buffered2DGraphic *Icon);
		ButtonElement &ReadyAnimation(bool Backwards);
		ButtonElement &setLoop(bool loop) { Visual->setLoop(loop); return *this; }
		ButtonElement &Reverse();
		ButtonElement &Activate();
		ButtonElement &Deactivate();

		int getAnimationDuration() { return this->Visual->getDuration(); }

		resource::BitmapFontResource *Font;
		resource::SoundResource *Sound;
		graphics::RGBint ForeColor,ShadowColor;
		Alignment TextAlign;
		Alignment IconAlign;
		int Duration;
		Bounds TextMargins;
		Bounds IconMargins;
		graphics::RGBint IconColor;
		Point Offset;
		Point ShadowOffset;

	protected:
		utils::SGuid font_guid;
		utils::SGuid sound_guid;
		resource::File *file;
		resource::ResizableObject *Visual;
	};
} }
