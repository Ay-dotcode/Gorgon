#include "CheckboxStyleGroup.h"

namespace gorgonwidgets {

	CheckboxStyleGroup::CheckboxStyleGroup(void) {
	}

	void CheckboxStyleGroup::Prepare(gge::GGEMain *main) {
		ResourceBase::Prepare(main);
		NormalStyle		= (CheckboxElement*) file->FindObject(guid_normalstyle		);
		HoverStyle		= (CheckboxElement*) file->FindObject(guid_hoverstyle		);
		PressedStyle	= (CheckboxElement*) file->FindObject(guid_pressedstyle		);
		DisabledStyle	= (CheckboxElement*) file->FindObject(guid_disabledstyle	);
		Normal2Hover	= (CheckboxElement*) file->FindObject(guid_normal2hover		);
		Normal2Pressed	= (CheckboxElement*) file->FindObject(guid_normal2pressed	);
		Hover2Pressed	= (CheckboxElement*) file->FindObject(guid_hover2pressed	);
		Hover2Normal	= (CheckboxElement*) file->FindObject(guid_hover2normal		);
		Pressed2Normal	= (CheckboxElement*) file->FindObject(guid_pressed2normal	);
		Pressed2Hover	= (CheckboxElement*) file->FindObject(guid_pressed2hover	);
		ToNextStyle		= (CheckboxElement*) file->FindObject(guid_tonextstyle		);

		if(HoverStyle==NULL)
			HoverStyle=NormalStyle;
		if(PressedStyle==NULL)
			PressedStyle=NormalStyle;
		if(DisabledStyle==NULL)
			DisabledStyle=NormalStyle;

		cNormalStyle	= (CheckboxElement*) file->FindObject(guid_cnormalstyle		);
		cHoverStyle		= (CheckboxElement*) file->FindObject(guid_choverstyle		);
		cPressedStyle	= (CheckboxElement*) file->FindObject(guid_cpressedstyle	);
		cDisabledStyle	= (CheckboxElement*) file->FindObject(guid_cdisabledstyle	);
		cNormal2Hover	= (CheckboxElement*) file->FindObject(guid_cnormal2hover	);
		cNormal2Pressed	= (CheckboxElement*) file->FindObject(guid_cnormal2pressed	);
		cHover2Pressed	= (CheckboxElement*) file->FindObject(guid_chover2pressed	);
		cHover2Normal	= (CheckboxElement*) file->FindObject(guid_chover2normal	);
		cPressed2Normal	= (CheckboxElement*) file->FindObject(guid_cpressed2normal	);
		cPressed2Hover	= (CheckboxElement*) file->FindObject(guid_cpressed2hover	);
		cToNextStyle	= (CheckboxElement*) file->FindObject(guid_ctonextstyle		);

		if(cHoverStyle==NULL)
			cHoverStyle=cNormalStyle;
		if(cPressedStyle==NULL)
			cPressedStyle=cNormalStyle;
		if(cDisabledStyle==NULL)
			cDisabledStyle=cNormalStyle;
	}

}