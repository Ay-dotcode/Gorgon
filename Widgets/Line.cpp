#include "Line.h"
#include "../Resource/ResourceFile.h"
#include <stdexcept>

using namespace gge::resource;
using std::max;


namespace gge { namespace widgets {

	LineResource::LineResource(void) {
		animStart=NULL;
		animLoop=NULL;
		animEnd=NULL;

		isTiled=false;
		isVertical=false;
	}

	void Line::SetParent(LineResource* parent) {
#ifdef _DEBUG
		if(!parent->animEnd || !parent->animLoop || !parent->animStart)
			throw std::runtime_error("Incomplete line resource definition.");
#endif
		Parent=parent; 
		Start=parent->animStart->getAnimation();
		Loop=parent->animLoop->getAnimation();
		End=parent->animEnd->getAnimation();
		this->isVertical=parent->isVertical;
		isTiled=parent->isTiled;
		DrawStart=true;
		DrawEnd=true;
	}


	void Line::DrawResized(graphics::I2DGraphicsTarget *Target, int X, int Y, int W, int H, Alignment Align) {

	//Vertical
		if(isVertical) { 
			int h=IntegralSize.Calculate(
				H,
					(DrawStart ? Start->Height() : 0)+
					(DrawEnd   ? End->Height() : 0),
				Loop->Height()
			);

			int ow=Parent->Width;
			int w=Tiling.Calculate(ow, W);

			if(Align & ALIGN_MIDDLE)
				Y+=(H-h)/2;
			else if(Align & ALIGN_BOTTOM)
				Y+= H-h;

			if(Align & ALIGN_CENTER)
				X+=(W-w)/2;
			else if(Align & ALIGN_RIGHT)
				X+= W-w;

			if(w==ow) { //normal draw
				if(DrawStart)
					Start->Draw(Target, X, Y);
				if(DrawEnd) {
					h-=End->Height();
					End->Draw(Target, X, Y+h);
				}
				if(DrawStart) {
					h-=Start->Height(); //this is correct
					Y+=Start->Height();
				}

				if(H>0) {
					if(isTiled)
						Loop->DrawTiled(Target, X,Y , ow,h);
					else
						Loop->Draw(Target, X, Y, Loop->Width(), h);
				}

			} else if(Tiling.Type==ResizableObject::Stretch) { //stretch draw
				if(DrawStart)
					Start->Draw(Target, X,Y, w,Start->Height());
				if(DrawEnd) {
					h-=End->Height();
					End->Draw(Target, X,Y+h, w,End->Height());
				}
				if(DrawStart) {
					h-=Start->Height();
					Y+=Start->Height();
				}

				if(H>0) {
					if(isTiled)
						Loop->DrawVTiled(Target, X,Y, w,h);
					else
						Loop->Draw(Target, X,Y, w,h);
				}

			} else { //tile draw
				if(DrawStart)
					Start->DrawTiled(Target, X,Y, w,Start->Height());
				if(DrawEnd) {
					h-=End->Height();
					End->DrawTiled(Target, X,Y+h, w,End->Height());
				}
				if(DrawStart) {
					h-=Start->Height(); //this is correct
					Y+=Start->Height();
				}

				if(w>0 && h>0) {
					if(isTiled)
						Loop->DrawTiled(Target, X,Y, w,h);
					else
						Loop->DrawHTiled(Target, X,Y, w,h);
				}
			}



	//Horizontal
		} else { 

			int w=IntegralSize.Calculate(
				W,
					(DrawStart ? Start->Width() : 0)+
					(DrawEnd   ? End->Width() : 0),
				IntegralSize.Increment=Loop->Width()
			);
			int oh=Parent->Height;
			int h=Tiling.Calculate(oh, H);

			if(Align & ALIGN_CENTER)
				X+=(W-w)/2;
			else if(Align & ALIGN_RIGHT)
				X+= W-w;

			if(Align & ALIGN_MIDDLE)
				Y+=(H-h)/2;
			else if(Align & ALIGN_BOTTOM)
				Y+= H-h;

			if(h==oh) { //normal draw
				if(DrawStart)
					Start->Draw(Target, X,Y);
				if(DrawEnd) {
					w-=End->Width();
					End->Draw(Target, X+w,Y);
				}
				if(DrawStart) {
					w-=Start->Width(); //this is correct
					X+=Start->Width();
				}

				if(H>0) {
					if(isTiled)
						Loop->DrawTiled(Target, X,Y, w,Loop->Height());
					else
						Loop->Draw(Target, X,Y, w,Loop->Height());
				}

			} else if(Tiling.Type==ResizableObject::Stretch) { //stretch draw
				if(DrawStart)
					Start->Draw(Target, X,Y, Start->Width(),h);
				if(DrawEnd) {
					w-=End->Width();
					End->Draw(Target, X+w,Y, End->Width(),h);
				}
				if(DrawStart) {
					w-=Start->Width(); //this is correct
					X+=Start->Width();
				}

				if(H>0) {
					if(isTiled)
						Loop->DrawHTiled(Target, X,Y, w,h);
					else
						Loop->Draw(Target, X,Y, w,h);
				}

			} else { //tile draw
				if(DrawStart)
					Start->DrawTiled(Target, X,Y, Start->Width(),h);
				if(DrawEnd) {
					w-=End->Width();
					End->DrawTiled(Target, X+w,Y, End->Width(),h);
				}
				if(DrawStart) {
					w-=Start->Width(); //this is correct
					X+=Start->Width();
				}

				if(w>0 && h>0) {
					if(isTiled)
						Loop->DrawTiled(Target, X,Y, w,h);
					else
						Loop->DrawVTiled(Target, X,Y, w,h);
				}
			}
		}
	}

	resource::ResourceBase* LoadLineResource(resource::ResourceFile* file,FILE* gfile,int sz) {
		LineResource *line =new LineResource();

		int gid,size,cnt=0;

		int tpos=ftell(gfile)+sz;
		while(ftell(gfile)<tpos) {
			fread(&gid,4,1,gfile);
			fread(&size,4,1,gfile);

			switch(gid) {
			case GID_GUID:
				line->guid=new Guid(gfile);
				break;
			case GID_LINE_PROPS:
				int t;
				fread(&t,4,1,gfile);

				line->isTiled=t!=0;
				if(size>4) {
					fread(&t,4,1,gfile);
					line->isVertical=t;
					if(size>8)
						EatChunk(gfile,size-8);
				}

				break;
			case GID_ANIMATION:
				AnimationResource *tmp;
				tmp=(AnimationResource*)LoadAnimationResource(file,gfile,size);
				line->Subitems.Add(tmp);
				switch(cnt) {
				case 0:
					line->animStart=tmp;
					break;
				case 1:
					line->animLoop=tmp;
					break;
				case 2:
					line->animEnd=tmp;
					break;
				}

				cnt++;
				break;
			default:
				EatChunk(gfile,size);
			}
		}

		line->Height=max(max(line->animStart->getHeight(), line->animLoop->getHeight()), line->animEnd->getHeight());
		line->Width=max(max(line->animStart->getWidth(), line->animLoop->getWidth()), line->animEnd->getWidth());

		return line;
	}
	void Line::Reset(bool Reverse) {
		Start->Reset(Reverse);
		Loop->Reset(Reverse);
		End->Reset(Reverse);
	}
	void Line::Reverse() {
		Start->Reverse();
		Loop->Reverse();
		End->Reverse();
	}
	void Line::Play() {
		Start->Play();
		Loop->Play();
		End->Play();
	}
	void Line::Pause() {
		Start->Pause();
		Loop->Pause();
		End->Pause();
	}
	void Line::setLoop(bool Loop) {
		Start->setLoop(Loop);
		this->Loop->setLoop(Loop);
		End->setLoop(Loop);
	}
} }
