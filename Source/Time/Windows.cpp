#include "../Time.h"
#include <Windows.h>

namespace Gorgon { namespace Time {

	unsigned long GetTime() {
		return timeGetTime();
	}

} }
