#include "LinkNode.h"
#include "File.h"

using namespace gge;
using namespace gge::utils;
using namespace std;

namespace gge { namespace resource {
	void LinkNodeResource::Resolve(resource::File &file) {
		Base *parent=file.Root().FindParent(guid);
		file.Redirects.Add(new Redirect(this->guid, target));

		if(parent) {

			for(SortedCollection<Base>::Iterator item=parent->Subitems.First();
				item.IsValid(); item.Next()) {
				if(item->isEqual(guid)) {
					Base *t=file.Root().FindObject(target);
					if(t) {
						file.Multiples[t]++;
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

	Base * LinkNodeResource::GetTarget(resource::File &file) {
		return file.Root().FindObject(target);
	}

	LinkNodeResource *LoadLinkNodeResource(File &File, istream &Data, int Size) {
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

		return link;
	}

} }
