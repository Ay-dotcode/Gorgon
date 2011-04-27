#include "Rectangle.h"
#include "../Resource/ResourceFile.h"
#include <stdexcept>

using namespace gge::resource;
using std::max;

namespace gge { namespace widgets {
	ResourceBase* LoadRectangleResource(File* file,FILE* gfile,int sz) {
		RectangleResource *Rect =new RectangleResource();

		int gid,size,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				Rect->guid.Load(gfile);
				break;
			case GID_RECT_PROPS:
				int t[6];

				fread(t,4,6,gfile);

				Rect->TileCH=t[0]!=0;
				Rect->TileCV=t[1]!=0;
				Rect->TileT=t[2]!=0;
				Rect->TileB=t[3]!=0;
				Rect->TileL=t[4]!=0;
				Rect->TileR=t[5]!=0;

				if(size>24)
					EatChunk(gfile,size-24);

				break;
			case GID_ANIMATION:
				AnimationResource *tmp;
				tmp=(AnimationResource*)LoadAnimationResource(file,gfile,size);
				Rect->Subitems.Add(tmp);
				switch(cnt) {
			case 0:
				Rect->animT=tmp;
				break;
			case 1:
				Rect->animTL=tmp;
				break;
			case 2:
				Rect->animTR=tmp;
				break;
			case 3:
				Rect->animC=tmp;
				break;
			case 4:
				Rect->animL=tmp;
				break;
			case 5:
				Rect->animR=tmp;
				break;
			case 6:
				Rect->animB=tmp;
				break;
			case 7:
				Rect->animBL=tmp;
				break;
			case 8:
				Rect->animBR=tmp;
				break;
				}

				cnt++;
				break;
			default:
				EatChunk(gfile,size);
				}
		}

		Rect->topheight=max(max(Rect->animTL->getHeight(), Rect->animT->getHeight()), Rect->animTR->getHeight());
		Rect->bottomheight=max(max(Rect->animBL->getHeight(), Rect->animB->getHeight()), Rect->animBR->getHeight());
		Rect->leftwidth=max(max(Rect->animTL->getWidth(), Rect->animL->getWidth()), Rect->animBL->getWidth());
		Rect->rightwidth=max(max(Rect->animTR->getWidth(), Rect->animR->getWidth()), Rect->animBR->getWidth());

		Rect->TLx=Rect->leftwidth-Rect->animTL->getWidth();
		Rect->TLy=Rect->topheight-Rect->animTL->getHeight();
		Rect->Ty=0;//Rect->topheight-Rect->animT->getHeight();
		Rect->TRy=Rect->topheight-Rect->animTR->getHeight();
		Rect->Lx=0;//Rect->leftwidth-Rect->animL->getWidth();
		Rect->BLx=0;//Rect->leftwidth-Rect->animBL->getWidth();

		Rect->centeronly=(Rect->animT->getFrameCount()+Rect->animTL->getFrameCount()+Rect->animTR->getFrameCount()+
			Rect->animL->getFrameCount()+Rect->animR->getFrameCount()+
			Rect->animB->getFrameCount()+Rect->animBL->getFrameCount()+Rect->animBR->getFrameCount()==0);

		return Rect;
	}

	RectangleResource::RectangleResource() {
		animTL=NULL;
		animT=NULL;
		animTR=NULL;
		animL=NULL;
		animC=NULL;
		animR=NULL;
		animBL=NULL;
		animB=NULL;
		animBR=NULL;

		TileT=false;
		TileB=false;
		TileL=false;
		TileR=false;
		TileCH=false;
		TileCV=false;
	}

	void ResizableRect::init(gge::widgets::RectangleResource *parent) {
		if(parent==NULL)
			throw std::runtime_error("Supplied Resizable Rectangle parent is Null");
		Parent=parent;
		TileT=parent->TileT;
		TileB=parent->TileB;
		TileL=parent->TileL;
		TileR=parent->TileR;
		TileCH=parent->TileCH;
		TileCV=parent->TileCV;

		animTL=parent->animTL->getAnimation();
		animT=parent->animT->getAnimation();
		animTR=parent->animTR->getAnimation();
		animBL=parent->animBL->getAnimation();
		animB=parent->animB->getAnimation();
		animBR=parent->animBR->getAnimation();
		animL=parent->animL->getAnimation();
		animC=parent->animC->getAnimation();
		animR=parent->animR->getAnimation();

		CenterOnly=parent->centeronly;
	}

	void ResizableRect::DrawAround(graphics::I2DGraphicsTarget *Target, int X, int Y, int W, int H) {
		DrawResized(Target, X-Parent->leftwidth, Y-Parent->topheight, W+Parent->leftwidth+Parent->rightwidth, H+Parent->topheight+Parent->bottomheight, ALIGN_MIDDLE_CENTER);
	}

	void ResizableRect::DrawResized(graphics::I2DGraphicsTarget *Target, int X, int Y, int W, int H, Alignment Align) {


		if(CenterOnly) {
			int w=HSizing.Calculate(W, 0, animC->Width());
			int h=VSizing.Calculate(H, 0, animC->Height());
			
			animC->DrawResized(graphics::Buffered2DGraphic::Tiling(TileCH,TileCV), Target, X,Y, w,h);

			return;
		}

		{
			int w=HSizing.Calculate(W, Parent->leftwidth+Parent->rightwidth, animC->Width());
			int h=VSizing.Calculate(H, Parent->topheight+Parent->bottomheight, animC->Height());

			if( (Align&ALIGN_MASK_HORIZONTAL) ==ALIGN_CENTER)
				X+=(W-w)/2;
			else if( (Align&ALIGN_MASK_HORIZONTAL) ==ALIGN_RIGHT)
				X+=(W-w);

			if( (Align&ALIGN_MASK_VERTICAL) ==ALIGN_MIDDLE)
				Y+=(H-h)/2;
			else if( (Align&ALIGN_MASK_VERTICAL) ==ALIGN_BOTTOM)
				Y+=(H-h);

			W=w;
			H=h;
		}

		int x=X,y=Y;


		x+=Parent->TLx;
		animTL->Draw(Target, x, y+Parent->TLy);
		x=X+Parent->leftwidth;
		if(TileT)
			animT->DrawHTiled(Target, x, y+Parent->Ty, W-(Parent->leftwidth+Parent->rightwidth), Parent->animT->getHeight());
		else
			animT->Draw(Target, x, y+Parent->Ty, W-(Parent->leftwidth+Parent->rightwidth), Parent->animT->getHeight());

		animTR->Draw(Target, X+W-Parent->rightwidth, y+Parent->TRy);



		x=X+Parent->Lx;
		y=Y+Parent->topheight;
		if(TileL)
			animL->DrawVTiled(Target, x, y, Parent->animL->getWidth(), H-(Parent->topheight+Parent->bottomheight));
		else
			animL->Draw(Target, x, y, Parent->animL->getWidth(), H-(Parent->topheight+Parent->bottomheight));
		
		x=X+Parent->Lx+Parent->animL->getWidth();
		y=Y+Parent->animT->getHeight();
		int w=W-(Parent->animL->getWidth()+Parent->animR->getWidth());
		int h=H-(Parent->animT->getHeight()+Parent->animB->getHeight());
		if(TileCH) {
			if(TileCV) {
				animC->DrawTiled(Target, x, y, w, h);
			} else {
				animC->DrawHTiled(Target, x, y, w, h);
			}
		} else {
			if(TileCV) {
				animC->DrawVTiled(Target, x, y, w, h);
			} else {
				animC->Draw(Target, x, y, w, h);
			}
		}

		x=X+W-Parent->rightwidth;
		y=Y+Parent->topheight;
		if(TileL)
			animR->DrawVTiled(Target, x, y, Parent->animR->getWidth(), H-(Parent->animT->getHeight()+Parent->animB->getHeight()));
		else
			animR->Draw(Target, x, y, Parent->animR->getWidth(), H-(Parent->animT->getHeight()+Parent->animB->getHeight()));



		x=X;
		y=Y+H-Parent->animB->getHeight();
		animBL->Draw(Target, x, y);
		x=X+Parent->animBL->getWidth();
		if(TileB)
			animB->DrawHTiled(Target, x, y, W-(Parent->animBL->getWidth()+Parent->animBR->getWidth()), Parent->animB->getHeight());
		else
			animB->Draw(Target, x, y, W-(Parent->animBL->getWidth()+Parent->animBR->getWidth()), Parent->animB->getHeight());

		animBR->Draw(Target, X+W-Parent->rightwidth, y);
	}

	void ResizableRect::Reset(bool Reverse) {
		animTL->Reset(Reverse);
		animT->Reset(Reverse);
		animTR->Reset(Reverse);

		animL->Reset(Reverse);
		animC->Reset(Reverse);
		animR->Reset(Reverse);

		animBL->Reset(Reverse);
		animB->Reset(Reverse);
		animBR->Reset(Reverse);
	}
	void ResizableRect::Reverse() {
		animTL->Reverse();
		animT->Reverse();
		animTR->Reverse();

		animL->Reverse();
		animC->Reverse();
		animR->Reverse();

		animBL->Reverse();
		animB->Reverse();
		animBR->Reverse();
	}
	void ResizableRect::Play() {
		animTL->Play();
		animT->Play();
		animTR->Play();

		animL->Play();
		animC->Play();
		animR->Play();

		animBL->Play();
		animB->Play();
		animBR->Play();
	}
	void ResizableRect::Pause() {
		animTL->Pause();
		animT->Pause();
		animTR->Pause();

		animL->Pause();
		animC->Pause();
		animR->Pause();

		animBL->Pause();
		animB->Pause();
		animBR->Pause();
	}
	void ResizableRect::setLoop(bool Loop) {
		animTL->setLoop(Loop);
		animT->setLoop(Loop);
		animTR->setLoop(Loop);

		animL->setLoop(Loop);
		animC->setLoop(Loop);
		animR->setLoop(Loop);

		animBL->setLoop(Loop);
		animB->setLoop(Loop);
		animBR->setLoop(Loop);
	}
} }
