///@file Types.h contains type definitions for Gorgon system
#include <stdint.h>

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
    
    typedef uint32_t Char;
    
    static const Float PI = (Float)3.141592653589793;
	
	/// A class that has no members and can be used as placeholder
	class Empty {};


	/// Where acceptable, denotes that the object will assume the ownership
	class AssumeOwnershipTag { };

	/// Returns the number of bits that are 1 in a number
	inline int NumberOfSetBits(uint32_t i) {
		i = i - ((i >> 1) & 0x55555555);
		i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
		return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
	}

	/// Where acceptable, denotes that the object will assume the ownership
	static constexpr AssumeOwnershipTag AssumeOwnership;
    
#ifdef DOXYGEN
#   define ENUMCLASS enum
#else
#   define ENUMCLASS enum class
#endif
}
