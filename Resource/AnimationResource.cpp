#include "AnimationResource.h"
#include "ResourceFile.h"
#include "ImageResource.h"

namespace gre {
	ResourceBase *LoadAnimationResource(ResourceFile* File, FILE* Data, int Size) {
		AnimationResource *anim=new AnimationResource;

		int tpos=ftell(Data)+Size;
		while(ftell(Data)<tpos) {
			int gid,size;
			fread(&gid,1,4,Data);
			fread(&size,1,4,Data);

			if(gid==GID_ANIMATION_DUR) {
				anim->Durations=new int[size/4];

				fread(anim->Durations,size,1,Data);
			} else if(gid==GID_GUID) {
				anim->guid=new Guid(Data);
			} else if(gid==GID_ANIMATION_IMAGE) {
				anim->Subitems.Add(LoadImageResource(File,Data,size));
			} else {
				EatChunk(Data,size);
			}
		}

		anim->FrameCount=anim->Subitems.getCount();

		return anim;
	}

	int* ImageAnimation::FrameDurations() { 
		return parent->Durations; 
	}
	void ImageAnimation::ProcessFrame(int frame) {
		this->Texture = ((ImageResource*)parent->Subitems[frame])->Texture; 
	}
	ImageAnimation::ImageAnimation(AnimationResource *parent) : DiscreteAnimatorBase() {
		this->parent=parent;
		if(parent->Subitems.getCount())
			this->Texture = ((ImageResource*)parent->Subitems[0])->Texture; 
		else
			this->Texture.ID = 0;

		if(parent->FrameCount)
			setTotalFrames(parent->FrameCount);
		else
			Pause();
	}
	void ImageAnimation::DrawResized(I2DGraphicsTarget *Target, int X, int Y, int W, int H, gge::Alignment Align) {
		if(Align & ALIGN_CENTER)
			X+=(W-this->parent->getWidth())/2;
		else if(Align & ALIGN_RIGHT)
			X+= W-this->parent->getWidth();

		if(Align & ALIGN_MIDDLE)
			Y+=(H-this->parent->getHeight())/2;
		else if(Align & ALIGN_BOTTOM)
			Y+= H-this->parent->getHeight();

		this->Draw(Target, X, Y);
	}
	int  ImageAnimation::Width(int W) { return parent->getWidth(); }
	int  ImageAnimation::Height(int H) { return parent->getHeight(); }
}
