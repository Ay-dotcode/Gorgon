#pragma once

#include "../Utils/GGE.h"
#include "../Engine/Layer.h"
#include "../Engine/GraphicLayers.h"

namespace gge {
	class GGEMain;
}

namespace gge { namespace resource {
	class File;
} }

namespace gge { namespace widgets {
	void RegisterWidgetLoaders(resource::File *File);
	inline void RegisterWidgetLoaders(resource::File &File) { RegisterWidgetLoaders(&File); }
	void InitializeWidgets(GGEMain *Main);

	class WidgetLayer :  public input::BasicPointerTarget, public graphics::Basic2DLayer {
	public:
		WidgetLayer(int X=0, int Y=0, int W=100, int H=100) : graphics::Basic2DLayer(X,Y,W,H) {
		}
		////Renders this layer
		virtual void Render() { graphics::Basic2DLayer::Render(); }

	protected:
		////Processes the mouse event for the current layer, default
		/// handling is to pass the request to the sub-layers
		virtual bool PropagateMouseEvent(input::MouseEventType event, int x, int y, void *data);
		virtual bool PropagateMouseScrollEvent(int amount, input::MouseEventType event, int x, int y, void *data);
	};

	extern WidgetLayer *DialogLayer;
} }
