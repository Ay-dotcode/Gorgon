#pragma once

#include "GRE.h"
#include "ResourceBase.h"
#include "../Engine/Animation.h"
#include "ImageResource.h"
#include "../Resource/ResizableObject.h"
#include "../Utils/Point2D.h"
#include "AnimationResource.h"
#include "../Engine/Pointer.h"

#pragma warning(push)
#pragma warning(disable:4250)

namespace gge { namespace resource {

	class File;
	class PointerResource;

	PointerResource *LoadPointerResource(File &File, std::istream &Data, int Size);

	class PointerResource : public AnimationResource {
		friend PointerResource *LoadPointerResource(File &File, std::istream &Data, int Size);
	public:
		utils::Point Hotspot;
		Pointer::PointerType Type;

	protected:
		virtual void LoadExtra(File &File, std::istream &Data, GID::Type gid, int size);
	};

}}