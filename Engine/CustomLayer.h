#pragma once

#include "Layer.h"
#include <functional>

namespace gge {


	class CustomLayer : public LayerBase {
	public:
		typedef std::function<void()> CustomRenderer;

		CustomLayer(CustomRenderer Renderer) : Renderer(Renderer), LayerBase() { init(); }

		CustomLayer(CustomRenderer Renderer, const utils::Bounds &b) : Renderer(Renderer), LayerBase(b) { init(); }

		CustomLayer(CustomRenderer Renderer, int L, int T, int R, int B) : Renderer(Renderer), LayerBase(L,T,R,B) { init(); }

		CustomLayer(CustomRenderer Renderer, int X,int Y) : Renderer(Renderer), LayerBase(X,Y) { init(); }

		CustomLayer(CustomRenderer Renderer, const utils::Point &p) : Renderer(Renderer), LayerBase(p) { init(); }

		virtual void Render();

		virtual void Clear() { }
		void init();

		float Xs, Xe, Ys, Ye, Zs, Ze;

		CustomRenderer Renderer;
	};

}