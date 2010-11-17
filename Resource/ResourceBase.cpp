#include "ResourceBase.h"

namespace gre {
	void ResourceBase::Prepare(GGEMain *main) {
		LinkedListIterator<ResourceBase>it=Subitems;
		ResourceBase *resource;

		while(resource=it) {
			resource->Prepare(main);
		}
	}

	void ResourceBase::Resolve() {
		LinkedListIterator<ResourceBase>it=Subitems;
		ResourceBase *resource;

		while(resource=it) {
			resource->Resolve();
		}
	}

	ResourceBase *ResourceBase::FindObject(Guid *guid) {
		LinkedListIterator<ResourceBase>it=Subitems;
		ResourceBase *resource;

		while(resource=it) {
			if(resource->isEqual(guid))
				return resource;

			resource=resource->FindObject(guid);
			if(resource)
				return resource;
		}

		return NULL;
	}

	ResourceBase *ResourceBase::FindParent(Guid *guid) {
		LinkedListIterator<ResourceBase>it=Subitems;
		ResourceBase *resource;

		while(resource=it) {
			if(resource->isEqual(guid))
				return this;

			resource=resource->FindParent(guid);
			if(resource)
				return resource;
		}

		return NULL;
	}

	ResourceBase::ResourceBase() {
		name="";
		caption="";
		guid=NULL;
	}

	ResourceBase::~ResourceBase() {
		if(guid)
			delete guid;

		LinkedListIterator<ResourceBase>it=Subitems;
		ResourceBase *resource;

		while(resource=it) {
			if(resource)
				delete resource;
		}
	}

}