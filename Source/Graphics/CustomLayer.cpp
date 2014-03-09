#include "CustomLayer.h"
#include "Graphics.h"
#include "GGEMain.h"
#include <GL/gl.h>

namespace gge {
	namespace graphics {
		extern RGBfloat CurrentLayerColor;
		extern utils::Point translate;
		extern utils::Bounds scissors;
	}

	void CustomLayer::Render() {

		Renderer();

	}

	void CustomLayer::init() {
		Xs=-10;
		Xe= 10;
		Ys=-10;
		Ye= 10;
		Zs=-10;
		Ze= 10;
	}

}