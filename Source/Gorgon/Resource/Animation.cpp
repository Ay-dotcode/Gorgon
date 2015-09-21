#include "Animation.h"
#include "File.h"

namespace Gorgon { namespace Resource {

	Animation *Animation::LoadResourceWith(File &file, std::istream &data, unsigned long totalsize, std::function<Base*(File &, std::istream&, GID::Type, unsigned long)> loadfn) {
		auto target = data.tellg()+totalsize;

		std::unique_ptr<Animation> anim{new Animation};
		std::vector<uint32_t> durations;

		while(data.tellg()<target) {
			auto gid = file.ReadGID();
			auto size= file.ReadChunkSize();

			if(gid==GID::Animation_Durations) {
				durations.resize(size/4);
				file.ReadArray(&durations[0], durations.size());
			} else {
				Base *obj=nullptr;
				if(loadfn) {
					obj=loadfn(file, data, gid, size);
				} else {
					obj=file.LoadChunk(*anim, data, gid, size, true);
				}

				if(obj) {
					anim->children.Add(obj);
				}
			}
		}

		unsigned time=0;
		auto images = anim->begin();
		
		for(auto &dur : durations) {
#ifndef NDEBUG
			assert((images!=anim->end() && images->GetGID()==GID::Image) && "Animation is empty");
#endif
			anim->frames.emplace_back(dynamic_cast<Image&>(*images), dur, time);
			++images;
			time+=dur;
		}
		anim->duration = time;

		return anim.release();
	}

	Animation::Animation(Animation &&other) {
		Swap(other);
	}

	void Animation::Swap(Animation &other) {
		using std::swap;

		swap(other.duration, duration);
		swap(other.frames, frames);

		this->swap(other);
	}

	unsigned Animation::FrameAt(unsigned t) const {
		auto count=frames.size();

#ifndef NDEBUG
		assert(count!=0 && "Animation has no frames");
#endif

		if(t>=(frames.end()-1)->GetStart())
			return count-1;

		int guessed=(int)floor( ((float)t/duration)*count );

		if(frames[guessed].GetStart()>t) {
			while(frames[guessed].GetStart()>t)
				guessed--;

			return guessed;
		}
		else if(frames[guessed+1].GetStart()<t) {
			while(frames[guessed+1].GetStart()<t)
				guessed++;

			return guessed;
		} 
		else
			return guessed;
	}

	ImageAnimation::ImageAnimation(const Resource::Animation &parent, Gorgon::Animation::Timer &controller) :
		Animation::Base(controller), parent(parent)
	{
			if(parent.GetCount()>0)
				current=&parent.ImageAt(0);
			else
				current=nullptr;
	}

	ImageAnimation::ImageAnimation( const Resource::Animation &parent, bool create ) : 
		Animation::Base(create), parent(parent)
	{
			if(parent.GetCount()>0)
				current=&parent.ImageAt(0);
			else
				current=nullptr;
	}

	bool ImageAnimation::Progress(unsigned &leftover) {
		if(!controller) return false;

		if(parent.GetCount()==0) return false;

		unsigned progress=controller->GetProgress();

		if(progress>parent.GetDuration()) {
			current=&parent[parent.GetCount()-1];
			leftover=progress-parent.GetDuration();
			return false;
		}
		else {
			current=&parent.ImageAt(progress);
			return true;
		}
	}



} }
