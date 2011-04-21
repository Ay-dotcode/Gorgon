#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/AnimationResource.h"
#include "../Engine/Layer.h"
#include "../Resource/ResizableObject.h"
#include "../Utils/Margins.h"

namespace gge { namespace widgets {

#define GID_RECT		0x05120000
#define	GID_RECT_PROPS	0x05120101

	resource::ResourceBase *LoadRectangleResource(resource::ResourceFile* File, FILE* Data, int Size);

	class RectangleResource : public resource::ResourceBase
	{
		friend resource::ResourceBase *LoadRectangleResource(resource::ResourceFile* File, FILE* Data, int Size);
		friend class ResizableRect;
	public:
		virtual int getGID() { return GID_RECT; }
		RectangleResource();

		virtual bool Save(resource::ResourceFile * File, FILE * Data) { return true; }

		resource::AnimationResource *animTL;
		resource::AnimationResource *animT;
		resource::AnimationResource *animTR;
		resource::AnimationResource *animL;
		resource::AnimationResource *animC;
		resource::AnimationResource *animR;
		resource::AnimationResource *animBL;
		resource::AnimationResource *animB;
		resource::AnimationResource *animBR;

		bool TileT,
			 TileB,
			 TileL,
			 TileR,
			 TileCH,
			 TileCV;

		int getTopHeight() { return topheight; }
		int getBottomHeight() { return bottomheight; }
		int getLeftWidth() { return leftwidth; }
		int getRightWidth() { return rightwidth; }

		Margins getBorderWidth() { return Margins(leftwidth, topheight, rightwidth, bottomheight); }

	protected:
		int topheight;
		int bottomheight;
		int leftwidth;
		int rightwidth;

		int TLy,TLx;
		int Ty;
		int TRy;

		int Lx;

		int BLx;

		bool centeronly;
	};

	class ResizableRect : public resource::ResizableObject {
	public:
		RectangleResource *Parent;
		bool TileT,
			 TileB,
			 TileL,
			 TileR,
			 TileCH,
			 TileCV;

		resource::ImageAnimation *animTL;
		resource::ImageAnimation *animT;
		resource::ImageAnimation *animTR;
		resource::ImageAnimation *animL;
		resource::ImageAnimation *animC;
		resource::ImageAnimation *animR;
		resource::ImageAnimation *animBL;
		resource::ImageAnimation *animB;
		resource::ImageAnimation *animBR;

		ResizableRect(RectangleResource *parent) { init(parent); }
		ResizableRect(RectangleResource &parent) { init(&parent); }

		ResizableRect(RectangleResource *parent, ResizableObject::IntegralSize HSizing, ResizableObject::IntegralSize VSizing) { 
			init(parent); 
			SetResizingOptions(HSizing,VSizing);
		}
		ResizableRect(RectangleResource &parent, ResizableObject::IntegralSize HSizing, ResizableObject::IntegralSize VSizing) { 
			init(&parent); 
			SetResizingOptions(HSizing,VSizing);
		}

		virtual void DrawResized(graphics::I2DGraphicsTarget *Target, int X, int Y, int W, int H, Alignment Align=ALIGN_MIDDLE_CENTER);
		virtual void DrawResized(graphics::I2DGraphicsTarget &Target, int X, int Y, int W, int H, Alignment Align=ALIGN_MIDDLE_CENTER) { DrawResized(&Target, X,Y, W,H, Align); }
		virtual void DrawAround(graphics::I2DGraphicsTarget *Target, int X, int Y, int W, int H);
		virtual void Reset(bool Reverse=false);
		virtual void Reverse();
		virtual void Play();
		virtual void Pause();
		virtual void setLoop(bool Loop);
		virtual int  Width(int W=-1) { return W; }
		virtual int  Height(int H=-1) { return H; }

		virtual int getDuration() { if(animC) { return animC->getDuration(); } else { return animT->getDuration(); } }


		ResizableObject::IntegralSize HSizing;
		ResizableObject::IntegralSize VSizing;

		void SetResizingOptions( ResizableObject::IntegralSize HSizing, ResizableObject::IntegralSize VSizing ) {
			this->HSizing=HSizing;
			this->VSizing=VSizing;
		}

		bool CenterOnly;

		virtual Margins getBorderWidth() { return Parent->getBorderWidth(); }

	protected:

		void init(gge::widgets::RectangleResource *parent);
	};
} }
