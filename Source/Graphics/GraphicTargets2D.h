#pragma once
#include "Graphics.h"


namespace gge { namespace graphics {

	////This interface defines a class that can be used
	/// as a common target for images
	class ImageTarget2D {
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
		virtual void Draw(const GLTexture *Image,float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4) = 0;

		virtual void Draw(const GLTexture *Image,float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) = 0;

		////Draws a simple image to the screen.
		/// This function does not support any transformations
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		virtual void Draw(const GLTexture *Image, int X, int Y, int W, int H) = 0;

		////Draws a simple image to the screen.
		/// This function does not support any transformations
		///@Image	: image texture to be drawn, this can be obtained
		/// using generate texture function
		virtual void Draw(const GLTexture *Image, Tiling2D::Type Tiling, int X, int Y, int W, int H) = 0;

		////Clears drawing buffer, in layer architecture this request only affects
		/// the layer itself, not sub-layers
		virtual void Clear()=0;

		////Size of the target
		virtual int GetWidth() const  = 0;
		virtual int GetHeight() const = 0;

		utils::Size GetSize() { 
			return utils::Size(GetWidth(), GetHeight()); 
		}
		
		virtual BasicSurface::DrawMode GetDrawMode() const = 0;
		virtual void SetDrawMode(BasicSurface::DrawMode mode) = 0 ;
	};

	class ColorizableTarget {
	public:
		virtual void SetCurrentColor(RGBfloat color) = 0;

		virtual RGBint GetCurrentColor() = 0;
	};

	class ColorizableImageTarget2D : public ImageTarget2D, public ColorizableTarget {
	};

} }
