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
    
    Pointer *Pointer::LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size) {
        Pointer *p = new Pointer();
        
        return p;
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
