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

using namespace gre;
using namespace gge;
using namespace std;

namespace gorgonwidgets {

#define GID_BUTTON_ELEMENT			0x5240000
#define GID_BUTTON_ELEMENT_PROPS	0x5240101

	class ButtonElement : public ResourceBase
	{
		friend ResourceBase *LoadButtonElement(ResourceFile*,FILE*,int);
		friend class ButtonStyleGroup;
	public:
		ButtonElement(void);

		virtual int getGID() { return GID_BUTTON_ELEMENT; }
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }
		virtual void Prepare(gge::GGEMain *main);

		ButtonElement &Draw(WidgetLayer &Target,Colorizable2DLayer &TextTarget,Colorizable2DLayer &IconTarget,string Caption,Buffered2DGraphic *Icon);
		ButtonElement &ReadyAnimation(bool Backwards);
		ButtonElement &setLoop(bool loop) { Visual->setLoop(loop); return *this; }
		ButtonElement &Reverse();
		ButtonElement &Activate();
		ButtonElement &Deactivate();

		int getAnimationDuration() { return this->Visual->getDuration(); }

		BitmapFontResource *Font;
		SoundResource *Sound;
		RGBint ForeColor,ShadowColor;
		Alignment TextAlign;
		Alignment IconAlign;
		int Duration;
		Bounds TextMargins;
		Bounds IconMargins;
		RGBint IconColor;
		Point Offset;
		Point ShadowOffset;

	protected:
		Guid *font_guid;
		Guid *sound_guid;
		ResourceFile *file;
		ResizableObject *Visual;
	};
}