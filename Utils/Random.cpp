#include <stdlib.h>
#include <time.h>

namespace gge { namespace utils {
	namespace {
		struct random_initializer {
			random_initializer() {
				srand((unsigned)time(NULL));
			}
		} ri;
	}
} }
