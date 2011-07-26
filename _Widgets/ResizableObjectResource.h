#pragma once

#include "../Resource/ResourceFile.h"
#include "../Resource/ResourceBase.h"
#include "../Resource/ResizableObject.h"
#include "../Engine/GGEMain.h"

#define GID_RESIZABLEOBJECT			0x05130000
#define GID_RESIZABLEOBJECT_PROPS	0x05130101

namespace gge { namespace widgets {
	class ResizableObjectResource : public resource::ResourceBase {
		friend resource::ResourceBase *LoadResizableObject(resource::File* File, FILE* Data, int Size);

	public:
		utils::SGuid guid_target;
		resource::ResourceBase *target;
		resource::ResizableObject::TileOption		HTile, VTile;
		resource::ResizableObject::IntegralSize	HIntegral, VIntegral;
		
		ResizableObjectResource() : guid_target(nullptr), target(nullptr) { }

		virtual int getGID() { return GID_RESIZABLEOBJECT; }

		////Currently does nothing
		virtual bool Save(resource::File *File, FILE *Data) { return false; }

		operator resource::ResizableObject *();

		operator resource::ResizableObject &() { return Generate(); }

		resource::ResizableObject &Generate();

		virtual void Prepare(GGEMain &main);

	protected:
		resource::File *file;

	};

	resource::ResourceBase *LoadResizableObject(resource::File* File, FILE* Data, int Size);
} }
