#pragma once

#include "../Resource/ResourceBase.h"
#include "TextboxElement.h"

namespace gge { namespace widgets {

#define GID_TEXTBOX_GRP			0x5330000
#define GID_TEXTBOX_GRP_PROPS	0x5330101

	class TextboxStyleGroup : public resource::ResourceBase
	{
		friend resource::ResourceBase* LoadTextboxStyleGroup(resource::ResourceFile*,FILE*,int);
	public:
		TextboxStyleGroup(void);
		virtual int getGID() { return GID_TEXTBOX_GRP; }
		virtual void Prepare(gge::GGEMain *main);
		virtual bool Save(resource::ResourceFile *File, FILE *Data) { return false; }

		enum {
			SIZE_RESIZABLE,
			SIZE_FIXED_HEIGHT,
			SIZE_FIXED
		} SizeMode;

		int PreferredWidth;
		int PreferredHeight;

		TextboxElement	*NormalStyle,
						*HoverStyle,
						*ActiveStyle,
						*DisabledStyle,
						*Normal2Hover,
						*Normal2Active,
						*Hover2Active,
						*Hover2Normal,
						*Active2Normal,
						*Active2Hover;

		bool			RevHover2NormalState,
						RevActive2HoverState,
						RevActive2NormalState;

	protected:
		resource::ResourceFile* file;

		Guid *guid_normalstyle	,
			 *guid_hoverstyle	,
			 *guid_activestyle	,
			 *guid_disabledstyle,
			 *guid_normal2hover	,
			 *guid_normal2active,
			 *guid_hover2active	,
			 *guid_hover2normal	,
			 *guid_active2normal,
			 *guid_active2hover	;
	};

} }
