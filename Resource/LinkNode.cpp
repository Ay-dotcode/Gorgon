#include "LinkNode.h"
#include "ResourceFile.h"

using namespace gge;

namespace gre {
	void LinkNodeResource::Resolve() {
		ResourceBase *parent=File->Root->FindParent(guid);
		File->Redirects.Add(new Redirect(*this->guid, target));

		if(parent) {
			LinkedListIterator<ResourceBase> it=parent->Subitems;
			LinkedListItem<ResourceBase> *item;
			while(item=it) {
				if(item->Item->isEqual(guid)) {
					item->Item=File->Root->FindObject(&target);
					break;
				}
			}

			delete this;
		}
	}

	ResourceBase *LoadLinkNodeResource(ResourceFile* File, FILE* Data, int Size) {
		LinkNodeResource *link=new LinkNodeResource;

		int tpos=ftell(Data)+Size;
		while(ftell(Data)<tpos) {
			int gid,size,tmpint;

			fread(&gid,1,4,Data);
			fread(&size,1,4,Data);

			if(gid==GID_GUID) {
				link->guid=new Guid(Data);
			}
			else if(gid==GID_LINKNODETARGET) {
				link->target.ReadFrom(Data);
			}
			else
				EatChunk(Data,size);
		}

		link->File=File;
		return link;
	}

}