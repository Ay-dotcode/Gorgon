#include "Base.h"

using namespace gge::utils;

namespace gge { namespace resource {
	void Base::Prepare(GGEMain &main, File &file) {
		for(SortedCollection<Base>::Iterator resource=Subitems.First();
			resource.IsValid(); resource.Next()) {

			resource->Prepare(main, file);
		}
	}

	void Base::Resolve(File &file) {
		for(SortedCollection<Base>::Iterator resource=Subitems.First();
			resource.IsValid();) {

			Base &r=resource;
			resource.Next();

			r.Resolve(file);
		}
	}

	Base *Base::FindObject(utils::SGuid guid) {
		for(SortedCollection<Base>::Iterator resource=Subitems.First();
			resource.IsValid(); resource.Next()) {

			if(resource->isEqual(guid))
				return resource.CurrentPtr();

			Base *temp=resource->FindObject(guid);
			if(temp)
				return temp;
		}

		return NULL;
	}

	Base *Base::FindParent(utils::SGuid guid) {
		for(SortedCollection<Base>::Iterator resource=Subitems.First();
			resource.IsValid(); resource.Next()) {

			if(resource->isEqual(guid))
				return this;

			Base *temp=resource->FindParent(guid);
			if(temp)
				return temp;
		}

		return NULL;
	}

	Base::Base() : guid(nullptr), name(""), caption("") { }

	Base::~Base() {
		Subitems.Destroy();
	}

} }
