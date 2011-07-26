#pragma once
#include "..\Utils\UtilsBase.h"
#include "..\Utils\CastableManagedBuffer.h"
#include "Graphics.h"
#include "GraphicTargets2D.h"


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

	protected:
		utils::CastableManagedBuffer<Byte> Data;
		int Width,Height;
		ColorMode::Type Mode;
	};


	class TextureImage {
	public:

		virtual int getimagewidth () const { return Texture.W; }
		virtual int getimageheight() const { return Texture.H; };

		void drawto(ImageTarget2D &target, int X1,int Y1, int X2,int Y2, int X3,int Y3, int X4,int Y4) {
			target.Draw(&Texture,X1,Y1,X2,Y2,X3,Y3,X4,Y4);
		}

		void drawto(ColorizableImageTarget2D &target, int X1,int Y1, int X2,int Y2, int X3,int Y3, int X4,int Y4, RGBint color) {
			target.SetCurrentColor(color);
			target.Draw(&Texture,X1,Y1,X2,Y2,X3,Y3,X4,Y4);
		}

		void drawto(ImageTarget2D &target, int X1,int Y1, int X2,int Y2, int X3,int Y3, int X4,int Y4,float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) {
			target.Draw(&Texture,X1,Y1,X2,Y2,X3,Y3,X4,Y4,S1,U1,S2,U2,S3,U3,S4,U4);
		}

		void drawto(ColorizableImageTarget2D &target, int X1,int Y1, int X2,int Y2, int X3,int Y3, int X4,int Y4,float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4, RGBint color) {
			target.SetCurrentColor(color);
			target.Draw(&Texture,X1,Y1,X2,Y2,X3,Y3,X4,Y4,S1,U1,S2,U2,S3,U3,S4,U4);
		}

		void drawto(ImageTarget2D &target, int X,int Y, int W,int H) {
			target.Draw(&Texture, X,Y,W,H);
		}

		void drawto(ColorizableImageTarget2D &target, int X,int Y, int W,int H, RGBint color) {
			target.SetCurrentColor(color);
			target.Draw(&Texture, X,Y,W,H);
		}

		void drawto(ImageTarget2D &target, Tiling2D::Type Tiling, int X,int Y, int W,int H) {
			target.Draw(&Texture,Tiling, X,Y,W,H);
		}

		void drawto(ColorizableImageTarget2D &target, Tiling2D::Type Tiling, int X,int Y, int W,int H, RGBint color) {
			target.SetCurrentColor(color);
			target.Draw(&Texture,Tiling, X,Y,W,H);
		}

		void drawto(ImageTarget2D& Target, SizeController2D &controller, int X, int Y, int W, int H);

		int calculatewidthusing(const SizeController2D &controller, int w) const;
		int calculateheightusing(const SizeController2D &controller, int h) const;

		GLTexture &GetTexture() { return Texture; }

	protected:
		GLTexture Texture;
	};
} }
