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

	/// This class represents a range of values. Generally, start is
	/// assumed to be included while end is excluded.
	template<class T_>
	class Range {
	public:
		Range() = default;
		Range(T_ start, T_ end) : Start(start), End(end) { }

		/// Normalizes the range ensuring Start is less than or equal
		/// to the end.
		void Normalize() {
			if(Start > End) {
				auto t = End;
				End = Start;
				Start = t;
			}
		}

		/// Returns the difference between start and end
		T_ Difference() const { 
			return End - Start;
		}

		T_ Start = T_();
		T_ End   = T_();
	};
    
    typedef uint32_t Char;
    
    static const Float PI = (Float)3.141592653589793;
	
	/// A class that has no members and can be used as placeholder
	class Empty {};
    
    /// This enumeration helps with systems that has boolena parameters
    /// that can be detected automatically, but can also be overriden.
    enum class YesNoAuto {
        No = 0,
        Yes = 1,
        Auto = 2
    };

	/// Returns the number of bits that are 1 in a number
	inline int NumberOfSetBits(uint32_t i) {
		i = i - ((i >> 1) & 0x55555555);
		i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
		return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
	}
	
	/// Calculate the smalllest power of two larger than this value
	inline uint32_t CeilToPowerOf2(uint32_t v) {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        
        return v;
    }
    
    /// Performs clamping on the given value.
    template<class T_>
    void FitInto(T_ &v, T_ min, T_ max) {
        //replace with std::clamp once C++17 is enforced
        
        v = v < min ? min : (v > max ? max : v);
    }
    
    /// Performs clamping on the given value and returns the result.
    template<class T_>
    T_ Clamp(T_ v, T_ min, T_ max) {
        //replace with std::clamp once C++17 is enforced
        
        return v < min ? min : (v > max ? max : v);
    }

	/// Returns if the given number is within the given range including min
	/// but excluding max
	template<class T_>
	bool Between(T_ v, T_ min, T_ max) {
		return v >= min && v < max;
	}

	/// Returns if the given number is within the given range including min
	/// but excluding max
	template<class T_>
	bool Between(Range<T_> range, T_ v) {
		return v >= range.Start && v < range.End;
	}

	/// Returns if the given number is within the given range including min
	/// and max
	template<class T_>
	bool BetweenInclusive(T_ v, T_ min, T_ max) {
		return v >= min && v <= max;
	}
	template <typename T_>
	int Sign(T_ val) {
		return (T_{} < val) - (val < T_{});
	}
	
	template <typename T_>
	T_ PositiveMod(T_ value, T_ mod) {
        return ((value%mod)+mod)%mod;
    }

	/// Where acceptable, denotes that the object will assume the ownership
	class AssumeOwnershipTag { };
    
	/// Where acceptable, denotes that the object will assume the ownership
	static constexpr AssumeOwnershipTag AssumeOwnership;

	/// Marks an object that can be updated
	class Updatable {
	public:
		virtual ~Updatable() {}

		virtual void Update() = 0;
	};
    
#ifdef DOXYGEN
#   define ENUMCLASS enum
#else
#   define ENUMCLASS enum class
#endif
}
