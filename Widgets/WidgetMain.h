#pragma once

#include "../Utils/GGE.h"
#include "../Engine/Layer.h"
#include "../Engine/GraphicLayers.h"

namespace gge {
	class GGEMain;
}

namespace gre {
	class ResourceFile;
}

using namespace gge;
using namespace gre;


namespace gge { namespace widgets {
	void RegisterWidgetLoaders(ResourceFile *File);
	inline void RegisterWidgetLoaders(ResourceFile &File) { RegisterWidgetLoaders(&File); }
	void InitializeWidgets(GGEMain *Main);

	extern WidgetLayer *DialogLayer;
} }

