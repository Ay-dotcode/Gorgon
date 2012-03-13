#pragma once

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
			isVisible=true;
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
		virtual void Draw(const GLTexture *Image,int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4);

		virtual void Draw(const GLTexture *Image,int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4);

		virtual void Draw(const GLTexture *Image, int X, int Y, int W, int H) { Draw(Image, X,Y, X+W,Y, X+W,Y+H, X,Y+H); }

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
			isVisible=true;
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
		virtual void Draw(const GLTexture *Image,int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4);

		virtual void Draw(const GLTexture *Image,int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4);

		virtual void Draw(const GLTexture *Image, int X, int Y, int W, int H) { Draw(Image, X,Y, X+W,Y, X+W,Y+H, X,Y+H); }

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

		virtual void SetCurrentColor(RGBint color) { CurrentColor=color; }
		virtual void SetCurrentColor(RGBfloat color) { CurrentColor=color; }
		virtual RGBint GetCurrentColor() { return CurrentColor; }

		virtual BasicSurface::DrawMode GetDrawMode() const { return DrawMode; }
		virtual void SetDrawMode(BasicSurface::DrawMode mode) { DrawMode=mode; }

	protected:
		RGBfloat CurrentColor;
	};

} }
