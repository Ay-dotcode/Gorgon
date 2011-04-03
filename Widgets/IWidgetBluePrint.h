#pragma once

#include "IWidgetObject.h"
#include "IWidgetContainer.h"
#include "../Utils/GGE.h"
#include "../Engine/GGEMain.h"
#include "../Engine/Pointer.h"

using namespace gge;

namespace gge { namespace widgets {
	class IWidgetBluePrint {
	public:
		IWidgetBluePrint() : useAlphaAnimation(false), PointerType(Pointer::None) { }
		virtual IWidgetObject *Create(IWidgetContainer& Container,int X,int Y,int Width,int Height)=0;
		virtual IWidgetObject *Create(IWidgetContainer& Container,Point position,int Width,int Height);

		bool useAlphaAnimation;
		Pointer::PointerTypes PointerType;
	};
} }

