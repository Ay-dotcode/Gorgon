#pragma once

#include <stdint.h>

#include "GL.h"
#include "Geometry/Point.h"
#include "Geometry/Size.h"
#include "Geometry/Bounds.h"
#include "Geometry/Rectangle.h"

namespace Gorgon {


	/// Contains generic 2D graphics related data structures and functions. These functions are tied to
	/// underlying GL system through textures.
	namespace Graphics {

		/// Details which directions a texture should tile. If its not tiled for that direction, it will
		/// be stretched. If the target size is smaller, tiling causes partial draw instead of shrinking.
		/// @todo Should be fitted to String::Enum
		enum class Tiling {
			None		= 0,
			Horizontal	= 1,
			Vertical	= 2,
			Both		= 3,
		};

		/// Creates a Tiling class from the given horizontal, vertical tiling info.
		inline Tiling Tile(bool hor, bool vert) {
			return (hor ?
				(vert ? Tiling::Both     : Tiling::Horizontal) :
				(vert ? Tiling::Vertical : Tiling::None)
			);
		}

		/// Prints tile
		std::ostream &operator <<(std::ostream &out, const Tiling &tile) { 
			switch(tile) {
			case Tiling::None:
				out<<"None";
				break;
			case Tiling::Horizontal:
				out<<"Horizontal";
				break;
			case Tiling::Vertical:
				out<<"Vertical";
				break;
			case Tiling::Both:
				out<<"Both";
				break;
#ifndef NDEBUG
			default:
				throw std::runtime_error("Unknown mode");
				break;
#endif
			}

			return out;
		}

		/// Defines how an object is aligned
		enum class Alignment {
			/// Placed at the start of the axis
			Start		= 1,

			/// Centered along the axis
			Center		= 4,

			/// Placed at the end of the axis
			End			= 2,
		};


		/// Defines how a text is aligned. Justification should be used as an independent
		/// flag as a text could both be justified and centered (for partial lines).
		enum class TextAlignment {
			/// Text is aligned to left
			Left		= 8,

			/// Text is aligned to center
			Center		= 32,

			/// Text is aligned to right
			Right		= 16,
		};

		/// Defines how an object is placed in a 2D axis system
		enum class Placement {
			/// Placed at top left
			TopLeft			=  9,

			/// Placed at top center
			TopCenter		= 33,

			/// Placed at top right
			TopRight		= 17,


			/// Placed at middle left
			MiddleLeft		= 12,

			/// Placed at the center
			MiddleCenter	= 36,

			/// Placed at middle right
			MiddleRight		= 20,

							  
			/// Placed at bottom
			BottomLeft		= 10,

			/// Placed at bottom center
			BottomCenter	= 34,

			/// Placed at bottom right
			BottomRight		= 18,
		};

		/// Returns horizontal alignment from a placement
		inline Alignment GetHorizontal(Placement placement) {
			return Alignment((int)placement & 56);
		}

		/// Returns vertical alignment from a placement
		inline Alignment GetVertical(Placement placement) {
			return Alignment(((int)placement & 7) << 3);
		}

		/// Returns the offset of the object according to the given placement rule when there is the given 
		/// remainder between object size and the area its being drawn on.
		inline Geometry::Point CalculateOffset(Placement place, Geometry::Size remainder) {
			switch(GetHorizontal(place)) {
			case Alignment::Start:
				remainder.Width=0;
				break;
			case Alignment::Center:
				remainder.Width/=2;
				break;
			case Alignment::End:
				break;
#ifndef NDEBUG
			default:
				throw std::runtime_error("Unknown mode");
				break;
#endif
			}

			switch(GetVertical(place)) {
			case Alignment::Start:
				remainder.Height=0;
				break;
			case Alignment::Center:
				remainder.Height/=2;
				break;
			case Alignment::End:
				break;
#ifndef NDEBUG
			default:
				throw std::runtime_error("Unknown mode");
				break;
#endif
			}

			return remainder;
		}


		/// This class allows control over a sizable object
		class SizeController {
		public:
			/// Controls how a direction is tiled
			enum Tiling {
				/// The drawing is drawn only once with its original size. The placement of this single
				/// drawing will be determined by the alignment.
				Single				= 0,

				/// The drawing is stretched along this axis to cover the given size. If the given drawing
				/// object is truly scalable, this value acts same as Tile.
				Stretch				= 2,

				/// The drawing is tiled along this axis to cover the given size. If the area is smaller,
				/// drawing will be cut from the given size. If the area is larger, drawing will be repeated
				/// as necessary. While repeating, it is possible for drawing not to cover the area exactly,
				/// In this case, alignment determines which side will be incomplete. If center alignment
				/// is selected, both sides will have same amount of incomplete drawing.
				Tile				= 1,

				/// The drawing is tiled along this axis to cover the given size. In this mode, the drawing will 
				/// never placed incomplete. Instead, it will be repeated to cover as much area as possible without
				/// exceeding the given size. Any area that is left will be distributed to the edges according to 
				/// the selected alignment.
				Integral_Smaller	= 13,

				/// The drawing is tiled along this axis to cover the given size. In this mode, the drawing will 
				/// never placed incomplete. Instead, it will be repeated to cover entire area. Excess drawing
				/// will be aligned depending on the selected alignment.
				Integral_Fill		= 21,

				/// The drawing is tiled along this axis to cover the given size. In this mode, the drawing will 
				/// never placed incomplete. Instead, it will be repeated to cover entire area. If the last drawing
				/// that will be partial is more than 50% of the size of the original drawing, it will be drawn. Excess
				/// drawing or the area left empty is distributed according to the selected alignment.
				Integral_Best		= 45,
			};

			/// Creates a default size controller which tiles the object from top-left
			SizeController() : Horizontal(Tile), Vertical(Tile), Placement(Placement::TopLeft) { }

			/// Creates a size controller that places a single object to the given placement. This is an implicit conversion
			/// constructor.
			SizeController(Placement p) : Horizontal(Single), Vertical(Single), Placement(p) { }

			/// Creates a new size controller with the given tiling options and placement
			SizeController(Tiling h, Tiling v, Placement p=Placement::TopLeft) : Horizontal(h), Vertical(v), Placement(p) { }

			/// Creates a new size controller with the given tiling options and placement
			SizeController(Graphics::Tiling tile, Placement p=Placement::TopLeft) :
				Horizontal(tile==Graphics::Tiling::Horizontal ? Tile : Stretch),
				Vertical  (tile==Graphics::Tiling::Vertical   ? Tile : Stretch),
				Placement(p)
			{ }

			/// Calculates the size of the object according to the tiling rules
			Geometry::Size CalculateSize(Geometry::Size objectsize, const Geometry::Size &area) const {
				switch(Horizontal) {
				case Integral_Smaller:
					objectsize.Width=(area.Width/objectsize.Width)*objectsize.Width;
					break;
				case Integral_Fill:
					objectsize.Width=int(std::ceil(float(area.Width)/objectsize.Width))*objectsize.Width;
					break;
				case Integral_Best:
					objectsize.Width=int(std::round(float(area.Width)/objectsize.Width))*objectsize.Width;
					break;
				case Stretch:
				case Tile:
					objectsize.Width=area.Width;
					break;
				case Single:
					break;
#ifndef NDEBUG
				default:
					throw std::runtime_error("Unknown mode");
					break;
#endif
				}

				switch(Vertical) {
				case Integral_Smaller:
					objectsize.Width=(area.Width/objectsize.Width)*objectsize.Width;
					break;
				case Integral_Fill:
					objectsize.Width=int(std::ceil(float(area.Width)/objectsize.Width))*objectsize.Width;
					break;
				case Integral_Best:
					objectsize.Width=int(std::round(float(area.Width)/objectsize.Width))*objectsize.Width;
					break;
				case Stretch:
				case Tile:
					objectsize.Height=area.Height;
					break;
				case Single:
					break;
#ifndef NDEBUG
				default:
					throw std::runtime_error("Unknown mode");
					break;
#endif
				}

				return objectsize;
			}

			/// Calculates the size of the object according to the tiling and placement rules
			Geometry::Point CalculateOffset(const Geometry::Size &objectsize, const Geometry::Size &area) const {
				Graphics::CalculateOffset(Placement, area-CalculateSize(objectsize, area));
			}

			/// Calculates the drawing area of the object according to the tiling and placement rules
			Geometry::Rectangle CalculateArea(const Geometry::Size &objectsize, const Geometry::Size &area) const {
				auto size=CalculateSize(objectsize, area);
				return{Graphics::CalculateOffset(Placement, area-size), size};
			}

			/// Calculates the size of the object according to the tiling rules
			Geometry::Size CalculateSize(Geometry::Size repeatingsize, const Geometry::Size &fixedsize, const Geometry::Size &area) const {
				switch(Horizontal) {
				case Integral_Smaller:
					repeatingsize.Width=((area.Width-fixedsize.Width)/repeatingsize.Width)*repeatingsize.Width+fixedsize.Width;
					break;
				case Integral_Fill:
					repeatingsize.Width=int(std::ceil(float(area.Width-fixedsize.Width)/repeatingsize.Width))*repeatingsize.Width+fixedsize.Width;
					break;
				case Integral_Best:
					repeatingsize.Width=int(std::round(float(area.Width-fixedsize.Width)/repeatingsize.Width))*repeatingsize.Width+fixedsize.Width;
					break;
				case Stretch:
				case Tile:
					repeatingsize.Width=area.Width;
					break;
				case Single:
					break;
#ifndef NDEBUG
				default:
					throw std::runtime_error("Unknown mode");
					break;
#endif
				}

				switch(Vertical) {
				case Integral_Smaller:
					repeatingsize.Width=((area.Width-fixedsize.Width)/repeatingsize.Width)*repeatingsize.Width+fixedsize.Width;
					break;
				case Integral_Fill:
					repeatingsize.Width=int(std::ceil(float(area.Width-fixedsize.Width)/repeatingsize.Width))*repeatingsize.Width+fixedsize.Width;
					break;
				case Integral_Best:
					repeatingsize.Width=int(std::round(float(area.Width-fixedsize.Width)/repeatingsize.Width))*repeatingsize.Width+fixedsize.Width;
					break;
				case Stretch:
				case Tile:
					repeatingsize.Height=area.Height;
					break;
				case Single:
					break;
#ifndef NDEBUG
				default:
					throw std::runtime_error("Unknown mode");
					break;
#endif
				}

				return repeatingsize;
			}

			/// Calculates the size of the object according to the tiling and placement rules
			Geometry::Point CalculateOffset(const Geometry::Size &repeatingsize, const Geometry::Size &fixedsize, const Geometry::Size &area) const {
				Graphics::CalculateOffset(Placement, area-CalculateSize(repeatingsize, fixedsize, area));
			}

			/// Calculates the drawing area of the object according to the tiling and placement rules
			Geometry::Rectangle CalculateArea(const Geometry::Size &repeatingsize, const Geometry::Size &fixedsize, const Geometry::Size &area) const {
				auto size=CalculateSize(repeatingsize, fixedsize, area);
				return{Graphics::CalculateOffset(Placement, area-size), size};
			}

			Graphics::Tiling GetTiling() const {
				return Graphics::Tile(Horizontal!=Single && Horizontal!=Stretch, Vertical!=Single && Vertical!=Stretch);
			}

			/// Horizontal tiling mode
			Tiling				Horizontal;

			/// Vertical tiling mode
			Tiling				Vertical;

			/// Placement method
			Graphics::Placement	Placement;
		};

		/// This interface represents a GL texture source.
		class TextureSource {
		public:
			/// Should return GL::Texture to be drawn. This could be 0 to denote no texture is to be used.
			virtual GL::Texture GetID() const = 0;

			/// Should returns the size of the texture in pixels
			virtual Geometry::Size GetSize() const = 0;

			/// Should returns the coordinates of the texture to be used
			virtual const Geometry::Pointf *GetCoordinates() const = 0;

			/// Returns whether this texture uses only a part of the GL::Texture. This indicates that the tiling
			/// operations should be performed without texture repeating method.
			bool IsPartial() const {
				return memcmp(GetCoordinates(), fullcoordinates, sizeof(fullcoordinates))!=0;
			}

		protected:
			/// Coordinates that selects the entire texture to be used
			static const Geometry::Pointf fullcoordinates[4];
		};

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