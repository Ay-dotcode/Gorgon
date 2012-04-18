#include "../Engine/GGEMain.h"
#include "NullImage.h"

namespace gge { namespace resource {

	void Init(GGEMain &Main) {
		if(!NullImage::ni)
			NullImage::ni=new NullImage();
	}

}}
