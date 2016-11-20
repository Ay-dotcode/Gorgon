#include "Animation.h"
#include "File.h"
#include "../Utils/Assert.h"

namespace Gorgon { namespace Resource {

	Animation *Animation::LoadResourceWith(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long totalsize, std::function<Base*(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, GID::Type, unsigned long)> loadfn) {
		auto target = reader->Target(totalsize);

		std::unique_ptr<Animation, void(*)(Animation*)> anim{new Animation, [](Animation *a) {a->DeleteResource();}};
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

		auto images = anim->cbegin();
		
		for(auto &dur : durations) {
			ASSERT(images!=anim->cend() && images->GetGID()==GID::Image, "Animation is empty");

			anim->Add(dynamic_cast<Image&>(*images), dur);
		}

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
			Image::SaveThis(writer, frame.GetImage());
		}
	}

	void Animation::save(Writer &writer) const {
		auto start=writer.WriteObjectStart(this);

		savedata(writer);

		writer.WriteEnd(start);
	}

	Graphics::BitmapAnimationProvider Animation::MoveOut() {
		Graphics::BitmapAnimationProvider anim;

		for(auto &frame : frames) {
            auto bmp = &frame.GetImage();
            auto img = dynamic_cast<Image*>(bmp);
            
            if(img) {
                anim.Add(img->MoveOut(), frame.GetDuration());
            }
            else {
                anim.Add(std::move(*bmp), frame.GetDuration());
            }
		}

		children.Clear();
		frames.clear();
		duration = 0;

		return anim;
	}


} }
