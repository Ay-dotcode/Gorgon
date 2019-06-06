#pragma once

#include <string.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdint.h>
#include <stdexcept>

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
	inline int GetAlphaIndex(ColorMode mode) {
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
		explicit RGBA(Byte lum, Byte a=255) : RGBA(lum, lum, lum, a) { }

		/// Conversion from integer
		constexpr RGBA(int color) : R((color>>0)&0xff), G((color>>8)&0xff), B((color>>16)&0xff), A((color>>24)&0xff) {
			static_assert(sizeof(int)>=4, "This conversion requires size of int to be at least 4 bytes");
		}

		/// Conversion from uint32_t
		constexpr RGBA(uint32_t color) : R((color>>0)&0xff), G((color>>8)&0xff), B((color>>16)&0xff), A((color>>24)&0xff) {
			static_assert(sizeof(int)>=4, "This conversion requires size of int to be at least 4 bytes");
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

		operator std::string() const {
			std::stringstream ss;

			ss<<"("<<R<<", "<<G<<", "<<B<<", "<<A<<")";

			return ss.str();
		}
		
		RGBAf operator *(const RGBAf &other) const {
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

		/// Blends the given color into this one with the given factor that is applied to all channels
		void Blend(const RGBAf &color, float factor) {
			auto m = 1 - factor;

			R = m * R + factor * color.R;
			G = m * G + factor * color.G;
			B = m * B + factor * color.B;
			A = m * A + factor * color.A;
		}

		/// Blends the given color into this one with the given factor that is applied to color and alpha
		/// channels separately
		void Blend(const RGBAf &color, float factor_color, float factor_alpha) {
			auto mc = 1 - factor_color;
			auto ma = 1 - factor_alpha;

			R = mc * R + factor_color * color.R;
			G = mc * G + factor_color * color.G;
			B = mc * B + factor_color * color.B;
			A = ma * A + factor_alpha * color.A;
		}

		/// Blends the given color into this one with the given color as blending factor
		void Blend(const RGBAf &color, const RGBAf &factor) {
			R = (1 - factor.R) * R + factor.R * color.R;
			G = (1 - factor.G) * G + factor.G * color.G;
			B = (1 - factor.B) * B + factor.B * color.B;
			A = (1 - factor.A) * A + factor.A * color.A;
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

	/// Prints the given color to the stream
	inline std::ostream &operator <<(std::ostream &stream, const RGBAf &color) {
		stream<<(std::string)color;

		return stream;
	}

	/// Contains commonly used colors identified by XKCD survey containing 140000 people.
	/// List is truncated to 300 most popular entries and cleaned up.
	namespace Color {
        constexpr RGBA Purple	= 0xff7e1e9c;
        constexpr RGBA Green	= 0xff15b01a;
        constexpr RGBA Blue	= 0xff0343df;
        constexpr RGBA Pink	= 0xffff81c0;
        constexpr RGBA Brown	= 0xff653700;
        constexpr RGBA Red	= 0xffe50000;
        constexpr RGBA LightBlue	= 0xff95d0fc;
        constexpr RGBA Teal	= 0xff029386;
        constexpr RGBA Orange	= 0xfff97306;
        constexpr RGBA LightGreen	= 0xff96f97b;
        constexpr RGBA Magenta	= 0xffc20078;
        constexpr RGBA Yellow	= 0xffffff14;
        constexpr RGBA SkyBlue	= 0xff75bbfd;
        constexpr RGBA Grey	= 0xff929591;
        constexpr RGBA LimeGreen	= 0xff89fe05;
        constexpr RGBA LightPurple	= 0xffbf77f6;
        constexpr RGBA Violet	= 0xff9a0eea;
        constexpr RGBA DarkGreen	= 0xff033500;
        constexpr RGBA Turquoise	= 0xff06c2ac;
        constexpr RGBA Lavender	= 0xffc79fef;
        constexpr RGBA DarkBlue	= 0xff00035b;
        constexpr RGBA Tan	= 0xffd1b26f;
        constexpr RGBA Cyan	= 0xff00ffff;
        constexpr RGBA Aqua	= 0xff13eac9;
        constexpr RGBA ForestGreen	= 0xff06470c;
        constexpr RGBA Mauve	= 0xffae7181;
        constexpr RGBA DarkPurple	= 0xff35063e;
        constexpr RGBA BrightGreen	= 0xff01ff07;
        constexpr RGBA Maroon	= 0xff650021;
        constexpr RGBA Olive	= 0xff6e750e;
        constexpr RGBA Salmon	= 0xffff796c;
        constexpr RGBA Beige	= 0xffe6daa6;
        constexpr RGBA RoyalBlue	= 0xff0504aa;
        constexpr RGBA NavyBlue	= 0xff001146;
        constexpr RGBA Lilac	= 0xffcea2fd;
        constexpr RGBA Black	= 0xff000000;
        constexpr RGBA HotPink	= 0xffff028d;
        constexpr RGBA LightBrown	= 0xffad8150;
        constexpr RGBA PaleGreen	= 0xffc7fdb5;
        constexpr RGBA Peach	= 0xffffb07c;
        constexpr RGBA OliveGreen	= 0xff677a04;
        constexpr RGBA DarkPink	= 0xffcb416b;
        constexpr RGBA Periwinkle	= 0xff8e82fe;
        constexpr RGBA SeaGreen	= 0xff53fca1;
        constexpr RGBA Lime	= 0xffaaff32;
        constexpr RGBA Indigo	= 0xff380282;
        constexpr RGBA Mustard	= 0xffceb301;
        constexpr RGBA LightPink	= 0xffffd1df;
        constexpr RGBA Rose	= 0xffcf6275;
        constexpr RGBA BrightBlue	= 0xff0165fc;
        constexpr RGBA NeonGreen	= 0xff0cff0c;
        constexpr RGBA BurntOrange	= 0xffc04e01;
        constexpr RGBA Aquamarine	= 0xff04d8b2;
        constexpr RGBA Navy	= 0xff01153e;
        constexpr RGBA GrassGreen	= 0xff3f9b0b;
        constexpr RGBA PaleBlue	= 0xffd0fefe;
        constexpr RGBA DarkRed	= 0xff840000;
        constexpr RGBA BrightPurple	= 0xffbe03fd;
        constexpr RGBA YellowGreen	= 0xffc0fb2d;
        constexpr RGBA BabyBlue	= 0xffa2cffe;
        constexpr RGBA Gold	= 0xffdbb40c;
        constexpr RGBA MintGreen	= 0xff8fff9f;
        constexpr RGBA Plum	= 0xff580f41;
        constexpr RGBA RoyalPurple	= 0xff4b006e;
        constexpr RGBA BrickRed	= 0xff8f1402;
        constexpr RGBA DarkTeal	= 0xff014d4e;
        constexpr RGBA Burgundy	= 0xff610023;
        constexpr RGBA Khaki	= 0xffaaa662;
        constexpr RGBA BlueGreen	= 0xff137e6d;
        constexpr RGBA SeafoamGreen	= 0xff7af9ab;
        constexpr RGBA PeaGreen	= 0xff8eab12;
        constexpr RGBA Taupe	= 0xffb9a281;
        constexpr RGBA DarkBrown	= 0xff341c02;
        constexpr RGBA DeepPurple	= 0xff36013f;
        constexpr RGBA Chartreuse	= 0xffc1f80a;
        constexpr RGBA BrightPink	= 0xfffe01b1;
        constexpr RGBA LightOrange	= 0xfffdaa48;
        constexpr RGBA Mint	= 0xff9ffeb0;
        constexpr RGBA PastelGreen	= 0xffb0ff9d;
        constexpr RGBA Sand	= 0xffe2ca76;
        constexpr RGBA DarkOrange	= 0xffc65102;
        constexpr RGBA SpringGreen	= 0xffa9f971;
        constexpr RGBA Puce	= 0xffa57e52;
        constexpr RGBA Seafoam	= 0xff80f9ad;
        constexpr RGBA GreyBlue	= 0xff6b8ba4;
        constexpr RGBA ArmyGreen	= 0xff4b5d16;
        constexpr RGBA DarkGrey	= 0xff363737;
        constexpr RGBA DarkYellow	= 0xffd5b60a;
        constexpr RGBA Goldenrod	= 0xfffac205;
        constexpr RGBA Slate	= 0xff516572;
        constexpr RGBA LightTeal	= 0xff90e4c1;
        constexpr RGBA Rust	= 0xffa83c09;
        constexpr RGBA DeepBlue	= 0xff040273;
        constexpr RGBA PalePink	= 0xffffcfdc;
        constexpr RGBA Cerulean	= 0xff0485d1;
        constexpr RGBA LightRed	= 0xffff474c;
        constexpr RGBA MustardYellow	= 0xffd2bd0a;
        constexpr RGBA Ochre	= 0xffbf9005;
        constexpr RGBA PaleYellow	= 0xffffff84;
        constexpr RGBA Crimson	= 0xff8c000f;
        constexpr RGBA Fuchsia	= 0xffed0dd9;
        constexpr RGBA HunterGreen	= 0xff0b4008;
        constexpr RGBA BlueGrey	= 0xff607c8e;
        constexpr RGBA SlateBlue	= 0xff5b7c99;
        constexpr RGBA PalePurple	= 0xffb790d4;
        constexpr RGBA SeaBlue	= 0xff047495;
        constexpr RGBA PinkishPurple	= 0xffd648d7;
        constexpr RGBA LightGrey	= 0xffd8dcd6;
        constexpr RGBA LeafGreen	= 0xff5ca904;
        constexpr RGBA LightYellow	= 0xfffffe7a;
        constexpr RGBA Eggplant	= 0xff380835;
        constexpr RGBA SteelBlue	= 0xff5a7d9a;
        constexpr RGBA MossGreen	= 0xff658b38;
        constexpr RGBA White	= 0xffffffff;
        constexpr RGBA GreyGreen	= 0xff789b73;
        constexpr RGBA Sage	= 0xff87ae73;
        constexpr RGBA Brick	= 0xffa03623;
        constexpr RGBA BurntSienna	= 0xffb04e0f;
        constexpr RGBA ReddishBrown	= 0xff7f2b0a;
        constexpr RGBA Cream	= 0xffffffc2;
        constexpr RGBA Coral	= 0xfffc5a50;
        constexpr RGBA OceanBlue	= 0xff03719c;
        constexpr RGBA Greenish	= 0xff40a368;
        constexpr RGBA DarkMagenta	= 0xff960056;
        constexpr RGBA RedOrange	= 0xfffd3c06;
        constexpr RGBA BluishPurple	= 0xff703be7;
        constexpr RGBA MidnightBlue	= 0xff020035;
        constexpr RGBA LightViolet	= 0xffd6b4fc;
        constexpr RGBA DustyRose	= 0xffc0737a;
        constexpr RGBA GreenishYellow	= 0xffcdfd02;
        constexpr RGBA YellowishGreen	= 0xffb0dd16;
        constexpr RGBA PurplishBlue	= 0xff601ef9;
        constexpr RGBA GreyishBlue	= 0xff5e819d;
        constexpr RGBA Grape	= 0xff6c3461;
        constexpr RGBA LightOlive	= 0xffacbf69;
        constexpr RGBA CornflowerBlue	= 0xff5170d7;
        constexpr RGBA PinkishRed	= 0xfff10c45;
        constexpr RGBA BrightRed	= 0xffff000d;
        constexpr RGBA Azure	= 0xff069af3;
        constexpr RGBA BluePurple	= 0xff5729ce;
        constexpr RGBA DarkTurquoise	= 0xff045c5a;
        constexpr RGBA ElectricBlue	= 0xff0652ff;
        constexpr RGBA OffWhite	= 0xffffffe4;
        constexpr RGBA PowderBlue	= 0xffb1d1fc;
        constexpr RGBA Wine	= 0xff80013f;
        constexpr RGBA DullGreen	= 0xff74a662;
        constexpr RGBA AppleGreen	= 0xff76cd26;
        constexpr RGBA LightTurquoise	= 0xff7ef4cc;
        constexpr RGBA NeonPurple	= 0xffbc13fe;
        constexpr RGBA Cobalt	= 0xff1e488f;
        constexpr RGBA Pinkish	= 0xffd46a7e;
        constexpr RGBA OliveDrab	= 0xff6f7632;
        constexpr RGBA DarkCyan	= 0xff0a888a;
        constexpr RGBA PurpleBlue	= 0xff632de9;
        constexpr RGBA DarkViolet	= 0xff34013f;
        constexpr RGBA DarkLavender	= 0xff856798;
        constexpr RGBA ForrestGreen	= 0xff154406;
        constexpr RGBA PaleOrange	= 0xffffa756;
        constexpr RGBA GreenishBlue	= 0xff0b8b87;
        constexpr RGBA DarkTan	= 0xffaf884a;
        constexpr RGBA GreenBlue	= 0xff06b48b;
        constexpr RGBA BluishGreen	= 0xff10a674;
        constexpr RGBA PastelBlue	= 0xffa2bffe;
        constexpr RGBA Moss	= 0xff769958;
        constexpr RGBA Grass	= 0xff5cac2d;
        constexpr RGBA DeepPink	= 0xffcb0162;
        constexpr RGBA BloodRed	= 0xff980002;
        constexpr RGBA SageGreen	= 0xff88b378;
        constexpr RGBA AquaBlue	= 0xff02d8e9;
        constexpr RGBA Terracotta	= 0xffca6641;
        constexpr RGBA PastelPurple	= 0xffcaa0ff;
        constexpr RGBA Sienna	= 0xffa9561e;
        constexpr RGBA DarkOlive	= 0xff373e02;
        constexpr RGBA GreenYellow	= 0xffc9ff27;
        constexpr RGBA Scarlet	= 0xffbe0119;
        constexpr RGBA GreyishGreen	= 0xff82a67d;
        constexpr RGBA Chocolate	= 0xff3d1c02;
        constexpr RGBA BlueViolet	= 0xff5d06e9;
        constexpr RGBA BabyPink	= 0xffffb7ce;
        constexpr RGBA Charcoal	= 0xff343837;
        constexpr RGBA PineGreen	= 0xff0a481e;
        constexpr RGBA Pumpkin	= 0xffe17701;
        constexpr RGBA GreenishBrown	= 0xff696112;
        constexpr RGBA RedBrown	= 0xff8b2e16;
        constexpr RGBA BrownishGreen	= 0xff6a6e09;
        constexpr RGBA Tangerine	= 0xffff9408;
        constexpr RGBA SalmonPink	= 0xfffe7b7c;
        constexpr RGBA AquaGreen	= 0xff12e193;
        constexpr RGBA Raspberry	= 0xffb00149;
        constexpr RGBA GreyishPurple	= 0xff887191;
        constexpr RGBA RosePink	= 0xfff7879a;
        constexpr RGBA NeonPink	= 0xfffe019a;
        constexpr RGBA CobaltBlue	= 0xff030aa7;
        constexpr RGBA OrangeBrown	= 0xffbe6400;
        constexpr RGBA DeepRed	= 0xff9a0200;
        constexpr RGBA OrangeRed	= 0xfffd411e;
        constexpr RGBA DirtyYellow	= 0xffcdc50a;
        constexpr RGBA Orchid	= 0xffc875c4;
        constexpr RGBA ReddishPink	= 0xfffe2c54;
        constexpr RGBA ReddishPurple	= 0xff910951;
        constexpr RGBA YellowOrange	= 0xfffcb001;
        constexpr RGBA LightCyan	= 0xffacfffc;
        constexpr RGBA Sky	= 0xff82cafc;
        constexpr RGBA LightMagenta	= 0xfffa5ff7;
        constexpr RGBA PaleRed	= 0xffd9544d;
        constexpr RGBA Emerald	= 0xff01a049;
        constexpr RGBA DarkBeige	= 0xffac9362;
        constexpr RGBA Jade	= 0xff1fa774;
        constexpr RGBA GreenishGrey	= 0xff96ae8d;
        constexpr RGBA DarkSalmon	= 0xffc85a53;
        constexpr RGBA PurplishPink	= 0xffce5dae;
        constexpr RGBA DarkAqua	= 0xff05696b;
        constexpr RGBA BrownishOrange	= 0xffcb7723;
        constexpr RGBA LightOliveGreen	= 0xffa4be5c;
        constexpr RGBA LightAqua	= 0xff8cffdb;
        constexpr RGBA Clay	= 0xffb66a50;
        constexpr RGBA BurntUmber	= 0xffa0450e;
        constexpr RGBA DullBlue	= 0xff49759c;
        constexpr RGBA PaleBrown	= 0xffb1916e;
        constexpr RGBA EmeraldGreen	= 0xff028f1e;
        constexpr RGBA Brownish	= 0xff9c6d57;
        constexpr RGBA Mud	= 0xff735c12;
        constexpr RGBA DarkRose	= 0xffb5485d;
        constexpr RGBA BrownishRed	= 0xff9e3623;
        constexpr RGBA PinkPurple	= 0xffdb4bda;
        constexpr RGBA PinkyPurple	= 0xffc94cbe;
        constexpr RGBA CamoGreen	= 0xff526525;
        constexpr RGBA FadedGreen	= 0xff7bb274;
        constexpr RGBA DustyPink	= 0xffd58a94;
        constexpr RGBA PurplePink	= 0xffe03fd8;
        constexpr RGBA DeepGreen	= 0xff02590f;
        constexpr RGBA ReddishOrange	= 0xfff8481c;
        constexpr RGBA Mahogany	= 0xff4a0100;
        constexpr RGBA Aubergine	= 0xff3d0734;
        constexpr RGBA DullPink	= 0xffd5869d;
        constexpr RGBA Evergreen	= 0xff05472a;
        constexpr RGBA DarkSkyBlue	= 0xff448ee4;
        constexpr RGBA IceBlue	= 0xffd7fffe;
        constexpr RGBA LightTan	= 0xfffbeeac;
        constexpr RGBA DirtyGreen	= 0xff667e2c;
        constexpr RGBA NeonBlue	= 0xff04d9ff;
        constexpr RGBA Denim	= 0xff3b638c;
        constexpr RGBA Eggshell	= 0xffffffd4;
        constexpr RGBA JungleGreen	= 0xff048243;
        constexpr RGBA DarkPeach	= 0xffde7e5d;
        constexpr RGBA Umber	= 0xffb26400;
        constexpr RGBA BrightYellow	= 0xfffffd01;
        constexpr RGBA DustyBlue	= 0xff5a86ad;
        constexpr RGBA ElectricGreen	= 0xff21fc0d;
        constexpr RGBA LighterGreen	= 0xff75fd63;
        constexpr RGBA SlateGrey	= 0xff59656d;
        constexpr RGBA TealGreen	= 0xff25a36f;
        constexpr RGBA MarineBlue	= 0xff01386a;
        constexpr RGBA Avocado	= 0xff90b134;
        constexpr RGBA Forest	= 0xff0b5509;
        constexpr RGBA PeaSoup	= 0xff929901;
        constexpr RGBA Lemon	= 0xfffdff52;
        constexpr RGBA MuddyGreen	= 0xff657432;
        constexpr RGBA Marigold	= 0xfffcc006;
        constexpr RGBA Ocean	= 0xff017b92;
        constexpr RGBA LightMauve	= 0xffc292a1;
        constexpr RGBA Bordeaux	= 0xff7b002c;
        constexpr RGBA Pistachio	= 0xffc0fa8b;
        constexpr RGBA LemonYellow	= 0xfffdff38;
        constexpr RGBA RedViolet	= 0xff9e0168;
        constexpr RGBA DuskyPink	= 0xffcc7a8b;
        constexpr RGBA Dirt	= 0xff8a6e45;
        constexpr RGBA Pine	= 0xff2b5d34;
        constexpr RGBA Vermillion	= 0xfff4320c;
        constexpr RGBA Amber	= 0xfffeb308;
        constexpr RGBA Silver	= 0xffc5c9c7;
        constexpr RGBA Coffee	= 0xffa6814c;
        constexpr RGBA Sepia	= 0xff985e2b;
        constexpr RGBA FadedRed	= 0xffd3494e;
        constexpr RGBA CanaryYellow	= 0xfffffe40;
        constexpr RGBA CherryRed	= 0xfff7022a;
        constexpr RGBA Ocre	= 0xffc69c04;
        constexpr RGBA Ivory	= 0xffffffcb;
        constexpr RGBA Copper	= 0xffb66325;
        constexpr RGBA DarkLime	= 0xff84b701;
        constexpr RGBA Strawberry	= 0xfffb2943;
        constexpr RGBA DarkNavy	= 0xff000435;
        constexpr RGBA Cinnamon	= 0xffac4f06;
        constexpr RGBA CloudyBlue	= 0xffacc2d9;
        
        inline const std::vector<std::pair<std::string, RGBA>> &Names() {
            static std::vector<std::pair<std::string, RGBA>> names = {
                {"Purple", Purple},
                {"Green", Green},
                {"Blue", Blue},
                {"Pink", Pink},
                {"Brown", Brown},
                {"Red", Red},
                {"Light Blue", LightBlue},
                {"Teal", Teal},
                {"Orange", Orange},
                {"Light Green", LightGreen},
                {"Magenta", Magenta},
                {"Yellow", Yellow},
                {"Sky Blue", SkyBlue},
                {"Grey", Grey},
                {"Lime Green", LimeGreen},
                {"Light Purple", LightPurple},
                {"Violet", Violet},
                {"Dark Green", DarkGreen},
                {"Turquoise", Turquoise},
                {"Lavender", Lavender},
                {"Dark Blue", DarkBlue},
                {"Tan", Tan},
                {"Cyan", Cyan},
                {"Aqua", Aqua},
                {"Forest Green", ForestGreen},
                {"Mauve", Mauve},
                {"Dark Purple", DarkPurple},
                {"Bright Green", BrightGreen},
                {"Maroon", Maroon},
                {"Olive", Olive},
                {"Salmon", Salmon},
                {"Beige", Beige},
                {"Royal Blue", RoyalBlue},
                {"Navy Blue", NavyBlue},
                {"Lilac", Lilac},
                {"Black", Black},
                {"Hot Pink", HotPink},
                {"Light Brown", LightBrown},
                {"Pale Green", PaleGreen},
                {"Peach", Peach},
                {"Olive Green", OliveGreen},
                {"Dark Pink", DarkPink},
                {"Periwinkle", Periwinkle},
                {"Sea Green", SeaGreen},
                {"Lime", Lime},
                {"Indigo", Indigo},
                {"Mustard", Mustard},
                {"Light Pink", LightPink},
                {"Rose", Rose},
                {"Bright Blue", BrightBlue},
                {"Neon Green", NeonGreen},
                {"Burnt Orange", BurntOrange},
                {"Aquamarine", Aquamarine},
                {"Navy", Navy},
                {"Grass Green", GrassGreen},
                {"Pale Blue", PaleBlue},
                {"Dark Red", DarkRed},
                {"Bright Purple", BrightPurple},
                {"Yellow Green", YellowGreen},
                {"Baby Blue", BabyBlue},
                {"Gold", Gold},
                {"Mint Green", MintGreen},
                {"Plum", Plum},
                {"Royal Purple", RoyalPurple},
                {"Brick Red", BrickRed},
                {"Dark Teal", DarkTeal},
                {"Burgundy", Burgundy},
                {"Khaki", Khaki},
                {"Blue Green", BlueGreen},
                {"Seafoam Green", SeafoamGreen},
                {"Pea Green", PeaGreen},
                {"Taupe", Taupe},
                {"Dark Brown", DarkBrown},
                {"Deep Purple", DeepPurple},
                {"Chartreuse", Chartreuse},
                {"Bright Pink", BrightPink},
                {"Light Orange", LightOrange},
                {"Mint", Mint},
                {"Pastel Green", PastelGreen},
                {"Sand", Sand},
                {"Dark Orange", DarkOrange},
                {"Spring Green", SpringGreen},
                {"Puce", Puce},
                {"Seafoam", Seafoam},
                {"Grey Blue", GreyBlue},
                {"Army Green", ArmyGreen},
                {"Dark Grey", DarkGrey},
                {"Dark Yellow", DarkYellow},
                {"Goldenrod", Goldenrod},
                {"Slate", Slate},
                {"Light Teal", LightTeal},
                {"Rust", Rust},
                {"Deep Blue", DeepBlue},
                {"Pale Pink", PalePink},
                {"Cerulean", Cerulean},
                {"Light Red", LightRed},
                {"Mustard Yellow", MustardYellow},
                {"Ochre", Ochre},
                {"Pale Yellow", PaleYellow},
                {"Crimson", Crimson},
                {"Fuchsia", Fuchsia},
                {"Hunter Green", HunterGreen},
                {"Blue Grey", BlueGrey},
                {"Slate Blue", SlateBlue},
                {"Pale Purple", PalePurple},
                {"Sea Blue", SeaBlue},
                {"Pinkish Purple", PinkishPurple},
                {"Light Grey", LightGrey},
                {"Leaf Green", LeafGreen},
                {"Light Yellow", LightYellow},
                {"Eggplant", Eggplant},
                {"Steel Blue", SteelBlue},
                {"Moss Green", MossGreen},
                {"White", White},
                {"Grey Green", GreyGreen},
                {"Sage", Sage},
                {"Brick", Brick},
                {"Burnt Sienna", BurntSienna},
                {"Reddish Brown", ReddishBrown},
                {"Cream", Cream},
                {"Coral", Coral},
                {"Ocean Blue", OceanBlue},
                {"Greenish", Greenish},
                {"Dark Magenta", DarkMagenta},
                {"Red Orange", RedOrange},
                {"Bluish Purple", BluishPurple},
                {"Midnight Blue", MidnightBlue},
                {"Light Violet", LightViolet},
                {"Dusty Rose", DustyRose},
                {"Greenish Yellow", GreenishYellow},
                {"Yellowish Green", YellowishGreen},
                {"Purplish Blue", PurplishBlue},
                {"Greyish Blue", GreyishBlue},
                {"Grape", Grape},
                {"Light Olive", LightOlive},
                {"Cornflower Blue", CornflowerBlue},
                {"Pinkish Red", PinkishRed},
                {"Bright Red", BrightRed},
                {"Azure", Azure},
                {"Blue Purple", BluePurple},
                {"Dark Turquoise", DarkTurquoise},
                {"Electric Blue", ElectricBlue},
                {"Off White", OffWhite},
                {"Powder Blue", PowderBlue},
                {"Wine", Wine},
                {"Dull Green", DullGreen},
                {"Apple Green", AppleGreen},
                {"Light Turquoise", LightTurquoise},
                {"Neon Purple", NeonPurple},
                {"Cobalt", Cobalt},
                {"Pinkish", Pinkish},
                {"Olive Drab", OliveDrab},
                {"Dark Cyan", DarkCyan},
                {"Purple Blue", PurpleBlue},
                {"Dark Violet", DarkViolet},
                {"Dark Lavender", DarkLavender},
                {"Forrest Green", ForrestGreen},
                {"Pale Orange", PaleOrange},
                {"Greenish Blue", GreenishBlue},
                {"Dark Tan", DarkTan},
                {"Green Blue", GreenBlue},
                {"Bluish Green", BluishGreen},
                {"Pastel Blue", PastelBlue},
                {"Moss", Moss},
                {"Grass", Grass},
                {"Deep Pink", DeepPink},
                {"Blood Red", BloodRed},
                {"Sage Green", SageGreen},
                {"Aqua Blue", AquaBlue},
                {"Terracotta", Terracotta},
                {"Pastel Purple", PastelPurple},
                {"Sienna", Sienna},
                {"Dark Olive", DarkOlive},
                {"Green Yellow", GreenYellow},
                {"Scarlet", Scarlet},
                {"Greyish Green", GreyishGreen},
                {"Chocolate", Chocolate},
                {"Blue Violet", BlueViolet},
                {"Baby Pink", BabyPink},
                {"Charcoal", Charcoal},
                {"Pine Green", PineGreen},
                {"Pumpkin", Pumpkin},
                {"Greenish Brown", GreenishBrown},
                {"Red Brown", RedBrown},
                {"Brownish Green", BrownishGreen},
                {"Tangerine", Tangerine},
                {"Salmon Pink", SalmonPink},
                {"Aqua Green", AquaGreen},
                {"Raspberry", Raspberry},
                {"Greyish Purple", GreyishPurple},
                {"Rose Pink", RosePink},
                {"Neon Pink", NeonPink},
                {"Cobalt Blue", CobaltBlue},
                {"Orange Brown", OrangeBrown},
                {"Deep Red", DeepRed},
                {"Orange Red", OrangeRed},
                {"Dirty Yellow", DirtyYellow},
                {"Orchid", Orchid},
                {"Reddish Pink", ReddishPink},
                {"Reddish Purple", ReddishPurple},
                {"Yellow Orange", YellowOrange},
                {"Light Cyan", LightCyan},
                {"Sky", Sky},
                {"Light Magenta", LightMagenta},
                {"Pale Red", PaleRed},
                {"Emerald", Emerald},
                {"Dark Beige", DarkBeige},
                {"Jade", Jade},
                {"Greenish Grey", GreenishGrey},
                {"Dark Salmon", DarkSalmon},
                {"Purplish Pink", PurplishPink},
                {"Dark Aqua", DarkAqua},
                {"Brownish Orange", BrownishOrange},
                {"Light Olive Green", LightOliveGreen},
                {"Light Aqua", LightAqua},
                {"Clay", Clay},
                {"Burnt Umber", BurntUmber},
                {"Dull Blue", DullBlue},
                {"Pale Brown", PaleBrown},
                {"Emerald Green", EmeraldGreen},
                {"Brownish", Brownish},
                {"Mud", Mud},
                {"Dark Rose", DarkRose},
                {"Brownish Red", BrownishRed},
                {"Pink Purple", PinkPurple},
                {"Pinky Purple", PinkyPurple},
                {"Camo Green", CamoGreen},
                {"Faded Green", FadedGreen},
                {"Dusty Pink", DustyPink},
                {"Purple Pink", PurplePink},
                {"Deep Green", DeepGreen},
                {"Reddish Orange", ReddishOrange},
                {"Mahogany", Mahogany},
                {"Aubergine", Aubergine},
                {"Dull Pink", DullPink},
                {"Evergreen", Evergreen},
                {"Dark Sky Blue", DarkSkyBlue},
                {"Ice Blue", IceBlue},
                {"Light Tan", LightTan},
                {"Dirty Green", DirtyGreen},
                {"Neon Blue", NeonBlue},
                {"Denim", Denim},
                {"Eggshell", Eggshell},
                {"Jungle Green", JungleGreen},
                {"Dark Peach", DarkPeach},
                {"Umber", Umber},
                {"Bright Yellow", BrightYellow},
                {"Dusty Blue", DustyBlue},
                {"Electric Green", ElectricGreen},
                {"Lighter Green", LighterGreen},
                {"Slate Grey", SlateGrey},
                {"Teal Green", TealGreen},
                {"Marine Blue", MarineBlue},
                {"Avocado", Avocado},
                {"Forest", Forest},
                {"Pea Soup", PeaSoup},
                {"Lemon", Lemon},
                {"Muddy Green", MuddyGreen},
                {"Marigold", Marigold},
                {"Ocean", Ocean},
                {"Light Mauve", LightMauve},
                {"Bordeaux", Bordeaux},
                {"Pistachio", Pistachio},
                {"Lemon Yellow", LemonYellow},
                {"Red Violet", RedViolet},
                {"Dusky Pink", DuskyPink},
                {"Dirt", Dirt},
                {"Pine", Pine},
                {"Vermillion", Vermillion},
                {"Amber", Amber},
                {"Silver", Silver},
                {"Coffee", Coffee},
                {"Sepia", Sepia},
                {"Faded Red", FadedRed},
                {"Canary Yellow", CanaryYellow},
                {"Cherry Red", CherryRed},
                {"Ocre", Ocre},
                {"Ivory", Ivory},
                {"Copper", Copper},
                {"Dark Lime", DarkLime},
                {"Strawberry", Strawberry},
                {"Dark Navy", DarkNavy},
                {"Cinnamon", Cinnamon},
                {"Cloudy Blue", CloudyBlue},
            };
            
            return names;
        }
	}
} }
