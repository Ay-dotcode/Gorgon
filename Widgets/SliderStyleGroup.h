#pragma once

#include "../Resource/ResourceBase.h"
#include "SliderElement.h"
#include "Button.h"


namespace gge { namespace widgets {

#define GID_SLIDER_GRP			0x5360000
#define GID_SLIDER_GRP_PROPS	0x5360101

	class SliderStyleGroup : public resource::ResourceBase
	{
		friend resource::ResourceBase* LoadSliderStyleGroup(resource::ResourceFile*,FILE*,int);
	public:
		SliderStyleGroup(void);
		virtual int getGID() { return GID_SLIDER_GRP; }
		virtual void Prepare(GGEMain *main);
		virtual bool Save(resource::ResourceFile *File, FILE *Data) { return false; }

		int PreferredWidth;
		int PreferredHeight;

		bool AllowArrows;
		Point ArrowOffset;
		Point UpArrowSize;
		Point DownArrowSize;

		SliderStyles Style;
		ButtonBP *Up, *Down;


		SliderElement	*NormalStyle,
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
