#pragma once

#include "../Resource/ResourceBase.h"
#include "IWidgetBluePrint.h"
#include "TextboxStyleGroup.h"
#include "IWidgetContainer.h"
#include "Textbox.h"
#include <math.h>

using namespace gre;

namespace gorgonwidgets {

#define GID_TEXTBOX			0x05830000
#define GID_TEXTBOX_PROPS	0x05830101
#define GID_TEXTBOX_NAMES	0x05830102
#define GID_TEXTBOX_NAME	0x05830103

	class TextboxBP : public ResourceBase, public IWidgetBluePrint
	{
		friend ResourceBase *LoadTextbox(ResourceFile*,FILE*,int);
		friend class Textbox;
	public:
		TextboxBP(void);
		virtual int getGID() { return GID_TEXTBOX; }
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }
		virtual IWidgetObject *Create(IWidgetContainer &Container,int X,int Y,int Cx,int Cy);
		SoundResource *TypingSound;
		AnimationResource *Caret;
		void Prepare(gge::GGEMain *main);

	protected:
		utils::Collection<TextboxStyleGroup> StyleGroups;
		Guid *guid_typingsound;
		ResourceFile *file;
	};
}
