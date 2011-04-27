#include "AnimationResource.h"
#include "ResourceFile.h"
#include "ImageResource.h"

namespace gge { namespace resource {
	ResourceBase *LoadAnimationResource(File* File, FILE* Data, int Size) {
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
				anim->guid.Load(Data);
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

	void ImageAnimation::init() {
		if(parent->Subitems.getCount())
			this->Texture = ((ImageResource*)parent->Subitems[0])->Texture; 
		else
			this->Texture.ID = 0;

		if(parent->FrameCount)
			setTotalFrames(parent->FrameCount);
		else
			Pause();
	}
	void ImageAnimation::DrawResized(graphics::I2DGraphicsTarget *Target, int X, int Y, int W, int H, gge::Alignment Align) {

		int h=this->Height(H);
		int w=this->Width(W);

		if(Align & ALIGN_CENTER)
			X+=(W-w)/2;
		else if(Align & ALIGN_RIGHT)
			X+= W-w;

		if(Align & ALIGN_MIDDLE)
			Y+=(H-h)/2;
		else if(Align & ALIGN_BOTTOM)
			Y+= H-h;

		if(VerticalTiling.Type==ResizableObject::Stretch) {
			if(HorizontalTiling.Type==ResizableObject::Stretch) {
				this->Draw(Target, X,Y , w,h);
			} else {
				this->DrawHTiled(Target, X,Y , w,h);
			}
		} else {
			if(HorizontalTiling.Type==ResizableObject::Stretch) {
				this->DrawVTiled(Target, X,Y , w,h);
			} else {
				this->DrawTiled(Target, X,Y , w,h);
			}
		}
	}

	int  ImageAnimation::Width(int W) { 
		if(W==-1) return parent->getWidth();

		return HorizontalTiling.Calculate(parent->getWidth(), W); 
	}
	int  ImageAnimation::Height(int H) { 
			if(H==-1) return parent->getHeight(); 
		
			return VerticalTiling.Calculate(parent->getHeight(), H); 
		}
} }
