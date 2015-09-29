#pragma once
#include <stdint.h>
#include <iostream>
#include <vector>
#include "../Types.h"
#include "../SGuid.h"

/// IO Related functionality. Stream operations in this namespace do not check the validity of the stream.
/// it is up to the caller
namespace Gorgon { namespace IO { 

	/// Reads an enumeration that is saved as 32bit integer
	template<class E_>
	inline E_ ReadEnum32(std::istream &stream) {
		int32_t r;
		stream.read(reinterpret_cast<char*>(&r), sizeof(int32_t));

		return E_(r);
	}

	/// Reads a 32-bit integer from the stream. A long is at least 32 bits, could be more
	/// however, only 4 bytes will be read from the stream
	inline long ReadInt32(std::istream &stream) {
		int32_t r;
		stream.read(reinterpret_cast<char*>(&r), sizeof(int32_t));

		return r;
	}

	/// Reads a 32-bit unsigned integer from the stream. An unsigned long is at least 32 bits, could be more
	/// however, only 4 bytes will be read from the stream
	inline unsigned long ReadUInt32(std::istream &stream) {
		uint32_t r;
		stream.read(reinterpret_cast<char*>(&r), sizeof(uint32_t));

		return r;
	}

	/// Reads a 16-bit integer from the stream. An int is at least 16 bits, could be more
	/// however, only 2 bytes will be read from the stream
	inline int ReadInt16(std::istream &stream) {
		int16_t r;
		stream.read(reinterpret_cast<char*>(&r), sizeof(int16_t));

		return r;
	}

	/// Reads a 16-bit unsigned integer from the stream. An unsigned int is at least 32 bits, could be more
	/// however, only 2 bytes will be read from the stream
	inline unsigned ReadUInt16(std::istream &stream) {
		uint16_t r;
		stream.read(reinterpret_cast<char*>(&r), sizeof(uint16_t));

		return r;
	}

	/// Reads an 8-bit integer from the stream. A char is at least 8 bits, could be more
	/// however, only 1 byte will be read from the stream
	inline char ReadInt8(std::istream &stream) {
		int8_t r;
		stream.read(reinterpret_cast<char*>(&r), sizeof(int8_t));

		return r;
	}

	/// Reads an 8-bit unsigned integer from the stream. A char is at least 8 bits, could be more
	/// however, only 1 byte will be read from the stream
	inline Byte ReadUInt8(std::istream &stream) {
		uint8_t r;
		stream.read(reinterpret_cast<char*>(&r), sizeof(uint8_t));

		return r;
	}

	/// Reads a 32 bit IEEE floating point number from the stream. This function only works on systems that
	/// that have native 32 bit floats.
	inline float ReadFloat(std::istream &stream) {
		static_assert(sizeof(float) == 4, "Current implementation only supports 32bit floats");

		float r;
		stream.read(reinterpret_cast<char*>(&r), 4);

		return r;
	}

	/// Reads a 64 bit IEEE double precision floating point number from the stream. This function only works 
	/// on systems that have native 64 bit doubles.
	inline float ReadDouble(std::istream &stream) {
		static_assert(sizeof(double) == 8, "Current implementation only supports 64bit floats");

		float r;
		stream.read(reinterpret_cast<char*>(&r), 4);

		return r;
	}

	/// Reads a boolean value. In resource 1.0, booleans are stored as 32bit integers
	inline bool ReadBool(std::istream &stream) {
		return ReadInt32(stream) != 0;
	}

	/// Reads a string from a given stream. Assumes the size of the string is appended before the string as
	/// 32-bit unsigned value.
	inline std::string ReadString(std::istream &stream) {
		unsigned long size;
		std::string ret;
		size = ReadUInt32(stream);
		ret.resize(size);
		stream.read(&ret[0], size);

		return ret;
	}

	/// Reads a string with the given size.
	inline std::string ReadString(std::istream &stream, unsigned long size) {
		std::string ret;
		ret.resize(size);
		stream.read(&ret[0], size);
		return ret;
	}

	/// Reads an array from the stream. Array type should be a fixed size construct (ie. a byte, int32_t), otherwise
	/// a mismatch between binary formats will cause trouble.
	/// @param  data is the data to be read from the stream
	/// @param  size is the number of elements to be read
	template<class T_>
	inline void ReadArray(std::istream &stream, T_ *data, unsigned long size) {
		stream.read((char*)data, size*sizeof(T_));
	}

	/// Reads a GUID from the given stream.
	inline SGuid ReadGuid(std::istream &stream) {
		return SGuid(stream);
	}

	
	/// Writes an enumeration as a 32-bit integer
	template<class E_>
	inline void WriteEnum32(std::ostream &stream, E_ v) {
		int32_t r = reinterpret_cast<int32_t>(v);
		stream.write(reinterpret_cast<char*>(&r), sizeof(int32_t));
	}

	/// Writes a 32-bit integer to the stream.
	inline void WriteInt32(std::ostream &stream, long value) {
		int32_t r = value;
		stream.write(reinterpret_cast<const char*>(&r), sizeof(int32_t));
	}

	/// Writes a 32-bit unsigned integer from the stream. An unsigned long is at least 32 bits, could be more
	/// however, only 4 bytes will be written to the stream
	inline void WriteUInt32(std::ostream &stream, unsigned long value) {
		uint32_t r = value;
		stream.write(reinterpret_cast<const char*>(&r), sizeof(uint32_t));
	}

	/// Writes a 16-bit integer from the stream. An int is at least 16 bits, could be more
	/// however, only 2 bytes will be written to the stream
	inline void WriteInt16(std::ostream &stream, int value) {
		int16_t r = value;
		stream.write(reinterpret_cast<const char*>(&r), sizeof(int16_t));
	}

	/// Writes a 16-bit unsigned integer from the stream. An unsigned int is at least 16 bits, could be more
	/// however, only 2 bytes will be written to the stream
	inline void WriteUInt16(std::ostream &stream, unsigned value) {
		uint16_t r = value;
		stream.write(reinterpret_cast<const char*>(&r), sizeof(uint16_t));
	}

	/// Writes an 8-bit integer from the stream. A char is at least 8 bits, could be more
	/// however, only 1 byte will be written to the stream
	inline void WriteInt8(std::ostream &stream, char value) {
		stream.write(reinterpret_cast<const char*>(&value), sizeof(int8_t));
	}

	/// Writes an 8-bit unsigned integer from the stream. A char is at least 8 bits, could be more
	/// however, only 1 byte will be written to the stream
	inline void WriteUInt8(std::ostream &stream, Byte value) {
		stream.write(reinterpret_cast<const char*>(&value), sizeof(uint8_t));
	}

	/// Writes a 32 bit IEEE floating point number from the stream. This function only works on systems that
	/// that have native 32 bit floats.
	inline void WriteFloat(std::ostream &stream, float value) {
		static_assert(sizeof(float) == 4, "Current implementation only supports 32bit floats");

		stream.write(reinterpret_cast<const char*>(&value), 4);
	}

	/// Writes a 64 bit IEEE double precision floating point number from the stream. This function only works 
	/// on systems that have native 64 bit doubles.
	inline void WriteDouble(std::ostream &stream, double value) {
		static_assert(sizeof(double) == 8, "Current implementation only supports 64bit floats");

		stream.write(reinterpret_cast<const char*>(&value), 4);
	}

	/// Writes a boolean value. In resource 1.0, booleans are stored as 32bit integers
	inline void WriteBool(std::ostream &stream, bool value) {
		return WriteInt32(stream, value);
	}

	/// Writes a string from a given stream. The size of the string is appended before the string as
	/// 32-bit unsigned value.
	inline void WriteStringWithSize(std::ostream &stream, const std::string &value) {
		WriteUInt32(stream, value.size());
		
		stream.write(&value[0], value.size());
	}

	/// Writes a string without its size.
	inline void WriteString(std::ostream &stream, const std::string &value) {
		stream.write(&value[0], value.size());
	}

	/// Writes an array to the stream. Array type should be a fixed size construct (ie. a byte, int32_t), otherwise
	/// a mismatch between binary formats will cause trouble.
	/// @param  data is the data to be written to the stream
	/// @param  size is the number of elements to be write
	template<class T_>
	inline void WriteArray(std::ostream &stream, const T_ *data, unsigned long size) {
		stream.write((const char*)data, size*sizeof(T_));
	}
	

	/// Writes a vector to the stream. Type of vector elements should be given a fixed size construct, otherwise
	/// a mismatch between binary formats will cause trouble.
	template<class T_>
	inline void WriteVector(std::ostream &stream, const std::vector<T_> &data, unsigned long size) {
		stream.write((const char*)data[0], data.size()*sizeof(T_));
	}
	

	/// Writes a GUID from the given stream.
	inline void WriteGuid(std::ostream &stream, const SGuid &value) {
		WriteArray(stream, value.Bytes, 8);
	}

} }


/// Adds an integer to streampos
inline long operator +(const std::streampos &l, long r) {
	return (long)l + r;
}

/// Adds an unsigned integer to streampos
inline unsigned long operator +(const std::streampos &l, unsigned long r) {
	return (unsigned long)l + r;
}

/// Subtracts an integer from streampos
inline long operator -(const std::streampos &l, unsigned long r) {
	return (long)l - r;
}

/// Subtracts an integer from streampos
inline long operator -(const std::streampos &l, long r) {
	return (long)l - r;
}

/// Subtracts an unsigned integer from streampos
inline long operator -(const std::streampos &l, int r) {
	return (unsigned long)l - r;
}
