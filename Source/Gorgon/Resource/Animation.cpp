#include "Animation.h"
#include "File.h"
#include "../Utils/Assert.h"

namespace Gorgon { namespace Resource {

	Animation *Animation::LoadResourceWith(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize, std::function<Base*(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, GID::Type, unsigned long)> loadfn) {
		auto target = reader->Target(totalsize);

		std::unique_ptr<Animation> anim{new Animation};
		std::vector<uint32_t> durations;

		auto f=file.lock();

		while(!target) {
			auto gid = reader->ReadGID();
			auto size= reader->ReadChunkSize();

			if(gid==GID::Animation_Durations) {
				durations.resize(size/4);
				reader->ReadArray(&durations[0], durations.size());
			} else {
				Base *obj=nullptr;

				if(loadfn) {
					obj=loadfn(file, reader, gid, size);
				}
				else {
					obj=f->LoadChunk(*anim, gid, size);
				}

				if(obj) {
					ASSERT(dynamic_cast<Image*>(obj), "Image animation members should be derived from Resource::Image.");
					anim->children.Add(obj);
				}
			}
		}

		unsigned time=0;
		auto images = anim->cbegin();
		
		for(auto &dur : durations) {
			ASSERT(images!=anim->cend() && images->GetGID()==GID::Image, "Animation is empty");

			anim->frames.emplace_back(dynamic_cast<Image&>(*images), dur, time);
			++images;
			time+=dur;
		}
		anim->duration = time;

		return anim.release();
	}

	void Animation::savedata(Writer &writer) const {
		//durations
		writer.WriteChunkHeader(GID::Animation_Durations, frames.size()*4);
		for(auto &frame : frames) {
			writer.WriteInt32(frame.GetDuration());
		}

		//images
		for(auto &frame : frames) {
			frame.GetImage().Save(writer);
		}
	}

	void Animation::save(Writer &writer) const {
		auto start=writer.WriteObjectStart(this);

		savedata(writer);

		writer.WriteEnd(start);
	}

	Animation::Animation(Animation &&other) {
		Swap(other);
	}

	void Animation::Swap(Animation &other) {
		using std::swap;

		swap(other.duration, duration);
		swap(other.frames, frames);

		throw "to be fixed";
	}

	unsigned Animation::FrameAt(unsigned t) const {
		auto count=frames.size();

#ifndef NDEBUG
		ASSERT(count!=0, "Animation has no frames");
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
