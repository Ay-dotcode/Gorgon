#pragma once

#include "../Resource/ResourceBase.h"
#include "IWidgetBluePrint.h"
#include "CheckboxStyleGroup.h"
#include "IWidgetContainer.h"
#include <math.h>


namespace gge { namespace widgets {

#define GID_CHECKBOX			0x05850000
#define GID_CHECKBOX_PROPS		0x05850101
#define GID_CHECKBOX_NAMES		0x05850102
#define GID_CHECKBOX_NAME		0x05850103

	class CheckboxBP : public resource::ResourceBase, public IWidgetBluePrint {
		friend resource::ResourceBase *LoadCheckbox(resource::ResourceFile*,FILE*,int);
		friend class CheckboxBase;

	public:
		CheckboxBP(void);
		virtual int getGID() { return GID_CHECKBOX; }
		virtual IWidgetObject *Create(IWidgetContainer &Container,int X,int Y,int Cx,int Cy);
		virtual bool Save(resource::ResourceFile *File, FILE *Data) { return false; }

	protected:
		utils::Collection<CheckboxStyleGroup> StyleGroups;
	};
} }
