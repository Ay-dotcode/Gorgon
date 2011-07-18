#pragma once

#include "GGE.h"
#include "Layer.h"
#include "../Utils/PAClassList.h"
#include "Graphics.h"

namespace gge { namespace graphics {
	////_private
	struct ColorizableSurface : public BasicSurface {
		RGBfloat Color; 
	};

	////_private
	struct RawSurface : public BasicSurface {
		Byte *Data;
		ColorMode Mode;
		int Width;
		int Height;
	};

	////This layer is a basic 2D graphics target
	class Basic2DLayer : public LayerBase, public I2DGraphicsTarget {
	public:
		////This list contains surfaces to be drawn
		PAClassList<BasicSurface> Surfaces;
		////Whether or not enable clipping
		bool EnableClipping;


		Basic2DLayer() : LayerBase() { init(); }

		Basic2DLayer(const utils::Bounds &b) : LayerBase(b) { init(); }

		Basic2DLayer(int L, int T, int R, int B) : LayerBase(L,T,R,B) { init(); }

		Basic2DLayer(int X,int Y) : LayerBase(X,Y) { init(); }

		Basic2DLayer(const utils::Point &p) : LayerBase(p) { init(); }

		void init() {
			isVisible=true;
			EnableClipping=false;
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
		virtual void Draw(GLTexture *Image,int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4);
		////Draws a tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		virtual void DrawTiled(GLTexture *Image,int X,int Y,int W,int H);
		////Draws a horizontally tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		virtual void DrawHTiled(GLTexture *Image,int X,int Y,int W,int H);
		////Draws a vertically tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		///@W		: the width of the image to be drawn
		/// if it is more than the size of the image
		virtual void DrawVTiled(GLTexture *Image,int X,int Y,int W,int H);
		////Renders the current layer, default handling is to pass
		/// the request to the sub-layers
		virtual void Render();
		virtual void Clear() { Surfaces.Clear(); }

		virtual int Width() { return BoundingBox.Width(); }
		virtual int Height() { return BoundingBox.Height(); }
	};
	////This layer is a 2D graphics target and also has colorization support
	class Colorizable2DLayer : public I2DColorizableGraphicsTarget, public LayerBase {
	public:
		////Whether or not enable clipping
		bool EnableClipping;
		////Default constructor to initialize variables

		Colorizable2DLayer() : LayerBase() { init(); }

		Colorizable2DLayer(const utils::Bounds &b) : LayerBase(b) { init(); }

		Colorizable2DLayer(int L, int T, int R, int B) : LayerBase(L,T,R,B) { init(); }

		Colorizable2DLayer(int X,int Y) : LayerBase(X,Y) { init(); }

		Colorizable2DLayer(const utils::Point &p) : LayerBase(p) { init(); }

		void init() {
			Ambient=RGBint(0xffffffff);
			isVisible=true;
			EnableClipping=false;
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
		///@Color	: color
		virtual void Draw(GLTexture *Image,int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4, RGBint Color);
		virtual void Draw(GLTexture *Image,int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4);
		virtual void Render();
		virtual void Clear() { Surfaces.Clear(); }
		////Draws a tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		virtual void DrawTiled(GLTexture *Image,int X,int Y,int W,int H);
		////Draws a horizontally tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		virtual void DrawHTiled(GLTexture *Image,int X,int Y,int W,int H);
		////Draws a vertically tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		///@W		: the width of the image to be drawn
		/// if it is more than the size of the image
		virtual void DrawVTiled(GLTexture *Image,int X,int Y,int W,int H);

		virtual int Width() { return BoundingBox.Width(); }
		virtual int Height() { return BoundingBox.Height(); }
	};
	class Basic2DRawGraphicsLayer : public I2DRawGraphicsTarget, public LayerBase {
	public:
		////Whether or not enable clipping
		bool EnableClipping;
		////This list contains surfaces to be drawn
		PAClassList<RawSurface> Surfaces;

		////Draws a given data to the screen
		virtual void Draw(Byte *Image, int Width, int Height, ColorMode Mode,int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4);
	
		void init() {
			isVisible=true;
			EnableClipping=false;
		}

		Basic2DRawGraphicsLayer() : LayerBase() { init(); }

		Basic2DRawGraphicsLayer(const utils::Bounds &b) : LayerBase(b) { init(); }

		Basic2DRawGraphicsLayer(int L, int T, int R, int B) : LayerBase(L,T,R,B) { init(); }

		Basic2DRawGraphicsLayer(int X,int Y) : LayerBase(X,Y) { init(); }

		Basic2DRawGraphicsLayer(const utils::Point &p) : LayerBase(p) { init(); }

		////Renders this layer
		virtual void Render();
		////Clear the contents of the layer
		virtual void Clear() { Surfaces.Clear(); }
	};

} }
