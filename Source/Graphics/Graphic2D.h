#pragma once


#include "Graphics.h"
#include "GraphicTargets2D.h"
#include "../Utils/Size2D.h"


namespace Gorgon { namespace Graphics {

	class Graphic2D {
	public:
		void Draw(ImageTarget2D &target, int X, int Y) const {
			draw(target, X,Y);
		}

		void Draw(ImageTarget2D &target, utils::Point p) const {
			draw(target, p.x,p.y);
		}

	protected:
		virtual void draw(ImageTarget2D& Target, int X, int Y) const = 0;
	};



	class SizelessGraphic2D {
	public:

		//With size factor, scaling the image
		void DrawIn(ImageTarget2D &target, int X, int Y, int W, int H) const {
			drawin(target, X,Y, W,H);
		}

		void DrawIn(ImageTarget2D &target, utils::Point p, int W, int H) const {
			drawin(target, p.x,p.y, W,H);
		}


		void DrawIn(ImageTarget2D &target, int X, int Y, utils::Size s) const {
			drawin(target, X,Y, s.Width,s.Height);
		}

		void DrawIn(ImageTarget2D &target, utils::Point p, utils::Size s) const {
			drawin(target, p.x,p.y, s.Width,s.Height);
		}



		//Draw using a rectangle, still point and size
		void DrawIn(ImageTarget2D &target, utils::Rectangle r) const {
			drawin(target, r.Left,r.Top, r.Width,r.Height);
		}

		void DrawIn(ImageTarget2D &target, utils::Bounds b) const {
			drawin(target, b.Left,b.Top, b.Width(), b.Height());
		}

		void DrawIn(ImageTarget2D &target) const {
			drawin(target, 0,0, target.GetSize().Width, target.GetSize().Height);
		}

		//Draw size controlled
		void DrawIn(ImageTarget2D &target, const SizeController2D &controller) const {
			drawin(target, controller, 0,0, target.GetSize().Width, target.GetSize().Height);
		}



		//Draw size controlled
		void DrawIn(ImageTarget2D &target, const SizeController2D &controller, int X, int Y, int W, int H) const {
			drawin(target, controller, X,Y, W,H);
		}

		void DrawIn(ImageTarget2D &target, const SizeController2D &controller, utils::Point p, int W, int H) const {
			drawin(target, controller, p.x,p.y, W,H);
		}

		void DrawIn(ImageTarget2D &target, const SizeController2D &controller, int X, int Y, utils::Size s) const {
			drawin(target, controller, X,Y, s.Width, s.Height);
		}

		void DrawIn(ImageTarget2D &target, const SizeController2D &controller, utils::Point p, utils::Size s) const {
			drawin(target, controller, p.x,p.y, s.Width, s.Height);
		}


		void DrawIn(ImageTarget2D &target, const SizeController2D &controller, utils::Rectangle r) const {
			drawin(target, controller, r.Left,r.Top, r.Width, r.Height);
		}

		void DrawIn(ImageTarget2D &target, const SizeController2D &controller, utils::Bounds b) const {
			drawin(target, controller, b.Left,b.Top, b.Width(), b.Height());
		}



		//Size calculation
		int CalculateWidth(int W=-1) const {
			return calculatewidth(W);
		}
		int CalculateHeight(int H=-1) const {
			return calculateheight(H);
		}
		utils::Size	CalculateSize(int W=-1, int H=-1) const {
			return utils::Size(calculatewidth(W), calculateheight(H));
		}
		utils::Size	CalculateSize(utils::Size s) const {
			return utils::Size(calculatewidth(s.Width), calculateheight(s.Height));
		}

		int CalculateWidth(const SizeController2D &controller, int W=-1) const {
			return calculatewidth(controller, W);
		}
		int CalculateHeight(const SizeController2D &controller, int H=-1) const {
			return calculateheight(controller, H);
		}
		utils::Size	CalculateSize(const SizeController2D &controller, int W=-1, int H=-1) const {
			return utils::Size(calculatewidth(controller, W), calculateheight(controller, H));
		}
		utils::Size	CalculateSize(const SizeController2D &controller, utils::Size s) const {
			return utils::Size(calculatewidth(controller, s.Width), calculateheight(controller, s.Height));
		}

	protected:
		virtual void drawin(ImageTarget2D& Target, int X, int Y, int W, int H) const = 0;
		virtual void drawin(ImageTarget2D& Target, const SizeController2D &controller, int X, int Y, int W, int H) const = 0;

		virtual int calculatewidth (int w=-1) const = 0;
		virtual int calculateheight(int h=-1) const = 0;

		virtual int calculatewidth (const SizeController2D &controller, int w=-1) const = 0;
		virtual int calculateheight(const SizeController2D &controller, int h=-1) const = 0;
	};


	////This is a basic drawing object
	class RectangularGraphic2D : public virtual Graphic2D, public virtual SizelessGraphic2D {
	public:
		//public methods

		using Graphic2D::Draw;

		//With size factor, scaling the image
		void DrawStretched(ImageTarget2D &target, int X, int Y, int W, int H) const {
			drawstretched(target, X,Y, W,H);
		}
		void DrawStretched(ImageTarget2D *target, int X, int Y, int W, int H) const {
			drawstretched(*target, X,Y, W,H);
		}

		void DrawStretched(ImageTarget2D &target, utils::Point p, int W, int H) const {
			drawstretched(target, p.x,p.y, W,H);
		}
		void DrawStretched(ImageTarget2D *target, utils::Point p, int W, int H) const {
			drawstretched(*target, p.x,p.y, W,H);
		}


		void DrawStretched(ImageTarget2D &target, int X, int Y, utils::Size s) const {
			drawstretched(target, X,Y, s.Width,s.Height);
		}
		void DrawStretched(ImageTarget2D *target, int X, int Y, utils::Size s) const {
			drawstretched(*target, X,Y, s.Width,s.Height);
		}

		void DrawStretched(ImageTarget2D &target, utils::Point p, utils::Size s) const {
			drawstretched(target, p.x,p.y, s.Width,s.Height);
		}
		void DrawStretched(ImageTarget2D *target, utils::Point p, utils::Size s) const {
			drawstretched(*target, p.x,p.y, s.Width,s.Height);
		}



		//Draw using a rectangle, still point and size
		void DrawStretched(ImageTarget2D &target, utils::Rectangle r) const {
			drawstretched(target, r.Left,r.Top, r.Width,r.Height);
		}
		void DrawStretched(ImageTarget2D *target, utils::Rectangle r) const {
			drawstretched(*target, r.Left,r.Top, r.Width,r.Height);
		}



		//Draw using 4 coords
		void Draw(ImageTarget2D& Target, float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4) const {
			draw(Target, X1,Y1, X2,Y2, X3,Y3, X4,Y4);
		}
		void Draw(ImageTarget2D* Target, float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4) const {
			draw(*Target, X1,Y1, X2,Y2, X3,Y3, X4,Y4);
		}

		void Draw(ImageTarget2D& Target, utils::Point2D p1, utils::Point2D p2, utils::Point2D p3, utils::Point2D p4) const {
			draw(Target, p1.x,p1.y, p2.x,p2.y, p3.x,p3.y, p4.x,p4.y);
		}
		void Draw(ImageTarget2D* Target, utils::Point2D p1, utils::Point2D p2, utils::Point2D p3, utils::Point2D p4) const {
			draw(*Target, p1.x,p1.y, p2.x,p2.y, p3.x,p3.y, p4.x,p4.y);
		}



		//Draw rotated
		void DrawRotated(ImageTarget2D& Target, utils::Point p, float angle, utils::Point2D origin=utils::Point(0,0)) const;
		void DrawRotated(ImageTarget2D* Target, utils::Point p, float angle, utils::Point2D origin=utils::Point2D(0,0)) const {
			DrawRotated(*Target, p, angle, origin);
		}

		void DrawRotated(ImageTarget2D& Target, int X,int Y, float angle, float oX=0,float oY=0) const {
			DrawRotated(Target, utils::Point(X,Y), angle, utils::Point2D(oX,oY));
		}
		void DrawRotated(ImageTarget2D* Target, int X,int Y, float angle, float oX=0,float oY=0) const {
			DrawRotated(*Target, utils::Point(X,Y), angle, utils::Point2D(oX,oY));
		}



		//Draw tiled
		void Draw(ImageTarget2D &target, Tiling2D::Type tiling, int X, int Y, int W, int H) const {
			draw(target, tiling, X,Y, W,H);
		}
		void Draw(ImageTarget2D *target, Tiling2D::Type tiling, int X, int Y, int W, int H) const {
			draw(*target, tiling, X,Y, W,H);
		}

		void Draw(ImageTarget2D &target, Tiling2D::Type tiling, utils::Point p, int W, int H) const {
			draw(target, tiling, p.x,p.y, W,H);
		}
		void Draw(ImageTarget2D *target, Tiling2D::Type tiling, utils::Point p, int W, int H) const {
			draw(*target, tiling, p.x,p.y, W,H);
		}

		void Draw(ImageTarget2D &target, Tiling2D::Type tiling, int X, int Y, utils::Size s) const {
			draw(target, tiling, X,Y, s.Width, s.Height);
		}
		void Draw(ImageTarget2D *target, Tiling2D::Type tiling, int X, int Y, utils::Size s) const {
			draw(*target, tiling, X,Y, s.Width, s.Height);
		}

		void Draw(ImageTarget2D &target, Tiling2D::Type tiling, utils::Point p, utils::Size s) const {
			draw(target, tiling, p.x,p.y, s.Width, s.Height);
		}
		void Draw(ImageTarget2D *target, Tiling2D::Type tiling, utils::Point p, utils::Size s) const {
			draw(*target, tiling, p.x,p.y, s.Width, s.Height);
		}


		void Draw(ImageTarget2D &target, Tiling2D::Type tiling, utils::Rectangle r) const {
			draw(target, tiling, r.Left,r.Top, r.Width, r.Height);
		}
		void Draw(ImageTarget2D *target, Tiling2D::Type tiling, utils::Rectangle r) const {
			draw(*target, tiling, r.Left,r.Top, r.Width, r.Height);
		}





		//Draw partial
		void Draw(ImageTarget2D& target, float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) const {
			draw(target, X1,Y1, X2,Y2, X3,Y3, X4,Y4, S1,U1, S2,U2, S3,U3, S4,U4);
		}
		void Draw(ImageTarget2D* target, float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) const {
			draw(*target, X1,Y1, X2,Y2, X3,Y3, X4,Y4, S1,U1, S2,U2, S3,U3, S4,U4);
		}

		void Draw(ImageTarget2D& target, utils::Point2D p1, utils::Point2D p2, utils::Point2D p3, utils::Point2D p4, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) const {
			draw(target, p1.x,p1.y, p2.x,p2.y, p3.x,p3.y, p4.x,p4.y, S1,U1, S2,U2, S3,U3, S4,U4);
		}
		void Draw(ImageTarget2D* target, utils::Point2D p1, utils::Point2D p2, utils::Point2D p3, utils::Point2D p4, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) const {
			draw(*target, p1.x,p1.y, p2.x,p2.y, p3.x,p3.y, p4.x,p4.y, S1,U1, S2,U2, S3,U3, S4,U4);
		}

		void Draw(ImageTarget2D& target, float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4, utils::Point2D t1, utils::Point2D t2, utils::Point2D t3, utils::Point2D t4) const {
			draw(target, X1,Y1, X2,Y2, X3,Y3, X4,Y4, t1.x,t1.y, t2.x,t2.y, t3.x,t3.y, t4.x,t4.y);
		}
		void Draw(ImageTarget2D* target, float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4, utils::Point2D t1, utils::Point2D t2, utils::Point2D t3, utils::Point2D t4) const {
			draw(*target, X1,Y1, X2,Y2, X3,Y3, X4,Y4, t1.x,t1.y, t2.x,t2.y, t3.x,t3.y, t4.x,t4.y);
		}

		void Draw(ImageTarget2D& target, utils::Point2D p1, utils::Point2D p2, utils::Point2D p3, utils::Point2D p4, utils::Point2D t1, utils::Point2D t2, utils::Point2D t3, utils::Point2D t4) const {
			draw(target, p1.x,p1.y, p2.x,p2.y, p3.x,p3.y, p4.x,p4.y, t1.x,t1.y, t2.x,t2.y, t3.x,t3.y, t4.x,t4.y);
		}
		void Draw(ImageTarget2D* target, utils::Point2D p1, utils::Point2D p2, utils::Point2D p3, utils::Point2D p4, utils::Point2D t1, utils::Point2D t2, utils::Point2D t3, utils::Point2D t4) const {
			draw(*target, p1.x,p1.y, p2.x,p2.y, p3.x,p3.y, p4.x,p4.y, t1.x,t1.y, t2.x,t2.y, t3.x,t3.y, t4.x,t4.y);
		}

		void Draw(ImageTarget2D& target, float X,float Y,float W,float H, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) const {
			draw(target, X,Y, X+W,Y, X+W,Y+H, X,Y+H, S1,U1, S2,U2, S3,U3, S4,U4);
		}
		void Draw(ImageTarget2D* target, float X,float Y,float W,float H, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) const {
			draw(*target, X,Y, X+W,Y, X+W,Y+H, X,Y+H, S1,U1, S2,U2, S3,U3, S4,U4);
		}

		void Draw(ImageTarget2D& target, float X,float Y,float W,float H, utils::Point2D t1, utils::Point2D t2, utils::Point2D t3, utils::Point2D t4) const {
			draw(target, X,Y, X+W,Y, X+W,Y+H, X,Y+H, t1.x,t1.y, t2.x,t2.y, t3.x,t3.y, t4.x,t4.y);
		}
		void Draw(ImageTarget2D* target, float X,float Y,float W,float H, utils::Point2D t1, utils::Point2D t2, utils::Point2D t3, utils::Point2D t4) const {
			draw(*target, X,Y, X+W,Y, X+W,Y+H, X,Y+H, t1.x,t1.y, t2.x,t2.y, t3.x,t3.y, t4.x,t4.y);
		}

		void Draw(ImageTarget2D& target, utils::Point2D p,float W,float H, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) const {
			draw(target, p.x,p.y, p.x+W,p.y, p.x+W,p.y+H, p.x,p.y+H, S1,U1, S2,U2, S3,U3, S4,U4);
		}
		void Draw(ImageTarget2D* target, utils::Point2D p,float W,float H, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) const {
			draw(*target, p.x,p.y, p.x+W,p.y, p.x+W,p.y+H, p.x,p.y+H, S1,U1, S2,U2, S3,U3, S4,U4);
		}

		void Draw(ImageTarget2D& target, utils::Point2D p,float W,float H, utils::Point2D t1, utils::Point2D t2, utils::Point2D t3, utils::Point2D t4) const {
			draw(target, p.x,p.y, p.x+W,p.y, p.x+W,p.y+H, p.x,p.y+H, t1.x,t1.y, t2.x,t2.y, t3.x,t3.y, t4.x,t4.y);
		}
		void Draw(ImageTarget2D* target, utils::Point2D p,float W,float H, utils::Point2D t1, utils::Point2D t2, utils::Point2D t3, utils::Point2D t4) const {
			draw(*target, p.x,p.y, p.x+W,p.y, p.x+W,p.y+H, p.x,p.y+H, t1.x,t1.y, t2.x,t2.y, t3.x,t3.y, t4.x,t4.y);
		}

		void Draw(ImageTarget2D& target, float X,float Y,utils::Size2D s, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) const {
			draw(target, X,Y, X+s.Width,Y, X+s.Width,Y+s.Height, X,Y+s.Height, S1,U1, S2,U2, S3,U3, S4,U4);
		}
		void Draw(ImageTarget2D* target, float X,float Y,utils::Size2D s, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) const {
			draw(*target, X,Y, X+s.Width,Y, X+s.Width,Y+s.Height, X,Y+s.Height, S1,U1, S2,U2, S3,U3, S4,U4);
		}

		void Draw(ImageTarget2D& target, float X,float Y,utils::Size2D s, utils::Point2D t1, utils::Point2D t2, utils::Point2D t3, utils::Point2D t4) const {
			draw(target, X,Y, X+s.Width,Y, X+s.Width,Y+s.Height, X,Y+s.Height, t1.x,t1.y, t2.x,t2.y, t3.x,t3.y, t4.x,t4.y);
		}
		void Draw(ImageTarget2D* target, float X,float Y,utils::Size2D s, utils::Point2D t1, utils::Point2D t2, utils::Point2D t3, utils::Point2D t4) const {
			draw(*target, X,Y, X+s.Width,Y, X+s.Width,Y+s.Height, X,Y+s.Height, t1.x,t1.y, t2.x,t2.y, t3.x,t3.y, t4.x,t4.y);
		}

		void Draw(ImageTarget2D& target, utils::Point2D p,utils::Size2D s, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) const {
			draw(target, p.x,p.y, p.x+s.Width,p.y, p.x+s.Width,p.y+s.Height, p.x,p.y+s.Height, S1,U1, S2,U2, S3,U3, S4,U4);
		}
		void Draw(ImageTarget2D* target, utils::Point2D p,utils::Size2D s, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) const {
			draw(*target, p.x,p.y, p.x+s.Width,p.y, p.x+s.Width,p.y+s.Height, p.x,p.y+s.Height, S1,U1, S2,U2, S3,U3, S4,U4);
		}

		void Draw(ImageTarget2D& target, utils::Point2D p,utils::Size2D s, utils::Point2D t1, utils::Point2D t2, utils::Point2D t3, utils::Point2D t4) const {
			draw(target, p.x,p.y, p.x+s.Width,p.y, p.x+s.Width,p.y+s.Height, p.x,p.y+s.Height, t1.x,t1.y, t2.x,t2.y, t3.x,t3.y, t4.x,t4.y);
		}
		void Draw(ImageTarget2D* target, utils::Point2D p,utils::Size2D s, utils::Point2D t1, utils::Point2D t2, utils::Point2D t3, utils::Point2D t4) const {
			draw(*target, p.x,p.y, p.x+s.Width,p.y, p.x+s.Width,p.y+s.Height, p.x,p.y+s.Height, t1.x,t1.y, t2.x,t2.y, t3.x,t3.y, t4.x,t4.y);
		}





		int GetWidth() const {
			return getwidth();
		}
		int GetHeight() const {
			return getheight();
		}
		utils::Size	GetSize() const {
			return utils::Size(getwidth(), getheight());
		}



	protected:
		virtual void draw(ImageTarget2D& Target, float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4) const = 0;
		virtual void draw(ImageTarget2D& Target, float X1,float Y1,float X2,float Y2,float X3,float Y3,float X4,float Y4) const = 0;
		virtual void drawstretched(ImageTarget2D& Target, int X, int Y, int W, int H) const = 0;
		virtual void draw(ImageTarget2D& Target, Tiling2D::Type Tiling, int X, int Y, int W, int H) const = 0;
		virtual void draw(ImageTarget2D& Target, int X, int Y) const {
			drawstretched(Target, X,Y, getwidth(), getheight());
		}

		virtual int getwidth () const = 0;
		virtual int getheight() const = 0;
	};

	class Image2D : public virtual RectangularGraphic2D {
	public:
		virtual GLTexture &GetTexture() = 0;
	};

} }
