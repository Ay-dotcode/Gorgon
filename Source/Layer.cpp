#include "Layer.h"

namespace Gorgon {
	
	void Layer::Render() {
		//TODO
		for(auto &l : children) {
			l.Render();
		}
	}

	const Geometry::Bounds Layer::EntireRegion;

}
