#include "ResourceBase.h"

namespace gge { namespace resource {
	void ResourceBase::Prepare( GGEMain &main )
	{
		utils::LinkedListIterator<ResourceBase>it=Subitems;
		ResourceBase *resource;

		while(resource=it) {
			resource->Prepare(main);
		}
	}

	void ResourceBase::Resolve() {
		utils::LinkedListIterator<ResourceBase>it=Subitems;
		ResourceBase *resource;

		while(resource=it) {
			resource->Resolve();
		}
	}

	ResourceBase *ResourceBase::FindObject(utils::SGuid guid) const {
		foreach(ResourceBase, resource, Subitems)  {
			if(resource->isEqual(guid))
				return resource;

			resource=resource->FindObject(guid);
			if(resource)
				return resource;
		}

		return NULL;
	}

	ResourceBase *ResourceBase::FindParent(utils::SGuid guid) const {
		const_foreach(ResourceBase, resource, Subitems)  {
			if(resource->isEqual(guid))
				return const_cast<ResourceBase*>(this);

			resource=resource->FindParent(guid);
			if(resource)
				return resource;
		}

		return NULL;
	}

	ResourceBase::ResourceBase() : guid(nullptr), name(""), caption("") { }

	ResourceBase::~ResourceBase() {
		Subitems.Destroy();
	}

} }
