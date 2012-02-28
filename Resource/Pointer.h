#pragma once

#include "GRE.h"
#include "Base.h"
#include "../Engine/Animation.h"
#include "Image.h"
#include "../Resource/ResizableObject.h"
#include "../Utils/Point2D.h"
#include "Animation.h"
#include "../Engine/Pointer.h"

#pragma warning(push)
#pragma warning(disable:4250)

namespace gge { namespace resource {

	class File;
	class Pointer;

	Pointer *LoadPointerResource(File &File, std::istream &Data, int Size);

	class Pointer : public Animation {
		friend Pointer *LoadPointerResource(File &File, std::istream &Data, int Size);
	public:
		utils::Point Hotspot;
		gge::Pointer::PointerType Type;

	protected:
		virtual void LoadExtra(File &File, std::istream &Data, GID::Type gid, int size);
	};

}}