///@file Types.h contains type definitions for Gorgon system

#pragma once

namespace Gorgon {
		
	/// Represents smallest cell in memory.
	typedef unsigned char Byte;
	
#ifdef GORGON_USE_DOUBLE
	typedef double Float;
#else
	/// Represents floating point data type. GORGON_USE_DOUBLE definition
	/// allows switching to double for precision sensitive functions.
	typedef float Float;
#endif
	
	/// A class that has no members and can be used as placeholder
	class Empty {};
	
}
