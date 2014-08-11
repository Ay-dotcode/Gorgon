#pragma once

#include "../Layer.h"
#include "TextureTargets.h"
#include "../External/glutil/MatrixStack.h"

namespace Gorgon { namespace Graphics {

	/// @cond INTERNAL
	namespace internal {

		/// This class represents a drawable surface.
		class Surface {
		public:

			/// Sets the source to the given source. This variant uses texture coordinates given by the source.
			Surface(const TextureSource &source, const Geometry::Pointf &p1, const Geometry::Pointf &p2,
				const Geometry::Pointf &p3, const Geometry::Pointf &p4) {
				
				if(texture) delete[] texture;

				texture      = nullptr;

				vertices[0]  = {p1.X, p1.Y, 0.f, 1.f};
				vertices[1]  = {p2.X, p2.Y, 0.f, 1.f};
				vertices[2]  = {p3.X, p3.Y, 0.f, 1.f};
				vertices[3]  = {p4.X, p4.Y, 0.f, 1.f};

				this->source = &source;
			}

			/// Sets the source to the given source. This variant uses supplied texture coordinates.
			Surface(const TextureSource &source, const Geometry::Pointf &p1, const Geometry::Pointf &p2,
				const Geometry::Pointf &p3, const Geometry::Pointf &p4,
				const Geometry::Pointf &t1, const Geometry::Pointf &t2,
				const Geometry::Pointf &t3, const Geometry::Pointf &t4) { 

				if(!texture) texture = new Geometry::Pointf[4];

				texture [0]  = t1;
				texture [1]  = t2;
				texture [2]  = t3;
				texture [3]  = t4;

				vertices[0]  = {p1.X, p1.Y, 0.f, 1.f};
				vertices[1]  = {p2.X, p2.Y, 0.f, 1.f};
				vertices[2]  = {p3.X, p3.Y, 0.f, 1.f};
				vertices[3]  = {p4.X, p4.Y, 0.f, 1.f};

				this->source = &source;
			}

			GL::Texture TextureID() const {
				return source->GetID();
			}

			glm::mat4x3 GetVertices(const glm::mat4 &transform) {
				glm::mat4 result =transform*vertices;
				for(int i=0; i<4; i++) result[i]/=result[i].w;
				return{result[0].xyz(), result[1].xyz(), result[2].xyz(), result[3].xyz()};
			}

			glm::mat4x2 GetTextureCoords() {
				if(texture)
					return{{texture[0].X, texture[0].Y}, {texture[1].X, texture[1].Y}, {texture[2].X, texture[2].Y}, {texture[3].X, texture[3].Y}};
				else {
					auto texture=source->GetCoordinates();
					return{{texture[0].X, texture[0].Y}, {texture[1].X, texture[1].Y}, {texture[2].X, texture[2].Y}, {texture[3].X, texture[3].Y}};
				}
			}

			~Surface() {
				if(texture) delete[] texture;
				texture      = nullptr;
			}

		private:

			glm::mat4 vertices;
			Geometry::Pointf *texture = nullptr;

			const TextureSource *source = nullptr;
		};
	}
	/// @endcond

	class Layer : public Gorgon::Layer, public Graphics::TextureTarget {
	public:

		virtual void Draw(const TextureSource &image, const Geometry::Pointf &p1, const Geometry::Pointf &p2, 
			const Geometry::Pointf &p3, const Geometry::Pointf &p4) override {
			surfaces.emplace_back(image, p1, p2, p3, p4);
		}

		virtual void Draw(const TextureSource &image, const Geometry::Pointf &p1, const Geometry::Pointf &p2, 
			const Geometry::Pointf &p3, const Geometry::Pointf &p4, 
			const Geometry::Pointf &tex1, const Geometry::Pointf &tex2, 
			const Geometry::Pointf &tex3, const Geometry::Pointf &tex4) override {
			surfaces.emplace_back(image, p1, p2, p3, p4, tex1, tex2, tex3, tex4);
		}

		virtual void Draw(const TextureSource &image, Tiling tiling, const Geometry::Rectangle &location) override {
			if(tiling==Tiling::None) {
				Draw(image, location.TopLeft(), location.TopRight(), location.BottomRight(), location.BottomLeft());
			}
			else {
				assert(false);
				//!
			}
		}

		virtual void Clear() override {
			surfaces.clear();
		}

		virtual void Render() override;

		virtual DrawMode GetDrawMode() const override { return mode; }

		virtual void SetDrawMode(DrawMode mode) override { this->mode=mode; }

		virtual Geometry::Size GetTargetSize() const override { return Gorgon::Layer::GetSize(); }

		using Gorgon::Layer::GetSize;

	private:
		std::vector<internal::Surface> surfaces;

		DrawMode mode;
	};
	 
	class ColorizableLayer {

	};

} }

#ifdef aaaaa

#include "GGE.h"
#include "Layer.h"
#include "../Utils/PAClassList.h"
#include "Graphics.h"
#include "GraphicTargets2D.h"

namespace gge { namespace graphics {
	////_private
	struct ColorizableSurface : public BasicSurface {
		RGBfloat Color; 
	};

	////_private
	struct RawSurface : public BasicSurface {
		Byte *Data;
		ColorMode::Type Mode;
		int Width;
		int Height;
	};

	////This layer is a basic 2D graphics target
	class Basic2DLayer : public LayerBase, public ImageTarget2D {
	public:
		////This list contains surfaces to be drawn
		PAClassList<BasicSurface> Surfaces;
		////Whether or not enable clipping
		bool ClippingEnabled;
		BasicSurface::DrawMode DrawMode;


		Basic2DLayer() : LayerBase() { init(); }

		Basic2DLayer(const utils::Bounds &b) : LayerBase(b) { init(); }

		Basic2DLayer(int L, int T, int R, int B) : LayerBase(L,T,R,B) { init(); }

		Basic2DLayer(int X,int Y) : LayerBase(X,Y) { init(); }

		Basic2DLayer(const utils::Point &p) : LayerBase(p) { init(); }

		~Basic2DLayer();

		void init() {
			IsVisible=true;
			ClippingEnabled=false;
			DrawMode=BasicSurface::Normal;
		}

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
		virtual void Draw(const GLTexture *Image,float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4);

		virtual void Draw(const GLTexture *Image,float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4);

		virtual void Draw(const GLTexture *Image, int X, int Y, int W, int H) { Draw(Image, (float)X,(float)Y, (float)X+W,(float)Y, (float)X+W,(float)Y+H, (float)X,(float)Y+H); }

		virtual void Draw(const GLTexture *Image, Tiling2D::Type Tiling, int X, int Y, int W, int H) {
			switch(Tiling) {
			case Tiling2D::None:
				Draw(Image,X,Y, W,H);
				break;
			case Tiling2D::Horizontal:
				DrawHTiled(Image,X,Y, W,H);
				break;
			case Tiling2D::Vertical:
				DrawVTiled(Image,X,Y, W,H);
				break;
			case Tiling2D::Both:
				DrawTiled(Image,X,Y, W,H);
				break;
			}
		}

		////Draws a tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		void DrawTiled(const GLTexture *Image,int X,int Y,int W,int H);
		////Draws a horizontally tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		void DrawHTiled(const GLTexture *Image,int X,int Y,int W,int H);
		////Draws a vertically tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		///@W		: the width of the image to be drawn
		/// if it is more than the size of the image
		void DrawVTiled(const GLTexture *Image,int X,int Y,int W,int H);

		////Renders the current layer, default handling is to pass
		/// the request to the sub-layers
		virtual void Render();
		virtual void Clear() { Surfaces.Clear(); }

		virtual int GetWidth() const { return BoundingBox.Width(); }
		virtual int GetHeight() const { return BoundingBox.Height(); }

		virtual BasicSurface::DrawMode GetDrawMode() const { return DrawMode; }
		virtual void SetDrawMode(BasicSurface::DrawMode mode) { DrawMode=mode; }

	};

	////This layer is a 2D graphics target and also has colorization support
	class Colorizable2DLayer : public ColorizableImageTarget2D, public LayerBase {
	public:
		////Whether or not enable clipping
		bool ClippingEnabled;
		BasicSurface::DrawMode DrawMode;
		////Default constructor to initialize variables

		Colorizable2DLayer() : LayerBase() { init(); }

		Colorizable2DLayer(const utils::Bounds &b) : LayerBase(b) { init(); }

		Colorizable2DLayer(int L, int T, int R, int B) : LayerBase(L,T,R,B) { init(); }

		Colorizable2DLayer(int X,int Y) : LayerBase(X,Y) { init(); }

		Colorizable2DLayer(const utils::Point &p) : LayerBase(p) { init(); }

		void init() {
			Ambient=RGBint(0xffffffff);
			CurrentColor=RGBint(0xffffffff);
			IsVisible=true;
			ClippingEnabled=false;
			DrawMode=BasicSurface::Normal;
		}

		////This list contains surfaces to be drawn
		PAClassList<ColorizableSurface> Surfaces;

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
		virtual void Draw(const GLTexture *Image,float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4);

		virtual void Draw(const GLTexture *Image,float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4);

		virtual void Draw(const GLTexture *Image, int X, int Y, int W, int H) { Draw(Image, (float)X,(float)Y, (float)X+W,(float)Y, (float)X+W,(float)Y+H, (float)X,(float)Y+H); }

		virtual void Draw(const GLTexture *Image, Tiling2D::Type Tiling, int X, int Y, int W, int H) {
			switch(Tiling) {
			case Tiling2D::None:
				Draw(Image,X,Y, W,H);
				break;
			case Tiling2D::Horizontal:
				DrawHTiled(Image,X,Y, W,H);
				break;
			case Tiling2D::Vertical:
				DrawVTiled(Image,X,Y, W,H);
				break;
			case Tiling2D::Both:
				DrawTiled(Image,X,Y, W,H);
				break;
			}
		}

		////Draws a tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		void DrawTiled(const GLTexture *Image,int X,int Y,int W,int H);
		////Draws a horizontally tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		void DrawHTiled(const GLTexture *Image,int X,int Y,int W,int H);
		////Draws a vertically tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		///@W		: the width of the image to be drawn
		/// if it is more than the size of the image
		void DrawVTiled(const GLTexture *Image,int X,int Y,int W,int H);



		////Renders the current layer, default handling is to pass
		/// the request to the sub-layers
		virtual void Render();
		virtual void Clear() { Surfaces.Clear(); }


		virtual int GetWidth() const { return BoundingBox.Width(); }
		virtual int GetHeight() const { return BoundingBox.Height(); }

		RGBint Ambient;

		virtual void SetCurrentColor(RGBfloat color) { CurrentColor=color; }
		virtual RGBint GetCurrentColor() { return CurrentColor; }

		virtual BasicSurface::DrawMode GetDrawMode() const { return DrawMode; }
		virtual void SetDrawMode(BasicSurface::DrawMode mode) { DrawMode=mode; }

	protected:
		RGBfloat CurrentColor;
	};

} }

#endif