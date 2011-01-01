#pragma once

#include "../Resource/ResourceFile.h"
#include "../Resource/ResourceBase.h"
#include "../Resource/ResizableObject.h"
#include "../Engine/GGEMain.h"

#define GID_RESIZABLEOBJECT			0x05130000
#define GID_RESIZABLEOBJECT_PROPS	0x05130101

namespace gorgonwidgets {
	class ResizableObjectResource : public gre::ResourceBase {
		friend ResourceBase *LoadResizableObject(ResourceFile* File, FILE* Data, int Size);

	public:
		Guid *guid_target;
		ResourceBase *target;
		gre::ResizableObject::TileOption		HTile, VTile;
		gre::ResizableObject::IntegralSize	HIntegral, VIntegral;

		ResizableObjectResource() : guid_target(NULL), target(NULL) { }

		virtual int getGID() { return GID_RESIZABLEOBJECT; }

		////Currently does nothing
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }

		operator gre::ResizableObject *();

		operator gre::ResizableObject &() { return Generate(); }

		gre::ResizableObject &Generate();

		virtual void Prepare(GGEMain *main);

	protected:
		ResourceFile *file;

	};

	ResourceBase *LoadResizableObject(ResourceFile* File, FILE* Data, int Size);
}