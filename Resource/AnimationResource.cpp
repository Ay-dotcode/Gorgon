#include "AnimationResource.h"
#include "ResourceFile.h"
#include "ImageResource.h"
#include "..\Utils\BasicMath.h"

using namespace gge::utils;
using namespace gge::graphics;
using namespace std;

namespace gge { namespace resource {

	void LoadAnimationResourceEx(AnimationResource *anim, File &File, std::istream &Data, int Size) {
		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			if(gid==GID::Animation_Durations) {
				unsigned t=0;

				for(int i=0;i<size/4;i++) {
					unsigned d=ReadFrom<int>(Data);
					anim->Frames.push_back(AnimationResourceFrame(d,t));

					t+=d;
				}

				anim->TotalLength=t;
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
				anim->LoadExtra(File, Data, gid, size);
			}
		}

		int i=0;
		for(SortedCollection<ResourceBase>::Iterator it=anim->Subitems.First();it.isValid();it.Next(), i++)
			anim->Frames[i].Image=&dynamic_cast<ImageResource&>(*it);

		anim->FrameCount=anim->Subitems.getCount();
	}


	AnimationResource *LoadAnimationResource(File &File, std::istream &Data, int Size) {
		AnimationResource *anim=new AnimationResource;
		LoadAnimationResourceEx(anim, File, Data, Size);

		return anim;
	}

	animation::ProgressResult::Type ImageAnimation::Progress() {
		if(Controller && parent.Frames.size()) { 
			if(Controller->GetType()==animation::AnimationTimer::Discrete) {
				animation::DiscreteController *dc = dynamic_cast<animation::DiscreteController *>(Controller);

				int cf=dc->CurrentFrame();
				int fc=parent.FrameCount;

				cf=cf % fc;

				if(cf<0) {
					Texture.ID=0;
					return animation::ProgressResult::None;
				}

				Texture=parent[cf].GetTexture();

				return animation::ProgressResult::None;
			}
			else {
				animation::ProgressResult::Type ret=animation::ProgressResult::None;

				int t=Controller->GetProgress();
				int tl=(int)parent.GetTotalLength();

				//this will cause the animation to loop if its simple timer
				//and stop if it is controlled.
				if( t>=parent.StartOf(parent.GetNumberofFrames()-1) ) {
					ret=animation::ProgressResult::Finished;
				}
				if(t<0) {
					t=0;
					ret=animation::ProgressResult::Finished;
				}

				t=PositiveMod(t,tl);
				GLTexture &tx=parent.ImageAt(t).GetTexture();
				if(tx.ID!=Texture.ID)
					Texture=tx;

				return ret;
			}
		}
		else {
			Texture.ID=0;

			return animation::ProgressResult::None;
		}
	}

	ImageAnimation::ImageAnimation( AnimationResource &parent, animation::AnimationTimer &controller, bool owner ) : 
	animation::AnimationBase(controller, owner), parent(parent)
	{
		Texture=parent.ImageAt(0).GetTexture();
	}

	ImageAnimation::ImageAnimation( AnimationResource &parent, bool create ) : 
	animation::AnimationBase(create), parent(parent)
	{
		Texture=parent.ImageAt(0).GetTexture();
	}



	int AnimationResource::FrameAt( unsigned t ) const {
		if(Subitems.getCount()==0) return -1;

		if(t>=(Frames.end()-1)->Start)
			return FrameCount-1;

		int Guessed=(int)floor( ((float)t/TotalLength)*FrameCount );

		if(Frames[Guessed].Start>t) {
			while(Frames[Guessed].Start>t)
				Guessed--;

			return Guessed;
		}
		else if(Frames[Guessed+1].Start<t) {
			while(Frames[Guessed+1].Start<t)
				Guessed++;

			return Guessed;
		} 
		else
			return Guessed;
	}

	void AnimationResource::LoadExtra(File &File, std::istream &Data, GID::Type gid, int size) {
		EatChunk(Data, size);
	}


} }
