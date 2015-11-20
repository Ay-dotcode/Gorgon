#pragma once

#include "Utils/Logging.h"

namespace Gorgon {
/// Contains audio subsystem. For audio primitives look into multimedia namespace.
namespace Audio {

	void Initialize();
	
	bool IsAvailable();
	
	extern Utils::Logger Log;
}
}
