#include "../Engine/GGEMain.h"
#include "NullImage.h"

namespace gge { namespace resource {

	void Init(GGEMain &Main) {
		NullImage::ni=new NullImage();
	}

}}
