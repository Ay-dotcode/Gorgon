#pragma once


#include "GRE.h"
#include "Base.h"
#include "../Engine/Graphics.h"
#include "ResizableObject.h"
#include "../Engine/Animation.h"
#include "../Engine/Image.h"
#include "../Engine/GGEMain.h"

#pragma warning(push)
#pragma warning(disable:4250)

namespace gge { namespace resource {

	class NullImage : 
		public Base, virtual public ResizableObject, public virtual graphics::ImageTexture, public ResizableObjectProvider, 
		public animation::RectangularGraphic2DSequenceProvider, virtual public animation::RectangularGraphic2DAnimation
	{
		friend void Init(GGEMain &Main);
	public:

		virtual GID::Type getGID() const { return GID::Image_NULL; }
		virtual void DeleteAnimation() { } //if used as animation, it will not be deleted
		virtual NullImage &CreateAnimation(animation::Timer &controller, bool owner=false) { return *this; }
		virtual NullImage &CreateAnimation(bool create=false) { return *this; }

		virtual NullImage &CreateResizableObject(animation::Timer &controller, bool owner=false) { return *this; }
		virtual NullImage &CreateResizableObject(bool create=false) { return *this; }

		virtual graphics::RectangularGraphic2D &GraphicAt(unsigned time) { return *this; }

		virtual graphics::Image2D & ImageAt(int time) { return *this; }

		virtual int GetDuration() const	{ return 1; }
		virtual int GetDuration(unsigned Frame) const { return 1; }
		virtual int GetNumberofFrames() const { return 1; }

		//Caller is responsible to supply a time between 0 and GetDuration()-1, if no frame exists it should return -1
		virtual int		 FrameAt(unsigned Time) const { return 0; }
		//Should always return a time between 0 and GetDuration unless Frame does not exists it should return -1
		virtual int		 StartOf(unsigned Frame) const { return 0; }
		virtual	int		 EndOf(unsigned Frame) const { return 1; }

		static NullImage &Get() {
			return *ni;
		}

	protected:
		NullImage() : animation::Base() {  }

		static NullImage *ni;

		virtual animation::ProgressResult::Type Progress() { return animation::ProgressResult::None; };

		virtual void drawin(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) const
		{  } 

		virtual void drawin(graphics::ImageTarget2D& Target, const graphics::SizeController2D &controller, int X, int Y, int W, int H)  const
		{  };

		virtual int calculatewidth (int w=-1) const { return w; }
		virtual int calculateheight(int h=-1) const { return h; }

		virtual int calculatewidth (const graphics::SizeController2D &controller, int w=-1) const  { return controller.CalculateWidth(w, 1); }
		virtual int calculateheight(const graphics::SizeController2D &controller, int h=-1) const  { return controller.CalculateHeight(h, 1); }

		virtual void draw(graphics::ImageTarget2D& Target, int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4, float S1,float U1, float S2,float U2,float S3,float U3,float S4,float U4)  const
		{  }
		virtual void draw(graphics::ImageTarget2D& Target, int X1,int Y1,int X2,int Y2,int X3,int Y3,int X4,int Y4)  const
		{  }
		virtual void drawstretched(graphics::ImageTarget2D& Target, int X, int Y, int W, int H) const
		{  }
		virtual void draw(graphics::ImageTarget2D& Target, graphics::Tiling2D::Type Tiling, int X, int Y, int W, int H) const
		{  }
		virtual void draw(graphics::ImageTarget2D& Target, graphics::SizeController2D &controller, int X, int Y, int W, int H) const
		{  }

		virtual int getwidth () const { return 1; }
		virtual int getheight() const { return 1; }
	};


}}
#pragma warning(pop)