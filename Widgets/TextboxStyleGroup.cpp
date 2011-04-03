#include "TextboxStyleGroup.h"

namespace gge { namespace widgets {

	TextboxStyleGroup::TextboxStyleGroup(void)
	{
	}

	void TextboxStyleGroup::Prepare(gge::GGEMain *main) {
		ResourceBase::Prepare(main);
		NormalStyle		= (TextboxElement*) file->FindObject(guid_normalstyle	);
		HoverStyle		= (TextboxElement*) file->FindObject(guid_hoverstyle	);
		ActiveStyle		= (TextboxElement*) file->FindObject(guid_activestyle	);
		DisabledStyle	= (TextboxElement*) file->FindObject(guid_disabledstyle	);
		Normal2Hover	= (TextboxElement*) file->FindObject(guid_normal2hover	);
		Normal2Active	= (TextboxElement*) file->FindObject(guid_normal2active	);
		Hover2Active	= (TextboxElement*) file->FindObject(guid_hover2active	);
		Hover2Normal	= (TextboxElement*) file->FindObject(guid_hover2normal	);
		Active2Normal	= (TextboxElement*) file->FindObject(guid_active2normal	);
		Active2Hover	= (TextboxElement*) file->FindObject(guid_active2hover	);

		if(HoverStyle==NULL)
			HoverStyle=NormalStyle;
		if(ActiveStyle==NULL)
			ActiveStyle=NormalStyle;
		if(DisabledStyle==NULL)
			DisabledStyle=NormalStyle;
	}

} }
