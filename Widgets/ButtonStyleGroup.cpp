#include "ButtonStyleGroup.h"

namespace gorgonwidgets {

	ButtonStyleGroup::ButtonStyleGroup(void) {
	}

	void ButtonStyleGroup::Prepare(gge::GGEMain *main) {
		ResourceBase::Prepare(main);
		NormalStyle		= dynamic_cast<ButtonElement*>( file->FindObject(guid_normalstyle	));
		HoverStyle		= dynamic_cast<ButtonElement*>( file->FindObject(guid_hoverstyle	));
		PressedStyle	= dynamic_cast<ButtonElement*>( file->FindObject(guid_pressedstyle	));
		DisabledStyle	= dynamic_cast<ButtonElement*>( file->FindObject(guid_disabledstyle	));
		Normal2Hover	= dynamic_cast<ButtonElement*>( file->FindObject(guid_normal2hover	));
		Normal2Pressed	= dynamic_cast<ButtonElement*>( file->FindObject(guid_normal2pressed));
		Hover2Pressed	= dynamic_cast<ButtonElement*>( file->FindObject(guid_hover2pressed	));
		Hover2Normal	= dynamic_cast<ButtonElement*>( file->FindObject(guid_hover2normal	));
		Pressed2Normal	= dynamic_cast<ButtonElement*>( file->FindObject(guid_pressed2normal));
		Pressed2Hover	= dynamic_cast<ButtonElement*>( file->FindObject(guid_pressed2hover	));

		if(HoverStyle==NULL)
			HoverStyle=NormalStyle;
		if(PressedStyle==NULL)
			PressedStyle=NormalStyle;
		if(DisabledStyle==NULL)
			DisabledStyle=NormalStyle;
	}

}