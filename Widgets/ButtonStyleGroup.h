#pragma once

#include "../Resource/ResourceBase.h"
#include "ButtonElement.h"

namespace gge { namespace widgets {

#define GID_BUTTON_GRP			0x5340000
#define GID_BUTTON_GRP_PROPS	0x5340101

	class ButtonStyleGroup : public resource::ResourceBase
	{
		friend resource::ResourceBase* LoadButtonStyleGroup(resource::ResourceFile*,FILE*,int);
	public:
		ButtonStyleGroup(void);
		virtual int getGID() { return GID_BUTTON_GRP; }
		virtual void Prepare(gge::GGEMain *main);
		virtual bool Save(resource::ResourceFile *File, FILE *Data) { return false; }

		enum {
			SIZE_RESIZABLE,
			SIZE_FIXED_HEIGHT,
			SIZE_FIXED
		} SizeMode;

		int PreferredWidth;
		int PreferredHeight;

		ButtonElement	*NormalStyle,
						*HoverStyle,
						*PressedStyle,
						*DisabledStyle,
						*Normal2Hover,
						*Normal2Pressed,
						*Hover2Pressed,
						*Hover2Normal,
						*Pressed2Normal,
						*Pressed2Hover;

		bool			RevHover2NormalState,
						RevPressed2HoverState,
						RevPressed2NormalState;

	protected:

		resource::ResourceFile* file;

		Guid *guid_normalstyle		,
			 *guid_hoverstyle		,
			 *guid_pressedstyle		,
			 *guid_disabledstyle	,
			 *guid_normal2hover		,
			 *guid_normal2pressed	,
			 *guid_hover2pressed	,
			 *guid_hover2normal		,
			 *guid_pressed2normal	,
			 *guid_pressed2hover	;
	};

} }
