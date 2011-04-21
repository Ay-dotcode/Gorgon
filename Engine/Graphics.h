#pragma once

#include "../Utils/GGE.h"
#include "OS.h"

#define WINGDIAPI	__declspec(dllimport)
#define APIENTRY	__stdcall
#define CALLBACK	__stdcall

#include <gl/gl.h>
#include <gl/glu.h>
#include <assert.h>
#include <stdexcept>
#include "../Utils/ManagedBuffer.h"

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
	enum ColorMode {
		////RGB base color mode
		RGB = 1,
		////Palleted base color mode
		PALLETTED = 2,
		////Gray base color mode
		GRAY = 4,
		////Alpha base color mode
		ALPHA = 8,
		////BGR base color mode
		BGR = 16,
		////4-byte RGB color with alpha
		ARGB_32BPP = RGB | ALPHA,
		////4-byte revered RGB color with alpha
		ABGR_32BPP = BGR | ALPHA,
		////1-byte palleted color mode, is not available yet
		PALLETTED_8BPP = PALLETTED,
		////2-byte palleted color mode with alpha, is not available yet
		APALLETTED_16BPP = PALLETTED | ALPHA,
		////1-byte alpha only color mode, converted to AGRAYSCALE_16BPP
		ALPHAONLY_8BPP = ALPHA,
		////1-byte gray (luminance) only color mode, is not available yet
		GRAYSCALE_8BPP = GRAY,
		////2-byte gray (luminance) color mode with alpha
		AGRAYSCALE_16BPP = GRAY | ALPHA
	};

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
		////Width of the texture
		int TW;
		////Height of the texture
		int TH;

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
			TW=sl2(W);//+0.0001
			TH=sl2(H);//+0.0001
			S=(float)W/TW;
			T=(float)H/TH;

			ImageCoord[0].s=0;
			ImageCoord[0].t=0;
			ImageCoord[1].s=S;
			ImageCoord[1].t=0;
			ImageCoord[2].s=S;
			ImageCoord[2].t=T;
			ImageCoord[3].s=0;
			ImageCoord[3].t=T;
		}
	};

	union RGBfloat {
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

		////Empty constructor
		BasicSurface() {
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
	inline int getBPP(ColorMode color_mode) {
		switch(color_mode) {
		case GRAYSCALE_8BPP:
		case PALLETTED_8BPP:
		case ALPHAONLY_8BPP:
			return 1;
		case APALLETTED_16BPP:
		case AGRAYSCALE_16BPP:
			return 2;
		case RGB:
			return 3;
		default:
			return 4;
		}
	}

	////This interface defines a class that can be used
	/// as a common drawing target
	class I2DGraphicsTarget {
	public:
		////Draws a simple image to the screen.
		/// In this draw function every corner can be specified
		/// thus various transformations can be made
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		///@X1		: top-left corner
		///@Y1		: top-left corner
		///@X2		: top-right corner
		///@Y2		: top-right corner
		///@X3		: bottom-right corner
		///@Y3		: bottom-right corner
		///@X4		: bottom-left corner
		///@Y4		: bottom-left corner
		virtual void Draw(GLTexture *Image,int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4)=0;
		////Draws a simple image to the screen.
		/// This function supports scaling.
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		inline virtual void Draw(GLTexture *Image,int X,int Y,int W,int H) { Draw(Image,X,Y,X+W,Y,X+W,Y+H,X,Y+H); }
		////Draws a simple image to the screen.
		/// This function does not support any transformations
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		inline virtual void Draw(GLTexture *Image,int X,int Y) { Draw(Image,X,Y,X+Image->W,Y,X+Image->W,Y+Image->H,X,Y+Image->H); }
		////Draws a tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		virtual void DrawTiled(GLTexture *Image,int X,int Y,int W,int H)=0;
		////Draws a horizontally tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		virtual void DrawHTiled(GLTexture *Image,int X,int Y,int W,int H)=0;
		////Draws a vertically tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		///@W		: the width of the image to be drawn
		/// if it is more than the size of the image
		virtual void DrawVTiled(GLTexture *Image,int X,int Y,int W,int H)=0;
		////Clears drawing buffer, in layer architecture this request only affects
		/// the layer itself, not sub-layers
		virtual void Clear()=0;

		virtual int Width()  =0;
		virtual int Height() =0;
	};

	////This is a graphics target with colorization support
	class I2DColorizableGraphicsTarget : public I2DGraphicsTarget {
	public:
		////Global ambient color of this layer
		RGBint Ambient;
		////Draws a simple image to the screen.
		/// In this draw function every corner can be specified
		/// thus various transformations can be made
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		///@X1		: top-left corner
		///@Y1		: top-left corner
		///@X2		: top-right corner
		///@Y2		: top-right corner
		///@X3		: bottom-right corner
		///@Y3		: bottom-right corner
		///@X4		: bottom-left corner
		///@Y4		: bottom-left corner
		virtual void Draw(GLTexture *Image,int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4, RGBint Color)=0;
		////Draws a simple image to the screen.
		/// This function supports scaling.
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		inline virtual void Draw(GLTexture *Image,int X,int Y,int W,int H, RGBint Color) { Draw(Image,X,Y,X+W,Y,X+W,Y+H,X,Y+H,Color); }
		////Draws a simple image to the screen.
		/// This function does not support any transformations
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		inline virtual void Draw(GLTexture *Image,int X,int Y, RGBint Color) { Draw(Image,X,Y,X+Image->W,Y,X+Image->W,Y+Image->H,X,Y+Image->H,Color); }
	};

	class I2DRawGraphicsTarget {
	public:
		////Draws a simple image to the screen.
		/// In this draw function every corner can be specified
		/// thus various transformations can be made
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		///@X1		: top-left corner
		///@Y1		: top-left corner
		///@X2		: top-right corner
		///@Y2		: top-right corner
		///@X3		: bottom-right corner
		///@Y3		: bottom-right corner
		///@X4		: bottom-left corner
		///@Y4		: bottom-left corner
		virtual void Draw(Byte *Image, int Width, int Height, ColorMode Mode,int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4)=0;
		////Draws a simple image to the screen.
		/// This function supports scaling.
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		_inline virtual void Draw(Byte *Image, int Width, int Height, ColorMode Mode,int X,int Y,int W,int H) { Draw(Image,Width,Height,Mode,X,Y,X+W,Y,X+W,Y+H,X,Y+H); }
		////Draws a simple image to the screen.
		/// This function does not support any transformations
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		_inline virtual void Draw(Byte *Image, int Width, int Height, ColorMode Mode,int X,int Y) { Draw(Image,Width,Height,Mode,X,Y,X+Width,Y,X+Width,Y+Height,X,Y+Height); }
	};
	////This is a basic drawing object
	class Buffered2DGraphic {
	public:
		GLTexture Texture;

		enum TilingDirection {
			None,
			Horizontal=1,
			Vertical=2,
			Both=3,
		};

		static TilingDirection Tiling(bool H, bool V) {
			if(H) {
				if(V)
					return Both;
				else
					return Horizontal;
			} else {
				if(V)
					return Vertical;
				else
					return None;
			} 
		}

		Buffered2DGraphic() { Texture.ID=0; }

		////Draws this object to the given target
		///@Target	: The target that will be used to draw
		/// this image
		virtual void Draw(I2DGraphicsTarget *Target,Point pnt) { 
			Draw(Target, pnt.x, pnt.y, Texture.W, Texture.H);
		}
		////Draws this object to the given target
		///@Target	: The target that will be used to draw
		/// this image
		virtual void Draw(I2DGraphicsTarget &Target,Point pnt) { 
			Draw(Target, pnt.x, pnt.y, Texture.W, Texture.H);
		}
		virtual void Draw(I2DGraphicsTarget *Target,int X,int Y) { 
			Draw(Target, X, Y, Texture.W, Texture.H);
		}
		virtual void Draw(I2DGraphicsTarget &Target,int X,int Y) { Draw(&Target, X,Y); }


		////Draws this object to the given target with the specified size
		///@Target	: The target that will be used to draw
		/// this image
		virtual void Draw(I2DGraphicsTarget *Target,int X,int Y,int W,int H) { 
#ifdef _DEBUG
			if(Texture.ID==0)
				std::runtime_error("Invalid texture ID, image is not initialized in tiled draw.");
			if(Target==NULL)
				std::runtime_error("Target is NULL");
#endif
			Target->Draw(&Texture, X, Y, W, H);
		}
		virtual void Draw(I2DGraphicsTarget &Target,int X,int Y,int W,int H) { Draw(&Target, X,Y, W,H); }


		////Draws this object to the given target with the specified size
		///@Target	: The target that will be used to draw
		/// this image
		virtual void Draw(I2DGraphicsTarget *Target,int X1,int Y1, int X2,int Y2, int X3,int Y3, int X4, int Y4) { 
#ifdef _DEBUG
			if(Texture.ID==0)
				std::runtime_error("Invalid texture ID, image is not initialized in tiled draw.");
			if(Target==NULL)
				std::runtime_error("Target is NULL");
#endif
			Target->Draw(&Texture, X1,Y1, X2,Y2, X3,Y3, X4,Y4);
		}
		virtual void Draw(I2DGraphicsTarget &Target,int X1,int Y1, int X2,int Y2, int X3,int Y3, int X4, int Y4) { Draw(&Target, X1,Y1, X2,Y2, X3,Y3, X4,Y4); }


		virtual void DrawTiled(I2DGraphicsTarget *Target,int X,int Y, int W, int H) { 
#ifdef _DEBUG
			if(Texture.ID==0)
				std::runtime_error("Invalid texture ID, image is not initialized in tiled draw.");
			if(Target==NULL)
				std::runtime_error("Target is NULL");
#endif
			Target->DrawTiled(&Texture,X,Y,W,H);
		}
		virtual void DrawTiled(I2DGraphicsTarget &Target,int X,int Y, int W, int H) { DrawTiled(&Target, X,Y, W,H); }


		virtual void DrawResized(TilingDirection Tiling, I2DGraphicsTarget *Target,int X,int Y, int W, int H) { 
#ifdef _DEBUG
			if(Texture.ID==0)
				std::runtime_error("Invalid texture ID, image is not initialized in tiled draw.");
			if(Target==NULL)
				std::runtime_error("Target is NULL");
#endif
			switch(Tiling) {
				case None:
					Target->Draw(&Texture,X,Y,W,H);
					break;
				case Horizontal:
					Target->DrawHTiled(&Texture,X,Y,W,H);
					break;
				case Vertical:
					Target->DrawVTiled(&Texture,X,Y,W,H);
					break;
				case Both:
					Target->DrawTiled(&Texture,X,Y,W,H);
					break;
			}
		}
		virtual void DrawResized(TilingDirection Tiling, I2DGraphicsTarget &Target,int X,int Y, int W, int H) { DrawResized(Tiling, &Target, X,Y, W,H); }


		virtual void DrawHTiled(I2DGraphicsTarget *Target,int X,int Y, int W, int H) { 
#ifdef _DEBUG
			if(Texture.ID==0)
				std::runtime_error("Invalid texture ID, image is not initialized in tiled draw.");
			if(Target==NULL)
				std::runtime_error("Target is NULL");
#endif
			Target->DrawHTiled(&Texture,X,Y,W,H);
		}
		virtual void DrawHTiled(I2DGraphicsTarget &Target,int X,int Y, int W, int H) { DrawHTiled(&Target, X,Y, W,H); }


		virtual void DrawVTiled(I2DGraphicsTarget *Target,int X,int Y, int W, int H) { 
#ifdef _DEBUG
			if(Texture.ID==0)
				std::runtime_error("Invalid texture ID, image is not initialized in tiled draw.");
			if(Target==NULL)
				std::runtime_error("Target is NULL");
#endif
			Target->DrawVTiled(&Texture,X,Y,W,H);
		}
		virtual void DrawVTiled(I2DGraphicsTarget &Target,int X,int Y, int W, int H) { DrawVTiled(Target, X,Y, W,H); }
	};


	////This is a graphics object with colorization support
	class Colorizable2DGraphic : public Buffered2DGraphic {
	public:

		virtual void DrawColored(I2DColorizableGraphicsTarget *Target,int X,int Y,RGBint Color) { 
#ifdef _DEBUG
			if(Texture.ID==0) {
				os::DisplayMessage("Colorizable 2D Graphic","Invalid texture ID, image is not initialized.");
				assert(0);
			}
#endif
			Target->Draw(&Texture, X, Y, Color);
		}
		virtual void DrawColored(I2DColorizableGraphicsTarget &Target,int X,int Y,RGBint Color) { DrawColored(&Target, X,Y, Color); }

		inline void DrawColored(I2DColorizableGraphicsTarget *Target,int X,int Y,unsigned int Color) { DrawColored(Target,X,Y,ToRGBint(Color)); }
	};

	////This is a basic drawing object
	class Raw2DGraphic {
	public:
		ManagedBuffer<Byte> Data;
	protected:
		int Width;
		int Height;
		ColorMode Mode;

	public:
		Raw2DGraphic() { Width=0; Height=0; Mode=ARGB_32BPP; }
		Raw2DGraphic(Raw2DGraphic &graph) {
			Data=graph.Data;
			Width=graph.Width;
			Height=graph.Height;
			Mode=graph.Mode;
			
			Data++;
		}

		////Draws this object to the given target
		///@Target	: The target that will be used to draw
		/// this image
		virtual void DrawRaw(I2DRawGraphicsTarget *Target,int X,int Y) { 
#ifdef _DEBUG
			if(Data.GetSize()==0) {
				os::DisplayMessage("Raw 2D Graphic","Empty data, image is not initialized.");
				assert(0);
			}
#endif
			Target->Draw(Data, Width, Height, Mode, X, Y);
		}

		virtual void DrawRaw(I2DRawGraphicsTarget *Target,int X,int Y, int W, int H) { 
#ifdef _DEBUG
			if(Data==NULL) {
				os::DisplayMessage("Raw 2D Graphic","Empty data, image is not initialized.");
				assert(0);
			}
#endif
			Target->Draw(Data, Width, Height, Mode, X, Y, W, H);
		}

		virtual void DrawRaw(I2DRawGraphicsTarget *Target,int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4) {
#ifdef _DEBUG
			if(Data==NULL) {
				os::DisplayMessage("Raw 2D Graphic","Empty data, image is not initialized.");
				assert(0);
			}
#endif
			Target->Draw(Data, Width, Height, Mode, X1, Y1, X2, Y2, X3, Y3, X4, Y4);
		}

		virtual void DrawRawFlipped(I2DRawGraphicsTarget *Target,int X,int Y) { 
#ifdef _DEBUG
			if(Data==NULL) {
				os::DisplayMessage("Raw 2D Graphic","Empty data, image is not initialized.");
				assert(0);
			}
#endif
			Target->Draw(Data, Width, Height, Mode, X, Y+Height, X+Width, Y+Height, X+Width, Y, X, Y);
		}

		virtual void DrawRawFlipped(I2DRawGraphicsTarget *Target,int X,int Y, int W, int H) { 
#ifdef _DEBUG
			if(Data==NULL) {
				os::DisplayMessage("Raw 2D Graphic","Empty data, image is not initialized.");
				assert(0);
			}
#endif
			Target->Draw(Data, Width, Height, Mode, X, Y+H, X+W, Y+H, X+W, Y, X, Y);
		}

		virtual void Resize(int Width, int Height, ColorMode Mode) {
			int bits=getBPP(Mode);

			Data.Resize(Width*Height*bits);

			this->Width=Width;
			this->Height=Height;
			this->Mode=Mode;
		}

		~Raw2DGraphic() {  }
		int getWidth() { return Width; }
		int getHeight() { return Height; }
		ColorMode getMode() { return Mode; }
	};



	////Finds integer log of base 2
	inline int log2(int num);
	////Finds nearest 2^n value that is larger than num
	inline int sl2(int num);
	////Initializes OpenGL graphics with the given parameters,
	/// returns created device context
	///@hWnd		: Handle for the target window
	///@BitDepth	: Used if fullscreen, changed bitdepth of screen
	os::DeviceHandle Initialize(os::WindowHandle hWnd, int BitDepth, int Width, int Height);
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
	void SetTexture(Byte *data, int cx, int cy, ColorMode mode);
	////This function creates a texture from the given data and
	/// returns texture information in a structure
	///@Image		: Image data
	///@Mode		: Color mode
	GLTexture GenerateTexture(Byte *Image,int Width,int Height,ColorMode Mode);
	void DestroyTexture(GLTexture *texture);
	////Returns equivalent OpenGL color mode constant
	///@Mode		: GGE color mode constant
	GLenum getGLColorMode(ColorMode Mode);
	////Returns required bytes per pixel for a given color mode
	int getBPP(ColorMode Mode);
	////Cleans render buffer and prepares for rendering
	void PreRender();
	////Performs post render tasks
	///@hDC			: Device context that is created by
	/// initialize graphics function
	void PostRender(os::DeviceHandle Device);

	extern Point ScreenSize;
} }
