#include "PointerResource.h"
#include "ResourceFile.h"

namespace gge { namespace resource {

	void PointerResource::LoadExtra(File &File, std::istream &Data, GID::Type gid, int size) {
		if(gid==GID::Pointer_Props) {
			ReadFrom(Data, Hotspot);
			Type=(Pointer::PointerType)ReadFrom<int>(Data);
		}
		else {
			EatChunk(Data,size);
		}
	}

	PointerResource *LoadPointerResource(File &File, std::istream &Data, int Size) {
		PointerResource *pointer=new PointerResource;

		LoadAnimationResourceEx(pointer, File, Data, Size);

		return pointer;
	}

}}