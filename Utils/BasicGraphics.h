#pragma once

#ifdef GRAPH_USEDOUBLE
namespace gge {
#	define graphtype double
}

#	ifndef PI
#		define PI	3.1415926535898
#	endif
#else
namespace gge {
#	define graphtype float
}

#	ifndef PI
#		define PI	3.1415926535898f
#	endif
#endif
