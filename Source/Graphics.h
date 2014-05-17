#pragma once

#include <stdint.h>

#include "GL.h"
#include "Geometry/Point.h"
#include "Geometry/Size.h"
#include "Geometry/Bounds.h"

namespace Gorgon {


	/// Contains generic graphics related data structures and functions
	namespace Graphics {

		/// This class represents an image depends on a GL texture.
		class TextureImage {
		public:

			/// Default constructor, creates an empty texture
			TextureImage() {
				Set(0, {0, 0});
			}

			/// Regular, full texture constructor
			TextureImage(GL::Texture id, const Geometry::Size &size) {
				Set(id, size);
			}

			/// Atlas constructor, specifies a region of the texture
			TextureImage(GL::Texture id, const Geometry::Size &size, const Geometry::Bounds &location) {
				Set(id, size, location);
			}

			/// Sets the texture to the given ID with the given size. Resets the coordinates to cover entire
			/// GL texture
			void Set(GL::Texture id, const Geometry::Size &size) {
				ID  =id;
				Size=size;

				Coordinates[0] ={0.f, 0.f};
				Coordinates[1] ={1.f, 0.f};
				Coordinates[2] ={1.f, 1.f};
				Coordinates[3] ={0.f, 1.f};
			}

			/// Sets the texture to the given ID with the given size. Calculates the texture coordinates
			/// for the specified location in pixels
			/// @param   id ID of the texture, reported by the underlying GL framework
			/// @param   size of the GL texture in pixels
			/// @param   location is the location of this texture over GL texture in pixels.
			void Set(GL::Texture id, const Geometry::Size &size, const Geometry::Bounds &location) {
				ID  =id;
				Size=size;

				Coordinates[0] ={float(location.Left )/size.Width, float(location.Top   )/size.Height};
				Coordinates[1] ={float(location.Right)/size.Width, float(location.Top   )/size.Height};
				Coordinates[2] ={float(location.Right)/size.Width, float(location.Bottom)/size.Height};
				Coordinates[3] ={float(location.Left )/size.Width, float(location.Bottom)/size.Height};
			}

			/// GL texture id
			GL::Texture ID;

			/// Size of the texture
			Geometry::Size Size;

			/// Readily calculated texture coordinates of the image. Normally spans entire
			/// GL texture, however, could be changed to create texture atlas. These coordinates
			/// are kept in floating point u,v representation for quick consumption by the GL
			Geometry::Pointf Coordinates[4];
		};

		/// Details which directions a texture should tile. If its not tiled for that direction, it will
		/// be stretched. If the target size is smaller, tiling causes partial draw instead of shrinking.
		enum class Tiling {
			None		= 0,
			Horizontal	= 1,
			Vertical	= 2,
			Both		= 3,
		};

		/// Creates a Tiling class from the given horizontal, vertical tiling info.
		static Tiling Tile(bool hor, bool vert) {
			return (hor ?
				(vert ? Tiling::Both     : Tiling::Horizontal) :
				(vert ? Tiling::Vertical : Tiling::None)
			);
		}

	}
}



#ifdef fnonona
#pragma once

#include "GGE.h"
#include "OS.h"

#include <GL/gl.h>
#include <assert.h>
#include <stdexcept>
#include <string>
#include <array>

#ifdef WIN32
#	undef APIENTRY
#	undef WINGDIAPI
#endif

#include "../Utils/ManagedBuffer.h"
#include "../Utils/Point2D.h"
#include "../Utils/Bounds2D.h"
#include "../Utils/Rectangle2D.h"
#include "../Utils/Size2D.h"
#include "../Utils/Logging.h"
#include "External/glutil/MatrixStack.h"

#ifndef GL_BGR
#	define GL_BGR	0x80E0
#	define GL_BGRA	0x80E1
#endif

#ifndef PI
#	define PI	3.1415926535898f
#endif


////Namespace for Gorgon Game Engine
namespace gge { namespace graphics {

	extern gge::utils::Logger				log;

	extern glutil::MatrixStack				ModelViewMatrixStack;
	extern glutil::MatrixStack				ProjectionMatrixStack;

	class UnitQuad
	{
	public:								
		virtual								~UnitQuad();
		static UnitQuad&					Get() { static UnitQuad me; return me; }
		static void							Draw() { Get().GLDraw(); }		
	protected:
											UnitQuad();
		void								GLDraw();		
		static int							unit_quad[6];
		GLuint								vbo;
		GLuint								vao;

	private:
											UnitQuad(const UnitQuad &);
		UnitQuad&							operator=(const UnitQuad &);
	};

	class Quad : public UnitQuad
	{
	public:	
		static Quad&						Get() { static Quad me; return me; }
		static void							Draw() { Quad::Get().GLDraw(); }
		static void							UpdateVertexData(const std::array<float,24>& data) { Quad::Get().UpdateInstanceVertexData(data); }
	protected:
		void								GLDraw();
		void								UpdateInstanceVertexData(const std::array<float,24>& data);
	};	

	union TexturePosition { struct{float s,t;}; float vect[2];};
	union VertexPosition { struct{float x,y,z;};float vect[3];};


	namespace system {
		int sl2(int num);
		int log2(int num);
	}


	////Color range definition from one color
	/// to another, this also includes alpha
	struct RGBRangeint {
		////Starting value
		RGBint from;
		////Ending value
		RGBint to;
	};
	
	class TextureAttachment {
	public:
		////Coordinates of texture rectangle
		TexturePosition *TextureCoords;
		////Whether this surface has its own texture coordinates
		bool HasOwnTextureCoords;
		const GLTexture *Texture;
		
		TextureAttachment() : HasOwnTextureCoords(false), TextureCoords(NULL), Texture(NULL) {}

		////Deletes texture coordinate information
		void DeleteTextureCoords() {
			if(HasOwnTextureCoords) {
				HasOwnTextureCoords=false;
				delete[] TextureCoords;
			}
		}

		////Changes current texture to the given one
		void SetTexture(const GLTexture *texture) {
			DeleteTextureCoords();

			this->Texture=texture;
			TextureCoords=const_cast<TexturePosition*>((const TexturePosition*)texture->ImageCoord);
		}

		////Returns current texture
		const GLTexture *GetTexture() const {
			return Texture;
		}

		////Creates texture coordinate information,
		/// should be called before modifying texture
		/// coordinates
		void CreateTextureCoords() {
			if(!HasOwnTextureCoords) {
				HasOwnTextureCoords=true;
				TextureCoords=new TexturePosition[4];
			}
		}
		
		~TextureAttachment() {
			DeleteTextureCoords();
		}
	};

	////This class can be used to store basic surface information.
	/// It has also support for modifiable texture coordinates.
	class BasicSurface {
	public:
		////Coordinates of vertices
		VertexPosition VertexCoords[4];
		////Coordinates of texture rectangle, prefer not to modify manually
		TexturePosition *TextureCoords;
		////Whether this surface has its own texture coordinates
		bool HasOwnTextureCoords;

		enum DrawMode {
			Normal,
			OffscreenAlphaOnly,
			Offscreen,
			SetAlpha
		} Mode;

		////Empty constructor
		BasicSurface() : Mode(Normal) {
			Texture=NULL;
			Alpha=NULL;
			HasOwnTextureCoords=false;
			TextureCoords=NULL;
			VertexCoords[0].z=0;
			VertexCoords[1].z=0;
			VertexCoords[2].z=0;
			VertexCoords[3].z=0;
		}

		////Filling constructor that sets texture
		BasicSurface(GLTexture *texture) {
			this->Texture=texture;
			TextureCoords=texture->ImageCoord;

			HasOwnTextureCoords=false;
		}

		////Deletes texture coordinate information
		void DeleteTextureCoords() {
			if(HasOwnTextureCoords) {
				HasOwnTextureCoords=false;
				delete[] TextureCoords;
			}
		}

		////Changes current texture to the given one
		void SetTexture(const GLTexture *texture) {
			DeleteTextureCoords();

			this->Texture=texture;
			TextureCoords=const_cast<TexturePosition*>((const TexturePosition*)texture->ImageCoord);
			
			if(Alpha) {
				delete Alpha;
				Alpha=NULL;
			}
		}

		////Returns current texture
		const GLTexture *GetTexture() const {
			return Texture;
		}

		////Creates texture coordinate information,
		/// should be called before modifying texture
		/// coordinates
		void CreateTextureCoords() {
			if(!HasOwnTextureCoords) {
				HasOwnTextureCoords=true;
				TextureCoords=new TexturePosition[4];
			}
		}

		////Clears any unneeded data
		~BasicSurface() {
			if(HasOwnTextureCoords && TextureCoords)
				delete[] TextureCoords;
		}
		
		TextureAttachment *Alpha;
		
	protected:
		////The texture to be used
		const GLTexture *Texture;
	};

	std::array<float,24>					GetVertexData(const BasicSurface& surface);



	class SizeController2D {
	public:

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


		explicit SizeController2D(TilingType HorizontalTiling=Single, TilingType VerticalTiling=Single, Alignment::Type Align=Alignment::Middle_Center) : 
			HorizontalTiling(HorizontalTiling), VerticalTiling(VerticalTiling), Align(Align)
		{ }
		Alignment::Type Align;

		static const SizeController2D TileFit;
		static const SizeController2D StretchFit;
		static const SizeController2D SingleTopLeft;
		static const SizeController2D SingleBottomRight;
		static const SizeController2D SingleMiddleCenter;

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
		void UpdateTexture(GLTexture TextureID, Byte *Image,ColorMode::Type Mode);
		void DestroyTexture(GLTexture &texture);
		////Returns equivalent OpenGL color mode constant
		///@Mode		: GGE color mode constant
		GLenum getGLColorMode(ColorMode::Type Mode);
		////Cleans render buffer and prepares for rendering
		void PreRender();
		////Performs post render tasks
		///@hDC			: Device context that is created by
		/// initialize graphics function
		void PostRender(os::DeviceHandle Device, os::WindowHandle win);

		void ResizeGL(int Width, int Height);
	}

	extern utils::Size ScreenSize;
} }

#endif