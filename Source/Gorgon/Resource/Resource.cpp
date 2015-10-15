#include "../Resource.h"
#include "Data.h"


namespace Gorgon { namespace Resource {
	
	void Initialize() {
		Data::InitializeLoaders();
	}
	
} }
