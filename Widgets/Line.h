#pragma once

#include "../Resource/ResourceBase.h"
#include "../Resource/AnimationResource.h"
#include "../Engine/Animator.h"
#include "../Engine/Layer.h"
#include "../Engine/Graphics.h"
#include "../Resource/ResizableObject.h"
#include "../Utils/Margins.h"
#include "WidgetMain.h"

namespace gge { namespace widgets {

#define GID_LINE		0x05110000
#define	GID_LINE_PROPS	0x05110101

	class Line;


	resource::ResourceBase *LoadLineResource(resource::ResourceFile* File, FILE* Data, int Size);
	class LineResource : public resource::ResourceBase {
		friend ResourceBase *LoadLineResource(resource::ResourceFile* File, FILE* Data, int Size);
	public:
		LineResource();
		virtual int getGID() { return GID_LINE; }
		virtual bool Save(resource::ResourceFile * File, FILE * Data) { return true; }


		resource::AnimationResource *animStart;
		resource::AnimationResource *animLoop;
		resource::AnimationResource *animEnd;


		bool isTiled;
		bool isVertical;

		int Height;
		int Width;
	};


	class Line : public resource::ResizableObject {
	public:
		LineResource *Parent;
		bool isVertical;
		bool isTiled;
		bool DrawStart, DrawEnd;

		resource::ResizableObject::Tiling Tiling;
		resource::ResizableObject::IntegralSize IntegralSize;


		resource::ImageAnimation *Start, *Loop, *End;

		Line(LineResource *parent) : Tiling(resource::ResizableObject::Single) { SetParent(parent); }
		Line(LineResource &parent) : Tiling(resource::ResizableObject::Single) { SetParent(parent); }

		Line(LineResource *parent, resource::ResizableObject::Tiling Tiling, resource::ResizableObject::IntegralSize IntegralSize ) 
		{ SetParent(parent);  SetResizingOptions(Tiling, IntegralSize); }

		void SetParent(LineResource *parent)  ;
		void SetParent(LineResource &parent)  { SetParent(&parent); }

		void SetResizingOptions( resource::ResizableObject::Tiling Tiling, resource::ResizableObject::IntegralSize IntegralSize ) {
			this->Tiling=Tiling;
			this->IntegralSize=IntegralSize;
		}

		virtual void DrawResized(graphics::I2DGraphicsTarget *Target, int X, int Y, int W, int H, Alignment Align=ALIGN_MIDDLE_CENTER);
		virtual void DrawResized(graphics::I2DGraphicsTarget &Target, int X, int Y, int W, int H, Alignment Align=ALIGN_MIDDLE_CENTER)
		{ DrawResized(&Target, X,Y, W,H, Align); }
		virtual void Reset(bool Reverse=false);
		virtual void Reverse();
		virtual void Play();
		virtual void Pause();
		virtual void setLoop(bool Loop);
		virtual int getDuration() { return Loop->getDuration(); }

		virtual int  Width(int W=-1) {
			if(isVertical) 
				return Tiling.Calculate(Parent->Width, W); 
			else 
				return IntegralSize.Calculate(
					W								 , 
					(DrawStart ? Start->Width() : 0)+
					(DrawEnd   ? End->Width() : 0)  ,
					Loop->Width()
				); 
		}

		virtual int  Height(int H=-1) {
			if(isVertical) 
				return IntegralSize.Calculate(
					H								 , 
					(DrawStart ? Start->Height() : 0)+
					(DrawEnd   ? End->Height() : 0)  ,
					Loop->Height()
				); 
			else 
				return Tiling.Calculate(Parent->Height, H); 
		}

		virtual Margins getBorderWidth() {
			if(isVertical) 
				return Margins(0, (DrawStart ? Start->Height() : 0), 0, (DrawEnd   ? End->Height() : 0));
			else
				return Margins((DrawStart ? Start->Width() : 0), 0, (DrawEnd   ? End->Width() : 0), 0);
		}
	};
} }
