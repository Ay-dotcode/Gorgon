#pragma once

#include "../Resource/ResourceBase.h"
#include "IWidgetBluePrint.h"
#include "SliderStyleGroup.h"
#include "IWidgetContainer.h"
#include <math.h>

using namespace gre;

namespace gge { namespace widgets {

#define GID_SLIDER			0x05860000
#define GID_SLIDER_PROPS	0x05860101
#define GID_SLIDER_NAMES	0x05860102
#define GID_SLIDER_NAME		0x05860103

	class SliderBP : public ResourceBase, public IWidgetBluePrint
	{
		friend ResourceBase *LoadSlider(ResourceFile*,FILE*,int);
		friend class SliderBase;
	public:
		SoundResource *TickSound;

		SliderBP();
		virtual int getGID() { return GID_SLIDER; }
		virtual IWidgetObject *Create(IWidgetContainer &Container,int X,int Y,int Cx,int Cy);
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }
		void Prepare(gge::GGEMain *main);


	protected:
		utils::Collection<SliderStyleGroup> StyleGroups;
		Guid *guid_ticksound;
		ResourceFile *file;
	};
} }
