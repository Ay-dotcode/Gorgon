#include "ResourceBase.h"

using namespace gge::utils;

namespace gge { namespace resource {
	void ResourceBase::Prepare(GGEMain &main, File &file) {
		for(SortedCollection<ResourceBase>::Iterator resource=Subitems.First();
			resource.isValid(); resource.Next()) {

			resource->Prepare(main, file);
		}
	}

	void ResourceBase::Resolve(File &file) {
		for(SortedCollection<ResourceBase>::Iterator resource=Subitems.First();
			resource.isValid();) {

			ResourceBase &r=resource;
			resource.Next();

			r.Resolve(file);
		}
	}

	ResourceBase *ResourceBase::FindObject(utils::SGuid guid) {
		for(SortedCollection<ResourceBase>::Iterator resource=Subitems.First();
			resource.isValid(); resource.Next()) {

			if(resource->isEqual(guid))
				return resource;

			ResourceBase *temp=resource->FindObject(guid);
			if(temp)
				return temp;
		}

		return NULL;
	}

	ResourceBase *ResourceBase::FindParent(utils::SGuid guid) {
		for(SortedCollection<ResourceBase>::Iterator resource=Subitems.First();
			resource.isValid(); resource.Next()) {

			if(resource->isEqual(guid))
				return this;

			ResourceBase *temp=resource->FindParent(guid);
			if(temp)
				return temp;
		}

		return NULL;
	}

	ResourceBase::ResourceBase() : guid(nullptr), name(""), caption("") { }

	ResourceBase::~ResourceBase() {
		Subitems.Destroy();
	}

} }
