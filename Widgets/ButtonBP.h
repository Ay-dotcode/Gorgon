#pragma once

#include "../Resource/ResourceBase.h"
#include "IWidgetBluePrint.h"
#include "ButtonStyleGroup.h"
#include "IWidgetContainer.h"
#include "Button.h"
#include <math.h>

using namespace gre;

namespace gorgonwidgets {

#define GID_BUTTON			0x05840000
#define GID_BUTTON_PROPS	0x05840101
#define GID_BUTTON_NAMES	0x05840102
#define GID_BUTTON_NAME		0x05840103

	class ButtonBP : public ResourceBase, public IWidgetBluePrint
	{
		friend ResourceBase *LoadButton(ResourceFile*,FILE*,int);
		friend class Button;
	public:
		ButtonBP(void);
		virtual int getGID() { return GID_BUTTON; }
		virtual IWidgetObject *Create(IWidgetContainer &Container,int X,int Y,int Cx,int Cy);
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }

	protected:
		utils::Collection<ButtonStyleGroup> StyleGroups;
	};
}