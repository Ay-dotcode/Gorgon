#pragma once

#include "../Resource/ResourceBase.h"
#include "IWidgetBluePrint.h"
#include "SliderStyleGroup.h"
#include "IWidgetContainer.h"
#include <math.h>

namespace gge { namespace widgets {

#define GID_SLIDER			0x05860000
#define GID_SLIDER_PROPS	0x05860101
#define GID_SLIDER_NAMES	0x05860102
#define GID_SLIDER_NAME		0x05860103

	class SliderBP : public resource::ResourceBase, public IWidgetBluePrint
	{
		friend resource::ResourceBase *LoadSlider(resource::File*,FILE*,int);
		friend class SliderBase;
	public:
		resource::SoundResource *TickSound;

		SliderBP();
		virtual int getGID() { return GID_SLIDER; }
		virtual IWidgetObject *Create(IWidgetContainer &Container,int X,int Y,int Cx,int Cy);
		virtual bool Save(resource::File *File, FILE *Data) { return false; }
		void Prepare(gge::GGEMain &main, gge::resource::File &file);


	protected:
		utils::Collection<SliderStyleGroup> StyleGroups;
		utils::SGuid guid_ticksound;
		resource::File *file;
	};
} }
