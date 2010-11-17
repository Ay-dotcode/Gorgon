#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/AnimationResource.h"
#include "../Engine/Animator.h"
#include "../Engine/Layer.h"
#include "../Engine/Graphics.h"
#include "../Engine/ResizableObject.h"
#include "WidgetMain.h"

using namespace gre;
using namespace gge;

namespace gorgonwidgets {

#define GID_LINE		0x05110000
#define	GID_LINE_PROPS	0x05110101

	class Line;


	ResourceBase *LoadLineResource(ResourceFile* File, FILE* Data, int Size);
	class LineResource : public ResourceBase {
		friend ResourceBase *LoadLineResource(ResourceFile* File, FILE* Data, int Size);
	public:
		LineResource();
		virtual int getGID() { return GID_LINE; }
		virtual bool Save(gre::ResourceFile * File, FILE * Data) { return true; }


		AnimationResource *animStart;
		AnimationResource *animLoop;
		AnimationResource *animEnd;


		bool isTiled;
		bool isVerticle;

		int Height;
		int Width;
	};


	class Line : public ResizableObject {
	public:
		LineResource *Parent;
		bool isVerticle;
		bool isTiled;
		bool DrawStart, DrawEnd;

		ImageAnimation *Start, *Loop, *End;

		Line(LineResource *parent);

		virtual void DrawResized(I2DGraphicsTarget *Target, int X, int Y, int W, int H, Alignment Align);
		virtual void Reset(bool Reverse=false);
		virtual void Reverse();
		virtual void Play();
		virtual void Pause();
		virtual void setLoop(bool Loop);
		virtual int getDuration() { return Loop->getDuration(); }

		virtual int  Width(int W=-1) { if(isVerticle) return Parent->Width; else return W; }
		virtual int  Height(int H=-1) { if(isVerticle) return H; else return Parent->Height; }
	};
}