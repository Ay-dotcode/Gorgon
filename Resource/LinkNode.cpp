#include "LinkNode.h"
#include "ResourceFile.h"

using namespace gge;
using namespace gge::utils;
using namespace std;

namespace gge { namespace resource {
	void LinkNodeResource::Resolve() {
		ResourceBase *parent=File->Root().FindParent(guid);
		File->Redirects.Add(new Redirect(this->guid, target));

		if(parent) {

			for(SortedCollection<ResourceBase>::Iterator item=parent->Subitems.First();
				item.isValid(); item.Next()) {
				if(item->isEqual(guid)) {
					ResourceBase *t=File->Root().FindObject(target);
					if(t) {
						item.GetWrapper().SetItem(t);
						break;
					}
					else {
						item.Delete();
						return;
					}
				}
			}

			delete this;
		}
	}

	ResourceBase *LoadLinkNodeResource(File &File, istream &Data, int Size) {
		LinkNodeResource *link=new LinkNodeResource;

		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;

			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			if(gid==GID::Guid) {
				link->guid.LoadLong(Data);
			}
			else if(gid==GID::SGuid) {
				link->guid.Load(Data);
			}
			else if(gid==GID::LinkNode_Target) {
				if(size==8)
					link->target.Load(Data);
				else
					link->target.LoadLong(Data);
			}
			else
				EatChunk(Data,size);
		}

		link->File=&File;

		return link;
	}

} }
