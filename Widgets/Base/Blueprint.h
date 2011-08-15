#pragma once


#include "..\..\Engine\Pointer.h"
#include "..\..\Resource\ResourceBase.h"


namespace gge { namespace widgets {

	class WidgetBase;

	class Blueprint {
	public:
		Blueprint() : AlphaAnimation(0), Pointer(gge::Pointer::None)
		{ }

		int AlphaAnimation;
		Pointer::PointerType Pointer;
	};

}}
