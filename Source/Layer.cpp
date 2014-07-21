#include "Layer.h"

namespace Gorgon {
	
	void Layer::Render() {
		for(auto &l : children) {
			l.Render();
		}
	}

	const Geometry::Bounds Layer::EntireRegion;

}
