#pragma once


#include "ResourceBase.h"


namespace gge { namespace resource {
	class LinkNodeResource : public ResourceBase {
		friend ResourceBase *LoadLinkNodeResource(ResourceFile* File, FILE* Data, int Size);
	public:

		LinkNodeResource() { target.Empty(); }

		////02020000h (Basic, Image)
		virtual int getGID() { return GID_LINKNODE; }
		////Currently does nothing
		virtual bool Save(ResourceFile *File, FILE *Data) { return false; }

		virtual void Resolve();
		virtual void Prepare(GGEMain *main) { }

	protected:
		Guid target;
		ResourceFile* File;
	};

} }
