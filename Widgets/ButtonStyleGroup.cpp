#include "ButtonStyleGroup.h"

namespace gorgonwidgets {

	ButtonStyleGroup::ButtonStyleGroup(void) {
	}

	void ButtonStyleGroup::Prepare(gge::GGEMain *main) {
		ResourceBase::Prepare(main);
		NormalStyle		= (ButtonElement*) file->FindObject(guid_normalstyle	);
		HoverStyle		= (ButtonElement*) file->FindObject(guid_hoverstyle		);
		PressedStyle	= (ButtonElement*) file->FindObject(guid_pressedstyle	);
		DisabledStyle	= (ButtonElement*) file->FindObject(guid_disabledstyle	);
		Normal2Hover	= (ButtonElement*) file->FindObject(guid_normal2hover	);
		Normal2Pressed	= (ButtonElement*) file->FindObject(guid_normal2pressed	);
		Hover2Pressed	= (ButtonElement*) file->FindObject(guid_hover2pressed	);
		Hover2Normal	= (ButtonElement*) file->FindObject(guid_hover2normal	);
		Pressed2Normal	= (ButtonElement*) file->FindObject(guid_pressed2normal	);
		Pressed2Hover	= (ButtonElement*) file->FindObject(guid_pressed2hover	);

		if(HoverStyle==NULL)
			HoverStyle=NormalStyle;
		if(PressedStyle==NULL)
			PressedStyle=NormalStyle;
		if(DisabledStyle==NULL)
			DisabledStyle=NormalStyle;
	}

}