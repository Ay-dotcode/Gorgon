#include "Pointer.h"
#include "File.h"

namespace gge { namespace resource {

	void Pointer::LoadExtra(File &File, std::istream &Data, GID::Type gid, int size) {
		if(gid==GID::Pointer_Props) {
			ReadFrom(Data, Hotspot);
			Type=(gge::Pointer::PointerType)ReadFrom<int>(Data);
		}
		else {
			EatChunk(Data,size);
		}
	}

	Pointer *LoadPointerResource(File &File, std::istream &Data, int Size) {
		Pointer *pointer=new Pointer;

		LoadAnimationResourceEx(pointer, File, Data, Size);

		return pointer;
	}

}}