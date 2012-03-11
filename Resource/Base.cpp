#include "Base.h"
#include "File.h"

using namespace gge::utils;

namespace gge { namespace resource {
	void Base::Prepare(GGEMain &main, File &file) {
		this->file=&file;
		for(SortedCollection<Base>::Iterator resource=Subitems.First();
			resource.IsValid(); resource.Next()) {

			resource->Prepare(main, file);
		}
	}

	void Base::Resolve(File &file) {
		this->file=&file;
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

	Base::Base() : guid(nullptr), name(""), caption(""), file() { }

	Base::~Base() {
		for(auto it=Subitems.First();it.IsValid();it.Next()) {
			if(file && file->Multiples.count(it.CurrentPtr())) {
				file->Multiples[it.CurrentPtr()]--;
			}
			else {
				if(it.CurrentPtr())
					delete it.CurrentPtr();
			}
		}

		Subitems.Clear();
	}

} }
