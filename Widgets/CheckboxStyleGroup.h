#pragma once

#include "../Resource/ResourceBase.h"
#include "CheckboxElement.h"

using namespace gre;

namespace gge { namespace widgets {

#define GID_CHECKBOX_GRP			0x5350000
#define GID_CHECKBOX_GRP_PROPS		0x5350101
	
	enum CheckboxTypes {
		CT_Checkbox,
		CT_RadioButton
	};

	class CheckboxStyleGroup : public ResourceBase
	{
		friend ResourceBase* LoadCheckboxStyleGroup(ResourceFile*,FILE*,int);
	public:
		CheckboxStyleGroup(void);
		virtual int getGID() { return GID_CHECKBOX_GRP; }
		virtual void Prepare(gge::GGEMain *main);
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }

		CheckboxTypes CheckboxType;

		int PreferredWidth;
		int PreferredHeight;

		CheckboxElement	*NormalStyle,
						*HoverStyle,
						*PressedStyle,
						*DisabledStyle,
						*Normal2Hover,
						*Normal2Pressed,
						*Hover2Pressed,
						*Hover2Normal,
						*Pressed2Normal,
						*Pressed2Hover,
						*ToNextStyle;

		CheckboxElement	*cNormalStyle,
						*cHoverStyle,
						*cPressedStyle,
						*cDisabledStyle,
						*cNormal2Hover,
						*cNormal2Pressed,
						*cHover2Pressed,
						*cHover2Normal,
						*cPressed2Normal,
						*cPressed2Hover,
						*cToNextStyle;

		bool			RevHover2NormalState,
						RevPressed2HoverState,
						RevPressed2NormalState;

		bool			cRevHover2NormalState,
						cRevPressed2HoverState,
						cRevPressed2NormalState,
						cRevToNextStyle;

	protected:
		ResourceFile* file;

		Guid *guid_normalstyle	,
			 *guid_hoverstyle	,
			 *guid_pressedstyle	,
			 *guid_disabledstyle,
			 *guid_normal2hover	,
			 *guid_normal2pressed,
			 *guid_hover2pressed,
			 *guid_hover2normal	,
			 *guid_pressed2normal,
			 *guid_pressed2hover,
			 *guid_tonextstyle	;

		Guid*guid_cnormalstyle		,
			*guid_choverstyle		,
			*guid_cpressedstyle		,
			*guid_cdisabledstyle	,
			*guid_cnormal2hover		,
			*guid_cnormal2pressed	,
			*guid_chover2pressed	,
			*guid_chover2normal		,
			*guid_cpressed2normal	,
			*guid_cpressed2hover	,
			*guid_ctonextstyle		;


	};

} }
