#pragma once

#include <string.h>
#include <iostream>
#include <iomanip>
#include <stdint.h>

#include "../Types.h"

namespace Gorgon { namespace Graphics {

	/// Color modes for images
	enum class ColorMode {
		/// This is used to mark invalid color data
		Invalid = 0,

		/// This is used by some functions to mark color mode should be determined automatically
		Automatic = 0,

		/// 24bit red, green, blue color mode that has red component in the lowest byte order
		RGB = 1,

		/// 24bit red, green, blue color mode that has blue component in the lowest byte order
		BGR = 16,

		/// 8bit gray scale color mode
		Grayscale = 4,

		/// 8bit alpha only color mode
		Alpha = 8,

		/// 32bit red, green, blue and alpha channel image. Red component is in the lowest byte order and 
		RGBA = RGB | Alpha,

		/// 32bit red, green, blue and alpha channel image. Blue component is in the lowest byte order and 
		/// alpha is in the highest byte order.
		BGRA = BGR | Alpha,

		/// 16bit gray scale image color mode with an alpha channel. Alpha channel is in the high byte
		Grayscale_Alpha = Grayscale | Alpha
	};

	/// Returns bytes per pixel for the given color mode
	inline unsigned long GetBytesPerPixel(ColorMode mode) {
		switch(mode) {
		case ColorMode::Grayscale:
		case ColorMode::Alpha:
			return 1;
		case ColorMode::Grayscale_Alpha:
			return 2;
		case ColorMode::BGR:
		case ColorMode::RGB:
			return 3;
		case ColorMode::RGBA:
		case ColorMode::BGRA:
			return 4;
		default:
#ifndef NDEBUG
			throw std::runtime_error("Unknown mode");
#endif
			return 0;
		}
	}

	/// Returns if the given color mode has alpha channel
	inline bool HasAlpha(ColorMode mode) {
		return ((int)mode & (int)ColorMode::Alpha) != 0;
	}

	/// Returns the index of alpha channel. If alpha channel does not exists, this function returns -1.
	inline int AlphaIndex(ColorMode mode) {
		switch(mode) {
			case ColorMode::Alpha:
				return 0;
			case ColorMode::Grayscale_Alpha:
				return 1;
			case ColorMode::RGBA:
			case ColorMode::BGRA:
				return 3;
			default:
				return -1;
		}
	}

	/// This class represents a color information. Contains 4 channels, 8 bits each.
	/// Red is the lowest bit while alpha is the highest. Please note that conversion from/to integer
	/// will work in reverse of the HTML notation. 0xff800000 is dark blue not dark red.
	class RGBA {
	public:
		/// Data type for each channel
		typedef Byte ChannelType;

		/// Default constructor does not perform initialization
		RGBA() { }

		/// Copy constructor
		RGBA(const RGBA &) = default;

		/// Filling constructor
		RGBA(Byte r, Byte g, Byte b, Byte a=255) : R(r), G(g), B(b), A(a) { }

		/// Constructs a grayscale color from the given luminance
		explicit RGBA(Byte lum, Byte a=255) : RGBA(lum, lum, lum) { }

		/// Conversion from integer
		RGBA(const int &color) {
			static_assert(sizeof(int)>=4, "This conversion requires size of int to be at least 4 bytes");

			memcpy(this, &color, 4);
		}

		/// Conversion from uint32_t
		RGBA(const uint32_t &color) {
			memcpy(this, &color, 4);
		}

		/// Conversion from float. Assumes the given float value is a 0 to 1 luminance. Sets alpha to 255
		explicit RGBA(float lum) : A(255) {
			if(lum<0) lum=0;
			if(lum>1) lum=1;

			R=G=B=Byte(lum*255);
		}

		/// Conversion to integer
		operator int() const {
			static_assert(sizeof(int)>=4, "This conversion requires size of int to be at least 4 bytes");

			int ret;
			memcpy(&ret, this, 4);

			return ret;
		}

		/// Conversion to integer
		operator uint32_t() const {
			uint32_t ret;
			memcpy(&ret, this, 4);

			return ret;
		}

		/// Copy assignment
		RGBA &operator =(const RGBA &) = default;

		/// From integer assignment
		RGBA &operator =(const int &color) {
			static_assert(sizeof(int)>=4, "This conversion requires size of int to be at least 4 bytes");

			memcpy(this, &color, 4);
			
			return *this;
		}

		/// From integer assignment
		RGBA &operator =(const uint32_t &color) {
			memcpy(this, &color, 4);
			
			return *this;
		}

		/// From float assignment. Assumes the given float value is a 0 to 1 luminance. Sets alpha to 255
		RGBA &operator =(float lum) {
			if(lum<0) lum=0;
			if(lum>1) lum=1;

			R=G=B=Byte(lum*255);
			A=255;
			
			return *this;
		}

		/// Compares two colors
		bool operator==(const RGBA &other) const {
			return (int)(*this) == (uint32_t)other;
		}

		/// Compares two colors
		bool operator!=(const RGBA &other) const {
			return (uint32_t)(*this) != (uint32_t)other;
		}

		/// Returns the luminance of this color as a single byte number. The returned number could be supplied
		/// to a new color to create grayscale representation of this color. This function performs lots of shifts
		/// to calculate a luminance close to perceived grayscale value. Probably works even slower than accurate
		/// luminance, however, the value returned is directly a byte.
		Byte Luminance() const {
			return (R>>3) + (R>>4) + (R>>5) + (G>>1) + (G>>3) + (G>>4) + (G>>5) + (B>>4) + (B>>5);
		}

		/// Returns the luminance of this color as a floating point value between 0 and 1. The conversion is done to
		/// preserve perceived luminance.
		float AccurateLuminance() const {
			return  (0.2126f/255)*R + (0.7151f/255)*G + (0.0722f/255)*B;
		}

		/// Returns a six nibble HTML color
		std::string HTMLColor() const {
			std::stringstream str;

			str<<"#"<<std::fixed<<std::setfill('0')<<std::setw(2)<<R<<std::setw(2)<<G<<std::setw(2)<<B;

			return str.str();
		}

		/// Blends the given color into this one. This operation performs regular alpha blending with the current
		/// color being blended over.
		void Blend(const RGBA &color) {
			if(color.A==255) {
				A=255;
				R=color.R;
				G=color.G;
				B=color.B;
			}
			else {
				float alpha  =(float)color.A/255;
				float alpham1=(float)(255-color.A)/255;

				if(A<255) {
					int aa=(int)A+color.A;
					if(aa>255)
						A=255;
					else
						A=aa;
				}

				R=(Byte)(R*alpham1+color.R*alpha);
				G=(Byte)(G*alpham1+color.G*alpha);
				B=(Byte)(B*alpham1+color.B*alpha);
			}
		}

		/// Converts this color to a hex representation of this color
		operator std::string() const {
			std::stringstream str;
			str<<std::fixed<<std::setw(8)<<std::setfill('0')<<std::hex<<((const uint32_t)(*this));

			return str.str();
		}



		/// Red channel
		Byte R;

		/// Green channel
		Byte G;

		/// Blue channel
		Byte B;

		/// Alpha channel
		Byte A;

		// Maximum value for this color type, activate after c++14 support
		// static const Byte Max = 255;
	};

	/// Prints the given color to the stream
	inline std::ostream &operator <<(std::ostream &stream, const RGBA &color) {
		stream<<(std::string)color;

		return stream;
	}

	/// Reads a color from the stream. This color can either be in full HTML format with # in front or
	/// a hex representation of the color with an optional 0x in front.
	inline std::istream &operator>>(std::istream &in, RGBA &color) {
		while(isspace(in.peek())) in.ignore();

		if(in.peek()=='#') {
			color.A=255;
			in.ignore(1);

			auto flags=in.flags();
			in>>std::hex>>color.R>>color.G>>color.B;
			in.flags(flags);
		}
		else {
			if(in.peek()=='0') {
				in.ignore();
				if(in.peek()=='x') {
					in.ignore();
				}
				else {
					in.clear();
				}
			}

			auto flags=in.flags();
			in>>std::hex>>(*(unsigned int*)&color);
			in.flags(flags);
		}
		return in;
	}

	/// Represents a four channel 32 bit float per channel color information. 
	class RGBAf {
	public:
		/// Data type for each channel
		typedef float ChannelType;


		/// Default constructor does not perform initialization
		RGBAf() { }

		/// Filling constructor
		RGBAf(float r, float g, float b, float a=1.f) : R(r), G(g), B(b), A(a) { }

		/// Constructor that sets all color channels to the given value to create a grayscale color. Alpha is set to 1.0f
		RGBAf(float lum, float a=1.0f) : RGBAf(lum, lum, lum, a) { }

		/// Constructor that sets all color channels to the given value to create a grayscale color. Alpha is set to 1.0f
		explicit RGBAf(double lum, float a=1.0f) : RGBAf((float)lum, (float)lum, (float)lum, a) { }

		/// Converts a RGBA to RGBAf
		RGBAf(const RGBA &color) : R(color.R/255.f), G(color.G/255.f), B(color.B/255.f), A(color.A/255.f) { }

		/// Converts from an unsigned int
		RGBAf(unsigned color) : RGBAf(RGBA(color)) { }

		/// Converts from an unsigned int
		RGBAf(int) = delete;

		/// Converts from an unsigned int
		RGBAf(bool) = delete;

		/// Copy assignment
		RGBAf &operator = (const RGBAf &) = default;

		/// Assignment from RGBA
		RGBAf &operator =(const RGBA &color) {
			R = color.R/255.f;
			G = color.G/255.f;
			B = color.B/255.f;
			A = color.A/255.f;
			
			return *this;
		}

		/// Assignment from int
		RGBAf &operator =(const int &color) {
			return (*this = RGBA(color));
		}

		/// Assignment from float
		RGBAf &operator =(float lum) {
			R = lum;
			G = lum;
			B = lum;
			A = 1;

			return *this;
		}

		/// Assignment from float
		RGBAf &operator =(double lum) {
			R = (float)lum;
			G = (float)lum;
			B = (float)lum;
			A = 1;

			return *this;
		}

		/// Converts this color to RGBA without overflow checking
		explicit operator RGBA() const {
			return{Byte(R*255), Byte(G*255), Byte(B*255), Byte(A*255)};
		}

		/// Conversion to integer
		explicit operator int() const {
			return int(this->operator RGBA());
		}
		
		RGBAf operator *(const RGBAf &other) {
            return {R * other.R, G * other.G, B * other.B, A * other.A};
        }
		
		RGBAf &operator *=(const RGBAf &other) {
            R *= other.R;
            G *= other.G;
            B *= other.B;
            A *= other.A;
            
            return *this;
        }

		/// Converts this color to RGBA by clipping the values
		RGBA Convert() const { 
			return{
				Byte(R<0.f ? 0 : (R>1.f ? 255 : R*255)), 
				Byte(G<0.f ? 0 : (G>1.f ? 255 : G*255)),
				Byte(B<0.f ? 0 : (B>1.f ? 255 : B*255)),
				Byte(A<0.f ? 0 : (A>1.f ? 255 : A*255))
			};
		}

		/// Returns the luminance of this color as a floating point value between 0 and 1. The conversion is done to
		/// preserve perceived luminance.
		float Luminance() const {
			return  0.2126f*R + 0.7151f*G + 0.0722f*B;
		}

		/// Compares two colors
		bool operator ==(const RGBAf &other) const {
			return R==other.R && G==other.G && B==other.B && A==other.A;
		}

		/// Compares two colors
		bool operator !=(const RGBAf &other) const {
			return R!=other.R || G!=other.G || B!=other.B || A!=other.A;
		}

		/// Blends the given color into this one. This operation performs regular alpha blending with the current
		/// color being blended over.
		void Blend(const RGBAf &color) {
			if(color.A==1.f) {
				A=1.f;
				R=color.R;
				G=color.G;
				B=color.B;
			}
			else {
				float alpham1=1.f-color.A;

				A += color.A;
				if(A > 1.f)
					A=1.f;

				R=R*alpham1 + color.R*color.A;
				G=G*alpham1 + color.G*color.A;
				B=B*alpham1 + color.B*color.A;
			}
		}

		union {
			struct {
				/// Red channel
				float R;

				/// Green channel
				float G;

				/// Blue channel
				float B;

				/// Alpha channel
				float A;
			};

			/// Representation of this class as a float vector
			float Vector[4];
		};

		// Maximum value for this color type, activate after c++14 support
		// static const float Max = 1.0f;
	};

	/// Contains commonly used colors. These colors are adjusted for perceived values and are not pure.
	namespace Color {
		//Requires C++14 support, allows Red to be used with RGBA and RGBAf
		//template<class T_> 
		//constexpr T_ Red = {T_::ChannelType(T_::Max*0.878431.f), T_::ChannelType(T_::Max*0.039216.f), T_::ChannelType(T_::Max*0.039216.f)};

		/// Red
		static const RGBA Red	= 0xff1010e0;

		/// Green, not very bright
		static const RGBA Green	= 0xff109010;

		/// Blue
		static const RGBA Blue	= 0xffff1515;
	}
} }
