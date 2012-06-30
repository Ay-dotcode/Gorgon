#include "Pointer.h"
#include "File.h"

namespace gge { namespace resource {

	Pointer *LoadPointerResource(File &file, std::istream &Data, int Size) {
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
	}

}}
