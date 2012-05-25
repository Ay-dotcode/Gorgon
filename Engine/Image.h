#pragma once
#include "../Utils/UtilsBase.h"
#include "../Utils/CastableManagedBuffer.h"
#include "Graphics.h"
#include "GraphicTargets2D.h"
#include "Graphic2D.h"




namespace gge { namespace graphics {

	class ImageData {
	public:
		ImageData() : Width(0), Height(0), Mode(ColorMode::ARGB) {

		}

		virtual void Resize(int Width, int Height, ColorMode::Type Mode) {
			int bits=getBPP(Mode);

			Data.Resize(Width*Height*bits);

			this->Width=Width;
			this->Height=Height;
			this->Mode=Mode;
		}

		int GetWidth() const {
			return Width;
		}

		int GetHeight() const {
			return Height;
		}

		ColorMode::Type GetMode() const {
			return Mode;
		}

		Byte *RawData() { return Data.GetBuffer(); }

	protected:
		utils::CastableManagedBuffer<Byte> Data;
		int Width,Height;
		ColorMode::Type Mode;
	};


	class ImageTexture : public virtual Image2D {
	public:

		ImageTexture() {
			Texture.ID= 0;
			Texture.W = 0;
			Texture.H = 0;
		}

		//void drawto(ImageTarget2D& Target, const SizeController2D &controller, int X, int Y, int W, int H);

		virtual GLTexture &GetTexture() { return Texture; }

		~ImageTexture() {
			if(Texture.ID)
				graphics::system::DestroyTexture(Texture);
		}

	protected:
		virtual void drawin(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) const
		{ Target.Draw(&Texture,graphics::Tiling2D::Both, X,Y,W,H); } 

		virtual void drawin(graphics::ImageTarget2D& Target, const graphics::SizeController2D &controller, int X, int Y, int W, int H) const ;

		virtual void draw(graphics::ImageTarget2D& Target, float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4)  const
		{ Target.Draw(&Texture,X1,Y1,X2,Y2,X3,Y3,X4,Y4,S1,U1,S2,U2,S3,U3,S4,U4); }
		virtual void draw(graphics::ImageTarget2D& Target, float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4)  const
		{ Target.Draw(&Texture,X1,Y1,X2,Y2,X3,Y3,X4,Y4); }
		virtual void drawstretched(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) const
		{ Target.Draw(&Texture, X,Y,W,H); }
		virtual void draw(graphics::ImageTarget2D& Target, graphics::Tiling2D::Type Tiling, int X, int Y, int W, int H) const
		{ Target.Draw(&Texture,Tiling, X,Y,W,H); }

		virtual int getwidth () const { return Texture.W; }
		virtual int getheight() const { return Texture.H; }


		virtual int calculatewidth (int W=-1) const { return Texture.W; }
		virtual int calculateheight(int H=-1) const { return Texture.H; }

		virtual int calculatewidth(const SizeController2D &controller, int W=-1 ) const {
			if(controller.HorizontalTiling==SizeController2D::Single)
				return getwidth();

			return controller.CalculateWidth(W, getwidth());
		}
		virtual int calculateheight(const SizeController2D &controller, int H=-1 ) const {
			if(controller.HorizontalTiling==SizeController2D::Single)
				return getheight();

			return controller.CalculateHeight(H, getheight());
		}

	protected:
		GLTexture Texture;
	};


} }
