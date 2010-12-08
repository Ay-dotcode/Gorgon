#pragma once

#include "../Utils/GGE.h"
#include "Layer.h"
#include "../Utils/PAClassList.h"
#include "Graphics.h"

namespace gge {
	////_private
	struct ColorizableSurface : public BasicSurface {
		RGBfloat Color; 
	};

	////_private
	struct RawSurface : public BasicSurface {
		BYTE *Data;
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

		////Default constructor to initialize variables
		Basic2DLayer(int X=0, int Y=0, int W=100, int H=100);
		Basic2DLayer(gge::Rectangle r);

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
		////Draws a veritically tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		///@W		: the width of the image to be drawn
		/// if it is more than the size of the image
		virtual void DrawVTiled(GLTexture *Image,int X,int Y,int W,int H);
		////Renders the current layer, default handling is to pass
		/// the request to the sub-layers
		virtual void Render();
		virtual void Clear() { Surfaces.Clear(); }
	};
	////This layer is a 2D graphics target and also has colorization support
	class Colorizable2DLayer : public I2DColorizableGraphicsTarget, public LayerBase {
	public:
		////Whether or not enable clipping
		bool EnableClipping;
		////Default constructor to initialize variables
		Colorizable2DLayer(int X=0, int Y=0, int W=100, int H=100);
		Colorizable2DLayer(gge::Rectangle r);

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
		////Draws a veritically tiled image to the screen
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		///@W		: the width of the image to be drawn
		/// if it is more than the size of the image
		virtual void DrawVTiled(GLTexture *Image,int X,int Y,int W,int H);
	};
	class Basic2DRawGraphicsLayer : public I2DRawGraphicsTarget, public LayerBase {
	public:
		////Whether or not enable clipping
		bool EnableClipping;
		////This list contains surfaces to be drawn
		PAClassList<RawSurface> Surfaces;

		////Default constructor to initialize variables
		Basic2DRawGraphicsLayer(int X=0, int Y=0, int W=100, int H=100);

		////Draws a given data to the screen
		virtual void Draw(BYTE *Image, int Width, int Height, ColorMode Mode,int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4);

		////Renders this layer
		virtual void Render();
		////Clear the contents of the layer
		virtual void Clear() { Surfaces.Clear(); }
	};
	class WidgetLayer :  public BasicPointerTarget, public Basic2DLayer {
	public:
		WidgetLayer(int X=0, int Y=0, int W=100, int H=100) : Basic2DLayer(X,Y,W,H) {
		}
		////Renders this layer
		virtual void Render() { Basic2DLayer::Render(); }

	protected:
		////Processes the mouse event for the current layer, default
		/// handling is to pass the request to the sub-layers
		virtual bool PropagateMouseEvent(MouseEventType event, int x, int y, void *data);
		virtual bool PropagateMouseScrollEvent(int amount, MouseEventType event, int x, int y, void *data);
	};

}