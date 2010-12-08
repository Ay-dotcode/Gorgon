#pragma once

#include "Layer.h"

namespace gge {

	typedef void (*CustomRenderer)();

	class CustomLayer : public LayerBase {
	public:
		CustomLayer(CustomRenderer Renderer, int X=0, int Y=0, int W=100, int H=100);

		virtual void Render();

		virtual void Clear() { }

		float Xs, Xe, Ys, Ye, Zs, Ze;

		CustomRenderer Renderer;
	};

}