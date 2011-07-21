#include "AnimationResource.h"
#include "ResourceFile.h"
#include "ImageResource.h"

using namespace gge::utils;
using namespace std;

namespace gge { namespace resource {
	ResourceBase *LoadAnimationResource(File &File, std::istream &Data, int Size) {
		AnimationResource *anim=new AnimationResource;

		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			if(gid==GID::Animation_Durations) {
				anim->Durations=new int[size/4];

				Data.read((char*)anim->Durations,size);
			} 
			else if(gid==GID::Guid) {
				anim->guid.LoadLong(Data);
			} 
			else if(gid==GID::SGuid) {
				anim->guid.Load(Data);
			} 
			else if(gid==GID::Animation_Image) {
				anim->Subitems.Add(LoadImageResource(File,Data,size), anim->Subitems.HighestOrder()+1);
			} 
			else {
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
		this->Texture = dynamic_cast<ImageResource&>(parent->Subitems[frame]).Texture; 
	}

	void ImageAnimation::init() {
		if(parent->Subitems.getCount())
			this->Texture = dynamic_cast<ImageResource&>(parent->Subitems[0]).Texture; 
		else
			this->Texture.ID = 0;

		if(parent->FrameCount)
			setTotalFrames(parent->FrameCount);
		else
			Pause();
	}
	void ImageAnimation::DrawResized(graphics::I2DGraphicsTarget *Target, int X, int Y, int W, int H, Alignment::Type Align) {

		int h=this->Height(H);
		int w=this->Width(W);

		if(Alignment::isCenter(Align))
			X+=(W-w)/2;
		else if(Alignment::isRight(Align))
			X+= W-w;

		if(Alignment::isMiddle(Align))
			Y+=(H-h)/2;
		else if(Alignment::isBottom(Align))
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
