#pragma once

#ifdef GRAPH_USEDOUBLE
	#define graphtype double
	#define sinfn	sin
	#define cosfn	cos
	#define tanfn	tan
	#define atanfn	atan

	#ifndef PI
		#define PI	3.1415926535898
	#endif
#else
	#define graphtype float
	#define sinfn	sinf
	#define cosfn	cosf
	#define tanfn	tanf
	#define atanfn	atanf

	#ifndef PI
		#define PI	3.1415926535898f
	#endif
#endif
