#include "../Resource.h"
#include "DataItems.h"


namespace Gorgon { namespace Resource {
	
	void Initialize() {
		DataItem::InitializeLoaders();
	}
	
} }
