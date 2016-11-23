#include "Pointer.h"
#include "File.h"
#include "Writer.h"

namespace Gorgon { namespace Resource {

	/*Pointer *LoadPointerResource(File &file, std::istream &Data, int Size) {
		Pointer *pointer=new Pointer;

		pointer->animation->loadextra=[&](File &f, std::istream &in, GID::Type gid, int size) {
			if(gid==GID::Pointer_Props) {
				ReadFrom(in, pointer->Hotspot);
				pointer->Type=(gge::Pointer::PointerType)ReadFrom<int>(in);
			}
			else {
				EatChunk(in,size);
			}
		};


		LoadAnimationResourceEx(pointer->animation, file, Data, Size);

		return pointer;
	}*/
    
    Resource::Pointer* Pointer::LoadResource(std::weak_ptr< Gorgon::Resource::File > file, std::shared_ptr< Gorgon::Resource::Reader > reader, long unsigned int totalsize)
{
        Pointer *p = new Pointer();
        
		auto target = reader->Target(totalsize);

		std::vector<uint32_t> durations;

		auto f=file.lock();
        Base *obj = nullptr;

		while(!target) {
			auto gid = reader->ReadGID();
			auto size= reader->ReadChunkSize();

			if(gid==GID::Pointer_Props) {
				p->hotspot = reader->ReadPoint();
                p->type    = reader->ReadEnum32<Graphics::PointerType>();
			} 
			else if(gid == GID::Animation) {
				obj=Animation::LoadResource(file, reader, size);

				if(obj) {
					ASSERT(dynamic_cast<Animation*>(obj), "Pointer animation should be an animation resource.");
					ASSERT(p->children.GetCount() == 0, "A pointer can only have a single animation.");
                    
					p->children.Add(obj);
				}
			}
		}

		dynamic_cast<Graphics::BitmapAnimationProvider&>(*p) = std::move(*dynamic_cast<Animation*>(obj));
        
        return p;
    }
    
    Graphics::PointerProvider Pointer::MoveOut() {
        return Graphics::PointerProvider(*new Graphics::BitmapAnimationProvider(std::move(*this)), hotspot, true);        
    }

	void Pointer::save(Writer &writer) const {
		auto start=writer.WriteObjectStart(this);

		writer.WriteChunkHeader(GID::Pointer_Props, 12);
        writer.WritePoint(hotspot);
        writer.WriteEnum32(type);
        
        Animation::SaveThis(writer, *this);

		writer.WriteEnd(start);
	}

}}
