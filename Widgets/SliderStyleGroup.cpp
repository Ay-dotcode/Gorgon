#include "SliderStyleGroup.h"

namespace gge { namespace widgets {

	SliderStyleGroup::SliderStyleGroup(void)
	{
	}

	void SliderStyleGroup::Prepare(gge::GGEMain *main) {
		ResourceBase::Prepare(main);

		NormalStyle		= (SliderElement*) file->FindObject(guid_normalstyle	);
		HoverStyle		= (SliderElement*) file->FindObject(guid_hoverstyle		);
		PressedStyle	= (SliderElement*) file->FindObject(guid_pressedstyle	);
		DisabledStyle	= (SliderElement*) file->FindObject(guid_disabledstyle	);
		Normal2Hover	= (SliderElement*) file->FindObject(guid_normal2hover	);
		Normal2Pressed	= (SliderElement*) file->FindObject(guid_normal2pressed	);
		Hover2Pressed	= (SliderElement*) file->FindObject(guid_hover2pressed	);
		Hover2Normal	= (SliderElement*) file->FindObject(guid_hover2normal	);
		Pressed2Normal	= (SliderElement*) file->FindObject(guid_pressed2normal	);
		Pressed2Hover	= (SliderElement*) file->FindObject(guid_pressed2hover	);

		if(HoverStyle==NULL)
			HoverStyle=NormalStyle;
		if(PressedStyle==NULL)
			PressedStyle=NormalStyle;
		if(DisabledStyle==NULL)
			DisabledStyle=NormalStyle;

		int c=0;
		utils::LinkedListIterator<ResourceBase> it=Subitems;
		ResourceBase *resource;
		while(resource=it) {
			if(resource->getGID()==GID_BUTTON) {
				if(c==0)
					Up=(ButtonBP*)resource;
				else if(c==1)
					Down=(ButtonBP*)resource;

				c++;
			}
		}
	}
} }
