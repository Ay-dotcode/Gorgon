#pragma once

#include "GGE.h"
#include "OS.h"

#define WINGDIAPI	__declspec(dllimport)
#define APIENTRY	__stdcall
#define CALLBACK	__stdcall

#include <gl/gl.h>
#include <gl/glu.h>
#include <assert.h>
#include <stdexcept>
#include <string>

#include "../Utils/ManagedBuffer.h"
#include "../Utils/Point2D.h"
#include "../Utils/Bounds2D.h"
#include "../Utils/Rectangle2D.h"
#include "../Utils/Size2D.h"

#undef WINGDIAPI
#undef APIENTRY
#undef CALLBACK

#ifndef GL_BGR
#	define GL_BGR	0x80E0
#	define GL_BGRA	0x80E1
#endif

#ifndef PI
#	define PI	3.1415926535898f
#endif


////Namespace for Gorgon Game Engine
namespace gge { namespace graphics {
	union TexturePosition { struct{float s,t;}; float vect[2];};
	union VertexPosition { struct{float x,y,z;};float vect[3];};

	////GGE Color mode constants
	namespace ColorMode {
		enum Type {
			////RGB base color mode
			RGB = 1,
			////Palleted base color mode
			Palleted = 2,
			////Gray base color mode
			Grayscale = 4,
			////Alpha base color mode
			Alpha = 8,
			////BGR base color mode
			BGR = 16,
			////4-byte RGB color with alpha
			ARGB = RGB | Alpha,
			////4-byte revered RGB color with alpha
			ABGR = BGR | Alpha,
			////2-byte palleted color mode with alpha, is not available yet
			Palleted_Alpha = Palleted | Alpha,
			////2-byte gray (luminance) color mode with alpha
			Grayscale_Alpha = Grayscale | Alpha
		};
	};

	namespace system {
		int sl2(int num);
		int log2(int num);
	}

	////This structure contains all necessary information
	/// of a texture. Using this information, image can be
	/// drawn without requiring its actual size.
	/// Also contains data to increase performance
	struct GLTexture {
		////OpenGL texture id
		GLuint ID;
		////Width of the image (not texture's)
		int W;
		////Height of the image (not texture's)
		int H;

		////S (texture x) position for the width of the image
		float S;
		////T position (texture y) position for the height of
		/// the image
		float T;

		////Readily calculated texture coordinates of the image
		TexturePosition ImageCoord[4];

		////Calculates the necessary coordinates from
		/// the give image size
		void CalcuateCoordinates(int W,int H) {
			this->W=W;
			this->H=H;

			ImageCoord[0].s=0;
			ImageCoord[0].t=0;
			ImageCoord[1].s=1;
			ImageCoord[1].t=0;
			ImageCoord[2].s=1;
			ImageCoord[2].t=1;
			ImageCoord[3].s=0;
			ImageCoord[3].t=1;
		}
	};

	struct RGBint;

	union RGBfloat {
		RGBfloat(const RGBint &);
		RGBfloat() { }
		RGBfloat(float a, float r, float g, float b) : a(a), r(r), g(g), b(b) { }

		void Blend(RGBfloat color) {
			float alpha=color.a;
			float alpham1=1-color.a;
			
			a+=color.a;
			if(a>1) a=1;
			r=r*alpham1+color.r*alpha;
			g=g*alpham1+color.g*alpha;
			b=b*alpham1+color.b*alpha;
		}
		
		struct {
			////Color element
			float b,g,r;
			////Alpha value
			float a;
		};

		bool operator ==(const RGBfloat &c) const {
			return a==c.a && r==c.r && g==c.g && b==c.b;
		}

		RGBfloat &operator =(const RGBint &c);

		float vect[4];
	};

	////A structure to extend standard argb integer
	/// This structure helps with conversions
	struct RGBint {
		////Color element
		unsigned char b,g,r;
		////Alpha value
		unsigned char a;

		RGBint(unsigned int i) { memcpy(this,&i,4); }
		RGBint(int i) { memcpy(this,&i,4); }
		RGBint(Byte a, Byte r, Byte g, Byte b) : a(a), r(r), g(g), b(b) { }
		RGBint(RGBfloat f) { a=(Byte)(f.a*255); r=(Byte)(f.r*255); g=(Byte)(f.g*255); b=(Byte)(f.b*255); }
		RGBint(float lum) { a=255; r=(Byte)lum*255; g=(Byte)lum*255; b=(Byte)lum*255; }
		RGBint() { }
		
		////Converts RGBint structure to ARGB integer
		operator int () { return *(int*)this; }
		////Converts RGBint structure to ARGB (unsigned) integer
		operator unsigned int () { return *(unsigned int*)this; }
		RGBint &operator =(unsigned int i) { 
			memcpy(this,&i,4);
			return *this;
		}
		RGBint &operator =(int i) { 
			memcpy(this,&i,4);
			return *this;
		}
		RGBint &operator =(float lum) { 
			a=255; r=(Byte)(lum*255); g=(Byte)(lum*255); b=(Byte)(lum*255);
			return *this;
		}

		RGBint &operator +=(RGBint color) {
			r+=color.r;
			g+=color.g;
			b+=color.b;

			return *this;
		}

		RGBint operator +(RGBint color) {
			RGBint c;

			c.r=r+color.r;
			c.g=g+color.g;
			c.b=b+color.b;

			return c;
		}

		RGBint &operator -=(RGBint color) {
			r=(r-color.r)/2 + 127;
			g=(r-color.g)/2 + 127;
			b=(r-color.b)/2 + 127;

			return *this;
		}

		RGBint operator -(RGBint color) {
			RGBint c;

			c.r=(r-color.r)/2 + 127;
			c.g=(r-color.g)/2 + 127;
			c.b=(r-color.b)/2 + 127;

			return c;
		}
		
		RGBint &operator *=(float c) {
			r=(Byte)(r*c);
			g=(Byte)(g*c);
			b=(Byte)(b*c);

			return *this;
		}
		
		RGBint operator *(float c) {
			RGBint color;
			
			color.r=(Byte)(r*c);
			color.g=(Byte)(g*c);
			color.b=(Byte)(b*c);

			return color;
		}
		
		RGBint &operator /=(float c) {
			r=(Byte)(r/c);
			g=(Byte)(g/c);
			b=(Byte)(b/c);

			return *this;
		}
		
		RGBint operator /(float c) {
			RGBint color;
			
			color.r=(Byte)(r/c);
			color.g=(Byte)(g/c);
			color.b=(Byte)(b/c);

			return color;
		}


		operator RGBfloat() {
			RGBfloat f;
			f.a=(float)a/255;
			f.r=(float)r/255;
			f.g=(float)g/255;
			f.b=(float)b/255;

			return f;
		}
		int Luminance() { 
			return (r>>3) + (r>>4) + (r>>5) + (g>>1) + (g>>3) + (g>>4) + (g>>5) + (b>>4); 
		}
		float AccurateLuminance() { 
			return  0.2126f*r + 0.7151f*g + 0.0722f*b; 
		}

		operator float() { return AccurateLuminance()/255.0f; }

		string HTMLColor() {
			std::stringstream str2;
			std::stringstream str;
			str2<<std::hex<<((int)(*this)&0x00ffffff);

			str<<"#"<<std::fixed<<std::setw(6)<<std::setfill('0')<<str2.str();

			return str.str();
		}

		void Blend(RGBint color) {
			if(color.a==255) {
				a=255;
				r=color.r;
				g=color.g;
				b=color.b;
			} else {
				float alpha  =(float)color.a/255;
				float alpham1=(float)(255-color.a)/255;

				if(a<255) {
					int aa=(int)a+color.a;
					if(aa>255)
						a=255;
					else
						a=aa;
				}

				r=(Byte)(r*alpham1+color.r*alpha);
				g=(Byte)(g*alpham1+color.g*alpha);
				b=(Byte)(b*alpham1+color.b*alpha);
			}
		}

		operator string() {
			return HTMLColor();
		}
		////Returns BGRA integer
		int operator !() { return (r<<16)+(g<<8)+b+(a<<24); }
	};

	inline std::ostream &operator <<(std::ostream &stream, RGBint color) {
		stream<<(string)color;

		return stream;
	}

	inline RGBfloat::RGBfloat(const RGBint &c) {
		a=(float)c.a/255;
		r=(float)c.r/255;
		g=(float)c.g/255;
		b=(float)c.b/255;
	}

	inline RGBfloat &RGBfloat::operator =(const RGBint &c) {
		a=(float)c.a/255;
		r=(float)c.r/255;
		g=(float)c.g/255;
		b=(float)c.b/255;

		return *this;
	}

	////Color range definition from one color
	/// to another, this also includes alpha
	struct RGBRangeint {
		////Starting value
		RGBint from;
		////Ending value
		RGBint to;
	};

	////This class can be used to store basic surface information.
	/// It has also support for modifiable texture coordinates.
	class BasicSurface {
	public:
		////Coordinates of vertices
		VertexPosition VertexCoords[4];
		////Coordinates of texture rectangle
		TexturePosition *TextureCoords;
		////Whether this surface has its own texture coordinates
		bool hasOwnTextureCoords;

		enum DrawMode {
			Normal,
			AlphaOnly,
			UseDestinationAlpha
		} Mode;

		////Empty constructor
		BasicSurface() : Mode(Normal) {
			Texture=NULL;
			hasOwnTextureCoords=false;
			VertexCoords[0].z=0;
			VertexCoords[1].z=0;
			VertexCoords[2].z=0;
			VertexCoords[3].z=0;
		}

		////Filling constructor that sets texture
		BasicSurface(GLTexture *texture) {
			this->Texture=texture;
			TextureCoords=texture->ImageCoord;

			hasOwnTextureCoords=false;
		}

		////Changes current texture to the given one
		void setTexture(GLTexture *texture) {
			DeleteTextureCoords();

			this->Texture=texture;
			TextureCoords=texture->ImageCoord;
		}

		////Returns current texture
		GLTexture *getTexture() {
			return Texture;
		}

		////Deletes texture coordinate information
		__forceinline void DeleteTextureCoords() {
			if(hasOwnTextureCoords) {
				hasOwnTextureCoords=false;
				delete TextureCoords;
			}
		}

		////Creates texture coordinate information,
		/// should be called before modifying texture
		/// coordinates
		__forceinline void CreateTextureCoords() {
			if(!hasOwnTextureCoords) {
				hasOwnTextureCoords=true;
				TextureCoords=new TexturePosition[4];
			}
		}

		////Clears any unneeded data
		~BasicSurface() {
			if(hasOwnTextureCoords)
				delete TextureCoords;
		}
	protected:
		////The texture to be used
		GLTexture *Texture;
	};

	////this structure is used to ease conversions
	/// between RGB integer and float values used
	/// in graphic adapters.
	////Converts RGBint to RGBfloat
	inline RGBfloat ToRGBfloat(RGBint color) { RGBfloat f; f.a=(float)color.a/255; f.r=(float)color.r/255; f.g=(float)color.g/255; f.b=(float)color.b/255; return f; }

	////Converts an int color to RGBint structure
	inline RGBint ToRGBint(unsigned int argb) { RGBint r(argb); return r; }
	////Converts an int color to RGBint structure
	inline RGBint ToRGBint(int argb) { RGBint r(argb); return r; }

	////Returns the Bytes required for a given color mode
	inline int getBPP(ColorMode::Type color_mode) {
		switch(color_mode) {
		case ColorMode::Grayscale:
		case ColorMode::Alpha:
		case ColorMode::Palleted:
			return 1;
		case ColorMode::Palleted_Alpha:
		case ColorMode::Grayscale_Alpha:
			return 2;
		case ColorMode::RGB:
		case ColorMode::BGR:
			return 3;
		default:
			return 4;
		}
	}

	class Tiling2D {
	public:
		enum Type {
			None		= 0,
			Horizontal	= 1, 
			Vertical	= 2,
			Both		= 3,
		};

		static Type Tile(bool H, bool V) {
			return (H ?
				(V ? Both     : Horizontal) :
				(V ? Vertical : None)
			);
		}
	};

	class SizeController2D {
	public:
		SizeController2D() : HorizontalTiling(Single), VerticalTiling(Single), Align(Alignment::Middle_Center)
		{ }

		enum TilingType {
			//Should work as Continous if object is sizable
			Single					= B8(00000000),
			//Should work as Continous if object is sizable
			Stretch					= B8(00000010),

			/*
			Integral	= B8(00000100),
			Smaller		= B8(00001000),
			Closest		= B8(00010000),
			Best		= B8(00100000),
			*/

			//An object can either be sizable or tilable, 
			// Careful! there is code to be updated if 
			// integrals and tiles are changed to be different
			Continous			= B8(00000001),
			Integral_Smaller	= B8(00001101),
			Integral_Fill		= B8(00010101),
			Integral_Best		= B8(00100101),

			Tile_Continous			= B8(00000001),
			Tile_Integral_Smaller	= B8(00001101),
			Tile_Integral_Fill		= B8(00010101),
			Tile_Integral_Best		= B8(00100101),
		} HorizontalTiling, VerticalTiling;

		Alignment::Type Align;

		static const SizeController2D TileFit;
		static const SizeController2D StretchFit;

		int CalculateWidth(int W, int Increment) const {
			if(HorizontalTiling==SizeController2D::Tile_Integral_Best) {
				return (int)utils::Round((double)W/Increment)*Increment;
			}
			else if(HorizontalTiling==SizeController2D::Tile_Integral_Smaller) {
				return (int)std::floor((double)W/Increment)*Increment;
			}
			else if(HorizontalTiling==SizeController2D::Tile_Integral_Fill) {
				return (int)std::ceil((double)W/Increment)*Increment;
			}
			else
				return W;
		}

		int CalculateWidth(int W, int Increment, int Overhead) const {
			if(HorizontalTiling==SizeController2D::Integral_Best) {
				return (int)utils::Round((double)(W-Overhead)/Increment)*Increment+Overhead;
			}
			else if(HorizontalTiling==SizeController2D::Integral_Smaller) {
				return (int)std::floor((double)(W-Overhead)/Increment)*Increment+Overhead;
			}
			else if(HorizontalTiling==SizeController2D::Integral_Fill) {
				return (int)std::ceil((double)(W-Overhead)/Increment)*Increment+Overhead;
			}
			else
				return W;
		}

		int CalculateHeight(int H, int Increment) const {
			if(VerticalTiling==SizeController2D::Tile_Integral_Best) {
				return (int)utils::Round((double)H/Increment)*Increment;
			}
			else if(VerticalTiling==SizeController2D::Tile_Integral_Smaller) {
				return (int)std::floor((double)H/Increment)*Increment;
			}
			else if(VerticalTiling==SizeController2D::Tile_Integral_Fill) {
				return (int)std::ceil((double)H/Increment)*Increment;
			}
			else
				return H;
		}

		int CalculateHeight(int H, int Increment, int Overhead) const {
			if(VerticalTiling==SizeController2D::Integral_Best) {
				return (int)utils::Round((double)(H-Overhead)/Increment)*Increment+Overhead;
			}
			else if(VerticalTiling==SizeController2D::Integral_Smaller) {
				return (int)std::floor((double)(H-Overhead)/Increment)*Increment+Overhead;
			}
			else if(VerticalTiling==SizeController2D::Integral_Fill) {
				return (int)std::ceil((double)(H-Overhead)/Increment)*Increment+Overhead;
			}
			else
				return H;
		}
	};

	

	////Initializes OpenGL graphics with the given parameters,
	/// returns created device context
	///@hWnd		: Handle for the target window
	///@BitDepth	: Used if fullscreen, changed bitdepth of screen
	os::DeviceHandle Initialize(os::WindowHandle hWnd, int BitDepth, int Width, int Height);

	namespace system {

		////Logarithm Base 2
		inline int log2(int num) {
			int i=0;
			int s=1;
			while(num-s>0) {
				i++;
				s<<=1;
			}

			return i;
		}

		////Rounds the given number to the lowest 2^n (where n is integer) number 
		/// that is higher than the given number
		inline int sl2(int num) {
			int s=1;
			while(num-s>0) {
				s<<=1;
			}

			return s;
		}
		////Creates rectangle structure based on give parameters
		//RECT makerect(int x, int y, int w, int h);
		////Converts Alpha only image to Alpha and Luminance (grayscale) image.
		/// Destination array should be allocated. This algorithm is optimized
		/// for speed.
		///@Source		: Source data array, no justification is needed,
		///               should be 1 byte/pix
		///@Destination	: Destination data array, data is copied without
		///               any justification, should be allocated for 2 byte/pix
		void A8ToA8L8(int Width, int Height, Byte *Source, Byte *Destination);
		////This function sets the current texture to given data
		void SetTexture(Byte *data, int cx, int cy, ColorMode::Type mode);
		////This function creates a texture from the given data and
		/// returns texture information in a structure
		///@Image		: Image data
		///@Mode		: Color mode
		GLTexture GenerateTexture(Byte *Image,int Width,int Height,ColorMode::Type Mode);
		void DestroyTexture(GLTexture *texture);
		////Returns equivalent OpenGL color mode constant
		///@Mode		: GGE color mode constant
		GLenum getGLColorMode(ColorMode::Type Mode);
		////Cleans render buffer and prepares for rendering
		void PreRender();
		////Performs post render tasks
		///@hDC			: Device context that is created by
		/// initialize graphics function
		void PostRender(os::DeviceHandle Device);
	}

	extern utils::Size ScreenSize;
} }
