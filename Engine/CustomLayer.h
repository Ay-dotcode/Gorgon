#pragma once

#include "Layer.h"

namespace gge {

	typedef void (*CustomRenderer)();

	class CustomLayer : public LayerBase {
	public:
		CustomLayer(CustomRenderer Renderer, int X, int Y, int W, int H);

		virtual void Render();

		virtual void Clear() { }

		float Xs, Xe, Ys, Ye, Zs, Ze;

		CustomRenderer Renderer;
	};

}