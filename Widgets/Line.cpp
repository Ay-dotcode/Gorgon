#include "Line.h"
#include "../Resource/ResourceFile.h"

namespace gorgonwidgets {

	LineResource::LineResource(void) {
		animStart=NULL;
		animLoop=NULL;
		animEnd=NULL;

		isTiled=false;
		isVerticle=false;
	}

	Line::Line(gorgonwidgets::LineResource *parent) {
		Parent=parent; 
		Start=parent->animStart->getAnimation();
		Loop=parent->animLoop->getAnimation();
		End=parent->animEnd->getAnimation();
		this->isVerticle=parent->isVerticle;
		isTiled=parent->isTiled;
		DrawStart=true;
		DrawEnd=true;
	}

	void Line::DrawResized(I2DGraphicsTarget *Target, int X, int Y, int W, int H, Alignment Align) {
		if(isVerticle) {
			if(Align & ALIGN_CENTER)
				X+=(W-Parent->Width)/2;
			else if(Align & ALIGN_RIGHT)
				X+= W-Parent->Width;

			if(DrawStart)
				Start->Draw(Target, X, Y);
			if(DrawEnd) {
				H-=Parent->animEnd->getHeight();
				End->Draw(Target, X, Y+H);
			}
			if(DrawStart) {
				H-=Parent->animStart->getHeight(); //this is correct
				Y+=Parent->animStart->getHeight();
			}

			if(H>0) {
				if(isTiled)
					Loop->DrawTiled(Target, X, Y, Parent->animLoop->getWidth(), H);
				else
					Loop->Draw(Target, X, Y, Parent->animLoop->getWidth(), H);
			}
		} else {
			if(Align & ALIGN_MIDDLE)
				Y+=(H-Parent->Height)/2;
			else if(Align & ALIGN_BOTTOM)
				Y+= H-Parent->Height;

			if(DrawStart)
				Start->Draw(Target, X, Y);
			if(DrawEnd) {
				W-=Parent->animEnd->getWidth();
				End->Draw(Target, X+W, Y);
			}
			if(DrawStart) {
				W-=Parent->animStart->getWidth(); //this is correct
				X+=Parent->animStart->getWidth();
			}

			if(H>0) {
				if(isTiled)
					Loop->DrawTiled(Target, X, Y, W, Parent->animLoop->getHeight());
				else
					Loop->Draw(Target, X, Y, W, Parent->animLoop->getHeight());
			}
		}
	}

	ResourceBase* LoadLineResource(ResourceFile* file,FILE* gfile,int sz) {
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
					line->isVerticle=t;
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
}