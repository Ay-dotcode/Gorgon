#pragma once

#include "GRE.h"
#include "ResourceBase.h"
#include "../Engine/Animator.h"
#include "ImageResource.h"
#include "../Engine/ResizableObject.h"

namespace gre {
	class ResourceFile;
	
	////This function loads a text resource from the given file
	ResourceBase *LoadAnimationResource(ResourceFile* File, FILE* Data, int Size);

	class AnimationResource;

	////This class draws an animated image
	class ImageAnimation : public DiscreteAnimatorBase, public Buffered2DGraphic, public ResizableObject {
		friend class AnimationResource;
	public:
	protected:
		AnimationResource *parent;
		virtual int* FrameDurations();
		virtual void ProcessFrame(int frame);

	public:
		ImageAnimation(AnimationResource *parent);

		virtual void Reset(bool Reverse=false) {
			Pause();
			GotoFrame(0);
			if(Reverse)
				GoBackwards();
			else
				GoForwards();
		}
		virtual void Reverse() {
			if(isBackwards())
				GoForwards();
			else
				GoBackwards();
		}
		virtual void Play() { DiscreteAnimatorBase::Play(); }
		virtual void Pause() { DiscreteAnimatorBase::Pause(); }
		virtual void setLoop(bool Loop) { islooping=Loop; }
		virtual int getDuration() { return DiscreteAnimatorBase::duration; }
		virtual void Draw(I2DGraphicsTarget *Target,int X,int Y) { 
			Draw(Target, X, Y, Texture.W, Texture.H);
		}
		void Draw(I2DGraphicsTarget &Target,int X,int Y) { Draw(&Target, X,Y); }

		virtual void Draw(I2DGraphicsTarget *Target,int X,int Y,int W,int H) { 
			if(Texture.ID)
				Buffered2DGraphic::Draw(Target, X, Y, W, H);
		}
		void Draw(I2DGraphicsTarget &Target,int X,int Y,int W,int H) { Draw(&Target, X,Y, W,H); }

		virtual void Draw(I2DGraphicsTarget *Target,int X1,int Y1, int X2,int Y2, int X3,int Y3, int X4, int Y4) { 
			if(Texture.ID)
				Buffered2DGraphic::Draw(Target, X1, Y1, X2, Y2, X3, Y3, X4, Y4);
		}
		void Draw(I2DGraphicsTarget &Target,int X1,int Y1, int X2,int Y2, int X3,int Y3, int X4, int Y4) { Draw(&Target, X1,Y1, X2,Y2, X3,Y3, X4,Y4); }

		virtual void DrawTiled(I2DGraphicsTarget *Target,int X,int Y, int W, int H) { 
			if(Texture.ID)
				Buffered2DGraphic::DrawTiled(Target, X, Y, W, H);
		}
		void DrawTiled(I2DGraphicsTarget &Target,int X,int Y, int W, int H) { DrawTiled(&Target, X,Y, W,H); }

		virtual void DrawHTiled(I2DGraphicsTarget *Target,int X,int Y, int W, int H) { 
			if(Texture.ID)
				Buffered2DGraphic::DrawHTiled(Target, X, Y, W, H);
		}
		void DrawHTiled(I2DGraphicsTarget &Target,int X,int Y, int W, int H) { DrawHTiled(&Target, X,Y, W,H); }

		virtual void DrawVTiled(I2DGraphicsTarget *Target,int X,int Y, int W, int H) { 
			if(Texture.ID)
				Buffered2DGraphic::DrawVTiled(Target, X, Y, W, H);
		}
		void DrawVTiled(I2DGraphicsTarget &Target,int X,int Y, int W, int H) { DrawVTiled(&Target, X,Y, W,H); }


		virtual int  Width(int W=-1);
		virtual int  Height(int H=-1);

	public:
		virtual void DrawResized(I2DGraphicsTarget *Target, int X, int Y, int W, int H, Alignment Align);
		virtual void DrawResized(I2DGraphicsTarget &Target, int X, int Y, int W, int H, Alignment Align) { DrawResized(&Target, X,Y ,W,H, Align); }
	};

	////This is basic text resource, it holds a simple string. This resource is mostly useless
	/// because of data array resource. Still it can be used to hold a long text which is not
	/// related with other data.
	class AnimationResource : public ResourceBase {
		friend ResourceBase *LoadAnimationResource(ResourceFile* File, FILE* Data, int Size);
		friend class ImageAnimation;
	public:
		////03010000h (Gaming, Animation)
		virtual int getGID() { return GID_ANIMATION; }
		////Currently does nothing
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }
		
		////Default constructor
		AnimationResource() : ResourceBase() { Durations=NULL; FrameCount=0; }

		////Returns the width of the first image
		int getWidth() { if(Subitems.getCount()>0) return ((ImageResource*)Subitems[0])->Width; return 0; }
		////Returns the height of the first image
		int getHeight() { if(Subitems.getCount()>0) return ((ImageResource*)Subitems[0])->Height; return 0; }
		////Returns number of frames
		int getFrameCount() { return FrameCount; }
		////Creates a new Image animation from this resource
		ImageAnimation *getAnimation() { 
			if(Subitems.getCount()) {
				ImageAnimation *ret=new ImageAnimation(this); 
				return ret;
			} else
				return NULL;
		}

		operator ImageAnimation*() { return getAnimation(); }

	protected:
		////Total number of frames that this animation have
		int FrameCount;
		////Frame durations
		int *Durations;
		////Main object
		GGEMain *main;
	};
}