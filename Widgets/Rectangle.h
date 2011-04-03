#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/AnimationResource.h"
#include "../Engine/Layer.h"
#include "../Resource/ResizableObject.h"
#include "../Utils/Margins.h"

using namespace gre;
using namespace gge;

namespace gge { namespace widgets {

#define GID_RECT		0x05120000
#define	GID_RECT_PROPS	0x05120101

	ResourceBase *LoadRectangleResource(ResourceFile* File, FILE* Data, int Size);

	class RectangleResource : public ResourceBase
	{
		friend ResourceBase *LoadRectangleResource(ResourceFile* File, FILE* Data, int Size);
		friend class ResizableRect;
	public:
		virtual int getGID() { return GID_RECT; }
		RectangleResource();

		virtual bool Save(gre::ResourceFile * File, FILE * Data) { return true; }

		AnimationResource *animTL;
		AnimationResource *animT;
		AnimationResource *animTR;
		AnimationResource *animL;
		AnimationResource *animC;
		AnimationResource *animR;
		AnimationResource *animBL;
		AnimationResource *animB;
		AnimationResource *animBR;

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

	class ResizableRect : public ResizableObject {
	public:
		RectangleResource *Parent;
		bool TileT,
			 TileB,
			 TileL,
			 TileR,
			 TileCH,
			 TileCV;

		ImageAnimation *animTL;
		ImageAnimation *animT;
		ImageAnimation *animTR;
		ImageAnimation *animL;
		ImageAnimation *animC;
		ImageAnimation *animR;
		ImageAnimation *animBL;
		ImageAnimation *animB;
		ImageAnimation *animBR;

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

		virtual void DrawResized(I2DGraphicsTarget *Target, int X, int Y, int W, int H, Alignment Align=ALIGN_MIDDLE_CENTER);
		virtual void DrawResized(I2DGraphicsTarget &Target, int X, int Y, int W, int H, Alignment Align=ALIGN_MIDDLE_CENTER) { DrawResized(&Target, X,Y, W,H, Align); }
		virtual void DrawAround(I2DGraphicsTarget *Target, int X, int Y, int W, int H);
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
